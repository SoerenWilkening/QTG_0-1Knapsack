/* 
 * =============================================================================
 *                            includes
 * =============================================================================
 */

#if defined(_WIN32) || defined(_WIN64)
    #include "..\include\knapsack.h"
#else
    #include "../include/knapsack.h"
#endif

/* 
 * =============================================================================
 *                            macros
 * =============================================================================
 */

#define TRUE                    1
#define FALSE                   0

#define MIN(a,b)                ((a) < (b) ? (a) : (b))
#define MAX(a,b)                ((a) > (b) ? (a) : (b))
#define SWAP(a, b, T)           do { register T q; q = *(a); *(a) = *(b); \
                                *(b) = q; } while(0)

/* 
 * =============================================================================
 *                            enum names
 * =============================================================================
 */

const char*
get_sort_name(sort_t method) {
    switch (method) {
        case COST: {
            return "Cost";
        }

        case PROFIT: {
            return "Profit";
        }

        case RATIO: {
            return "Ratio";
        }

        default: {
            return "";
        }
    }
}

const char*
get_lb_name(lb_t method) {
    switch (method) {
        case IGREEDY: {
            return "Integer Greedy";
        }

        default: {
            return "";
        }
    }
}

const char*
get_ub_name(ub_t method) {
    switch (method) {
        case SUM: {
            return "Sum";
        }

        case FGREEDY: {
            return "Fractional Greedy";
        }

        default: {
            return "";
        }
    }
}

const char*
get_category_name(category_t category) {
    switch (category) {
        case SMALL: {
            return "Small coefficients";
        }

        case LARGE: {
            return "Large coefficients";
        }

        case HARD: {
            return "Hard instances";
        }

        default: {
            return "";
        }
    }
}

/* 
 * =============================================================================
 *                            number of digits
 * =============================================================================
 */

bit_t
num_digits(num_t number) {
    bit_t result = 0;
    num_t remainder = number;
    do {
        remainder /= 10; /* removes last digit */
        ++result; /* increases counter */
    } while (0 != remainder);

    return result;
}

/* 
 * =============================================================================
 *                            create/copy/free
 * =============================================================================
 */

item_t*
create_item(num_t cost, num_t profit) {
    item_t* new_item = malloc(sizeof(item_t));
    new_item->cost = cost;
    new_item->profit = profit;
    new_item->included = FALSE;
    return new_item;
}

void
free_path(path_t* path) {
    mpz_clear(path->vector);
    free(path);
}

knapsack_t*
create_empty_knapsack(bit_t size, num_t capacity) {
    knapsack_t* new_knapsack = malloc(sizeof(knapsack_t));
    new_knapsack->size = size;
    new_knapsack->capacity = capacity;
    new_knapsack->remain_cost = capacity;
    new_knapsack->tot_profit = 0;
    new_knapsack->items = calloc(size, sizeof(item_t));
    new_knapsack->name = malloc(256 * sizeof(char));
    return new_knapsack;
}

void
pisinger_filename(category_t category, size_t num_file, bit_t size, \
                  num_t coeff_range, char buffer[], size_t buffer_size) {
    switch (category) {
        case SMALL: {
            snprintf(buffer, buffer_size, "instances%csmallcoeff_" \
                     "pisinger%cknapPI_%zu_%"PRIu64"_%"PRIu64".csv", \
                     path_sep(), path_sep(), num_file, (uint64_t)size, \
                     (uint64_t)coeff_range);
            break;
        }

        case LARGE: {
            snprintf(buffer, buffer_size, "instances%clargecoeff_" \
                     "pisinger%cknapPI_%zu_%"PRIu64"_%"PRIu64".csv", \
                     path_sep(), path_sep(), num_file, (uint64_t)size, \
                     (uint64_t)coeff_range);
            break;
        }

        case HARD: {
            snprintf(buffer, buffer_size, "instances%chardinstances" \
                     "_pisinger%cknapPI_%zu_%"PRIu64"_%"PRIu64".csv", \
                     path_sep(), path_sep(), num_file, (uint64_t)size, \
                     (uint64_t)coeff_range);
            break;
        }

        default: {
            printf("Unspecified category!\n");
            return;
        }
    }
}

void
jooken_filename(bit_t size, num_t capacity, bit_t num_groups, \
                double group_frac, double pert, num_t range, \
                char buffer[], size_t buffer_size) {
    snprintf(buffer, buffer_size, "instances%cproblemInstances%cn_%" \
             PRIu64"_c_%"PRIu64"_g_%"PRIu64"_f_%.1f_eps_%.4g_s_%"PRIu64 \
             "%ctest.in", path_sep(), path_sep(), (uint64_t)size, \
             (uint64_t)capacity, (uint64_t)num_groups, group_frac, pert, \
             (uint64_t)range, path_sep());
}

