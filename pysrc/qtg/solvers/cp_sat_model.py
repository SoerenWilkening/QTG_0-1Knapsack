from qtg.bindings._qtg_bindings import Knapsack
from ortools.sat.python import cp_model

from qtg.solvers.solution import KnapsackSolution


class KnapsackCPModel(cp_model.CpModel):
    def __init__(self, instance: Knapsack):
        super().__init__()
        self._instance = instance

        self._vars = None

    def build(self):
        self._vars = [self.NewBoolVar(name="x") for _ in range(len(self._instance.items))]
        self.Add(
            sum(self._vars[i] * item.cost for i, item in enumerate(self._instance.items))
            <= self._instance.capacity
        )

        self.Maximize(sum(self._vars[i] * item.profit for i, item in enumerate(self._instance.items)))

    def solve(self, time_limit):
        solver = cp_model.CpSolver()
        solver.parameters.max_time_in_seconds = time_limit

        status = solver.Solve(self)

        return KnapsackSolution(
            objective_value=solver.ObjectiveValue(),
            elapsed_time=solver.WallTime(),
            item_assignments=[solver.Value(self._vars[i]) == 1 for i in range(len(self._vars))]
            if status in [cp_model.FEASIBLE, cp_model.OPTIMAL] else None,
            lower_bound=solver.BestObjectiveBound(),
            optimal=status == cp_model.OPTIMAL
        )
