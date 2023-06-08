//
// Created by Sören Wilkening on 10.05.23.
//

/* 
 * =============================================================================
 *                                  includes
 * =============================================================================
 */

#include "combo_wrap.h"

/* 
 * =============================================================================
 *                                   macros
 * =============================================================================
 */

#define TRUE                    1
#define FALSE                   0

num_t cpp_combo_wrap(knapsack_t* k,
                    bit_t first_item,
                    bool_t define_sol,
                    bool_t relx,
                    bool_t exec_combo) {

    num_t opt_sol;
    num_t trivial_profit;
    /* check whether instance is trivial */
    if (is_trivial(k, &trivial_profit)) {
        return trivial_profit;
    }

    /* Set lower and upper bound */
    num_t lbi = int_greedy(k, RATIO);
    //num_t ubi = frac_greedy(k, RATIO);

    /* conversation of item_t structure to combo's item structure */
    item items[k->size];
    for (size_t i = 0; i < k->size; ++i) {
        items[i].p = k->items[i].profit;
        items[i].w = k->items[i].cost;
        items[i].x = k->items[i].included;
    }
    item* f = items;
    item* l = items + k->size - 1 - first_item; 

    if(exec_combo) {
        opt_sol = combo(f, l, k->capacity, lbi, 0, define_sol, relx); //opt_sol is the solution from the algorithm
    }
    return opt_sol;
}
