from pathlib import Path

from qtg.solvers import KnapsackSolver
from qtg.utils import load_instance
from qtg.bindings import execute_combo

import numpy as np


def test_combo():
    path = Path(__file__).parent / "data" / "example.knap"
    instance = load_instance(path)
    solution = execute_combo(instance, True, False)

    assert solution.objective_value == 7500303027
    assert all(a == b for a, b in zip(
        solution.item_assignments,
        [False, False, False, False, False, False,
         False, False, False, False, False, False,
         False, False, False, False, False, False,
         False, False, False, False, False, False,
         False, False, True, False, True, False, False,
         True, False, False, True, True, True, True,
         True, True, True, True, True, True, True,
         True, True, True, True, True]
    ))


def test_ip_model():
    path = Path(__file__).parent / "data" / "example.knap"
    instance = load_instance(path)

    solver = KnapsackSolver(instance=instance, model_type="ip")
    solution = solver.solve()

    combo_solution = execute_combo(instance, True, False)

    assert solution.objective_value == combo_solution.objective_value
    assert all([a == b for a, b in zip(solution.item_assignments, combo_solution.item_assignments)])
    assert solution.elapsed_time > 0
    assert solution.lower_bound == combo_solution.objective_value
    assert solution.optimal


def test_cp_sat_model():
    path = Path(__file__).parent / "data" / "example.knap"
    instance = load_instance(path)

    solver = KnapsackSolver(instance=instance, model_type="cp-sat")
    solution = solver.solve()

    combo_solution = execute_combo(instance, True, False)

    assert solution.objective_value == combo_solution.objective_value
    assert all([a == b for a, b in zip(solution.item_assignments, combo_solution.item_assignments)])
    assert solution.elapsed_time > 0
    assert solution.lower_bound == combo_solution.objective_value
    assert solution.optimal


def test_timeout_ip():
    path = Path(__file__).parent / "data" / "large.knap"
    instance = load_instance(path)

    solver = KnapsackSolver(instance=instance, model_type="ip")
    solution = solver.solve(time_limit=10)

    assert np.isclose(solution.elapsed_time, 10, rtol=1e-1), f"Elapsed time: {solution.elapsed_time}"
    assert not solution.optimal


def test_timeout_cp_sat():
    path = Path(__file__).parent / "data" / "large.knap"
    instance = load_instance(path)

    solver = KnapsackSolver(instance=instance, model_type="cp-sat")
    solution = solver.solve(time_limit=10)

    assert np.isclose(solution.elapsed_time, 10, rtol=1e-1), f"Elapsed time: {solution.elapsed_time}"
    assert not solution.optimal
