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
 *                            Combo void mask
 * =============================================================================
 */

void
combo_mask(void* arguments) {
    combo_args_t* args = (combo_args_t*)arguments;
    combo(args->f, args->l, args->c, args->lb, args->ub, args->def, args->relx);
}

/* 
 * =============================================================================
 *                            Combo wrapper
 * =============================================================================
 */

num_t
combo_wrap(const knapsack_t* k, bit_t first_item, num_t capacity, bool_t def, \
           bool_t relx, bool_t read) {

    num_t opt_sol;

    knapsack_t k_new = { .size = k->size - first_item, .capacity = capacity, \
                         .remain_cost = capacity, .tot_profit = 0, \
                         .items = k->items + first_item, .name = k->name};
    /* check whether instance is trivial */
    if (is_trivial(&k_new, &opt_sol)) {
        return opt_sol;
    }
    /* Check whether instance's solution was already calculated by Combo */
    uint64_t start;
    uint64_t end;
    FILE* stream;
    char instancename[256];
    char pathname[256];
    char filename[256];
    char line[128];
    item* f;
    item* l;
    snprintf(instancename, sizeof(instancename), "instances%c%s", path_sep(), \
             k->name);
    snprintf(pathname, sizeof(pathname), "%s%ccombo", instancename, path_sep());
    snprintf(filename, sizeof(filename), "%s%csize=%"PRIu64"_capacity=" \
             "%"PRIu64".txt", pathname, path_sep(), \
             (uint64_t)(k->size - first_item), (uint64_t)capacity);
    if (file_exists(filename) && read) {
        // printf("File exists!\n");
        stream = fopen(filename, "r");
        opt_sol = atoi(fgets(line, sizeof(line), stream));
        fclose(stream);
        return opt_sol;
    }

    /* Set lower and upper bound */
    // num_t lbi = int_greedy(&k_new, RATIO);
    // num_t ubi = frac_greedy(k, RATIO);

    /* conversation of item_t structure to Combo's item structure */
    item items[k->size - first_item];
    for (size_t i = 0; i < k->size - first_item; ++i) {
        items[i].p = k_new.items[i].profit;
        items[i].w = k_new.items[i].cost;
        items[i].x = k_new.items[i].included;
    }
    f = items;
    l = items + k->size - 1 - first_item; 
    /* start Combo and couting elapsed cycles*/
    start = rdtsc();
    opt_sol = combo(f, l, k_new.capacity, 0, 0, def, relx);
    /* stop the count */
    end = rdtsc();
    /* if Combo is slower than ca. 0.0003 seconds, the result is saved */
    if (((double)(end - start) / (2.6 * pow(10, 9)) > .0003) \
        && !file_exists(filename)) {
        create_dir(instancename);
        create_dir(pathname);
        FILE* file = fopen(filename, "w");
        // printf("Passed opening file for writing!\n");
        fprintf(file, "%"PRIu64"\n", (uint64_t)opt_sol);
        // printf("Passed writing in file!\n");
        fclose(file);
        // printf("Passed closing file!\n");
    }
    return opt_sol;
}

/* 
 * =============================================================================
 *                            measure Combo
 * =============================================================================
 */

void
measure_combo(const knapsack_t* k) {
    char instancename[256];
    char pathname[256];
    char filename_false[256];
    char filename_true[256];
    snprintf(instancename, sizeof(instancename), "instances%c%s", path_sep(), \
             k->name);
    snprintf(pathname, sizeof(pathname), "%s%ccombo", instancename, path_sep());
    snprintf(filename_false, sizeof(filename_false), "%s%cmeas_def=false.txt", \
             pathname, path_sep());
    snprintf(filename_true, sizeof(filename_true), "%s%cmeas_def=true.txt", \
             pathname, path_sep());
    /* if both timing files exist, abort */
    FILE* file;
    uint64_t start;
    uint64_t end;
    uint64_t peak_mem;
    /* of not already existing, create directories */
    create_dir(instancename);
    create_dir(pathname);
    /* conversation of item_t structure to Combo's item structure */
    item items[k->size];
    for (size_t i = 0; i < k->size; ++i) {
        items[i].p = k->items[i].profit;
        items[i].w = k->items[i].cost;
        items[i].x = k->items[i].included;
    }
    combo_args_t args = { .f = items, .l = items + k->size - 1, \
                          .c = k->capacity, .lb = 0, .ub = 0, .def = FALSE, \
                          .relx = FALSE};
    /* first executed with def=FALSE */
    start = rdtsc();
    combo_mask(&args);
    end = rdtsc();
    peak_mem = rpmu(combo_mask, &args);
    /* safe elapsed cycles in file */
    file = fopen(filename_false, "a");
    fprintf(file, "%"PRIu64" %"PRIu64"\n", end - start, peak_mem);
    fclose(file);

    /* now executed with def=TRUE */
    args.def = TRUE;
    start = rdtsc();
    combo_mask(&args);
    end = rdtsc();
    peak_mem = rpmu(combo_mask, &args);
    file = fopen(filename_true, "a");
    fprintf(file, "%"PRIu64" %"PRIu64" \n", end - start, peak_mem);
    fclose(file);
}
