//
// Created by Sören Wilkening on 10.05.23.
//

/* 
 * =============================================================================
 *                            includes
 * =============================================================================
 */

#if defined(_WIN32) || defined(_WIN64)
    #include "..\include\combowrp.h"
#else
    #include "../include/combowrp.h"
#endif

/* 
 * =============================================================================
 *                            macros
 * =============================================================================
 */

#define TRUE                    1
#define FALSE                   0

/* 
 * =============================================================================
 *                            Combo wrapper
 * =============================================================================
 */

num_t
combo_wrap(const knapsack_t *k, bit_t first_item, num_t capacity, bool_t def, \
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

    item *f;
    item *l;

    /* Set lower and upper bound */
    // num_t lbi = int_greedy(&k_new, RATIO);
    // num_t ubi = frac_greedy(k, RATIO);

    /* conversation of item_t structure to Combo's item structure */
    item items[k->size - first_item];
    for (size_t i = 0; i < k_copy.size; ++i) {
        items[i].p = k_copy.items[i].profit;
        items[i].w = k_copy.items[i].cost;
        items[i].x = k_copy.items[i].included;
    }
    f = items;
    l = items + k_copy.size - 1;
    /* either start combo or return 0 */
    if (exe_combo) {
        char pathname[256];
        char filename_ndef[256];
        snprintf(pathname, sizeof(pathname), "instances%c%s%ccombo%c", path_sep(), \
             k->name, path_sep(), path_sep());
        create_dir(pathname);
        if (def == 0) {
            snprintf(filename_ndef, sizeof(filename_ndef), \
             "%scombo_counts_def=false.csv", pathname);
        } else {
            snprintf(filename_ndef, sizeof(filename_ndef), \
             "%scombo_counts_def=true.csv", pathname);
        }
        uint64_t start = rdtsc();
        opt_sol = combo(f, l, k_copy.capacity, 0, 0, def, relx);
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

/* 
 * =============================================================================
 *                            Combo data
 * =============================================================================
 */

num_t
combo_data(const knapsack_t *k, bit_t first_item, num_t capacity, bool_t def, \
           bool_t relx, bool_t read) {

    num_t opt_sol;

    /* Check whether instance's solution was already calculated by Combo */
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

    /* save the result */
    if (!file_exists(filename)) {
        create_dir(instancename);
        create_dir(pathname);
        FILE *file = fopen(filename, "w");
        fprintf(file, "%"PRIu64"\n", (uint64_t) opt_sol);
        fclose(file);
    }
    return opt_sol;
}

/* 
 * =============================================================================
 *                            measure Combo
 * =============================================================================
 */

void
measure_combo(const knapsack_t *k) {
    uint64_t mem_count[4];
    uint64_t cycle_count[4];
    char *executable = "cmbcount";
    size_t argc = 4; /* number of arguments */
    char *argv[argc]; /* initialize argument array */
    argv[0] = k->name; /* set name of instance */
    argv[1] = "0";
    argv[2] = "0"; /* relx will always be false */
    argv[3] = "0";
    for (bool_t def = 0; def <= 1; ++def) {
        argv[1] = (def) ? "1" : "0";
        for (bool_t exe_combo = 0; exe_combo <= 1; ++exe_combo) {
            argv[3] = (exe_combo) ? "1" : "0";
            rdmd(executable, argc, argv, mem_count + 2 * def + exe_combo, \
                 cycle_count + 2 * def + exe_combo);
        }
    }
    char pathname[256];
    char filename_def[256];
    char filename_ndef[256];
    snprintf(pathname, sizeof(pathname), "instances%c%s%ccombo%c", path_sep(), \
             k->name, path_sep(), path_sep());
    create_dir(pathname);
    snprintf(filename_ndef, sizeof(filename_ndef), \
             "%scombo_counts_def=false.csv", pathname);
    FILE *file_ndef = fopen(filename_ndef, "a");
    fprintf(file_ndef, "%"PRIu64" ", *(mem_count + 1) - *mem_count);
    fclose(file_ndef);

    snprintf(filename_def, sizeof(filename_def), \
             "%scombo_counts_def=true.csv", pathname);
    FILE *file_def = fopen(filename_def, "a");
    fprintf(file_def, "%"PRIu64" ", *(mem_count + 3) - \
            *(mem_count + 2));
    fclose(file_def);

    combo_wrap(k, 0, k->capacity, 0, 0, 1, 1);
    combo_wrap(k, 0, k->capacity, 1, 0, 1, 1);
}
