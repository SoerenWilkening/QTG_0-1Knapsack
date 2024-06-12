"""
This script runs the benchmark for the QTG algorithm.
"""
import os
from pathlib import Path

from algbench import Benchmark

from qtg.bindings import Knapsack, execute_ctg
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


def run_benchmark(alg_params: dict, instance: dict, instance_: Knapsack):
    """
    Run QTG for a single instance.
    :param measure_params: Parameters for the measurement
    :param instance: Passed for logging purposes of algbench
    :param instance_: Will not be part of logging but contains the actual instance.
    :return: A dictionary with the results of the benchmark.
    """

    ctg_result = execute_ctg(instance_,
                             alg_params["bias"],
                             alg_params["max_iter"],
                             alg_params["n_iterations"],
                             alg_params["seed"])

    return {
        "objective_values": ctg_result.objective_values,
        "elapsed_cycles": ctg_result.elapsed_cycles,
        "total_iterations": ctg_result.total_iterations,
        "qtg_estimate_cycles": ctg_result.qtg_estimate_cycles,
        "qtg_cycles": ctg_result.qtg_cycles,
    }


@slurminade.slurmify()
def run(instance_name: str, instance_path: str, benchmark_dir: str):
    instance = load_instance(instance_path)
    benchmark = Benchmark(benchmark_dir)
    benchmark.run(run_benchmark,
                  alg_params={
                      "max_iter": 700 + (instance.size * instance.size // 16),
                      "max_iter_str": "700 + n^2 / 16",
                      "bias": len(instance.items) / 4,
                      "n_iterations": 100,
                      "seed": 42,
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

    benchmark = Benchmark(args.out)

    instances = {
        row["parameters"]["args"]["instance"]["name"] for row in benchmark
    }

    with slurminade.JobBundling(max_size=2):  # automatically bundles up to 20 tasks
        for instance_path in list(Path(args.instances_dir).glob("**/*.kp")) + \
                             list(Path(args.instances_dir).glob("**/*.knap")):
            instance_name = instance_path.name

            instance = load_instance(str(instance_path))

            if instance_name in instances:
                print("Skipping", instance_name)
                continue

            print("Solving instance", instance_name)

            run.distribute(instance_name=instance_name,
                           instance_path=str(instance_path),
                           benchmark_dir=args.out)

    slurminade.join()  # make sure that the clean up jobs runs after all other jobs
    clean_up.distribute(benchmark_dir=args.out)
