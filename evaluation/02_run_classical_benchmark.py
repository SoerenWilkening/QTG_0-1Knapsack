"""
This script runs the benchmark for the combo algorithm.
Because we have to measure memory usage, we execute the solver in a different progress and
measure RSS with GNU time. Note that in order to run this script in a conda environment you might
have to pass a --gnu-time-cmd (namely a path to the gnu time executable).
"""

import json
import subprocess
import os
import tempfile
from algbench import Benchmark

from qtg.utils import load_instance

import slurminade

slurminade.update_default_configuration(
    partition="alg",
    constraint="alggen05",
    cpus_per_task=4,
    mem_per_cpu="3G",
    mail_type="FAIL",
)  # global options for slurm


def run_benchmark(measure_params: dict, instance: dict, solver: str):
    """
    Run the benchmark for a single instance.
    :param measure_params: Parameters for the measurement
    :param instance: Passed for logging purposes of algbench
    :param instance_: Will not be part of logging but contains the actual instance.
    :return: A dictionary with the results of the benchmark.
    """

    with tempfile.TemporaryDirectory() as tmp_dir:
        tmp_result_file = os.path.join(tmp_dir, "./tmp_result.json")
        measurement_file = os.path.join(tmp_dir, "./tmp_measurement.out")

        cmd = [measure_params["gnu_time_command"],
               '-o', measurement_file,
               '--format "%M %e %S %U"',
               "python",
               "01_run_solver.py",
               "--instance", instance["instance_path"],
               "-t", measure_params["timeout"],
               "--solver", solver,
               "--out", tmp_result_file]

        process = subprocess.run(' '.join(cmd), capture_output=True, text=True, shell=True, check=True)

        if process.returncode != 0:
            raise Exception("Error running solver with", ' '.join(cmd), process.stderr)

        # parse result file
        with open(tmp_result_file, "r") as f:
            result = json.load(f)

        os.remove(tmp_result_file)

        # parse measurement file
        with open(measurement_file, "r") as f:
            gtime_line = f.readlines()[-1]

        os.remove(measurement_file)
        maximum_resident_size_kb, elapsed_real_time, elapsed_system_time, elapsed_user_time = gtime_line.split()

        result["maximum_resident_size_kb"] = (int(maximum_resident_size_kb))
        result["elapsed_real_time"] = (float(elapsed_real_time))
        result["elapsed_system_time"] = (float(elapsed_system_time))
        result["elapsed_user_time"] = (float(elapsed_user_time))

    return result


@slurminade.slurmify(mail_type="ALL")
def run(benchmark_dir, instance_path, instance_name, gnu_time_cmd):
    instance = load_instance(instance_path)

    benchmark = Benchmark(benchmark_dir)

    for solver in ["ip", "cp-sat", "combo"]:
        benchmark.run(run_benchmark,
                      solver=solver,
                      measure_params={
                          "gnu_time_command": gnu_time_cmd,
                          "timeout": 900
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
                      })


@slurminade.slurmify()
def clean_up(benchmark_dir):
    print("Compressing")
    benchmark = Benchmark(benchmark_dir)
    benchmark.compress()


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("-d", "--instances-dir", type=str, required=True)
    parser.add_argument("-o", "--out", type=str, required=True)
    parser.add_argument("--gnu-time-cmd", type=str, default="gtime")
    args = parser.parse_args()

    with slurminade.JobBundling(max_size=5):  # automatically bundles up to 20 tasks
        for instance_name in sorted(os.listdir(args.instances_dir)):
            if not instance_name.endswith(".knap"):
                continue
            print("Solving instance", instance_name)

        instance_path = os.path.join(args.instances_dir, instance_name)
        run.distribute(benchmark_dir=args.out,
                       instance_path=instance_path,
                       instance_name=instance_name,
                       gnu_time_cmd=args.gnu_time_cmd)

    slurminade.join()  # make sure that the clean up jobs runs after all other jobs
    clean_up.distribute(benchmark_dir=args.out)