knapsack_t*
create_pisinger_knapsack(char* filename) {
    knapsack_t* new_knapsack;
    FILE* stream;
    char line[256];
    char profit[32];
    char cost[32];
    bit_t size;
    num_t capacity;
    size_t line_pos;
    size_t profit_pos;
    size_t cost_pos;
    size_t num_line = 0;

    if(!file_exists(filename)) {
        printf("File does not exist. Could not create knapsack.\n");
        return NULL;
    }
    stream = fopen(filename, "r");

    /* skip instance name */
    fgets(line, sizeof(line), stream);

    /* determine size of knapsack */
    size = atoi(fgets(line, sizeof(line), stream) + 2);

    /* determine capacity */
    capacity = atoi(fgets(line, sizeof(line), stream) + 2);

    /* instanciate knapsack and set name */
    new_knapsack = create_empty_knapsack(size, capacity);
    filename[strlen(filename) - 4] = '\0';
    sprintf(new_knapsack->name, "%s", filename + 10);

    /* skip optimal profit */
    fgets(line, sizeof(line), stream);

    /* skip combo timer */
    fgets(line, sizeof(line), stream);

    num_line = 0;
    while (fgets(line, sizeof(line), stream) != NULL && num_line < size) {
        line_pos = profit_pos = cost_pos = 0;
        /* skip item number and first comma */
        while(line[line_pos++] != ',') {
            ;
        }

        /* save profit and skip second comma */
        while((profit[profit_pos++] = line[line_pos++]) != ',') {
            ;
        }
        profit[profit_pos - 1] = '\0';

        new_knapsack->items[num_line].profit = atoi(profit);

        /* save cost*/
        while((cost[cost_pos++] = line[line_pos++]) != ',') {
            ;
        }
        cost[cost_pos - 1] = '\0';

        new_knapsack->items[num_line].cost = atoi(cost);

        ++num_line;
    }

    fclose(stream);

    return new_knapsack;
}

knapsack_t*
create_jooken_knapsack(char* filename) {
    knapsack_t* new_knapsack;
    FILE* stream;
    char line[256];
    char profit[64];
    char cost[64];
    bit_t size;
    size_t line_pos;
    size_t profit_pos;
    size_t cost_pos;
    size_t num_line = 0;
    int a;

    if(!file_exists(filename)) {
        return NULL;
    }

    stream = fopen(filename, "r");

    /* determine size of knapsack */
    fscanf(stream, "%d", &size);

    /* instanciate knapsack and set name (capacity is determined later */
    new_knapsack = create_empty_knapsack(size, 0);
    filename[strlen(filename) - 8] = '\0';
    sprintf(new_knapsack->name, "%s", filename + 10);

    while (num_line < size && fgets(line, sizeof(line), stream) != NULL) {
        fscanf(stream, "%d %ld %ld", &a, &(new_knapsack->items[num_line].profit), \
               &(new_knapsack->items[num_line].cost));
        ++num_line;
    }

    /* determine capacity */
    fscanf(stream, "%ld", &(new_knapsack->capacity));
    new_knapsack->remain_cost = new_knapsack->capacity;

    fclose(stream);

    return new_knapsack;
}

knapsack_t*
copy_knapsack(const knapsack_t* k) {
    knapsack_t* k_copy = create_empty_knapsack(k->size, k->capacity);
    memcpy(k_copy->items, k->items, k->size * sizeof(item_t));
    strcpy(k_copy->name, k->name);
    return k_copy;
}

void
assign_item_values(knapsack_t* k, num_t costs[], num_t profits[]) {
    for (bit_t i = 0; i < k->size; ++i) {
        k->items[i].cost = costs[i];
        k->items[i].profit = profits[i];
        k->items[i].included = FALSE;
    }
}

void
free_knapsack(knapsack_t* k) {
    free(k->items);
    free(k->name);
    free(k);
}

/* 
 * =============================================================================
 *                            item manipulation
 * =============================================================================
 */

bool_t
put_item(knapsack_t* k, bit_t index) {
    if (index < k->size
        && k->items[index].cost <= k->remain_cost
        && !(k->items[index].included)) {
        k->items[index].included = TRUE;
        k->remain_cost -= k->items[index].cost;
        k->tot_profit += k->items[index].profit;
        return TRUE;
    }
    return FALSE;
}

