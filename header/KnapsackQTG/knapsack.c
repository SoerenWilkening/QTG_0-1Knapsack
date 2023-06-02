#include "knapsack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>

/* ======================================================================
                   macros
   ====================================================================== */

#define MIN(a, b)        ((a) < (b) ? (a) : (b))
#define MAX(a, b)        ((a) > (b) ? (a) : (b))
#define SWAP(a, b, T)   do { register T q; q = *(a); *(a) = *(b); *(b) = q; } while(0)

const char *
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

const char *
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

const char *
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

item_t *
create_item(num_t cost, num_t profit) {
    item_t *new_item = malloc(sizeof(item_t));
    new_item->cost = cost;
    new_item->profit = profit;
    return new_item;
}

knapsack_t *
create_empty_knapsack(bit_t size, num_t capacity) {
    knapsack_t *new_knapsack = malloc(sizeof(knapsack_t));
    new_knapsack->size = size;
    new_knapsack->capacity = capacity;
    new_knapsack->remain_cost = capacity;
    new_knapsack->tot_profit = 0;
    new_knapsack->items = malloc(size * sizeof(item_t));
    new_knapsack->included = calloc(size, sizeof(uint8_t));
    return new_knapsack;
}

void
assign_item_values(knapsack_t *k, num_t costs[], num_t profits[]) {
    for (bit_t i = 0; i < k->size; ++i) {
        ((k->items)[i]).cost = costs[i];
        ((k->items)[i]).profit = profits[i];
    }
}

void
free_knapsack(knapsack_t *k) {
    free(k->items);
    free(k->included);
    free(k);
}

num_t
max_cost(const knapsack_t *k) {
    num_t max = ((k->items)[0]).cost;
    for (bit_t i = 0; i < k->size; ++i) {
        max = MAX(((k->items)[i]).cost, max);
    }
    return max;
}

num_t
min_cost(const knapsack_t *k) {
    num_t min = ((k->items)[0]).cost;
    for (bit_t i = 0; i < k->size; ++i) {
        min = MIN(((k->items)[i]).cost, min);
    }
    return min;
}

num_t
max_profit(const knapsack_t *k) {
    num_t max = ((k->items)[0]).profit;
    for (bit_t i = 0; i < k->size; ++i) {
        max = MAX(((k->items)[i]).profit, max);
    }
    return max;
}

num_t
min_profit(const knapsack_t *k) {
    num_t min = ((k->items)[0]).profit;
    for (bit_t i = 0; i < k->size; ++i) {
        min = MIN(((k->items)[i]).profit, min);
    }
    return min;
}

ratio_t
max_ratio(const knapsack_t *k) {
    ratio_t max = ((k->items)[0]).profit / ((k->items)[0]).cost;
    for (bit_t i = 0; i < k->size; ++i) {
        max = MAX(((k->items)[i]).profit / ((k->items)[i]).cost, max);
    }
    return max;
}

ratio_t
min_ratio(const knapsack_t *k) {
    ratio_t min = ((k->items)[0]).profit / ((k->items)[0]).cost;
    for (bit_t i = 0; i < k->size; ++i) {
        min = MIN(((k->items)[i]).profit / ((k->items)[i]).cost, min);
    }
    return min;
}

uint8_t
put_item(knapsack_t *k, bit_t index) {
    if (index < k->size
        && ((k->items)[index]).cost <= k->remain_cost
        && !((k->included)[index])) {
        (k->included)[index] = 1;
        k->remain_cost -= ((k->items)[index]).cost;
        k->tot_profit += ((k->items)[index]).profit;
        return 1;
    }
    return 0;
}

uint8_t
remove_item(knapsack_t *k, bit_t index) {
    if (index < k->size && ((k->included)[index])) {
        (k->included)[index] = 0;
        k->remain_cost += ((k->items)[index]).cost;
        k->tot_profit -= ((k->items)[index]).profit;
        return 1;
    }
    return 0;
}

void
remove_all_items(knapsack_t *k) {
    memset(k->included, 0, (k->size) * sizeof(uint8_t));
    k->remain_cost = k->capacity;
    k->tot_profit = 0;
}

void
swap_knapsack_items(knapsack_t *k, bit_t a, bit_t b) {
    SWAP(&((k->items)[a]), &((k->items)[b]), item_t);
    SWAP(&((k->included)[a]), &((k->included)[b]), uint8_t);
}

