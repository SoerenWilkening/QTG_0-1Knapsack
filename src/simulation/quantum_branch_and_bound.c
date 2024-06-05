#include "simulation/quantum_branch_and_bound.h"

num_t bnb_greedy(const knapsack_t *k, long cap, int first_item) {
    num_t tot_profit = 0, remaining_cap = cap;

    int j;
    for (j = first_item; j < k->size; ++j) {
        if (k->items[j].cost <= remaining_cap) {
            tot_profit += k->items[j].profit;
            remaining_cap -= k->items[j].cost;
        } else { break; }
    }
    if (j == k->size) return tot_profit;

    double frac_item = (double) k->items[j].profit / k->items[j].cost * remaining_cap;
    return tot_profit + ((num_t) floor(frac_item));
}

long branch_and_bound(knapsack_t *k, int level, long threshold, size_t *count_nodes, long current_objective,
                      long remaining_capacity, int *exec, qbnb_method method, unsigned long max_node_count) {

    if (current_objective == threshold && method == Ambainis) *exec = 0;
    if (level == k->size || exec == 0 || *count_nodes >= max_node_count) return current_objective;

    long left_ub, right_ub;
    left_ub = current_objective + bnb_greedy(k, remaining_capacity, level);
    right_ub = current_objective + k->items[level].profit +
               bnb_greedy(k, remaining_capacity - k->items[level].cost, level + 1);

    long right_val = 0, left_val = 0;
    if (((remaining_capacity - k->items[level].cost) >= 0) && right_ub >= threshold && *exec == 1) {
        (*count_nodes)++;
        right_val = branch_and_bound(k, level + 1, threshold, count_nodes,
                                     current_objective + k->items[level].profit,
                                     remaining_capacity - k->items[level].cost, exec, method,
                                     max_node_count);
    }
    if (left_ub >= threshold && *exec == 1) {
        (*count_nodes)++;
        left_val = branch_and_bound(k, level + 1, threshold, count_nodes,
                                    current_objective,
                                    remaining_capacity, exec, method, max_node_count);
    }
    if (right_val >= threshold) return right_val;
    else if (left_val >= threshold) return left_val;
}