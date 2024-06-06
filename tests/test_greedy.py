import os
from pathlib import Path

from qtg.solvers import KnapsackSolver
from qtg.utils import load_instance
from qtg.bindings import execute_combo


def test_greedy_all_smith_miles():
    path = Path(__file__).parent / ".." / "evaluation" / "instances" / "smith_miles"

    for file in path.glob("*.knap"):
        instance = load_instance(file)

        solver = KnapsackSolver(instance=instance, model_type="greedy")
        solution = solver.solve()
