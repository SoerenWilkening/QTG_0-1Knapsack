#include <cstdlib>
#include <iostream>
#include <vector>
#include <random>
#include <stdexcept>

size_t
sampling(const double prob[], size_t num_states, const gsl_rng* rng) {
    double random_num = gsl_rng_uniform(r);
    double cummulative_prob = 0;
    for (size_t i = 0; i < num_states; ++i) {
        cummulative_prob += prob[i];
        if (random_num <= cummulative_prob) {
            return i;
        }
    }
}

state_t*
ampl_amp(const node_t* nodes, size_t num_states, size_t calls \
         const gsl_rng* rng) {
    
    double amp_factor;
    double cummulative_prob = 0;
    double prob[num_states + 1];

    for (size_t i = 0; i < num_states) {
        total_prob *= nodes[i].prob;
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
        return NULL;
    } else {
        /* a state with improved profit was measured */
        return &(nodes[i].state)
    }
}

state_t*
q_search(const node_t* nodes, size_t num_states, size_t* rounds, \
         size_t max_iter, const gsl_rng* rng) {
    size_t m, j;
    size_t l = m_tot = 0;
    double c = 6. / 5;

    state_t* sample;
    while(m_tot < max_iter) {
        ++l;
        m = ceil(pow(c, l));
        j = gsl_rng_uniform_int(rng, m);
        *rounds += j;
        m_tot += 2 * j + 1;
        state_t* sample = ampl_amp(nodes, num_states, threshold, j, rng);
        if (sample != NULL) {  
            return sample;
        }
    }
    return NULL;
}

state_t*
q_max_search(knapsack_t* k, size_t* rounds, double bias, branch_t method, \
             size_t max_iter, const gsl_rng* rng) {
    /* reset round counter, initialize bit string for current solution */
    *rounds = 0;
    size_t num_states; /* number of states will be updated */
    mpz_t cur_sol;
    mpz_init2(cur_sol, k->size);

    /* 
     * The knapsack is sorted by ration. Then the integer greedy method is
     * applied to yield the initial current solution and the initial threshold.
     */
    sort_knapsack(k, RATIO);
    apply_int_greedy(k);
    num_t threshold = k->tot_profit;
    bit_rep(k, cur_sol);
    remove_all_items(k);

    /* store integer greedy solution as a state */
    state_t* cur_sol_state = malloc(sizeof(state_t));
    cur_sol_state->tot_profit = threshold;
    cur_sol_state->vecctor = cur_sol;

    /* obtain optimal solution via combo */
    num_t exact = combo_wrap(k, 0, k->capacity, FALSE, FALSE, TRUE, NULL);

    /*
     * The application of the QTG is simulated. Only states with total profit
     * above the threshold are stored in cur_nodes.
     */
    node_t* cur_nodes = breadth_first_search(k, threshold, exact, bias, method,
                                             cur_sol, &num_states);

    /*
     * QSearch is executed on the nodes created by breadth-first search. If this
     * yields a better solution, both the threshold and cur_sol are updated.
     * Otherwise, the routine is interrupted and the last cur_sol is returned.
     */
    state_t* cur_state = q_search(cur_nodes, num_states, rounds, max_iter, rng);
    if (cur_state != NULL) {

    }
}
