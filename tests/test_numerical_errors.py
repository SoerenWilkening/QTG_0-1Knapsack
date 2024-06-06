import pytest
from pathlib import Path

from qtg.solvers import KnapsackSolver
from qtg.utils import load_instance
from qtg.bindings import execute_combo, execute_expknap


@pytest.mark.long
def test_ip_model_large():
    path = Path(__file__).parent / "data" / "650.knap"
    instance = load_instance(path)

    solver = KnapsackSolver(instance=instance, model_type="ip")
    solution = solver.solve(time_limit=900)  # expect a result after ~600 seconds

    combo_solution = execute_combo(instance)

    assert sum(instance.items[i].profit for i in range(len(instance.items))
               if solution.item_assignments[i]) == solution.objective_value
    assert solution.objective_value == combo_solution.objective_value
    assert solution.best_bound == combo_solution.objective_value
    assert len(solution.item_assignments) == len(combo_solution.item_assignments)
    assert solution.elapsed_time > 0
    assert solution.optimal


@pytest.mark.long
def test_expknap_large():
    path = Path(__file__).parent / "data" / "1000.knap"
    instance = load_instance(path)
    solution = execute_expknap(instance, 60)

    assert solution.objective_value == -1


"""
def test_cpsat_large():
    path = Path(__file__).parent / "data" / "2650.knap"
    instance = load_instance(path)

    solver = KnapsackSolver(instance=instance, model_type="cp-sat")
    solution = solver.solve(time_limit=900) # expect a result after ~600 seconds
"""
