"""
This script runs the benchmark for the combo algorithm.
Because we have to measure memory usage, we execute the solver in a different progress and
measure RSS with GNU time. Note that in order to run this script in a conda environment you might
have to pass a --gnu-time-cmd (namely a path to the gnu time executable).
"""

import json
import subprocess
import sys
import os

import numpy as np
from algbench import Benchmark

benchmark = Benchmark("./results_combo")

sys.path.append(os.path.abspath('../pysrc'))

from qtg.bindings import execute_combo, Knapsack
from qtg.utils import load_instance


def run_benchmark(measure_params: dict, instance: dict, instance_: Knapsack):
    """
    Run the benchmark for a single instance.
    :param measure_params: Parameters for the measurement
    :param instance: Passed for logging purposes of algbench
    :param instance_: Will not be part of logging but contains the actual instance.
    :return: A dictionary with the results of the benchmark.
    """
    # Execute once without measurements to verify that all solutions were correct.
    base_result = execute_combo(instance_, True, False)

    obj_check = sum(int(assign) * item.profit for assign, item in zip(base_result.item_assignments, instance_.items))
    assert base_result.objective_value == obj_check

    tmp_result_file = "./tmp_result.json"
    measurement_file = "./tmp_measurement.out"

    # Measures that we collect and calculate the average with
    measures = {
        "maximum_resident_size_kb": [],
        "elapsed_real_time": [],
        "elapsed_system_time": [],
        "elapsed_user_time": [],
        "elapsed_cycles": [],
    }

    for _ in range(measure_params["iterations"]):
        cmd = [measure_params["gnu_time_command"],
               '-o', measurement_file,
               '--format "%M %e %S %U"',
               "python",
               "01_run_solver.py",
               "--instance", instance_path,
               "--out", tmp_result_file]
        process = subprocess.run(' '.join(cmd), capture_output=True, text=True, shell=True, check=True)

        if process.returncode != 0:
            raise Exception("Error running solver with", ' '.join(cmd), process.stderr)

        # parse result file
        with open(tmp_result_file, "r") as f:
            result = json.load(f)
        os.remove(tmp_result_file)

        # Calculate that the objective is correct
        obj_check = sum(int(assign) * item.profit for assign, item in zip(result["item_assignments"], instance_.items))

        assert result["objective_value"] == base_result.objective_value
        assert obj_check == base_result.objective_value

        # parse measurement file
        with open(measurement_file, "r") as f:
            gtime_line = f.readlines()[-1]
        os.remove(measurement_file)
        maximum_resident_size_kb, elapsed_real_time, elapsed_system_time, elapsed_user_time = gtime_line.split()

        measures["maximum_resident_size_kb"].append(int(maximum_resident_size_kb))
        measures["elapsed_real_time"].append(float(elapsed_real_time))
        measures["elapsed_system_time"].append(float(elapsed_system_time))
        measures["elapsed_user_time"].append(float(elapsed_user_time))
        measures["elapsed_cycles"].append(result["elapsed_cycles"])

    # Put in solution from the base solution which has been validated to be correct
    result = {
        "objective_value": base_result.objective_value,
        "item_assignments": base_result.item_assignments
    }

    # Calculate average of measures
    result.update({
        f"avg_{key}": np.average(measures[key]) for key in measures
    })

    return result


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("-d", "--instances-dir", type=str, required=True)
    parser.add_argument("--gnu-time-cmd", type=str, default="gtime")
    parser.add_argument("--iterations", type=int, default=10)
    args = parser.parse_args()
    for instance_name in sorted(os.listdir(args.instances_dir)):
        if not instance_name.endswith(".knap"):
            continue
        print("Solving instance", instance_name)
        instance_path = os.path.join(args.instances_dir, instance_name)
        instance = load_instance(instance_path)

        benchmark.run(run_benchmark,
                      measure_params={
                          "iterations": args.iterations,
                          "gnu_time_command": args.gnu_time_cmd,
                      },
                      instance={
                          "instance_path": instance_path,
                          "name": instance_name,
                          "items": [{
                              "cost": item.cost,
                              "profit": item.profit,
                          } for item in instance.items],
                          "capacity": instance.capacity,
                          "size": instance.size,
                      },
                      instance_=instance)
