import json
import os
from algbench import Benchmark

from qtg.solvers import KnapsackSolver
from qtg.utils import load_instance
import slurminade
import hwcounter


slurminade.update_default_configuration(
    partition="alg",
    constraint="alggen03",
    exclusive=True,
    mail_type="FAIL",
)  # global options for slurm

slurminade.set_dispatch_limit(200)

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


@slurminade.slurmify()
def run(benchmark_dir, instance_path, instance_name):
    instance = load_instance(instance_path)

    with open("cached/optimal_solutions.json", "r") as f:
        optimal_solutions = json.load(f)

    assert instance_name in optimal_solutions

    def run_solver(solver, bound, measure_params, **kwargs):
        solver = KnapsackSolver(instance, model_type=solver)
        start = hwcounter.count()
        solution = solver.solve(time_limit=measure_params["timeout"], bound=bound)
        elapsed_cycles = hwcounter.count_end() - start
        return {
            "solver": solver,
            "item_assignments": solution.item_assignments,
            "objective_value": solution.objective_value,
            "elapsed_time": solution.elapsed_time,
            "elapsed_cycles": elapsed_cycles,
            "best_bound": solution.best_bound,
            "optimal": solution.optimal,
            "node_count": solution.node_count
        }

    for solver in ["ip", "cp-sat"]:
        benchmark = Benchmark(benchmark_dir)
        benchmark.run(run_solver,
                      solver=solver,
                      bound=optimal_solutions[instance_name]["objective"],
                      measure_params={
                          "timeout": optimal_solutions[instance_name]["runtime"]
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


@slurminade.slurmify(mail_type="ALL")
def clean_up(benchmark_dir):
    print("Compressing")
    benchmark = Benchmark(benchmark_dir)
    benchmark.compress()


if __name__ == "__main__":

    instances_dirs = ["instances/classical_qtg_comparison/small", "instances/classical_qtg_comparison/large"]
    out_dir = "results/heuristics"

    with slurminade.JobBundling(max_size=5):  # automatically bundles up to 20 tasks
        for instances_dir in instances_dirs:
            for instance_name in sorted(os.listdir(instances_dir)):
                if not instance_name.endswith(".knap"):
                    continue
                print("Solving instance", instance_name)

                instance_path = os.path.join(instances_dir, instance_name)
                run.distribute(benchmark_dir=out_dir,
                               instance_path=instance_path,
                               instance_name=instance_name)

    slurminade.join()  # make sure that the clean up jobs runs after all other jobs
    clean_up.distribute(benchmark_dir=out_dir)
