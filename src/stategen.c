/* 
 * =============================================================================
 *                            includes
 * =============================================================================
 */

#if defined(_WIN32) || defined(_WIN64)
    #include "..\include\stategen.h"
#else
    #include "../include/stategen.h"
#endif

/* 
 * =============================================================================
 *                            macros
 * =============================================================================
 */

#define TRUE                    1
#define FALSE                   0

#define SWAP(a, b, T)           do { register T q; q = *(a); *(a) = *(b); \
                                *(b) = q; } while(0)

/* 
 * =============================================================================
 *                            enum names
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
 *                            free nodes
 * =============================================================================
 */

 void
 free_nodes(node_t nodes[], size_t num_nodes) {
    for (size_t i = 0; i < num_nodes; ++i) {
        mpz_clear(nodes[i].path.vector);
    }
    free(nodes);
 }

/* 
 * =============================================================================
 *                            branch probability
 * =============================================================================
 */

double
branch_prob(const knapsack_t* k, bit_t i, size_t bias, bool_t left, \
            branch_t method, mpz_t cur_sol) {
    switch (method) {
        case COMPARE: {
            if (left) {
                return (1. + (1 - mpz_tstbit(cur_sol, i)) * bias) \
                        / (bias + 2);
            } else {
                return (1. + mpz_tstbit(cur_sol, i) * bias) \
                        / (bias + 2);
            }
            break;
        }

        case SINGLE: {
            if (left) {
                return sqrt(1. / (bias + 2));
            } else {
                return sqrt((1. + bias) / (bias + 2)); 
            }
            
            break;
        }

        default: {
            printf("Unspecified branching method!\n");
            return 0;
        }
    }
}

/* 
 * =============================================================================
 *                            Quantum Tree Generator
 * =============================================================================
 */

