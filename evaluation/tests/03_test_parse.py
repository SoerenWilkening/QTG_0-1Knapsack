import sys
import os

from qtg.utils import load_instance
from qtg.bindings import execute_q_max_search, execute_combo

if __name__ == "__main__":
    knapsack = load_instance("test_instances/100.knap")
    print("Running")
    combo_result = execute_combo(knapsack)

    for seed in range(1):
        qtg_result = execute_q_max_search(knapsack,
                                          len(knapsack.items) // 4,
                                          1,
                                          seed)

        obj_check = sum(int(assign) * item.profit for assign, item in zip(qtg_result.item_assignments, knapsack.items))
        cost_check = sum(int(assign) * item.cost for assign, item in zip(qtg_result.item_assignments, knapsack.items))

        assert cost_check <= knapsack.capacity, f"Cost exceeds capacity {cost_check} > {knapsack.capacity}"
        assert qtg_result.objective_value == obj_check, f"Solution or obj function wrong {obj_check} != {qtg_result.objective_value}"

        print(combo_result.objective_value, qtg_result.objective_value)

        print((combo_result.objective_value - qtg_result.objective_value) / combo_result.objective_value)