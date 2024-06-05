#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "combo/combo.h"
#include "expknap/expknap.h"
#include "knapsack.h"
#include "simulation/simulate.h"

namespace utils {

    using capacity_type = long long;

    struct qtg_measurement {
        capacity_type remaining_cost;
        capacity_type objective_value;
        std::vector<bool> item_assignments;
        resource_t resources;
    };

    struct cpp_item {
        capacity_type profit;
        capacity_type cost;

        cpp_item(capacity_type profit, capacity_type cost) : profit(profit), cost(cost) {}

        [[nodiscard]] inline combo_item to_combo_item() const {
            return combo_item{0, profit, cost, false};
        }

        [[nodiscard]] inline exitem_2 to_expknap_item() const {
            return exitem_2{0,profit, cost, false};
        }
    };

    struct pissinger_measurement {
        capacity_type objective_value;
        std::vector<bool> item_assignments;
        uint64_t elapsed_cycles;
    };

    struct qbnb_measurement {
        capacity_type objective_value;
        uint64_t elapsed_cycles_lb;
    };

    struct ctg_measurement {
        double bias;
        long qtg_cycles;
        std::vector<uint64_t> elapsed_cycles;
        std::vector<int> total_iterations;
        std::vector<uint64_t> qtg_estimate_cycles;
        std::vector<capacity_type> objective_values;
    };

    struct cpp_knapsack {
        capacity_type size;
        capacity_type capacity;
        std::vector<cpp_item> items;
        std::string name;

        [[nodiscard]] inline capacity_type min_cost() const {
            capacity_type result = items[0].cost;
            for (auto &item: items) {
                result = std::min(item.cost, result);
            }
            return result;
        }

        [[nodiscard]] inline capacity_type cost_sum() const {
            capacity_type result = 0;
            for (auto &item: items) {
                result += item.cost;
            }
            return result;
        }

        [[nodiscard]] inline capacity_type profit_sum() const {
            capacity_type result = 0;
            for (auto &item: items) {
                result += item.profit;
            }
            return result;
        }
    };
}

#endif //UTILS_HPP