node_t*
qtg(const knapsack_t* k, num_t threshold, num_t exact, size_t bias, \
    branch_t method, mpz_t cur_sol, size_t* num_states) {
    if (threshold == exact) {
        /* return empty node array if optimal solution is already reached */
        *num_states = 0;
        return NULL;
    }
	*num_states = 1; /* start from the root */
    size_t a = 0; /* start from leftmost node */
    /* initialize root node as single-element node_t array */
    node_t* parent = malloc(sizeof(node_t));
    parent->path.remain_cost = k->capacity;
    parent->path.tot_profit = 0;
    mpz_init(parent->path.vector);
    parent->prob = 1.;
    parent->ub = exact;
    num_t left_ub, right_ub;
    for (bit_t i = 0; i < k->size; a = 0, ++i) { /* start from leftmost node */
        /*
         * The size of the child layer is upper bounded by twice the parent
         * layer's size which is given as the current number of states.
         */
        node_t* child = malloc(2 * (*num_states) * sizeof(node_t));
        for (size_t j = 0; j < (*num_states); ++j) {
            if (parent[j].path.remain_cost < k->items[i].cost) {
                /* item cannot be included, thus no branching */
                child[a].path.remain_cost = parent[j].path.remain_cost;
                child[a].path.tot_profit = parent[j].path.tot_profit;
                mpz_init(child[a].path.vector);
                mpz_set(child[a].path.vector, parent[j].path.vector);
                child[a].ub = parent[j].ub;
                child[a].prob = parent[j].prob;
                // printf("----------------\n");
                // printf("Node info:\n");
                // printf("Remaining cost: %ld\n", child[a].path.remain_cost);
                // printf("Total profit: %ld\n", child[a].path.tot_profit);
                // gmp_printf("Vector: %Zd\n", child[a].path.vector);
                // printf("Upper bound: %ld\n", child[a].ub);
                // printf("Probability: %.16f\n", child[a].prob);
                // printf("----------------\n");
                ++a;
                continue;
            }
            /* 
             * The item can be included. Calculate first the exact upper bound
             * for the left subtree, i.e., where the current item is not
             * included into the knapsack.
             */
            left_ub = combo_data(k, i + 1, parent[j].path.remain_cost, FALSE, \
                                 FALSE, TRUE) \
                      + parent[j].path.tot_profit;
            if (left_ub > threshold) {
                /*
                 * The left subtree has at least one path with objective value
                 * higher than the threshold. Therefore it cannot be omitted.
                 */
                /* remaining cost, total profit, and vector do not change */
                child[a].path.remain_cost = parent[j].path.remain_cost;
                child[a].path.tot_profit = parent[j].path.tot_profit;
                mpz_init(child[a].path.vector);
                mpz_set(child[a].path.vector, parent[j].path.vector);
                /* set new upper bound */
                child[a].ub = left_ub;
                /* update probability, then increase child index */
                child[a].prob = parent[j].prob * branch_prob(k, i, bias, \
                                                    TRUE, method, cur_sol);
                // printf("----------------\n");
                // printf("Node info:\n");
                // printf("Remaining cost: %ld\n", child[a].path.remain_cost);
                // printf("Total profit: %ld\n", child[a].path.tot_profit);
                // gmp_printf("Vector: %Zd\n", child[a].path.vector);
                // printf("Upper bound: %ld\n", child[a].ub);
                // printf("Probability: %.16f\n", child[a].prob);
                // printf("----------------\n");
                ++a;

                if (left_ub == parent[j].ub) {
                    /* The upper bound of left subtree was already the upper
                     * bound for the entire tree. The upper bound for the right
                     * subtree has to be calculated, too. Here, the current item
                     * is considered to be included into the knapsack.
                     */
                    right_ub = combo_data(k, i + 1, parent[j].path.remain_cost \
                                          - k->items[i].cost, FALSE, FALSE, \
                                          TRUE) \
                               + parent[j].path.tot_profit \
                               + k->items[i].profit;
                    if (right_ub > threshold) {
                        /*
                         * The right subtree has at least one path with
                         * objective value higher than the threshold. Therefore
                         * it cannot be omitted.
                         */

                        /* update remaining cost */
                        child[a].path.remain_cost = parent[j].path.remain_cost \
                                                    - k->items[i].cost;
                        /* update total profit */
                        child[a].path.tot_profit = k->items[i].profit \
                                                   + parent[j].path.tot_profit;
                        /* include item: set the corresponding bit to 1 */
                        mpz_init(child[a].path.vector);
                        mpz_set(child[a].path.vector, parent[j].path.vector);
                        mpz_setbit(child[a].path.vector, i);
                        /* set new upper bound */
                        child[a].ub = right_ub;

                        /* update probability, then increase child index */
                        child[a].prob = parent[j].prob * branch_prob(k, i, \
                                                  bias, FALSE, method, cur_sol);
                        // printf("----------------\n");
                        // printf("Node info:\n");
                        // printf("Remaining cost: %ld\n", child[a].path.remain_cost);
                        // printf("Total profit: %ld\n", child[a].path.tot_profit);
                        // gmp_printf("Vector: %Zd\n", child[a].path.vector);
                        // printf("Upper bound: %ld\n", child[a].ub);
                        // printf("Probability: %.16f\n", child[a].prob);
                        // printf("----------------\n");
                        ++a;

                    }
                } else {
                    /* The upper bound of right subtree is the upper bound for
                     * the entire tree and thus does not have to be calculated
                     * again. Especially, its upper bound is also above the
                     * threshold and therefore it has to be included.
                     */

                    /* update remaining cost */
                    child[a].path.remain_cost = parent[j].path.remain_cost \
                                           - k->items[i].cost;
                    /* update total profit */
                    child[a].path.tot_profit = k->items[i].profit \
                                             + parent[j].path.tot_profit;
                    /* include item: set the corresponding bit to 1 */
                    mpz_init(child[a].path.vector);
                    mpz_set(child[a].path.vector, parent[j].path.vector);
                    mpz_setbit(child[a].path.vector, i);
                    /* set new upper bound */
                    child[a].ub = parent[j].ub;

                    /* update probability, then increase child index */
                    child[a].prob = parent[j].prob * branch_prob(k, i, \
                                              bias, FALSE, method, cur_sol);
                    // printf("----------------\n");
                    // printf("Node info:\n");
                    // printf("Remaining cost: %ld\n", child[a].path.remain_cost);
                    // printf("Total profit: %ld\n", child[a].path.tot_profit);
                    // gmp_printf("Vector: %Zd\n", child[a].path.vector);
                    // printf("Upper bound: %ld\n", child[a].ub);
                    // printf("Probability: %.16f\n", child[a].prob);
                    // printf("----------------\n");
                    ++a;
                }
            } else {
                /* left subtree is omitted, but right subtree is not */

                /* update remaining cost */
                child[a].path.remain_cost = parent[j].path.remain_cost \
                                       - k->items[i].cost;
                /* update total profit */
                child[a].path.tot_profit = k->items[i].profit \
                                         + parent[j].path.tot_profit;
                /* include item: set the corresponding bit to 1 */
                mpz_init(child[a].path.vector);
                mpz_set(child[a].path.vector, parent[j].path.vector);
                mpz_setbit(child[a].path.vector, i);
                /* set new upper bound */
                child[a].ub = parent[j].ub;

                /* update probability, then increase child index */
                child[a].prob = parent[j].prob * branch_prob(k, i, bias, \
                                                    FALSE, method, cur_sol);
                // printf("----------------\n");
                // printf("Node info:\n");
                // printf("Remaining cost: %ld\n", child[a].path.remain_cost);
                // printf("Total profit: %ld\n", child[a].path.tot_profit);
                // gmp_printf("Vector: %Zd\n", child[a].path.vector);
                // printf("Upper bound: %ld\n", child[a].ub);
                // printf("Probability: %.16f\n", child[a].prob);
                // printf("----------------\n");
                ++a;
            }
        }
        /* swap pointer to parent and child layer */
        SWAP(&parent, &child, node_t*);
        /* resize new parent layer and free former parent layer */
        parent = realloc(parent, a * sizeof(node_t));
        free_nodes(child, *num_states);
        *num_states = a;
        // printf("---------------------------------\n");
        // printf("DONE WITH LAYER\n");
        // printf("---------------------------------\n");
    }
    /* final layer is comprised of all feasible paths above threshold */
    // printf("Number of states after QTG: %zu\n", *num_states);
    return parent;
}
