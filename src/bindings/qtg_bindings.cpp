#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <pybind11/stl.h>

#include "generator/jooken_generator.h"
#include "common/utils.hpp"
#include "combo/combo.h"
#include "common/syslinks.h"
#include "simulation/simulate.h"

namespace py = pybind11;

PYBIND11_MAKE_OPAQUE(std::vector<utils::cpp_item>);

utils::combo_measurement execute_combo(const utils::cpp_knapsack &instance, bool write_solution, bool relx) {
    /* check whether instance is trivial */
    if (instance.cost_sum() <= instance.capacity) {
        // Full knapsack
        return utils::combo_measurement{
                instance.profit_sum(),
                std::vector<bool>(instance.items.size(), true),
                0
        };
    } else if (instance.min_cost() > instance.capacity) {
        // Empty knapsack
        return utils::combo_measurement{
                0,
                std::vector<bool>(instance.items.size(), false),
                0
        };
    }

    auto combo_items = new combo_item[instance.items.size()];

    for (int i = 0; i < instance.items.size(); ++i) {
        combo_items[i] = instance.items[i].to_combo_item();
        combo_items[i].idx = i;

        assert(combo_items[i].p == instance.items[i].profit);
        assert(combo_items[i].w == instance.items[i].cost);
        assert(combo_items[i].x == false);
    }

    auto first_item = combo_items;
    auto last_item = &combo_items[instance.items.size() - 1];

    /* Set lower and upper bound */
    // capacity_type lbi = int_greedy(&k_new, RATIO);
    // capacity_type ubi = frac_greedy(instance, RATIO);

    /* either start combo or return 0 */
    uint64_t start = rdtsc();
    auto opt_sol = combo(first_item, last_item, instance.capacity, 0, 0, write_solution, relx);
    uint64_t end = rdtsc();

    std::vector<bool> solution = std::vector<bool>(instance.items.size(), false);

    if (write_solution) {
        for (int j = 0; j < instance.items.size(); ++j) {
            // Combo modifies the items, so we have to find the original index
            auto i = combo_items[j].idx;
            assert(combo_items[j].p == instance.items[i].profit);
            assert(combo_items[j].w == instance.items[i].cost);

            solution[i] = combo_items[j].x;
        }
    }

    free(combo_items);

    return utils::combo_measurement{
            opt_sol,
            solution,
            end - start
    };
}

/*
 * Function:        q_max_search
 * -----------------------------
 * Description:     This funtion simulates the application of the QMaxSearch
 *                  algorithm to a given knapsack instance. The routine starts
 *                  with sorting the input knapsack in-place and then calculates
 *                  its integer greedy solution, obtaining the first threshold.
 *                  It also calculates its optimal total profit via Combo.
 *                  Then, the application of the QTG, and subsequently of the
 *                  QSearch is simulated as long as an improved state (path) is
 *                  found. After each application of QSearch, the found state's
 *                  total profit serves as an updated threshold, determining
 *                  the parameter for the next application of QSearch. As soon
 *                  as no improved state is found by QSearch, the entire routine
 *                  terminates and returns the last found state.
 * Parameters:
 *      instance:   Knapsack instance that should be considered.
 *      bias:       Bias towards certain branch.
 *      maxiter:    Specifies how often the QTG should be applied in total per
 *                  QSearch subroutine.
 *      seed:       Seed for GSL's random number generator.
 * Returns:         Index of the state obtained by the simulated measurement.
 * Side Effect:     Allocates dynamically; pointer should eventually be freed.
 */
