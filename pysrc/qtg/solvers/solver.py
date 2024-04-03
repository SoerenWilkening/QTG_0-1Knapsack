from qtg.bindings._qtg_bindings import Knapsack

from .cp_sat_model import KnapsackCPModel
from .ip_model import KnapsackIPModel


class KnapsackSolver:
    def __init__(self, instance: Knapsack, model_type="ip"):
        self.instance = instance
        self.model_type = model_type

        assert model_type in ["ip", "cp-sat"]

    def solve(self, time_limit=60):
        if self.model_type == "ip":
            model = KnapsackIPModel("knapsack", instance=self.instance)
            model.build()
            return model.solve(time_limit=time_limit)
        elif self.model_type == "cp-sat":
            model = KnapsackCPModel(instance=self.instance)
            model.build()
            return model.solve(time_limit=time_limit)

        raise NotImplementedError(f"Model type {self.model_type} not implemented")
