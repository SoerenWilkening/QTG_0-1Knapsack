#ifndef UTILS_HPP
#define UTILS_HPP

#include <vector>
#include <string>

namespace utils {
    /*
     * Struct:          item_t
     * -----------------------
     * Description:     This struct represents an item assignable to a knapsack.
     * Contents:
     *      cost:       Integer cost of the item.
     *      profit:     Integer profit of the item.
     *      included:   Marks whether item is included into knapsack.
     */
    struct cpp_item {
        int profit;
        int cost;

        cpp_item(int profit, int cost) : profit(profit), cost(cost) {}
    };

    /*
     * Struct:              knapsack_t
     * -------------------------------
     * Description:         This struct represents a knapsack.
     * Contents:
     *      size:           Integer size of the knapsack.
     *      capacity:       Integer capacity of the knapsack.
     *      remain_cost:    Integer remaining cost of the knapsack.
     *      tot_profit:     Integer total profit of the knapsack.
     *      items:          Array of assigned items.
     *      name:           Instance name.
     */
    struct cpp_knapsack {
        int size;
        int capacity;
        std::vector<cpp_item> items;
        std::string name;
    };
}

#endif //UTILS_HPP