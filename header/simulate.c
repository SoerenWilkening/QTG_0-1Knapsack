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

#define MIN(a,b)                ((a) < (b) ? (a) : (b))
#define MAX(a,b)                ((a) > (b) ? (a) : (b))

/* 
 * =============================================================================
 *                            sampling
 * =============================================================================
 */

size_t
sampling(const double prob[], size_t num_states, const gsl_rng* rng) {
    double random_num = gsl_rng_uniform(rng);
    double cummulative_prob = 0;
    for (size_t i = 0; i < num_states; ++i) {
        cummulative_prob += prob[i];
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
        return NULL;
    } else {
        /* a state (path) with improved profit was measured */
        path_t* result = malloc(sizeof(path_t));
        result->tot_profit = nodes[measurement].path.tot_profit;
        result->remain_cost = nodes[measurement].path.remain_cost;
        mpz_init(result->vector);
        mpz_set(result->vector, nodes[measurement].path.vector);
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
         size_t* iter, size_t maxiter, const gsl_rng* rng) {
    size_t m, j, l, m_tot;
    l = m_tot = 0;
    double c = 6. / 5;
    *rounds = 0; /* reset rounds counter */
    *iter = 0; /* reset iteration counter */
    
    path_t* sample;
    while(m_tot < maxiter) {
        ++(*rounds);
        ++l;
        m = ceil(pow(c, l));
        j = gsl_rng_uniform_int(rng, m);
        *iter += j;
        m_tot += 2 * j + 1;
        path_t* sample = ampl_amp(nodes, num_states, j, rng);
        if (sample != NULL) {
            return sample;
        }
    }
    return NULL;
}

/* 
 * =============================================================================
 *                            QMaxSearch
 * =============================================================================
 */

path_t*
q_max_search(knapsack_t* k, size_t bias, branch_t method, size_t maxiter, \
             const gsl_rng* rng) {
    size_t rounds; /* counter of AA rounds within QSearch; will be updated */
    size_t iter; /* counter of AA calls within QSearch; will be updated */
    size_t num_states; /* number of states; will be updated */

    path_t* cur_sol; /* current solution; will be updated */
    bit_t profit_qubits; /* size of the profit register */
    /*
     * declare cycle and gate counter for the QTG with and without decomposing
     * arising Toffoli gates
     */
    count_t qtg_cycles, qtg_gates, qtg_cycles_decomp, qtg_gates_decomp;
    num_t exact; /* exact optimal profit for the knapsack instance */

    node_t* cur_nodes; /* set of states after applying QTG and filtering */
    path_t* cur_path; /* current result of applying QSearch */

    FILE* stream;
    char instancename[256];
    char filename[256];
    char line[128];

    /* 
     * The knapsack is sorted by ration. Then the integer greedy method is
     * applied to yield the initial current solution and the initial threshold.
     */
    sort_knapsack(k, RATIO);
    apply_int_greedy(k);
    cur_sol = path_rep(k);
    remove_all_items(k);

    /*
     * A resource counter is initialized. The number of required qubits is given
     * by the total number of qubits on all three registers of the QTG plus one
     * for saving the AA phase flip, plus the number of ancilla qubits required
     * to decompose the AA flip operator. The cycle and gate counts are
     * initialized to zero and will be updated after every call to QSearch.
     */
    profit_qubits = profit_reg_size(k, FGREEDY);
    resource_t res = { .qubit_count = qubit_count_qtg(k, FGREEDY, COPPERSMITH, \
                                                 DIRECT, TOFFOLI) + 1 \
                                 + anc_count_mc(profit_qubits, TOFFOLI),
                       .cycle_count = 0, .gate_count = 0, \
                       .cycle_count_decomp = 0, .gate_count_decomp = 0};
    qtg_cycles = cycle_count_qtg(k, FGREEDY, COPPERSMITH, DIRECT, \
                                         TOFFOLI, FALSE);
    qtg_gates = gate_count_qtg(k, FGREEDY, COPPERSMITH, DIRECT, \
                                         TOFFOLI, FALSE);
    qtg_cycles_decomp = cycle_count_qtg(k, FGREEDY, COPPERSMITH, DIRECT, \
                                         TOFFOLI, TRUE);
    qtg_gates_decomp = gate_count_qtg(k, FGREEDY, COPPERSMITH, DIRECT, \
                                         TOFFOLI, TRUE);

    /* obtain optimal solution via Combo */
    exact = combo_wrap(k, 0, k->capacity, FALSE, FALSE, TRUE);
    do {
        /*
         * The application of the QTG is simulated. Only states (paths) with 
         * total profit above the threshold are stored in cur_nodes.
         */
        cur_nodes = qtg(k, cur_sol->tot_profit, exact, bias, method, \
                        cur_sol->vector, &num_states);
        /*
         * QSearch is executed on the nodes created by the QTG. If this yields a
         * better solution, cur_sol (carrying the current threshold) is updated.
         * Otherwise, the routine is interrupted and prior cur_sol is returned.
         */
        cur_path = q_search(cur_nodes, num_states, &rounds, &iter, maxiter, \
                            rng);
        if (cur_nodes != NULL) {
            free_nodes(cur_nodes, num_states);
        }
        /* update cycle and gate count after application of QSearch */
        res.cycle_count += (rounds + 2 * iter) * qtg_cycles;
        res.cycle_count_decomp += (rounds + 2 * iter) * qtg_cycles_decomp;
        res.cycle_count += cycle_count_mc(profit_qubits, TOFFOLI, FALSE);
        res.cycle_count_decomp += cycle_count_mc(profit_qubits, TOFFOLI, TRUE);
        res.cycle_count += MIN(cycle_count_comp(profit_qubits, \
                               cur_sol->tot_profit, TOFFOLI, TRUE, FALSE), \
                               cycle_count_comp(profit_qubits, \
                               cur_sol->tot_profit, TOFFOLI, FALSE, FALSE));
        res.cycle_count_decomp += MIN(cycle_count_comp(profit_qubits, \
                                      cur_sol->tot_profit, TOFFOLI, TRUE, \
                                      TRUE), cycle_count_comp(profit_qubits, \
                                      cur_sol->tot_profit, TOFFOLI, FALSE, \
                                      TRUE));
        res.gate_count += (rounds + 2 * iter) * qtg_gates;
        res.gate_count_decomp += (rounds + 2 * iter) * qtg_gates_decomp;
        res.gate_count += gate_count_mc(profit_qubits, TOFFOLI, FALSE);
        res.gate_count_decomp += gate_count_mc(profit_qubits, TOFFOLI, TRUE);
        res.gate_count += MIN(gate_count_comp(profit_qubits, \
                              cur_sol->tot_profit, TOFFOLI, TRUE, TRUE),
                              gate_count_comp(profit_qubits, \
                              cur_sol->tot_profit, TOFFOLI, FALSE, TRUE));
        res.gate_count_decomp += MIN(gate_count_comp(profit_qubits, \
                                     cur_sol->tot_profit, TOFFOLI, TRUE, \
                                     TRUE), gate_count_comp(profit_qubits, \
                                     cur_sol->tot_profit, TOFFOLI, FALSE, \
                                     TRUE));
        if (cur_path != NULL) {
            free_path(cur_sol);
            cur_sol = cur_path;
        } else {
            snprintf(instancename, sizeof(instancename), "instances%c%s%cQTG", \
                     path_sep(), k->name, path_sep());
            snprintf(filename, sizeof(filename), "%s%cqtg_statistics_bias=%zu" \
                     "_maxiter=%zu.csv", instancename, path_sep(), bias, \
                     maxiter);
            create_dir(instancename);
            stream = fopen(filename, "a");
            fprintf(stream, "%"PRIu64" %"PRIu64" %"PRIu64" %"PRIu64" %"PRIu64 \
                    " %d\n", (uint64_t)res.qubit_count, \
                    (uint64_t)res.cycle_count, (uint64_t)res.gate_count, \
                    (uint64_t)res.cycle_count_decomp, \
                    (uint64_t)res.gate_count_decomp, \
                    (cur_sol->tot_profit == exact) ? 1 : 0);
            fclose(stream);
            return cur_sol;
        }
    } while (TRUE);
}
