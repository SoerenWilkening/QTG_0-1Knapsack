from pathlib import Path

from qtg.solvers import KnapsackSolver
from qtg.utils import load_instance
from qtg.bindings import execute_combo, execute_expknap, execute_qbnb, QBnBAlgorithm, execute_ctg

import numpy as np

solutions_for_instance = {
    Path(__file__).parent / "data" / "example.knap": [False, False, False, False, False, False,
                                                      False, False, False, False, False, False,
                                                      False, False, False, False, False, False,
                                                      False, False, False, False, False, False,
                                                      False, False, True, False, True, False, False,
                                                      True, False, False, True, True, True, True,
                                                      True, True, True, True, True, True, True,
                                                      True, True, True, True, True]
}


def test_combo():
    path = Path(__file__).parent / "data" / "example.knap"
    instance = load_instance(path)
    solution = execute_combo(instance)

    assert sum(instance.items[i].profit for i in range(len(instance.items))
               if solution.item_assignments[i]) == solution.objective_value
    assert solution.objective_value == 7500303027
    assert all(a == b for a, b in zip(
        solution.item_assignments,
        solutions_for_instance[path]
    ))


def test_greedy():
    path = Path(__file__).parent / "data" / "example.knap"
    instance = load_instance(path)

    solver = KnapsackSolver(instance=instance, model_type="greedy")
    solution = solver.solve()

    combo_solution = execute_combo(instance)

    assert sum(instance.items[i].profit for i in range(len(instance.items))
               if solution.item_assignments[i]) == solution.objective_value
    assert solution.objective_value <= combo_solution.objective_value
    assert solution.elapsed_time > 0
    assert solution.best_bound == 0
    assert solution.optimal == False


def test_expknap():
    path = Path(__file__).parent / "data" / "example.knap"
    instance = load_instance(path)
    solution = execute_expknap(instance, 200)

    assert sum(instance.items[i].profit for i in range(len(instance.items))
               if solution.item_assignments[i]) == solution.objective_value
    assert solution.objective_value == 7500202083


def test_ip_model():
    path = Path(__file__).parent / "data" / "example.knap"
    instance = load_instance(path)

    solver = KnapsackSolver(instance=instance, model_type="ip")
    solution = solver.solve()

    combo_solution = execute_combo(instance)

    assert sum(instance.items[i].profit for i in range(len(instance.items))
               if solution.item_assignments[i]) == solution.objective_value
    assert solution.objective_value == combo_solution.objective_value
    assert all([a == b for a, b in zip(solution.item_assignments, combo_solution.item_assignments)])
    assert solution.elapsed_time > 0
    assert solution.best_bound == combo_solution.objective_value
    assert solution.optimal


def test_cp_sat_model():
    path = Path(__file__).parent / "data" / "example.knap"
    instance = load_instance(path)

    solver = KnapsackSolver(instance=instance, model_type="cp-sat")
    solution = solver.solve()

    combo_solution = execute_combo(instance)

    assert sum(instance.items[i].profit for i in range(len(instance.items))
               if solution.item_assignments[i]) == solution.objective_value
    assert solution.objective_value == combo_solution.objective_value
    assert all([a == b for a, b in zip(solution.item_assignments, combo_solution.item_assignments)])
    assert solution.elapsed_time > 0
    assert solution.best_bound == combo_solution.objective_value
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


def test_qbnb():
    path = Path(__file__).parent / "data" / "650.knap"
    instance = load_instance(str(path))

    for solver in [QBnBAlgorithm.Montanaro, QBnBAlgorithm.Ambainis]:
        solution = execute_qbnb(instance, solver.value, int(1e10))

        assert solution.objective_value > 0
        assert solution.elapsed_cycles_lb > 0


def test_ctg():
    path = Path(__file__).parent / "data" / "650.knap"
    instance = load_instance(path)

    solution = execute_ctg(instance, 10, 10, 12345)
    print(solution)
    print(solution.objective_values)
    print(solution.elapsed_cycles)
    print(solution.total_iterations)
    print(solution.qtg_estimate_cycles)
    print(solution.bias)
    print(solution.qtg_cycles)

