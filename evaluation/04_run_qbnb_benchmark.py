"""
This script runs the benchmark for the QTG algorithm.
"""
import os
from pathlib import Path

from algbench import Benchmark

from qtg.bindings import Knapsack, execute_qbnb, QBnBAlgorithm
from qtg.utils import load_instance
import slurminade

slurminade.update_default_configuration(
    partition="alg",
    constraint="alggen03",
    exclusive=True,
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


def run_benchmark(alg_params: dict, instance: dict, instance_: Knapsack):
    """
    Run QTG for a single instance.
    :param measure_params: Parameters for the measurement
    :param instance: Passed for logging purposes of algbench
    :param instance_: Will not be part of logging but contains the actual instance.
    :return: A dictionary with the results of the benchmark.
    """

    if alg_params["algorithm"] == "ambainis":
        alg = QBnBAlgorithm.Ambainis.value
    elif alg_params["algorithm"] == "montanaro":
        alg = QBnBAlgorithm.Montanaro.value
    else:
        raise ValueError("Unknown algorithm")

    qbnb_result = execute_qbnb(instance_, alg, int(alg_params["max_node_count"]))

    return {
        "objective_value": qbnb_result.objective_value,
        "elapsed_cycles_lb": qbnb_result.elapsed_cycles_lb,
    }


@slurminade.slurmify()
def run(instance_name: str, instance_path: str, benchmark_dir: str):
    instance = load_instance(instance_path)
    benchmark = Benchmark(benchmark_dir)

    for algorithm in ["ambainis", "montanaro"]:
        benchmark.run(run_benchmark,
                      alg_params={
                          "algorithm": algorithm,
                          "max_node_count": 1e10,
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


@slurminade.slurmify(mail_type="ALL")
def clean_up(benchmark_dir):
    print("Compressing")
    benchmark = Benchmark(benchmark_dir)
    benchmark.compress()


if __name__ == "__main__":
    import argparse

    parser = argparse.ArgumentParser()
    parser.add_argument("-d", "--instances-dir", type=str, required=True)
    parser.add_argument("-o", "--out")
    args = parser.parse_args()

    with slurminade.JobBundling(max_size=5):  # automatically bundles up to 20 tasks
        for instance_path in list(Path(args.instances_dir).glob("**/*.kp")) + \
                             list(Path(args.instances_dir).glob("**/*.knap")):

            instance_name = instance_path.name
            print("Solving instance", instance_name)

            instance = load_instance(str(instance_path))

            run.distribute(instance_name=instance_name,
                           instance_path=str(instance_path),
                           benchmark_dir=args.out)

    slurminade.join()  # make sure that the clean up jobs runs after all other jobs
    clean_up.distribute(benchmark_dir=args.out)
