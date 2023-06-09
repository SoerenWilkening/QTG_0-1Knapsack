//
// Created by Sören Wilkening on 10.05.23.
//

/* 
 * =============================================================================
 *                                  includes
 * =============================================================================
 */

#include "combowrp.h"

/* 
 * =============================================================================
 *                                   macros
 * =============================================================================
 */

#define TRUE                    1
#define FALSE                   0

/* 
 * =============================================================================
 *                                 combo wrap
 * =============================================================================
 */

num_t
combo_wrap(knapsack_t* k, bit_t first_item, num_t capacity, \
               bool_t define_sol, bool_t relx, bool_t exec_combo, \
               uint64_t* timerecord) {

    //fs::path file = name + "/combo/size=" + std::to_string(k->size - first_item) + "_capacity=" + std::to_string(capacity) + ".txt";

    num_t opt_sol;
    num_t trivial_profit;

    knapsack_t k_new = { .size = k->size - first_item, .capacity = capacity, \
                         .remain_cost = capacity, .tot_profit = 0, \
                         .items = k->items + first_item, .name = k->name};
    /* check whether instance is trivial */
    if (is_trivial(&k_new, &trivial_profit)) {
        return trivial_profit;
    }

    /* Set lower and upper bound */
    num_t lbi = int_greedy(k, RATIO);
    //num_t ubi = frac_greedy(k, RATIO);

    /* conversation of item_t structure to combo's item structure */
    item items[k->size - first_item];
    for (size_t i = 0; i < k->size - first_item; ++i) {
        items[i].p = k_new.items[i].profit;
        items[i].w = k_new.items[i].cost;
        items[i].x = k_new.items[i].included;
    }
    item* f = items;
    item* l = items + k->size - 1 - first_item; 

    if(exec_combo) {
        opt_sol = combo(f, l, k_new.capacity, lbi, 0, define_sol, relx); //opt_sol is the solution from the algorithm
    }
    return opt_sol;
}
