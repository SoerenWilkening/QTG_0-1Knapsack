"""
This script runs the benchmark for the QTG algorithm.
"""
import os

from algbench import Benchmark

from qtg.bindings import execute_combo, Knapsack, execute_q_max_search
from qtg.utils import load_instance
import slurminade

slurminade.update_default_configuration(
    partition="alg",
    constraint="alggen03",
    cpus_per_task=1,
    mem_per_cpu="8G",
    mail_type="FAIL",
)  # global options for slurm


def configure_grb_license_path():
    import socket
    import os
    from pathlib import Path

    if "alg" not in socket.gethostname():
        return

    os.environ["GRB_LICENSE_FILE"] = os.path.join(
        Path.home(), ".gurobi", socket.gethostname(), "gurobi.lic"
    )
    if not os.path.exists(os.environ["GRB_LICENSE_FILE"]):
        msg = "Gurobi License File not found."
        raise RuntimeError(msg)


configure_grb_license_path()


def run_benchmark(measure_params: dict, alg_params: dict, instance: dict, instance_: Knapsack, base_result_):
    """
    Run QTG for a single instance.
    :param measure_params: Parameters for the measurement
    :param instance: Passed for logging purposes of algbench
    :param instance_: Will not be part of logging but contains the actual instance.
    :return: A dictionary with the results of the benchmark.
    """

    result = {
        "opt": {
            "objective_value": base_result_.objective_value,
            "item_assignments": base_result_.item_assignments,
        },
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
            "gap": (base_result_.objective_value - qtg_result.objective_value) / base_result_.objective_value,
            "item_assignments": qtg_result.item_assignments,
            "qubit_count": qtg_result.resources.qubit_count,
            "cycle_count": qtg_result.resources.cycle_count,
            "gate_count": qtg_result.resources.gate_count,
            "cycle_count_decomp": qtg_result.resources.cycle_count_decomp,
            "gate_count_decomp": qtg_result.resources.gate_count_decomp
        })

    return result


@slurminade.slurmify()
def run(instance_path: str, benchmark_dir: str, iterations: int):
    instance = load_instance(instance_path)
    benchmark = Benchmark(benchmark_dir)

    bias = len(instance.items) // 4
    qtg_iterations = 100 if len(instance.items) == 30 else 200

    # Execute once without measurements to verify that all solutions were correct.
    base_result = execute_combo(instance)
    obj_check = sum(int(assign) * item.profit for assign, item in zip(base_result.item_assignments, instance.items))
    assert base_result.objective_value == obj_check

    benchmark.run(run_benchmark,
                  measure_params={
                      "iterations": iterations,
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
                  instance_=instance, base_result_=base_result)


@slurminade.slurmify(mail_type="ALL")
def clean_up(benchmark_dir):
    print("Compressing")
    benchmark = Benchmark(benchmark_dir)
    benchmark.compress()


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("-d", "--instances-dir", type=str, required=True)
    parser.add_argument("--iterations", type=int, default=10)
    parser.add_argument("-o", "--out")
    args = parser.parse_args()

    with slurminade.JobBundling(max_size=5):  # automatically bundles up to 20 tasks
        for instance_name in sorted(os.listdir(args.instances_dir)):
            if not instance_name.endswith(".knap"):
                continue
            print("Solving instance", instance_name)
            instance_path = os.path.join(args.instances_dir, instance_name)

            run.distribute(instance_path=instance_path, benchmark_dir=args.out, iterations=args.iterations)

    slurminade.join()  # make sure that the clean up jobs runs after all other jobs
    clean_up.distribute(benchmark_dir=args.out)
