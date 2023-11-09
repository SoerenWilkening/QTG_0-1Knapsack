import json
import subprocess
import sys
import os

from algbench import Benchmark

benchmark = Benchmark("./results")

sys.path.append(os.path.abspath('../pysrc'))

from qtg.bindings import execute_combo
from qtg.utils import load_instance


def run_benchmark(instance_path):
    print("Solving instance", instance_path)
    instance = load_instance(instance_path)

    try:
        result = execute_combo(instance, True, False)
    except:
        return

    try:
        output = subprocess.run(f"scalene --json --cli 01_run_solver.py --instance {instance_path}",
                                capture_output=True,
                                text=True, check=True,
                                shell=True)
    except subprocess.CalledProcessError:
        return

    memory_result = json.loads(output.stdout)
    memory_footprint = memory_result["max_footprint_mb"]

    return {
        "objective_value": result.objective_value,
        "elapsed_cycles": result.elapsed_cycles,
        "memory_footprint": memory_footprint,
        "item_assignments": result.item_assignments,
        "items": [{
            "cost": item.cost,
            "profit": item.profit,
        } for item in instance.items],
        "capacity": instance.capacity,
        "size": instance.size,
    }


if __name__ == "__main__":
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--instances", type=str, required=False)
    args = parser.parse_args()
    for instance_name in os.listdir(args.instances):
        if not instance_name.endswith(".knap"):
            continue

        benchmark.run(run_benchmark, os.path.join(args.instances, instance_name))
