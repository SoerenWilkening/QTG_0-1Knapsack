/* 
 * =============================================================================
 *                                  includes
 * =============================================================================
 */

#include "stategen.h"

/* 
 * =============================================================================
 *                                   macros
 * =============================================================================
 */

#define TRUE                    1
#define FALSE                   0

#define SWAP(a, b, T)           do { register T q; q = *(a); *(a) = *(b); \
                                *(b) = q; } while(0)

/* 
 * =============================================================================
 *                                 enum names
 * =============================================================================
 */

const char*
get_branch_name(branch_t method) {
    switch (method) {
        case COMPARE: {
            return "Compare";
        }

        case SINGLE: {
            return "Single";
        }

        default: {
            return "";
        }
    }
}

/* 
 * =============================================================================
 *                              update probability
 * =============================================================================
 */

void
update_prob(knapsack_t* k, bit_t i, double bias, bool_t left, branch_t method, \
            mpz_t cur_sol, double* prob) {
    switch (method) {
        case COMPARE: {
            if (left) {
                *prob *= (1 + (1 - mpz_tstbit(cur_sol, k->size - i - 1)) \
                          * bias) / (bias + 2);
            } else {
                *prob *= (1 + mpz_tstbit(cur_sol, k->size - i - 1) * bias) \
                          / (bias + 2);
            }
            break;
        }

        case SINGLE: {
            if (left) {
                *prob *= sqrt(1. / (bias + 2));
            } else {
                *prob *= sqrt((1. + bias) / (bias + 2)); 
            }
            
            break;
        }

        default: {
            return;
        }
    }
}

/* 
 * =============================================================================
 *                              breadth-first search
 * =============================================================================
 */

node_t*
breadth_first_search(knapsack_t* k, num_t threshold, num_t exact, double bias, \
                     branch_t method, mpz_t cur_sol, size_t* num_states) {
	*num_states = 1; /* start from the root */
    size_t a = 0; /* start from leftmost node */
    node_t* parent = malloc(sizeof(node_t));
    parent->state.tot_profit = 0;
    mpz_init2(parent->state.vector, k->size);
    parent->prob = 1.;
    parent->capacity = k->capacity;
    parent->ub = exact;
    num_t opt_sol;
    uint64_t* timer;
    *timer = 0;
    for (bit_t i = 0; i < k->size; a = 0, ++i) { /* start from leftmost node */
        node_t* children = malloc(2 * *num_states * sizeof(node_t));
        for (size_t j = 0; j < *num_states; ++j) {
            if (parent[j].capacity < k->items[i].cost) {
                /* item cannot be included, thus no branching */
                children[a++] = parent[j];
                continue;
            }
            /* 
             * The item can be included. Calculate first the exact upper bound
             * for the left subtree, i.e., where the current item is not
             * included into the knapsack.
             */
            opt_sol = combo_wrap(k, i + 1, parent[j].capacity, FALSE, FALSE, \
                                 TRUE, timer);

            if (opt_sol + parent[j].state.tot_profit > threshold) {
                /*
                 * The left subtree has at least one state with objective value
                 * higher than the threshold. Therefore it cannot be omitted.
                 */

                children[a] = parent[j]; /* transfer information from parent */
                children[a].ub = opt_sol; /* set new upper bound */

                /* update amplitudes, then increase children index */
                update_prob(k, i, bias, TRUE, method, cur_sol, \
                            &(children[a].prob));
                ++a;

                if (opt_sol == parent[j].ub) {
                    /* The upper bound of left subtree was already the upper
                     * bound for the entire tree. The upper bound for the right
                     * subtree has to be calculated, too. Here, the current item
                     * is considered to be included into the knapsack.
                     */

                    opt_sol = combo_wrap(k, i + 1, parent[j].capacity \
                                         - k->items[i].cost, FALSE, FALSE, \
                                         TRUE, timer);

                    if (opt_sol + parent[j].state.tot_profit \
                        + k->items[i].profit > threshold) {
                        /*
                         * The right subtree has at least one state with
                         * objective value higher than the threshold. Therefore
                         * it cannot be omitted.
                         */

                        children[a] = parent[j];
                        children[a].ub = opt_sol; /* set new upper bound */
                        children[a].capacity -= k->items[i].cost;
                        children[a].state.tot_profit += k->items[i].profit;

                        /* 
                         * include current item, i.e., set the corresponding bit
                         * to 1.
                         */
                        mpz_setbit(children[a].state.vector, k->size - 1 - i);

                        /* update amplitudes, then increase children index */
                        update_prob(k, i, bias, FALSE, method, cur_sol, \
                            &(children[a].prob));
                        ++a;

                    }
                } else {
                    /* The upper bound of right subtree is the upper bound for
                     * the entire tree and thus does not have to be calculated
                     * again. Especially, its upper bound is also above the
                     * threshold and therefore has to be included.
                     */

                    children[a] = parent[j];
                    /* set new upper bound */
                    children[a].ub -= k->items[i].profit;
                    /* subtract current item cost from capacity */
                    children[a].capacity -= k->items[i].cost;
                    /* add current item profit to total profit */
                    children[a].state.tot_profit += k->items[i].profit;

                    /* 
                     * include current item, i.e., set the corresponding bit
                     * to 1.
                     */
                    mpz_setbit(children[a].state.vector, k->size - 1 - i);

                    update_prob(k, i, bias, FALSE, method, cur_sol, \
                            &(children[a].prob));
                    ++a;
                }
            } else {
                /* left subtree is omitted, but right subtree is not */

                children[a] = parent[j];
                /* set new upper bound */
                children[a].ub -= k->items[i].profit;
                /* subtract current item cost from capacity */
                children[a].capacity -= k->items[i].cost;
                /* add current item profit to total profit */
                children[a].state.tot_profit += k->items[i].profit;

                /* 
                 * include current item, i.e., set the corresponding bit
                 * to 1.
                 */
                mpz_setbit(children[a].state.vector, k->size - 1 - i);

                update_prob(k, i, bias, FALSE, method, cur_sol, \
                            &(children[a].prob));
                ++a;
            }
        }
        /* swap pointer to parent and children layer */
        SWAP(&parent, &children, node_t*);
        *num_states = a;
        /* resize new parent layer and delete former parent layer */
        parent = realloc(parent, *num_states * sizeof(node_t));
        free(children);
    }
    /* final layer is comprised of all feasible paths above threshold */
    return parent;
}