bool_t
remove_item(knapsack_t* k, bit_t index) {
    if (index < k->size && k->items[index].included) {
        k->items[index].included = FALSE;
        k->remain_cost += k->items[index].cost;
        k->tot_profit -= k->items[index].profit;
        return TRUE;
    }
    return FALSE;
}

void
remove_all_items(knapsack_t* k) {
    for (bit_t i = 0; i < k->size; ++i) {
        k->items[i].included = FALSE;
    }
    k->remain_cost = k->capacity;
    k->tot_profit = 0;
}

void
swap_knapsack_items(knapsack_t* k, bit_t a, bit_t b) {
    SWAP(&k->items[a], &k->items[b], item_t);
}

int32_t
knapsack_partition(knapsack_t* k, int32_t low, int32_t up, sort_t method) {
    ratio_t pvt;
    switch (method) {
        case COST: {
            pvt = k->items[up].cost;
            break;
        }

        case PROFIT: {
            pvt = k->items[up].profit;
            break;
        }

        case RATIO: {
            pvt = k->items[up].profit / ((ratio_t)k->items[up].cost);
            break;
        }

        default: {
            printf("Unspecified sortig method!");
            break;
        }
    }
    
    int32_t i = low - 1;

    for (int32_t j = low; j < up; ++j) {
        switch (method) {
            case COST: {
                if (k->items[j].cost <= pvt) {
                    ++i;
                    swap_knapsack_items(k, i, j);
                }
                break;
            }

            case PROFIT: {
                if (k->items[j].profit >= pvt) {
                    ++i;
                    swap_knapsack_items(k, i, j);
                }
                break;
            }

            case RATIO: {
                if (k->items[j].profit / ((ratio_t)k->items[j].cost) >= pvt) {
                    ++i;
                    swap_knapsack_items(k, i, j);
                }
                break;
            }

            default: {
                printf("Unspecified sortig method!");
                return -1;
            }
        }
    }

    swap_knapsack_items(k, i + 1, up);
    return i + 1;
}

void
knapsack_quicksort(knapsack_t* k, int32_t low, int32_t up, sort_t method) {
    if (low < up) {
        int32_t pi = knapsack_partition(k, low, up, method);
        knapsack_quicksort(k, low, pi - 1, method);
        knapsack_quicksort(k, pi + 1, up, method);
    }
}

void
sort_knapsack(knapsack_t* k, sort_t method) {
    knapsack_quicksort(k, 0, k->size - 1, method);
}

void
apply_int_greedy(knapsack_t* k) {
    for (bit_t i = 0; i < k->size; ++i) {
        put_item(k, i);
    }
}

/* 
 * =============================================================================
 *                            knapsack information
 * =============================================================================
 */

void
bit_rep(const knapsack_t* k, mpz_t bit_string) {
    for(bit_t i = 0; i < k->size; ++i) {
        if (k->items[i].included) {
        mpz_setbit(bit_string, i);
        } else {
            mpz_clrbit(bit_string, i);
        }
    }
}

path_t*
path_rep(const knapsack_t* k) {
    path_t* path = malloc(sizeof(path_t));
    path->remain_cost = k->remain_cost;
    path->tot_profit = k->tot_profit;
    mpz_init(path->vector);
    bit_rep(k, path->vector);
    return path;
}

num_t
tot_cost(const knapsack_t* k) {
    return k->capacity - k->remain_cost;
}

num_t
max_cost(const knapsack_t* k) {
    num_t max = ((k->items)[0]).cost;
    for (bit_t i = 0; i < k->size; ++i) {
        max = MAX(k->items[i].cost, max);
    }
    return max;
}

num_t
min_cost(const knapsack_t* k) {
    num_t min = ((k->items)[0]).cost;
    for (bit_t i = 0; i < k->size; ++i) {
        min = MIN(k->items[i].cost, min);
    }
    return min;
}

num_t
max_profit(const knapsack_t* k) {
    num_t max = k->items[0].profit;
    for (bit_t i = 0; i < k->size; ++i) {
        max = MAX(k->items[i].profit, max);
    }
    return max;
}

num_t
min_profit(const knapsack_t* k) {
    num_t min = k->items[0].profit;
    for (bit_t i = 0; i < k->size; ++i) {
        min = MIN(k->items[i].profit, min);
    }
    return min;
}

ratio_t
max_ratio(const knapsack_t* k) {
    ratio_t max = k->items[0].profit / k->items[0].cost;
    for (bit_t i = 0; i < k->size; ++i) {
        max = MAX(k->items[i].profit / k->items[i].cost, max);
    }
    return max;
}

ratio_t
min_ratio(const knapsack_t* k) {
    ratio_t min = k->items[0].profit / k->items[0].cost;
    for (bit_t i = 0; i < k->size; ++i) {
        min = MIN(k->items[i].profit / k->items[i].cost, min);
    }
    return min;
}

