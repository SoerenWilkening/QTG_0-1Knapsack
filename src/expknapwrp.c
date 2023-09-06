//
// Created by Sören Wilkening on 05.09.23.
//

#if defined(_WIN32) || defined(_WIN64)
#include "..\include\expknapwrp.h"
#else
#include "../include/expknapwrp.h"
#endif

num_t
expkap_wrap(const knapsack_t *k, bit_t first_item, num_t capacity, bool_t def, \
           bool_t relx, bool_t exe_combo, bool_t save) {

    num_t opt_sol;

    // knapsack_t* k_copy = create_empty_knapsack(k->size - first_item, capacity);
    // memcpy(k_copy->items, k->items, k_copy->size * sizeof(item_t));
    // strcpy(k_copy->name, k->name);

    knapsack_t k_copy = {.size = k->size - first_item, .capacity = capacity, \
                         .remain_cost = capacity, .tot_profit = 0, \
                         .items = k->items + first_item, .name = k->name};
    /* check whether instance is trivial */
    if (is_trivial(&k_copy, &opt_sol)) {
        return opt_sol;
    }

    exitem_2 *f;
    exitem_2 *l;

    /* Set lower and upper bound */
    // num_t lbi = int_greedy(&k_new, RATIO);
    // num_t ubi = frac_greedy(k, RATIO);

    /* conversation of item_t structure to Combo's item structure */
    exitem_2 items[k->size - first_item];
    for (size_t i = 0; i < k_copy.size; ++i) {
        items[i].p = k_copy.items[i].profit;
        items[i].w = k_copy.items[i].cost;
    }
    f = items;
    l = items + k_copy.size - 1;
    /* either start combo or return 0 */
    if (exe_combo) {
        char pathname[256];
        char filename_ndef[256];
        snprintf(pathname, sizeof(pathname), "instances%c%s%cexpknap%c", path_sep(), \
             k->name, path_sep(), path_sep());
        create_dir(pathname);
        if (def == 0) {
            snprintf(filename_ndef, sizeof(filename_ndef), \
             "%sexpknap_counts_def=false.csv", pathname);
        } else {
            snprintf(filename_ndef, sizeof(filename_ndef), \
             "%sexpknap_counts_def=true.csv", pathname);
        }
        uint64_t start = rdtsc();
        opt_sol = expknap(f, l, k->capacity);
        uint64_t end = rdtsc();
        if (save == 1) {
            FILE *file_def = fopen(filename_ndef, "a");
            fprintf(file_def, "%llu\n", end - start);
            fclose(file_def);
        }
    } else {
        opt_sol = 0;
    }
    // free(k_copy);
    return opt_sol;
}
