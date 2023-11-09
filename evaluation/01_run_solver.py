import argparse
import json
import sys
import os

sys.path.append(os.path.abspath('../pysrc'))

from qtg.bindings import execute_combo
from qtg.utils import load_instance

if __name__ == "__main__":
    parser = argparse.ArgumentParser()
    parser.add_argument("-i", "--instance", type=str, required=True)
    args = parser.parse_args()
    knapsack = load_instance(args.instance)

    result = execute_combo(knapsack, True, False)