num_t
profit_sum(const knapsack_t* k) {
    num_t result = 0;

    for (bit_t i = 0; i < k->size; ++i) {
        result += k->items[i].profit;
    }

    return result;
}

num_t
cost_sum(const knapsack_t* k) {
    num_t result = 0;

    for (bit_t i = 0; i < k->size; ++i) {
        result += k->items[i].cost;
    }

    return result;
}

bool_t
is_trivial(const knapsack_t* k, num_t* opt_profit) {
    if (cost_sum(k) <= k->capacity) {
        *opt_profit = profit_sum(k);
        return TRUE;
    } else if (min_cost(k) > k->capacity) {
        *opt_profit = 0;
        return TRUE;
    } else {
        return FALSE;
    }
}

bit_t
break_item(const knapsack_t* k) {
    num_t break_cost = 0;
    bit_t current_item = 0;
    do {
        break_cost += k->items[current_item].cost;
        ++current_item;
    } while(break_cost <= k->capacity);
    return current_item - 1;
}

num_t
int_greedy(const knapsack_t* k, sort_t method) {
    num_t tot_profit;
    knapsack_t* k_copy = copy_knapsack(k);
    remove_all_items(k_copy);
    sort_knapsack(k_copy, method);
    apply_int_greedy(k_copy);
    tot_profit = k_copy->tot_profit;
    free_knapsack(k_copy);
    return tot_profit;
}

num_t
frac_greedy(const knapsack_t* k, sort_t method) {
    bit_t i;
    num_t tot_profit;
    knapsack_t* k_copy = copy_knapsack(k);
    remove_all_items(k_copy);
    sort_knapsack(k_copy, method);

    for (i = 0; put_item(k_copy, i); ++i) {
    }
    tot_profit = k_copy->tot_profit;
    if (i == k_copy->size) {
        free_knapsack(k_copy);
        return tot_profit;
    }

    ratio_t frac_item = k_copy->remain_cost / ((ratio_t)k_copy->items[i].cost);
    frac_item *= k_copy->items[i].profit;
    free_knapsack(k_copy);
    return tot_profit + ((num_t)floor(frac_item));
}

num_t
get_lb(const knapsack_t* k, lb_t method) {
    switch (method) {
        case IGREEDY: {
            num_t greedy_lb = MAX(int_greedy(k, COST), int_greedy(k, PROFIT));
            return MAX(greedy_lb, int_greedy(k, RATIO));
        }

        default: {
            printf("Unspecified lower bound method!");
            return -1;
        }
    }
}

num_t
get_ub(const knapsack_t* k, ub_t method) {
    switch (method) {
        case SUM: {
            return profit_sum(k);
        }

        case FGREEDY: {
            num_t greedy_ub = MIN(frac_greedy(k, COST), frac_greedy(k, PROFIT));
            return MIN(greedy_ub, frac_greedy(k, RATIO));
        }

        default: {
            printf("Unspecified upper bound method!");
            return -1;
        }
    }
}

void
print_knapsack(const knapsack_t* k) {
    bit_t item_col = MAX(4, num_digits(k->size));
    bit_t cost_col = MAX(4, num_digits(max_cost(k)));
    bit_t profit_col = MAX(6, num_digits(max_profit(k)));
    printf("Instance: %s\n", k->name);
    for(size_t i = 0; i < 10 + strlen(k->name); ++i) {
        printf("-");
    }
    printf("\n - number of asssigned items: %"PRIu64"\n", (uint64_t)k->size);
    printf(" - capacity: %"PRIu64"\n", (uint64_t)k->capacity);
    printf(" - remaining cost: %"PRIu64"\n", (uint64_t)k->remain_cost);
    printf(" - total profit: %"PRIu64"\n\n", (uint64_t)k->tot_profit);
    printf("|%*s|", item_col, "item");
    printf("%*s|", cost_col, "cost");
    printf("%*s|", profit_col, "profit");
    printf("included|\n");
    for (size_t i = 0; i <= k->size; ++i) {
        for (size_t j = 0; j < (item_col + cost_col + profit_col + 13); ++j) {
            printf("-");
        }
        printf("\n");
        if (i != k->size) {
            printf("|%*zu|", item_col, i + 1);
            printf("%*"PRIu64"|", cost_col, (uint64_t)k->items[i].cost);
            printf("%*"PRIu64"|", profit_col, (uint64_t)k->items[i].profit);
            printf("%8s|\n", (k->items[i].included) ? "yes" : "no");
        }
    }
}
