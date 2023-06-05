/* 
 * =============================================================================
 *                                  includes
 * =============================================================================
 */

#include "knapsack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* 
 * =============================================================================
 *                                   macros
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
 *                                 enum names
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

/* 
 * =============================================================================
 *                              number of digits
 * =============================================================================
 */

bit_t
num_digits(num_t number) {
    bit_t result = 0;
    num_t remainder = number;
    do {
        remainder /= 10; // removes last digit
        ++result; // increases counter 
    } while (0 != remainder);

    return result;
}

/* 
 * =============================================================================
 *                                 create/free
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

knapsack_t*
create_empty_knapsack(bit_t size, num_t capacity) {
    knapsack_t* new_knapsack = malloc(sizeof(knapsack_t));
    new_knapsack->size = size;
    new_knapsack->capacity = capacity;
    new_knapsack->remain_cost = capacity;
    new_knapsack->tot_profit = 0;
    new_knapsack->items = calloc(size, sizeof(item_t));
    return new_knapsack;
}

knapsack_t*
create_pisinger_knapsack(category_t category, size_t num_file, bit_t size, \
                          num_t coeff_range, size_t num_instance) {
    char filename[256];
    char profit[32];
    char cost[32];
    size_t line_pos;
    size_t profit_pos;
    size_t cost_pos;

    switch (category) {
        case SMALL: {
            sprintf(filename, "smallcoeff_pisinger/knapPI_%zu_%"PRIu64"_%"
                    PRIu64".csv", num_file, (uint64_t)size,
                    (uint64_t)coeff_range);
            break;
        }

        case LARGE: {
            sprintf(filename, "largecoeff_pisinger/knapPI_%zu_%"PRIu64"_%"
                    PRIu64".csv", num_file, (uint64_t)size,
                    (uint64_t)coeff_range);
            break;
        }

        case HARD: {
            sprintf(filename, "hardinstances_pisinger/knapPI_%zu_%"PRIu64"_%"
                    PRIu64".csv", num_file, (uint64_t)size,
                    (uint64_t)coeff_range);
            break;
        }

        default: {
            printf("Unspecified category!");
            return 0;
        }
    }

    FILE* stream = fopen(filename, "r");
    if (stream == NULL) {
        printf("File does not exist!");
        return 0;
    }

    char line[256];
    size_t num_line = 0;
    while (fgets(line, sizeof(line), stream) != NULL && num_line < \
          (num_instance - 1) * (7 + size) + 2) {
        ++num_line;
    }
    printf("Current line after skip: %s", line);

    num_t capacity = atoi(fgets(line, sizeof(line), stream) + 2);

    printf("Current line after capacity: %s", line);

    knapsack_t* new_knapsack = create_empty_knapsack(size, capacity);

    do {
        fgets(line, sizeof(line), stream);
    } while(0);

    num_line = 0;
    while (fgets(line, sizeof(line), stream) != NULL && num_line < size) {
        line_pos = profit_pos = cost_pos = 0;
        printf("Current line: %s", line);
        /* skip item number and first comma */
        while(line[line_pos++] != ',') {
            ;
        }

        /* save profit and skip second comma */
        while((profit[profit_pos++] = line[line_pos++]) != ',') {
            ;
        }
        profit[profit_pos - 1] = '\0';
        printf("Current profit string: %s\n", profit);

        new_knapsack->items[num_line].profit = atoi(profit);

        /* save cost*/
        while((cost[cost_pos++] = line[line_pos++]) != ',') {
            ;
        }
        cost[cost_pos - 1] = '\0';
        printf("Current cost string: %s\n", cost);

        new_knapsack->items[num_line].cost = atoi(cost);

        ++num_line;
    }

    fclose(stream);

    return new_knapsack;
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
    free(k);
}

/* 
 * =============================================================================
 *                              item manipulation
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
    for (bit_t i = 0; put_item(k, i); ++i) {
        ;
    }
}

/* 
 * =============================================================================
 *                              knapsack information
 * =============================================================================
 */

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
is_trivial(const knapsack_t* k) {
    return (cost_sum(k) < k->capacity) || (min_cost(k) > capacity);
}

num_t
int_greedy(const knapsack_t* k, sort_t method) {
    knapsack_t k_copy = *k;
    remove_all_items(&k_copy);
    sort_knapsack(&k_copy, method);
    apply_int_greedy(&k_copy);
    return k_copy.tot_profit;
}

num_t
frac_greedy(const knapsack_t* k, sort_t method) {
    bit_t i;
    knapsack_t k_copy = *k;
    remove_all_items(&k_copy);
    sort_knapsack(&k_copy, method);

    for (i = 0; put_item(&k_copy, i); ++i) {
    }

    if (i == k_copy.size) {
        return k_copy.tot_profit;
    }

    ratio_t frac_item = k_copy.remain_cost / ((ratio_t)k_copy.items[i].cost);
    frac_item *= k_copy.items[i].profit;

    return k_copy.tot_profit + ((num_t)floor(frac_item));
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
print_knapsack(knapsack_t* k) {
    bit_t item_column_size = MAX(4, num_digits(k->size));
    bit_t cost_column_size = MAX(4, num_digits(max_cost(k)));
    bit_t profit_column_size = MAX(6, num_digits(max_profit(k)));
    printf("Knapsack information\n");
    printf("--------------------\n");
    printf(" - number of asssigned items: %"PRIu64"\n", (uint64_t)k->size);
    printf(" - capacity: %"PRIu64"\n", (uint64_t)k->capacity);
    printf(" - remaining cost: %"PRIu64"\n", (uint64_t)k->remain_cost);
    printf(" - total profit: %"PRIu64"\n\n", (uint64_t)k->tot_profit);
    printf("|%*s|", item_column_size, "item");
    printf("%*s|", cost_column_size, "cost");
    printf("%*s|", profit_column_size, "profit");
    printf("included|\n");
    for (size_t i = 0; i <= k->size; ++i) {
        for (size_t j = 0; j < (item_column_size +
                                cost_column_size +
                                profit_column_size + 13); ++j) {
            printf("-");
        }
        printf("\n");
        if (i != k->size) {
            printf("|%*zu|", item_column_size, i + 1);
            printf("%*"PRIu64"|", cost_column_size, k->items[i].cost);
            printf("%*"PRIu64"|", profit_column_size, k->items[i].profit);
            printf("%8s|\n", (k->items[i].included) ? "yes" : "no");
        }
    }
}
