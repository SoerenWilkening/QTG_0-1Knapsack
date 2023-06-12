/* 
 * =============================================================================
 *                            includes
 * =============================================================================
 */

#include "simulate.h"

/* 
 * =============================================================================
 *                            macros
 * =============================================================================
 */

#define TRUE                    1
#define FALSE                   0

/* 
 * =============================================================================
 *                            sampling
 * =============================================================================
 */

size_t
sampling(const double prob[], size_t num_states, const gsl_rng* rng) {
    double random_num = gsl_rng_uniform(rng);
    printf("Random number: %f\n", random_num);
    double cummulative_prob = 0;
    for (size_t i = 0; i < num_states; ++i) {
        cummulative_prob += prob[i];
        printf("Current cummulative prob: %.16f\n", cummulative_prob);
        if (random_num <= cummulative_prob) {
            return i;
        }
    }
    return num_states;
}

/* 
 * =============================================================================
 *                            amplitude amplification
 * =============================================================================
 */

path_t*
ampl_amp(const node_t nodes[], size_t num_states, size_t calls, \
         const gsl_rng* rng) {
    if (nodes == NULL) {
        printf("Reached optimum!\n");
        return NULL;
    }
    double amp_factor;
    double cummulative_prob = 0;
    double prob[num_states + 1];

    for (size_t i = 0; i < num_states; ++i) {
        cummulative_prob += nodes[i].prob;
    }

    amp_factor = pow(sin((2 * calls + 1) * asin(sqrt(cummulative_prob))), 2) \
                 / cummulative_prob;

    cummulative_prob = 0;
    for (size_t i = 0; i < num_states; ++i) {
        cummulative_prob += (prob[i] = nodes[i].prob * amp_factor);
    }

    prob[num_states] = 1. - cummulative_prob;
    /* executing a quantum measurement */
    size_t measurement = sampling(prob, num_states + 1, rng);
    if (measurement == num_states) {
        /* a junkyard state was measured */
        printf("Junkyard state was measured!\n");
        return NULL;
    } else {
        /* a state (path) with improved profit was measured */
        path_t* result = malloc(sizeof(path_t));
        *result = nodes[measurement].path;
        printf("Total profit of state (path) returned by AA: %ld\n", result->tot_profit);
        return result;
    }
}

/* 
 * =============================================================================
 *                            QSearch
 * =============================================================================
 */

path_t*
q_search(const node_t nodes[], size_t num_states, size_t* rounds, \
         size_t max_iter, const gsl_rng* rng) {
    size_t m, j, l, m_tot;
    l = m_tot = 0;
    double c = 6. / 5;

    path_t* sample;
    while(m_tot < max_iter) {
        ++l;
        m = ceil(pow(c, l));
        j = gsl_rng_uniform_int(rng, m);
        *rounds += j;
        m_tot += 2 * j + 1;
        path_t* sample = ampl_amp(nodes, num_states, j, rng);
        if (sample != NULL) {
            return sample;
        }
    }
    printf("QSearch returns Null!\n");
    return NULL;
}

/* 
 * =============================================================================
 *                            QMaxSearch
 * =============================================================================
 */

path_t*
q_max_search(knapsack_t* k, double bias, branch_t method, size_t max_iter, \
             const gsl_rng* rng) {
    size_t rounds; /* counter of AA calls within QSearch; will be updated */
    size_t num_states; /* number of states; will be updated */
    uint64_t timer; /* timer for combo execution, will be updated */

    /* 
     * The knapsack is sorted by ration. Then the integer greedy method is
     * applied to yield the initial current solution and the initial threshold.
     */
    sort_knapsack(k, RATIO);
    apply_int_greedy(k);
    path_t* cur_sol = path_rep(k);
    num_t threshold = k->tot_profit;
    printf("Initial threshold: %zu.\n", cur_sol->tot_profit);
    remove_all_items(k);

    /* obtain optimal solution via combo */
    num_t exact = combo_wrap(k, 0, k->capacity, &timer, FALSE, FALSE, TRUE, TRUE);

    node_t* cur_nodes;
    path_t* cur_path;

    while (TRUE) {
        /*
         * The application of the QTG is simulated. Only states (paths) with 
         * total profit above the threshold are stored in cur_nodes.
         */
        printf("BFS is performed with threshold %ld\n", cur_sol->tot_profit);
        cur_nodes = qtg(k, cur_sol->tot_profit, exact, bias, method, \
                        cur_sol->vector, &num_states);
        printf("%zu states after BFS.\n", num_states);
        for (size_t i = 0; i < num_states; ++i) {
            printf("%zu-th state has total profit of %ld.\n", i + 1, cur_nodes[i].path.tot_profit);
        }
        printf("-----------------\n");
        /*
         * QSearch is executed on the nodes created by the QTG. If this yields a
         * better solution, both the threshold and cur_sol are updated.
         * Otherwise, the routine is interrupted and prior cur_sol is returned.
         */
        cur_path = q_search(cur_nodes, num_states, &rounds, max_iter, rng);
        free(cur_nodes);
        if (cur_path != NULL) {
            free(cur_sol);
            cur_sol = cur_path;
            printf("New threshold: %ld\n", cur_sol->tot_profit);
        } else {
            return cur_sol;
        }
    }
}
