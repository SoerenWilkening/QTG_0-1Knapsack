//
// Created by Sören Wilkening on 10.05.23.
//

/* 
 * =============================================================================
 *                                  includes
 * =============================================================================
 */

#include "combo_wrap.h"

#define TRUE                    1
#define FALSE                   0

num_t cpp_combo_wrap(knapsack_t* k,
                    bit_t first_item,
                    bool_t define_sol,
                    bool_t exec_combo) {

    num_t opt_sol;
    
    /* we fist check if the instance is trivial. If it is, combo runs into problems */
    if (is_trivial(k)) {
        return 0;
    }

    //Can I simply define the initial lower bound as zero?
    num_t lbi = int_greedy(k, RATIO);
    bool_t def = define_sol;
    bool_t relx = FALSE;
    num_t ubi = frac_greedy(k, RATIO);

    /* type punning: item_t to item */
    item* f = (item*)(k->items);
    item* l = (item*)(k->items + k->size - 1);

    if(exec_combo) {
        opt_sol = combo(f, l, k->capacity, lbi, ubi, def, relx); //opt_sol is the solution from the algorithm

    }
    return opt_sol;
}