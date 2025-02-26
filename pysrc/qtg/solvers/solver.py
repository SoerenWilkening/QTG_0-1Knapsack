from qtg.bindings._qtg_bindings import Knapsack

from .cp_sat_model import KnapsackCPModel
from .greedy import GreedyKnapsack
from .ip_model import KnapsackIPModel


class KnapsackSolver:
    def __init__(self, instance: Knapsack, model_type="ip"):
        self.instance = instance
        self.model_type = model_type

        assert model_type in ["ip", "cp-sat", "greedy"]

    def solve(self, time_limit=60, bound=None):
        greedy = GreedyKnapsack(self.instance)
        greedy_solution = greedy.solve(timelimit=time_limit)

        if self.model_type == "greedy":
            return greedy_solution

        time_limit = time_limit - greedy_solution.elapsed_time

        if self.model_type == "ip":
            model = KnapsackIPModel("knapsack", instance=self.instance)
            model.build()
            model.add_solution(greedy_solution)

            if bound is not None:
                print("Setting bound to", bound)
                model.Params.Cutoff = bound
                model.Params.MIPFocus = 1

            return model.solve(time_limit=time_limit)
        elif self.model_type == "cp-sat":
            model = KnapsackCPModel(instance=self.instance)
            model.build()
            model.add_solution(greedy_solution)

            if bound is not None:
                model.set_bound(bound)

            return model.solve(time_limit=time_limit)

        raise NotImplementedError(f"Model type {self.model_type} not implemented")