utils::qtg_measurement execute_q_max_search(const utils::cpp_knapsack &instance,
                                            size_t bias, size_t maxiter, size_t seed) {


    // This is the default parameter and does not need to be set from within Python
    branch_t method = COMPARE;

    gsl_rng *rng;
    gsl_rng_env_setup();
    rng = gsl_rng_alloc(gsl_rng_default);

    gsl_rng_set(rng, seed);

    // We have to convert the knapsack instance into the used knapsack_t type.
    auto converted_knapsack = create_empty_knapsack((int) instance.size, instance.capacity);

    for (int i = 0; i < instance.items.size(); i++) {
        converted_knapsack->items[i].idx = i;
        converted_knapsack->items[i].cost = instance.items[i].cost;
        converted_knapsack->items[i].profit = instance.items[i].profit;
    }

    size_t rounds; /* counter of AA rounds within QSearch; will be updated */
    size_t iter; /* counter of AA calls within QSearch; will be updated */
    size_t num_states; /* number of states; will be updated */

    path_t *cur_sol; /* current solution; will be updated */
    bit_t profit_qubits; /* size of the profit register */
    /*
     * declare cycle and gate counter for the QTG with and without decomposing
     * arising Toffoli gates
     */
    count_t qtg_cycles, qtg_gates, qtg_cycles_decomp, qtg_gates_decomp;
    num_t exact; /* exact optimal profit for the knapsack instance */

    node_t *cur_nodes; /* set of states after applying QTG and filtering */
    path_t *cur_path; /* current result of applying QSearch */

    /*
     * The knapsack is sorted by ration. Then the integer greedy method is
     * applied to yield the initial current solution and the initial threshold.
     */
    sort_knapsack(converted_knapsack, RATIO);
    apply_int_greedy(converted_knapsack);
    cur_sol = path_rep(converted_knapsack);
    remove_all_items(converted_knapsack);

    /*
     * A resource counter is initialized. The number of required qubits is given
     * by the total number of qubits on all three registers of the QTG plus the
     * number of ancilla qubits necessary for decomposing the QTG's comparison
     * operations, plus one for saving the AA phase flip. The cycle and gate
     * counts are initialized to zero and will be updated after every call to
     * QSearch.
     */
    profit_qubits = profit_reg_size(converted_knapsack, FGREEDY);
    resource_t res = {
            .qubit_count = qubit_count_qtg(converted_knapsack, FGREEDY, COPPERSMITH, COPYDIRECT, TOFFOLI) + 1,
            .cycle_count = 0,
            .gate_count = 0,
            .cycle_count_decomp = 0,
            .gate_count_decomp = 0
    };
    qtg_cycles = cycle_count_qtg(converted_knapsack, FGREEDY, COPPERSMITH, COPYDIRECT, TOFFOLI, false);
    qtg_gates = gate_count_qtg(converted_knapsack, FGREEDY, COPPERSMITH, COPYDIRECT, TOFFOLI, false);
    qtg_cycles_decomp = cycle_count_qtg(converted_knapsack, FGREEDY, COPPERSMITH, COPYDIRECT, TOFFOLI, true);
    qtg_gates_decomp = gate_count_qtg(converted_knapsack, FGREEDY, COPPERSMITH, COPYDIRECT, TOFFOLI, true);

    /* obtain optimal solution via Combo */
    exact = execute_combo(instance, true, false).objective_value;

    do {
        /*
         * The application of the QTG is simulated. Only states (paths) with
         * total profit above the threshold are stored in cur_nodes.
         */
        cur_nodes = qtg(converted_knapsack, cur_sol->tot_profit, exact, bias, method, cur_sol->vector, &num_states);
        /*
         * QSearch is executed on the nodes created by the QTG. If this yields a
         * better solution, cur_sol (carrying the current threshold) is updated.
         * Otherwise, the routine is interrupted and prior cur_sol is returned.
         */
        cur_path = q_search(cur_nodes, num_states, &rounds, &iter, maxiter, rng);
        if (cur_nodes != nullptr) {
            free_nodes(cur_nodes, num_states);
        }
        /*
         * The cycle is updated after applying QSearch. The QTG is applied at
         * the start of every round plus two times within each application of
         * the Amplitude Amplification Operator.
         */
        res.cycle_count += (rounds + 2 * iter) * qtg_cycles;
        res.cycle_count_decomp += (rounds + 2 * iter) * qtg_cycles_decomp;
        /* adding the cycles for implementing the reflection about |0> */
        res.cycle_count += iter * cycle_count_mc(profit_qubits, TOFFOLI, false);
        res.cycle_count_decomp += iter * cycle_count_mc(profit_qubits, TOFFOLI, true);
        /* adding the cycles for implementing the phase oracle */
        res.cycle_count +=
                iter * std::min(cycle_count_comp(profit_qubits, cur_sol->tot_profit + 1, TOFFOLI, true, false),
                                cycle_count_comp(profit_qubits, cur_sol->tot_profit + 1, TOFFOLI, false, false));
        res.cycle_count_decomp +=
                iter * std::min(cycle_count_comp(profit_qubits, cur_sol->tot_profit + 1, TOFFOLI, true, true),
                                cycle_count_comp(profit_qubits, cur_sol->tot_profit + 1, TOFFOLI, false, true));
        /* updating the gate count according to the same rules */
        res.gate_count += (rounds + 2 * iter) * qtg_gates;
        res.gate_count_decomp += (rounds + 2 * iter) * qtg_gates_decomp;
        res.gate_count += iter * gate_count_mc(profit_qubits, TOFFOLI, false);
        res.gate_count_decomp += iter * gate_count_mc(profit_qubits, TOFFOLI, true);
        res.gate_count += iter * std::min(gate_count_comp(profit_qubits, cur_sol->tot_profit, TOFFOLI, true, true),
                                          gate_count_comp(profit_qubits, cur_sol->tot_profit, TOFFOLI, false, true));
        res.gate_count_decomp +=
                iter * std::min(gate_count_comp(profit_qubits, cur_sol->tot_profit, TOFFOLI, true, true),
                                gate_count_comp(profit_qubits, cur_sol->tot_profit, TOFFOLI, false, true));

        if (cur_path != nullptr) {
            free_path(cur_sol);
            cur_sol = cur_path;
        } else {
            std::vector<bool> solution = std::vector<bool>(instance.items.size(), false);

            for (unsigned int j = 0; j < instance.items.size(); ++j) {
                // The order might have changed
                unsigned int i = converted_knapsack->items[j].idx;
                solution[i] = mpz_tstbit(cur_sol->vector, j);
            }

            auto result = utils::qtg_measurement{
                    .remaining_cost = cur_sol->remain_cost,
                    .objective_value = cur_sol->tot_profit,
                    .item_assignments = solution,
                    .resources = res
            };

            free_path(cur_sol);
            gsl_rng_free(rng);

            return result;
        }
    } while (true);
}