int32_t
knapsack_partition(knapsack_t *k, int32_t low, int32_t up, sort_t method) {

    // select the rightmost element as pivot
    ratio_t pvt;
    switch (method) {
        case COST: {
            pvt = ((k->items)[up]).cost;
            break;
        }

        case PROFIT: {
            pvt = ((k->items)[up]).profit;
            break;
        }

        case RATIO: {
            pvt = ((k->items)[up]).profit / ((ratio_t) ((k->items)[up]).cost);
            break;
        }

        default: {
            pvt = ((k->items)[up]).profit;
            break;
        }
    }


    // pointer for smaller element
    int32_t i = low - 1;

    // traverse each element of the array
    // compare them with the pivot
    for (int32_t j = low; j < up; ++j) {
        switch (method) {
            case COST: {
                if (((k->items)[j]).cost <= pvt) {
                    i++;
                    swap_knapsack_items(k, i, j);
                }
                break;
            }

            case PROFIT: {
                if (((k->items)[j]).profit >= pvt) {
                    i++;
                    swap_knapsack_items(k, i, j);
                }
                break;
            }

            case RATIO: {
                if ((((k->items)[j]).profit / ((ratio_t) ((k->items)[j]).cost)) >= pvt) {
                    i++;
                    swap_knapsack_items(k, i, j);
                }
                break;
            }

            default: {
                if (((k->items)[j]).profit >= pvt) {
                    i++;
                    swap_knapsack_items(k, i, j);
                }
                break;
            }
        }
    }

    // swap the pivot element with the smaller element at i
    swap_knapsack_items(k, i + 1, up);

    // return the partition point
    return i + 1;
}

void
knapsack_quicksort(knapsack_t *k, int32_t low, int32_t up, sort_t method) {
    if (low < up) {

        // find the pivot element such that
        // elements smaller than pivot are on left of pivot
        // elements greater than pivot are on right of pivot
        int32_t pi = knapsack_partition(k, low, up, method);

        // recursive call on the left of pivot
        knapsack_quicksort(k, low, pi - 1, method);

        // recursive call on the right of pivot
        knapsack_quicksort(k, pi + 1, up, method);
    }
}

void
sort_knapsack(knapsack_t *k, sort_t method) {
    knapsack_quicksort(k, 0, k->size - 1, method);
}

void
print_knapsack(knapsack_t *k) {
    bit_t length_of_size = num_digits(k->size);
    printf("%u items are available for this knapsack with capacity %llu.\n", \
           k->size, k->capacity);
    printf("The remaining costs are %llu and the total profit is given by %llu.\n\n", \
           k->remain_cost, k->tot_profit);
    printf("%*s|", (4 > length_of_size) ? 4 : length_of_size, "item");
//    printf("%*s|", length_of_num_t, "cost");
//    printf("%*s|", length_of_num_t, "profit");
    printf("included\n");
//    for (size_t i = 0; i <= k->size; ++i) {
//        for (size_t j = 0; j < ((4 > length_of_size) ? 4 : length_of_size) \
// + 2 * length_of_num_t + 11; ++j) {
//            printf("-");
//        }
//        printf("\n");
//        if (i != k->size) {
//            printf("%4lu|", i + 1);
//            printf("%20llu|", ((k->items)[i]).cost);
//            printf("%20llu|", ((k->items)[i]).profit);
//            printf("%8s\n", ((k->included)[i]) ? "yes" : "no");
//        }
//    }
}

void
apply_int_greedy(knapsack_t *k) {
    for (bit_t i = 0; put_item(k, i); ++i) { ;
    }
}

num_t
int_greedy(const knapsack_t *k) {
    knapsack_t k_copy = *k;
    remove_all_items(&k_copy);
    sort_knapsack(&k_copy, PROFIT);
    apply_int_greedy(&k_copy);
    return k_copy.tot_profit;
}

num_t
get_lb(const knapsack_t *k, lb_t method) {
    switch (method) {
        case IGREEDY: {
            return int_greedy(k);
        }

        default: {
            return -1;
        }
    }
}

num_t
profit_sum(const knapsack_t *k) {
    num_t result = 0;

    for (bit_t i = 0; i < k->size; ++i) result += ((k->items)[i]).profit;

    return result;
}

num_t
frac_greedy(const knapsack_t *k) {
    bit_t i;
    knapsack_t k_copy = *k;
    remove_all_items(&k_copy);
    sort_knapsack(&k_copy, RATIO);

    for (i = 0; put_item(&k_copy, i); ++i)

        if (i == k_copy.size) return k_copy.tot_profit;

    ratio_t frac_item = k_copy.remain_cost / ((ratio_t) ((k_copy.items)[i]).cost);
    frac_item *= ((k_copy.items)[i]).profit;

    return k_copy.tot_profit + ((num_t) floor(frac_item));
}

num_t
get_ub(const knapsack_t *k, ub_t method) {
    switch (method) {
        case SUM: {
            return profit_sum(k);
        }

        case FGREEDY: {
            return frac_greedy(k);
        }

        default: {
            return -1;
        }
    }
}