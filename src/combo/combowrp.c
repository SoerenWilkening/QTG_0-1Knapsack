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

void *map;

num_t combo_wrap(const knapsack_t *k, bit_t first_item, num_t capacity, bool_t def, bool_t relx) {
    if (map == NULL)
        map = map_create();

    char key[256];
    snprintf(key, sizeof(key), "%s_%"PRIu64"_%"PRIu64, k->name, (uint64_t) (k->size - first_item), (uint64_t) capacity);

    num_t opt_sol = map_get(map, key);

    if (opt_sol >= 0) {
        return opt_sol;
    }

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

    printf("%"PRIu64"", map_size(map));
    map_put(map, key, opt_sol);
    printf("to %"PRIu64"\n", map_size(map));
    // free(k_copy);
    return opt_sol;
}

    /**
     * num_t
combo_data(const knapsack_t *k, bit_t first_item, num_t capacity, bool_t def, \
           bool_t relx, bool_t read) {

    num_t opt_sol;
    FILE *stream;
char instancename[256];
char pathname[256];
char filename[256];
char line[128];
snprintf(instancename, sizeof(instancename), "instances%c%s", path_sep(), \
             k->name);
snprintf(pathname, sizeof(pathname), "%s%ccombo", instancename, path_sep());
snprintf(filename, sizeof(filename), "%s%csize=%"PRIu64"_capacity=" \
             "%"PRIu64".txt", pathname, path_sep(), \
             (uint64_t) (k->size - first_item), (uint64_t) capacity);
if (file_exists(filename) && read) {
stream = fopen(filename, "r");
fscanf(stream, "%ld", &opt_sol);
fclose(stream);
return opt_sol;
}

opt_sol = combo_wrap(k, first_item, capacity, def, relx, TRUE, 0);

if (!file_exists(filename)) {
create_dir(instancename);
create_dir(pathname);
FILE *file = fopen(filename, "w");
fprintf(file, "%"PRIu64"\n", (uint64_t) opt_sol);
fclose(file);
}
return opt_sol;
}
    **/