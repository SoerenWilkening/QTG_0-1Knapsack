/*
 * =============================================================================
 *                            includes
 * =============================================================================
 */

#include "combo/combowrp.h"

/*
 * =============================================================================
 *                            Combo wrapper
 * =============================================================================
 */

num_t combo_wrap(const knapsack_t *k, bit_t first_item, num_t capacity, bool_t def, bool_t relx) {

    num_t opt_sol;

    knapsack_t k_copy = {.size = k->size - first_item, .capacity = capacity, \
                         .remain_cost = capacity, .tot_profit = 0, \
                         .items = k->items + first_item, .name = k->name};
    /* check whether instance is trivial */
    if (is_trivial(&k_copy, &opt_sol)) {
        return opt_sol;
    }

    combo_item *f;
    combo_item *l;

    /* conversation of item_t structure to Combo's item structure */
    combo_item items[k->size - first_item];
    for (size_t i = 0; i < k_copy.size; ++i) {
        items[i].p = k_copy.items[i].profit;
        items[i].w = k_copy.items[i].cost;
        items[i].x = k_copy.items[i].included;
    }
    f = items;
    l = items + k_copy.size - 1;
    /* either start combo or return 0 */

    opt_sol = combo(f, l, k_copy.capacity, 0, 0, def, relx);

    // free(k_copy);
    return opt_sol;
}
