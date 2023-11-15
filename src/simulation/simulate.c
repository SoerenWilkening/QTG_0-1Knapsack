/* 
 * =============================================================================
 *                            includes
 * =============================================================================
 */


#include "simulation/simulate.h"

/* 
 * =============================================================================
 *                            macros
 * =============================================================================
 */

#define TRUE                    1
#define FALSE                   0

#define MIN(a, b)                ((a) < (b) ? (a) : (b))
#define MAX(a, b)                ((a) > (b) ? (a) : (b))

/* 
 * =============================================================================
 *                            sampling
 * =============================================================================
 */

size_t
sampling(const double prob[], size_t num_states, const gsl_rng *rng) {
    double random_num = gsl_rng_uniform(rng);
    double cumulative_prob = 0;
    for (size_t i = 0; i < num_states; ++i) {
        cumulative_prob += prob[i];
        if (random_num <= cumulative_prob) {
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

path_t *
ampl_amp(const node_t nodes[], size_t num_states, size_t calls, \
         const gsl_rng *rng) {
    if (nodes == NULL) {
        return NULL;
    }
    double amp_factor;
    double cumulative_prob = 0;
    double *prob = malloc((num_states + 1) * sizeof(double));

    for (size_t i = 0; i < num_states; ++i) {
        cumulative_prob += nodes[i].prob;
    }

    amp_factor = pow(sin((2 * calls + 1) * asin(sqrt(cumulative_prob))), 2) / cumulative_prob;

    cumulative_prob = 0;
    for (size_t i = 0; i < num_states; ++i) {
        cumulative_prob += (prob[i] = nodes[i].prob * amp_factor);
    }

    prob[num_states] = 1. - cumulative_prob;
    /* executing a quantum measurement */
    size_t measurement = sampling(prob, num_states + 1, rng);
    if (measurement == num_states) {
        free(prob);
        /* a junkyard state was measured */
        return NULL;
    } else {
        /* a state (path) with improved profit was measured */
        path_t *result = malloc(sizeof(path_t));
        result->tot_profit = nodes[measurement].path.tot_profit;
        result->remain_cost = nodes[measurement].path.remain_cost;
        mpz_init(result->vector);
        mpz_set(result->vector, nodes[measurement].path.vector);
        free(prob);
        return result;
    }
}

/* 
 * =============================================================================
 *                            QSearch
 * =============================================================================
 */

path_t *
q_search(const node_t nodes[], size_t num_states, size_t *rounds, \
         size_t *iter, size_t maxiter, const gsl_rng *rng) {
    size_t m, j, l, m_tot;
    l = m_tot = 0;
    double c = 6. / 5;
    *rounds = 0; /* reset rounds counter */
    *iter = 0; /* reset iteration counter */

    path_t *sample;
    while (m_tot < maxiter) {
        ++(*rounds);
        ++l;
        m = ceil(pow(c, l));
        j = gsl_rng_uniform_int(rng, m) + 1;
        *iter += j;
        m_tot += 2 * j + 1;
        sample = ampl_amp(nodes, num_states, j, rng);
        if (sample != NULL) {
            return sample;
        }
    }
    return NULL;
}
