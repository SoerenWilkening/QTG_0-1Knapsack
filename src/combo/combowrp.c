/*
 * =============================================================================
 *                            includes
 * =============================================================================
 */

#include "combo/combowrp.h"
#include "map/map.h"

/*
 * =============================================================================
 *                            Combo wrapper
 * =============================================================================
 *
 * */


num_t combo_wrap(const knapsack_t *k, bit_t first_item, num_t capacity, bool_t def, bool_t relx) {
    static map value_map;

    if (value_map != NULL && map_size(value_map) > 100000) { // Destroy if too large to prevent oom errors.
        map_destroy(value_map);
        value_map = NULL;
    }

    if (value_map == NULL) {
        value_map = map_create();
    }

    char key[256];
    snprintf(key, sizeof(key), "%s_%"PRIu64"_%"PRIu64, k->name, (uint64_t) (k->size - first_item), (uint64_t) capacity);

    if (map_contains(value_map, key)) {
        return *(long long *) map_get(value_map, key);
    }

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

    long long *val = malloc(sizeof(long long));
    *val = opt_sol;
    map_set(value_map, key, val);

    return opt_sol;
}