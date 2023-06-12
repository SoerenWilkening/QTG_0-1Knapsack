//
// Created by Sören Wilkening on 10.05.23.
//

/* 
 * =============================================================================
 *                            includes
 * =============================================================================
 */

#include "combowrp.h"

/* 
 * =============================================================================
 *                            macros
 * =============================================================================
 */

#define TRUE                    1
#define FALSE                   0

/* 
 * =============================================================================
 *                            combo wrap
 * =============================================================================
 */

num_t
combo_wrap(const knapsack_t* k, bit_t first_item, num_t capacity, \
           uint64_t* timer, bool_t def, bool_t relx, bool_t read, \
           bool_t exec_combo) {

    num_t opt_sol;

    knapsack_t k_new = { .size = k->size - first_item, .capacity = capacity, \
                         .remain_cost = capacity, .tot_profit = 0, \
                         .items = k->items + first_item, .name = k->name};
    /* check whether instance is trivial */
    if (is_trivial(&k_new, &opt_sol)) {
        return opt_sol;
    }
    printf("Trivial check passed!\n");
    /* Check whether instance's solution was already calculated by combo */
    char pathname[256];
    char filename[256];
    snprintf(pathname, sizeof(pathname), "%s%ccombo", k->name, path_sep());
    snprintf(filename, sizeof(filename), "%s%csize=%"PRIu64"_capacity=" \
             "%"PRIu64".txt", pathname, path_sep(), \
             (uint64_t)(k->size - first_item), (uint64_t)capacity);
    printf("%s\n", filename);
    if (file_exists(filename) && read) {
        printf("File exists!\n");
        FILE* stream = fopen(filename, "r");
        printf("Open file worked!\n");
        char line[128];
        opt_sol = atoi(fgets(line, sizeof(line), stream));
        printf("Read file worked!\n");
        fclose(stream);
        return opt_sol;
    }
    printf("Passed reading!\n");

    /* Set lower and upper bound */
    // num_t lbi = int_greedy(&k_new, RATIO);
    // num_t ubi = frac_greedy(k, RATIO);

    /* conversation of item_t structure to combo's item structure */
    item items[k->size - first_item];
    for (size_t i = 0; i < k->size - first_item; ++i) {
        items[i].p = k_new.items[i].profit;
        items[i].w = k_new.items[i].cost;
        items[i].x = k_new.items[i].included;
    }
    item* f = items;
    item* l = items + k->size - 1 - first_item; 
    printf("Passed item translation!\n");
    if(exec_combo) {
        /* start combo and couting elapsed cycles*/
        uint64_t start = rdtsc();
        printf("Passed setting start!\n");
        opt_sol = combo(f, l, k_new.capacity, 0, 0, def, relx);
        printf("Passed executing combo!\n");
        /* stop the count */
        uint64_t end = rdtsc();
        printf("Passed setting end!\n");
        /* safe elapsed cycles in timer */
        *timer = end - start;
        printf("Passed executing and measuring Combo!\n");
        /* if combo is slower than ca. 0.0003 seconds, the result is saved */
        if (((double) *timer / (2.6 * pow(10, 9)) > .0003) \
            && !file_exists(filename)) {
            printf("Passed timing check!\n");
            create_dir(k->name);
            if (create_dir(pathname)) {
                printf("Passed creating directory!\n");
            } else {
                printf("Creating directory did not work!\n");
            }
            FILE* file = fopen(filename, "w");
            printf("Passed opening file for writing!\n");
            fprintf(file, "%"PRIu64"\n", (uint64_t)opt_sol);
            printf("Passed writing in file!\n");
            fclose(file);
            printf("Passed closing file!\n");
        }
    }
    return opt_sol;
}
