//
// Created by Sören Wilkening on 10.05.23.
//

#include "state_generator.h"

std::vector<node_t>
init_states(size_t number, bit_t bits) {
    std::vector<node_t> result(number);
//    result.states = calloc(number, sizeof(node_t));
    for (size_t i = 0; i < number; ++i) {
        result[i].amplitude = 1.;
        result[i].tot_profit = 0;
        result[i].capacity = 0;
        mpz_init2(result[i].vector, bits);
//        result[i].vector = 0.;
        result[i].ub = 0;
    }
    return result;
}

std::vector<node_t>
breadth_first_search(knapsack_t* k, num_t threshold, num_t exact, double bias, \
                     branch_t method, mpz_t prev_sol) {
    /**
     * TODO:
     *  - include calculations of amplitudes
     *  - enable multiprocessing
     * */

    size_t num_states = 1; /* start from the root */
    size_t a = 0; /* start from leftmost node */
    std::vector<node_t> parent = init_states(num_states, k->size);
    parent[0].ub = exact;
    parent[0].capacity = k->capacity;
    num_t opt_sol;
    double discarded = 0;
    long* timer = static_cast<long*>(calloc(1, sizeof(long)));
    for (bit_t i = 0; i < k->size; a = 0, ++i) { /* start from leftmost node */
        std::vector<node_t> children = init_states(2 * num_states, k->size);

        for (size_t j = 0; j < num_states; ++j) {
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
            opt_sol = cpp_combo_wrap(k, i + 1, parent[j].capacity, FALSE, \
                                     FALSE, TRUE, timer);

            if (opt_sol + parent[j].tot_profit > threshold) {
                /*
                 * The left subtree has at least one state with objective value
                 * higher than the threshold. Therefore it cannot be omitted.
                 */

                children[a] = parent[j]; /* transfer information from parent */
                children[a].ub = opt_sol; /* set new upper bound */

                /* update amplitudes, then increase children index */
                switch (method) {
                    case COMPARE: {
                        children[a++].amplitude *= sqrt((1 + (1 \
                         - mpz_tstbit(prev_sol, k->size - i - 1)) * bias) \ 
                         / (bias + 2));
                        break;
                    }

                    case SINGLE: {
                        children[a++].amplitude *= sqrt(1. / (bias + 2));
                        break;
                    }

                    default: {
                        return -1;
                    }
                }

                if (opt_sol == parent[j].ub) {
                    /* The upper bound of left subtree was already the upper
                     * bound for the entire tree. The upper bound for the right
                     * subtree has to be calculated, too. Here, the current item
                     * is considered to be included into the knapsack.
                     */

                    opt_sol = cpp_combo_wrap(k, i + 1, parent[j].capacity \
                                             - k->items[i].cost, FALSE, FALSE, \
                                             TRUE, timer);

                    if (opt_sol + parent[j].tot_profit \
                        + k->items[i].profit > threshold) {
                        /*
                         * The right subtree has at least one state with
                         * objective value higher than the threshold. Therefore
                         * it cannot be omitted.
                         */
                        
                        children[a].ub = opt_sol; /* set new upper bound */
                        /* subtract current item cost from capacity */
                        children[a].capacity = parent[j].capacity \
                                               - k->items[i].cost;
                        /* add current item profit to total profit */
                        children[a].tot_profit = parent[j].tot_profit \
                                                 + k->items[i].profit;

                        /* 
                         * copy binary vector from parent, but include current
                         * item, i.e., set the corresponding bit to 1.
                         */
                        mpz_set(children[a].vector, parent[j].vector);
                        mpz_setbit(children[a].vector, k->size - 1 - i);

                        /* update amplitudes, then increase children index */
                        switch (method) {
                            case COMPARE: {
                                children[a++].amplitude *= sqrt((1 \
                                + mpz_tstbit(prev_sol, k->size - i - 1) * bias) \ 
                                / (bias + 2));
                                break;
                            }

                            case SINGLE: {
                                children[a++].amplitude = parent[j].amplitude \
                                 * sqrt((1. + bias) / (bias + 2));
                                break;
                            }

                            default: {
                                return -1;
                            }
                        }
                    }
                } else {
                    /* The upper bound of right subtree is the upper bound for
                     * the entire tree and thus does not have to be calculated
                     * again. Especially, its upper bound is also above the
                     * threshold and therefore has to be included.
                     */

                    /* set new upper bound */
                    children[a].ub = parent[j].ub - k->items[i].profit;
                    /* subtract current item cost from capacity */
                    children[a].capacity = parent[j].capacity \
                                           - k->items[i].cost;
                    /* add current item profit to total profit */
                    children[a].tot_profit = parent[j].tot_profit \
                                             + k->items[i].profit;

                    /* 
                     * copy binary vector from parent, but include current
                     * item, i.e., set the corresponding bit to 1.
                     */
                    mpz_set(children[a].vector, parent[j].vector);
                    mpz_setbit(children[a].vector, k->size - 1 - i);

                    /* update amplitudes, then increase children index */
                    switch (method) {
                        case COMPARE: {
                            children[a++].amplitude *= sqrt((1 \
                            + mpz_tstbit(prev_sol, k->size - i - 1) * bias) \ 
                            / (bias + 2));
                            break;
                        }

                        case SINGLE: {
                            children[a++].amplitude = parent[j].amplitude \
                                * sqrt((1. + bias) / (bias + 2));
                            break;
                        }

                        default: {
                            return -1;
                        }
                    }
                }
            } else {
                /* left subtree is omitted, but right subtree is not */

                /* set new upper bound */
                children[a].ub = parent[j].ub - k->items[i].profit;
                /* subtract current item cost from capacity */
                children[a].capacity = parent[j].capacity - k->items[i].cost;
                /* add current item profit to total profit */
                children[a].tot_profit = parent[j].tot_profit \
                                         + k->items[i].profit;

                /* 
                 * copy binary vector from parent, but include current
                 * item, i.e., set the corresponding bit to 1.
                 */
                mpz_set(children[a].vector, parent[j].vector);
                mpz_setbit(children[a].vector, k->size - 1 - i);

                switch (method) {
                    case COMPARE: {
                        children[a++].amplitude *= sqrt((1 \
                        + mpz_tstbit(prev_sol, k->size - i - 1) * bias) \ 
                        / (bias + 2));
                        break;
                    }

                    case SINGLE: {
                        children[a++].amplitude = parent[j].amplitude \
                            * sqrt((1. + bias) / (bias + 2));
                        break;
                    }

                    default: {
                        return -1;
                    }
                }
            }
        }
        /* declare current layer the parent layer and clear parent layer */
        std::swap(parent, children);
        children.clear();
        children.shrink_to_fit();
        num_states = a;
    }
    /* final layer is comprised of all feasible paths above threshold */
    parent.resize(num_states);
    return parent;
}