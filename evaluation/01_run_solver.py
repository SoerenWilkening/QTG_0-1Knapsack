import argparse
import json

from qtg.bindings import execute_combo, execute_expknap
from qtg.solvers import KnapsackSolver
from qtg.utils import load_instance
import time

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--instance", type=str, required=True)
    parser.add_argument("-o", "--out", type=str, required=False)
    parser.add_argument("-t", "--timeout", type=float, required=True)
    parser.add_argument("--solver", type=str,
                        choices=["ip", "cp-sat", "combo", "expknap"], required=True)
    args = parser.parse_args()
    knapsack = load_instance(args.instance)

    if args.solver == "combo":
        start = time.time()
        result = execute_combo(knapsack)
        result = {
            "solver": "combo",
            "objective_value": result.objective_value,
            "best_bound": result.objective_value,
            "elapsed_cycles": result.elapsed_cycles,
            "elapsed_time": time.time()-start,
            "item_assignments": result.item_assignments,
            "optimal": True
        },
    elif args.solver == "expknap":
        start = time.time()
        result = execute_expknap(knapsack)
        result = {
            "solver": "expknap",
            "objective_value": result.objective_value,
            "elapsed_cycles": result.elapsed_cycles,
            "elapsed_time": time.time()-start,
            "item_assignments": result.item_assignments,
        }
    else:
        solver = KnapsackSolver(knapsack, model_type=args.solver)
        solution = solver.solve(time_limit=args.timeout)
        result = {
            "solver": args.solver,
            "item_assignments": solution.item_assignments,
            "objective_value": solution.objective_value,
            "elapsed_time": solution.elapsed_time,
            "best_bound": solution.best_bound,
            "optimal": solution.optimal
        }

    if args.out:
        with open(args.out, "w") as f:
            f.write(json.dumps(result))