PYBIND11_MODULE(_qtg_bindings, m
) {
    m.def("jooken_generate", &generator::generate);
    m.def("execute_combo", &execute_combo);
    m.def("execute_q_max_search", &execute_q_max_search);

    py::bind_vector<std::vector<utils::cpp_item>>(m, "ItemVector");

    py::class_<resource_t>(m, "QTGResources")
            .def_readwrite("cycle_count", &resource_t::cycle_count)
            .def_readwrite("cycle_count_decomp", &resource_t::cycle_count_decomp)
            .def_readwrite("gate_count", &resource_t::gate_count)
            .def_readwrite("gate_count_decomp", &resource_t::gate_count_decomp)
            .def_readwrite("qubit_count", &resource_t::qubit_count);

    py::class_<utils::qtg_measurement>(m, "QTGMeasurement")
            .def_readwrite("remaining_cost", &utils::qtg_measurement::remaining_cost)
            .def_readwrite("objective_value", &utils::qtg_measurement::objective_value)
            .def_readwrite("item_assignments", &utils::qtg_measurement::item_assignments)
            .def_readwrite("resources", &utils::qtg_measurement::resources);

    py::class_<utils::cpp_knapsack>(m, "Knapsack")
            .def(py::init<utils::capacity_type, utils::capacity_type, std::vector<utils::cpp_item>, std::string>())
            .def_readwrite("size", &utils::cpp_knapsack::size)
            .def_readwrite("capacity", &utils::cpp_knapsack::capacity)
            .def_readwrite("items", &utils::cpp_knapsack::items)
            .def_readwrite("name", &utils::cpp_knapsack::name);

    py::class_<utils::cpp_item>(m, "Item")
            .def(py::init<utils::capacity_type, utils::capacity_type>())
            .def_readwrite("profit", &utils::cpp_item::profit)
            .def_readwrite("cost", &utils::cpp_item::cost);


    py::class_<utils::combo_measurement>(m, "ComboMeasurement")
            .def_readwrite("objective_value", &utils::combo_measurement::objective_value)
            .def_readwrite("item_assignments", &utils::combo_measurement::item_assignments)
            .def_readwrite("elapsed_cycles", &utils::combo_measurement::elapsed_cycles);

}