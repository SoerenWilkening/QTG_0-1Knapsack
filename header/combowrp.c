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
    // printf("%s\n", filename);
    if (file_exists(filename) && read) {
        // printf("File exists!\n");
        stream = fopen(filename, "r");
        // printf("Open file worked!\n");
        opt_sol = atoi(fgets(line, sizeof(line), stream));
        // printf("Read file worked!\n");
        fclose(stream);
        return opt_sol;
    }
    // printf("Passed reading!\n");

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
    // printf("Passed item translation!\n");
    /* start Combo and couting elapsed cycles*/
    start = rdtsc();
    // printf("Passed setting start!\n");
    opt_sol = combo(f, l, k_new.capacity, 0, 0, def, relx);
    // printf("Passed executing Combo!\n");
    /* stop the count */
    end = rdtsc();
    // printf("Passed setting end!\n");
    // printf("Passed executing and measuring Combo!\n");
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
 *                            Combo timer
 * =============================================================================
 */

void
time_combo(const knapsack_t* k) {
    char instancename[256];
    char pathname[256];
    char filename_f[256];
    char filename_t[256];
    snprintf(instancename, sizeof(instancename), "instances%c%s", path_sep(), \
             k->name);
    snprintf(pathname, sizeof(pathname), "%s%ccombo", instancename, path_sep());
    snprintf(filename_f, sizeof(filename_f), "%s%ctimer_def=false.txt", \
             pathname, path_sep());
    snprintf(filename_t, sizeof(filename_t), "%s%ctimer_def=true.txt", \
             pathname, path_sep());
    /* if both timing files exist, abort */
    FILE* file;
    uint64_t start;
    uint64_t end;
    item* f;
    item* l;
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
    f = items;
    l = items + k->size - 1;
    /* first executed with def=FALSE */
    start = rdtsc();
    combo(f, l, k->capacity, 0, 0, FALSE, FALSE);
    end = rdtsc();
    /* safe elapsed cycles in file */
    file = fopen(filename_f, "a");
    // printf("Passed opening file for writing!\n");
    fprintf(file, "%"PRIu64"\n", end - start);
    // printf("Passed writing in file!\n");
    fclose(file);
    // printf("Passed closing file!\n");

    /* now executed with def=TRUE */
    start = rdtsc();
    combo(f, l, k->capacity, 0, 0, TRUE, FALSE);
    end = rdtsc();
    file = fopen(filename_t, "a");
    // printf("Passed opening file for writing!\n");
    fprintf(file, "%"PRIu64"\n", end - start);
    // printf("Passed writing in file!\n");
    fclose(file);
    // printf("Passed closing file!\n");
}