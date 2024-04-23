import time

import numpy as np
from qtg.bindings._qtg_bindings import Knapsack

from qtg.solvers.solution import KnapsackSolution


class GreedyKnapsack:

    def __init__(self, instance: Knapsack):
        self.instance = instance

    def solve(self, timelimit: int):
        start = time.time()

        def empty_solution():
            return KnapsackSolution(item_assignments=None,
                                    objective_value=0.0,
                                    elapsed_time=start - time.time(),
                                    best_bound=0.0,
                                    optimal=False)

        items = list(self.instance.items)
        sorted_indices = list(sorted(range(len(items)),
                                     key=lambda x: items[x].profit / items[x].cost if items[x].cost > 0 else np.inf,
                                     reverse=True))

        if time.time() - start > timelimit:
            return empty_solution()

        total_profit = 0.0
        remaining_capacity = self.instance.capacity
        item_assignments = [False] * len(items)

        # Looping through all Items
        for i in sorted_indices:
            item = items[i]
            if i % 100 == 0 and start - time.time() > timelimit:
                if time.time() - start > timelimit:
                    return empty_solution()

            if item.cost <= remaining_capacity:
                remaining_capacity -= item.cost
                total_profit += item.profit
                item_assignments[i] = True

        return KnapsackSolution(item_assignments=item_assignments,
                                objective_value=total_profit,
                                elapsed_time=time.time() - start,
                                best_bound=0,
                                optimal=False)
