from pathlib import Path

from qtg.solvers import KnapsackSolver
from qtg.utils import load_instance
from qtg.bindings import execute_combo, execute_expknap, execute_qbnb, QBnBAlgorithm, execute_ctg, execute_q_max_search

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
    n_iterations = 10
    solution = execute_ctg(instance, instance.size / 4, 10, n_iterations, 12345)

    print(solution.objective_values)
    print(solution.elapsed_cycles)
    print(solution.total_iterations)
    print(solution.qtg_estimate_cycles)
    print(solution.qtg_cycles)
    assert solution.objective_values == [7500330190, 7500330190, 7500330190, 7500330190, 7500330190, 7500330190, 7500330190, 7500330190, 7500330190, 7500330190]
    assert solution.elapsed_cycles[-1] > 0
    assert solution.total_iterations == [30, 32, 22, 24, 22, 22, 22, 32, 20, 24]
    assert solution.qtg_estimate_cycles == [356694, 380473, 261575, 285355, 261575, 261575, 261575, 380473, 237796, 285355]
    assert solution.qtg_cycles > 0
    assert len(solution.objective_values) == n_iterations


def test_qtg():
    path = Path(__file__).parent / "data" / "200.knap"
    instance = load_instance(path)
    n_iterations = 2

    for _ in range(n_iterations):
        qtg_result = execute_q_max_search(instance, instance.size / 4, 10, 1)

        obj_check = sum(int(assign) * item.profit for assign, item in zip(qtg_result.item_assignments, instance.items))
        cost_check = sum(int(assign) * item.cost for assign, item in zip(qtg_result.item_assignments, instance.items))

        assert cost_check <= instance.capacity
        assert qtg_result.objective_value == obj_check

        assert qtg_result.resources.qubit_count == 440
        assert qtg_result.resources.cycle_count == 238638
        assert qtg_result.resources.gate_count == 1241122
        assert qtg_result.resources.cycle_count_decomp == 239438
        assert qtg_result.resources.gate_count_decomp == 2341210
