import argparse
import json

from qtg.bindings import execute_combo
from qtg.utils import load_instance

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--instance", type=str, required=True)
    parser.add_argument("-o", "--out", type=str, required=False)
    args = parser.parse_args()
    knapsack = load_instance(args.instance)

    result = execute_combo(knapsack, True, False)

    if args.out:
        with open(args.out, "w") as f:
            f.write(json.dumps({
                "objective_value": result.objective_value,
                "elapsed_cycles": result.elapsed_cycles,
                "item_assignments": result.item_assignments
            }))

