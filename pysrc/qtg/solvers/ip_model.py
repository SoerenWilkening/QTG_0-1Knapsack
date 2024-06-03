from qtg.bindings._qtg_bindings import Knapsack
import gurobipy as gp
from gurobipy import GRB

from qtg.solvers.solution import KnapsackSolution


class KnapsackIPModel(gp.Model):
    def __init__(self, *args, instance: Knapsack, **kwargs):
        super().__init__(*args, **kwargs)
        self._instance = instance

        self._vars = None

    def build(self):
        self._vars = self.addVars(range(len(self._instance.items)), vtype=GRB.BINARY, name="x")
        self.setObjective(gp.quicksum(self._vars[i] * self._instance.items[i].profit for i in self._vars), GRB.MAXIMIZE)
        self.addConstr(gp.quicksum(self._vars[i] * self._instance.items[i].cost for i in self._vars) <=
                       self._instance.capacity)

    def add_solution(self, solution: KnapsackSolution):
        for i, value in enumerate(solution.item_assignments):
            self._vars[i].start = int(value)

    def solve(self, time_limit):
        self.Params.TimeLimit = time_limit
        self.Params.NumericFocus = 3
        self.Params.MIPGap = 0.0
        self.optimize()

        node_count = self.NodeCount

        return KnapsackSolution(
            objective_value=self.objVal,
            elapsed_time=self.Runtime,
            item_assignments=[bool(self._vars[i].x) for i in self._vars] if self.SolCount > 0 else None,
            best_bound=self.objBound,
            optimal=self.status == GRB.OPTIMAL,
            node_count=node_count
        )


