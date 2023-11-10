#include <pybind11/pybind11.h>
#include <pybind11/stl_bind.h>
#include <pybind11/stl.h>

#include "generator/jooken_generator.h"
#include "common/utils.hpp"
#include "combo/combo.h"
#include "common/syslinks.h"

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

PYBIND11_MODULE(_qtg_bindings, m
) {
    m.def("jooken_generate", &generator::generate);
    m.def("execute_combo", &execute_combo);

    py::bind_vector<std::vector<utils::cpp_item>>(m, "ItemVector");

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