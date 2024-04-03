"""
This script runs the benchmark for the QTG algorithm.
"""
import os

from algbench import Benchmark

benchmark = Benchmark("./results_qtg")

from qtg.bindings import execute_combo, Knapsack, execute_q_max_search
from qtg.utils import load_instance


def run_benchmark(measure_params: dict, alg_params: dict, instance: dict, instance_: Knapsack):
    """
    Run QTG for a single instance.
    :param measure_params: Parameters for the measurement
    :param instance: Passed for logging purposes of algbench
    :param instance_: Will not be part of logging but contains the actual instance.
    :return: A dictionary with the results of the benchmark.
    """
    # Execute once without measurements to verify that all solutions were correct.
    base_result = execute_combo(instance_, True, False)
    obj_check = sum(int(assign) * item.profit for assign, item in zip(base_result.item_assignments, instance_.items))
    assert base_result.objective_value == obj_check

    result = {
        "measurements": []
    }

    for seed in range(measure_params["iterations"]):
        qtg_result = execute_q_max_search(instance_, alg_params["bias"], alg_params["qtg_iterations"], seed)

        obj_check = sum(int(assign) * item.profit for assign, item in zip(qtg_result.item_assignments, instance_.items))
        cost_check = sum(int(assign) * item.cost for assign, item in zip(qtg_result.item_assignments, instance_.items))

        assert cost_check <= instance_.capacity, \
            f"Cost exceeds capacity {cost_check} > {instance_.capacity}"
        assert qtg_result.objective_value == obj_check, \
            f"Solution or obj function wrong {obj_check} != {qtg_result.objective_value}"

        result["measurements"].append({
            "objective_value": qtg_result.objective_value,
            "gap": (base_result.objective_value - qtg_result.objective_value) / base_result.objective_value,
            "item_assignments": qtg_result.item_assignments,
            "qubit_count": qtg_result.resources.qubit_count,
            "cycle_count": qtg_result.resources.cycle_count,
            "gate_count": qtg_result.resources.gate_count,
            "cycle_count_decomp": qtg_result.resources.cycle_count_decomp,
            "gate_count_decomp": qtg_result.resources.gate_count_decomp
        })

    return result


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("-d", "--instances-dir", type=str, required=True)
    parser.add_argument("--iterations", type=int, default=10)
    args = parser.parse_args()

    for instance_name in sorted(os.listdir(args.instances_dir)):
        if not instance_name.endswith(".knap"):
            continue
        print("Solving instance", instance_name)
        instance_path = os.path.join(args.instances_dir, instance_name)
        instance = load_instance(instance_path)

        bias = len(instance.items) // 4
        qtg_iterations = 100 if len(instance.items) == 30 else 200

        benchmark.run(run_benchmark,
                      measure_params={
                          "iterations": args.iterations,
                      },
                      alg_params={
                          "bias": bias,
                          "qtg_iterations": qtg_iterations,
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
