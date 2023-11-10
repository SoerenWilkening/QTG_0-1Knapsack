#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <unistd.h>

#include "combo/combo.h"

namespace utils {

    using capacity_type = long long;

    struct cpp_item {
        capacity_type profit;
        capacity_type cost;

        cpp_item(capacity_type profit, capacity_type cost) : profit(profit), cost(cost) {}

        [[nodiscard]] inline combo_item to_combo_item() const {
            return combo_item{0, profit, cost, false};
        }
    };

    struct combo_measurement {
        capacity_type objective_value;
        std::vector<bool> item_assignments;
        uint64_t elapsed_cycles;
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

    inline void process_mem_usage(double &vm_usage, double &resident_set) {
        vm_usage = 0.0;
        resident_set = 0.0;

        // the two fields we want
        unsigned long vsize;
        long rss;
        {
            std::string ignore;
            std::ifstream ifs("/proc/self/stat", std::ios_base::in);
            ifs >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
                >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore >> ignore
                >> ignore >> ignore >> vsize >> rss;
        }

        long page_size_kb = sysconf(_SC_PAGE_SIZE) / 1024; // in case x86-64 is configured to use 2MB pages
        vm_usage = vsize / 1024.0;
        resident_set = rss * page_size_kb;
    }
}

#endif //UTILS_HPP