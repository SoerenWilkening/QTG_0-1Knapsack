import sys
import os

sys.path.append(os.path.abspath('../../pysrc'))

from qtg.utils import save_instance, load_instance
from qtg.bindings import jooken_generate, execute_combo

if __name__ == "__main__":
    knapsack = load_instance("test_instances/large1.knap")

    print("Running")
    result = execute_combo(knapsack, True, False)
    print(result.objective_value)
    print(result.elapsed_cycles)


