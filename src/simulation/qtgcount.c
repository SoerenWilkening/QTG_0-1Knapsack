/* 
 * =============================================================================
 *                            includes
 * =============================================================================
 */

#include "simulation/qtgcount.h"

/* 
 * =============================================================================
 *                            macros
 * =============================================================================
 */

#define TRUE                    1
#define FALSE                   0

#define MIN(a, b)                ((a) < (b) ? (a) : (b))
#define MAX(a, b)                ((a) > (b) ? (a) : (b))
#define SWAP(a, b, T)           do { register T q; q = *(a); *(a) = *(b); \
                                *(b) = q; } while(0)

/* 
 * =============================================================================
 *                            enum names
 * =============================================================================
 */

const char *
get_qft_name(qft_t method) {
    switch (method) {
        case COPPERSMITH: {
            return "Coppersmith";
        }

        default: {
            return "Unspecified QFT method!";
        }
    }
}

const char *
get_mc_name(mc_t method) {
    switch (method) {
        case TOFFOLI: {
            return "Toffoli";
        }

        default: {
            return "Unspecified multi-control decomposition!";
        }
    }
}

/* 
 * =============================================================================
 *                            number of bits
 * =============================================================================
 */

bit_t
num_bits(num_t number) {
    bit_t result = 0;
    do {
        number >>= 1; /* removes last bit */
        ++result; /* increases counter */
    } while (0 != number);

    return result;
}

/* 
 * =============================================================================
 *                            least signifcant one
 * =============================================================================
 */

bit_t
lso(num_t number) {
    bit_t result = 0;
    while (!(number & 1)) {
        number >>= 1;
        ++result;
    }

    return result;
}

/* 
 * =============================================================================
 *                            counts
 * =============================================================================
 */

bit_t
path_reg_size(const knapsack_t *k) {
    return k->size;
}

bit_t
cost_reg_size(const knapsack_t *k) {
    return num_bits(k->capacity);
}

bit_t
profit_reg_size(const knapsack_t *k, ub_t method) {
    return num_bits(get_ub(k, method));
}

bit_t
anc_count_qft(bit_t reg_size, qft_t method) {
    switch (method) {
        case COPPERSMITH: { /* original QFT version without swaps */
            return 0;
        }

        default: {
            printf("Unspecified QFT method!");
            return -1;
        }
    }
}

count_t
cycle_count_qft(bit_t reg_size, qft_t method, bool_t tof_decomp) {
    switch (method) {
        case COPPERSMITH: { /* original QFT version without swaps */
            return 2 * reg_size - 1;
        }

        default: {
            printf("Unspecified QFT method!");
            return -1;
        }
    }
}

count_t
gate_count_qft(bit_t reg_size, qft_t method, bool_t tof_decomp) {
    switch (method) {
        case COPPERSMITH: { /* original QFT version without swaps */
            return reg_size * (reg_size + 1) / 2;
        }

        default: {
            printf("Unspecified QFT method!");
            return -1;
        }
    }
}

bit_t
anc_count_add(bit_t reg_size, num_t summand) {
    return reg_size - lso(summand) - 1;
}

count_t
cycle_count_add(bit_t reg_size, num_t summand) {
    return 2 * num_bits(reg_size - lso(summand) - 1) + 1;
}

count_t
gate_count_add(bit_t reg_size, num_t summand) {
    return 3 * (reg_size - lso(summand)) - 2;
}

bit_t
anc_count_mc(bit_t control_qubits, mc_t method) {
    switch (method) {
        case TOFFOLI: { /* decompose into cascade of Toffoli gates */
            return control_qubits - 1;
        }

        default: {
            printf("Unspecified multi-control decomposition!");
            return -1;
        }
    }
}

count_t
cycle_count_mc(bit_t control_qubits, mc_t method, bool_t tof_decomp) {
    switch (method) {
        case TOFFOLI: { /* decompose into cascade of Toffoli gates */
            count_t toffoli_cycles = 2 * num_bits(control_qubits - 1);
            return (tof_decomp) ? 5 * toffoli_cycles + 1 : toffoli_cycles + 1;
            /* decomposing Toffolis yields 5 addtional gates per Toffoli gate*/
        }

        default: {
            printf("Unspecified multi-control decomposition!");;
            return -1;
        }
    }
}

count_t
gate_count_mc(bit_t control_qubits, mc_t method, bool_t tof_decomp) {
    switch (method) {
        case TOFFOLI: { /* decompose into cascade of Toffoli gates */
            count_t toffolis = 2 * (control_qubits - 1);
            return (tof_decomp) ? 5 * toffolis + 1 : toffolis + 1;
            /* decomposing Toffolis yields 5 addtional gates per Toffoli gate*/
        }

        default: {
            printf("Unspecified multi-control decomposition!");
            return -1;
        }
    }
}

bit_t
anc_count_comp(bit_t reg_size, num_t to_compare, mc_t method, \
               bool_t unnegated) {
    bit_t i = 0;
    if (unnegated) {
        /* check for rightmost zero in binary represented (to_compare - 1) */
        while (((to_compare - 1) >> i) & 1) {
            ++i;
        }
    } else {
        /* check for rightmost one in binary represented to_compare */
        while (!((to_compare >> i) & 1)) {
            ++i;
        }
    }
    return anc_count_mc(reg_size - i, method);
}

count_t
cycle_count_comp(bit_t reg_size, num_t to_compare, mc_t method, \
                 bool_t unnegated, bool_t tof_decomp) {
    count_t cycle_count;

    if (unnegated) {
        /* check whether being strictly larger than (to_compare - 1) */
        cycle_count = 0;
        for (bit_t i = 0; i < num_bits(to_compare); ++i) {
            if (!(((to_compare - 1) >> i) & 1)) {
                /* invoke multi-controlled single-qubit gate if bit is a zero */
                cycle_count += cycle_count_mc(reg_size - i, method, tof_decomp);
            }
        }
    } else {
        /* check whether being strictly smaller than to_compare */
        cycle_count = 1; /* adding one single-qubit gate in the beginning */
        for (bit_t i = 0; i < num_bits(to_compare); ++i) {
            if ((to_compare >> i) & 1) {
                /* invoke multi-controlled single-qubit gate if bit is a one */
                cycle_count += cycle_count_mc(reg_size - i, method, tof_decomp);
            }
        }
    }
    return cycle_count;
}

count_t cycle_count_comp_new(bit_t reg_size, num_t to_compare, bool_t unnegated, comp_t comp, bool_t precomp) {
    count_t cycle_count = !unnegated;
    bit_t bits = num_bits(to_compare);
    int mc_gates[bits];

    int a = 0;
    if (unnegated) {
        for (int i = bits - 1; i >= 0; --i) {
            if (!(((to_compare - 1) >> i) & 1)) {
                mc_gates[a++] = reg_size - i;
            }
        }
    } else {
        for (int i = bits - 1; i >= 0; --i) {
            if (((to_compare >> i) & 1)) {
                mc_gates[a++] = reg_size - i;
            }
        }
    }

    if (a > 2 && mc_gates[0] > 8 && precomp) {
        cycle_count += 2 * num_bits(mc_gates[0] - 1);
        int all_controls = mc_gates[0];
        for (int i = 0; i < a; ++i) {
            mc_gates[i] -= all_controls;
            mc_gates[i] += 2;
        }
    }

    int x = 0;
    switch (comp) {
        case CASCADING:
            for (int i = 0; i < a; ++i) {
                if (mc_gates[i] == 1) cycle_count += 1;
                else if (mc_gates[i] == 2) cycle_count += 3;
                else if ((mc_gates[i] - x > 1) && i > 0) cycle_count += 2 * ((int) ceil(log2(mc_gates[i] - x + 1))) + 3;
                else if ((mc_gates[i] >= 2) && i == 0) cycle_count += 2 * ((int) ceil(log2(mc_gates[i] - 1))) + 3;
                else cycle_count += 5;
                x = mc_gates[i];
            }
            break;
        case MC_DECOMPOSITION:
            for (int i = 0; i < a; ++i) {
                cycle_count += 2 * num_bits(mc_gates[i] - 1) + 1;
            }
            break;
    }
    return cycle_count;
}

count_t cycle_count_comp_optimal(bit_t reg_size, num_t to_compare) {
    count_t c1 = MIN(cycle_count_comp_new(reg_size, to_compare, TRUE, CASCADING, TRUE),
                     cycle_count_comp_new(reg_size, to_compare, TRUE, CASCADING, FALSE));
    count_t c2 = MIN(cycle_count_comp_new(reg_size, to_compare, FALSE, CASCADING, TRUE),
                     cycle_count_comp_new(reg_size, to_compare, FALSE, CASCADING, FALSE));
    count_t c3 = MIN(cycle_count_comp_new(reg_size, to_compare, TRUE, MC_DECOMPOSITION, TRUE),
                     cycle_count_comp_new(reg_size, to_compare, TRUE, MC_DECOMPOSITION, FALSE));
    count_t c4 = MIN(cycle_count_comp_new(reg_size, to_compare, FALSE, MC_DECOMPOSITION, TRUE),
                     cycle_count_comp_new(reg_size, to_compare, FALSE, MC_DECOMPOSITION, FALSE));

    return MIN(c1, MIN(c2, MIN(c3, c4)));
}


count_t
gate_count_comp(bit_t reg_size, num_t to_compare, mc_t method, \
                bool_t unnegated, bool_t tof_decomp) {
    count_t gate_count;

    if (unnegated) {
        /* check whether being strictly larger than (to_compare - 1) */
        gate_count = 0;
        for (bit_t i = 0; i < num_bits(to_compare); ++i) {
            if (!(((to_compare - 1) >> i) & 1)) {
                /* invoke multi-controlled single-qubit gate if bit is a zero */
                gate_count += gate_count_mc(reg_size - i, method, tof_decomp);
            }
        }
    } else {
        /* check whether being strictly smaller than to_compare */
        gate_count = 1; /* adding one single-qubit gate in the beginning */
        for (bit_t i = 0; i < num_bits(to_compare); ++i) {
            if ((to_compare >> i) & 1) {
                /* invoke multi-controlled single-qubit gate if bit is a one */
                gate_count += gate_count_mc(reg_size - i, method, tof_decomp);
            }
        }
    }
    return gate_count;
}

bit_t
qubit_count_qtg(const knapsack_t *k, ub_t method_ub, qft_t method_qft, \
                mc_t method_mc) {
    bit_t reg_a = path_reg_size(k);
    bit_t reg_b = cost_reg_size(k);
    bit_t reg_c = profit_reg_size(k, method_ub);

    bit_t reg_anc = MAX(reg_a, reg_b);
    reg_anc = MAX(reg_anc, reg_c);

    return reg_a + reg_b + reg_c + reg_anc - 1;
}

count_t
cycle_count_qtg(const knapsack_t *k, ub_t method_ub, qft_t method_qft, \
                mc_t method_mc, bool_t tof_decomp) {
    bit_t reg_b = cost_reg_size(k);
    bit_t reg_c = profit_reg_size(k, method_ub);
    bit_t break__item = break_item(k);

    count_t cost_qft = cycle_count_qft(reg_b, method_qft, tof_decomp);
    count_t profit_qft = cycle_count_qft(reg_c, method_qft, tof_decomp);

    /*
     * first block consiting of:
     * - first comparison on the cost register for controlled operation on the
     *   path register
     * - QFT on the cost register
     * - first subtraction on the cost register
     * - inverse QFT on the cost register
     * - QFT on the profit register
     * - first addition on the profit register
     */
    count_t first_block;
    /* check whether unnegated or netaged first comparison is more efficient */

    /* first item can always be included => no comparison needed */
    if (reg_b > reg_c) {
        if (break__item == 0) {
            first_block = 2 * cost_qft + 1;
        } else {
            first_block = cost_qft + 1;
        }
    } else {
        if (break__item == 0) {
            first_block = profit_qft + cost_qft;
        } else {
            first_block = profit_qft + 1;
        }

    }

    /* 
     * second block consisting of k->size - 2 similar blocks, each consiting of:
     * - comparison on the cost register for controlled operation on the path
     *   register (only after break item)
     * - QFT on the cost register (only after break item)
     * - subtraction on the cost register
     * - inverse QFT on the cost register (starting at break item)
     * - addition on the profit register
     */
    count_t second_block = 0;
    count_t second_comp;
    count_t second_sub;
    count_t second_add;
    bit_t i;
    for (i = 1; i < break__item; ++i) {
        second_block += 2 * MAX(num_bits(reg_b - lso(k->items[i].cost) - 1), \
                                num_bits(reg_c - lso(k->items[i].profit) - 1)) + 2;
        /*
         * for each item: Since adding on the profit register can be
         * parallelized with the QFT and the inverse QFT on the cost register,
         * the maximum of their lengths rather than their sum contributes. Since
         * comparison and subtraction on the cost register canot be parallelized
         * further, both their lenghts have to be added.
         */
    }

    second_block += MAX(num_bits(reg_b - lso(k->items[break__item].cost) - 1), \
                        num_bits(reg_c - lso(k->items[break__item].profit) - 1)) \
 + 1 + cost_qft;

    for (i = break__item + 1; i < k->size - 1; ++i) {
        /*
         * for each item, use the  most efficient comparison method
         */
        second_comp = cycle_count_comp_optimal(reg_b, ((k->items)[i]).cost);

        second_block += second_comp + 2 * cost_qft + 1;
        /* 
         * for each item: Since adding on the profit register can be
         * parallelized with the QFT and the inverse QFT on the cost register,
         * the maximum of their lengths rather than their sum contributes. Since
         * comparison and subtraction on the cost register canot be parallelized
         * further, both their lenghts have to be added.
         */
    }

    /*
     * third block consisting of:
     * - last comparison on the cost register for controlled operation on the
     *   path register
     * - last addition on the profit register
     * - inverse QFT on the profit register
     * Note: the last subtraction together with the QFT and inverse QFT on the
     *       cost register can be dropped, since now further comparisons have to
     *       be executed and the cost register is not of independent interest
     *       for further applications.
     */
    count_t third_block;

    /* use most efficient comparison */
    count_t last_comp = cycle_count_comp_optimal(reg_b, ((k->items)[k->size - 1]).cost);

    count_t last_add = num_bits(reg_c - lso(k->items[k->size - 1].profit) - 1) + 1;
    /*
     * Since last comparison, last addition, and inverse QFT on the profit
     * register are not parallelizable, all their individual lengths have to be
     * added together.
     */
    third_block = last_comp + last_add + profit_qft;

    return first_block + second_block + third_block;
}

count_t
gate_count_qtg(const knapsack_t *k, ub_t method_ub, qft_t method_qft, \
               mc_t method_mc, bool_t tof_decomp) {
    bit_t reg_b = cost_reg_size(k);
    bit_t reg_c = profit_reg_size(k, method_ub);
    count_t sub_gates = 0;
    count_t add_gates = 0;
    count_t comp_gates = 0;
    bit_t break__item = break_item(k);
    for (bit_t i = break__item + 1; i < k->size; ++i) {
        comp_gates += MIN(gate_count_comp(reg_b, k->items[i].cost, method_mc, \
                                          1, tof_decomp), \
                          gate_count_comp(reg_b, k->items[i].cost, method_mc, \
                                          0, tof_decomp));
    }

    for (bit_t i = 0; i < k->size - 1; ++i) {
        add_gates += 2 * (MAX(reg_b, reg_c) - \
                          MIN(lso(k->items[i].profit), lso(k->items[i].cost)) - 1);
        sub_gates += reg_b - lso(k->items[i].cost);
        add_gates += reg_c - lso(k->items[i].profit);
    }
    /*
     * Since last subtraction is omitted, for the last item, only the
     * addition is considered.
     */
    add_gates += gate_count_add(reg_c, k->items[k->size - 1].profit);

    return comp_gates /* all comparison gates */ \
 + (2 * ((count_t) (k->size) - 1 - break__item - 1) + 1)
   * gate_count_qft(reg_b, method_qft, tof_decomp) /* (un)QFT-ing cost register */ \
 + sub_gates /* all subtraction gates */
           /* (un)QFT-ing profit register */ \
 + 2 * gate_count_qft(reg_c, method_qft, tof_decomp) \
 + add_gates; // all addition gates
}

void
print_qtg_counts(const knapsack_t *k, ub_t method_ub, qft_t method_qft,
                 mc_t method_mc, bool_t tof_decomp) {
    printf("The QTG ressources are estimated with the following methods:\n");
    printf("Upper bound on the total profit (for profit register size): %s\n", \
           get_ub_name(method_ub));
    printf("Implementation of the quantum Fourier transform (QFT): %s\n", \
           get_qft_name(method_qft));
    printf("Decomposition of multi-controlled single-qubit-gates: %s\n", \
           get_mc_name(method_mc));
    printf("Toffoli gates have%s been decomposed.\n--------\n", \
           (tof_decomp) ? "" : " not");
    printf("Number of qubits (main + ancilla registers): %u\n", \
           qubit_count_qtg(k, method_ub, method_qft, method_mc));
    printf("Number of cycles: %u\n", cycle_count_qtg(k, method_ub, method_qft, \
                                                     method_mc, tof_decomp));
    printf("Number of gates: %u\n\n", gate_count_qtg(k, method_ub, method_qft, \
                                                     method_mc, tof_decomp));
}

count_t cycleCountQbnbDiffucsionOperator(knapsack_t *k) {
    count_t cycle_count = 0;
    count_t single_count;
    int frac_register = profit_reg_size(k, 0);
    int cap_register = cost_reg_size(k);
//    int cap_register = 34;

    for (int item = 0; item < k->size; ++item) {
        single_count = 0;
        /* Adding up the profits of all the already assigned items
         * also the remaining capacities have to be calculated
         * When fractional greedy for both assignments is calculated in parallel,
         * the profit and the remaining cost has to be calculated on seperate registers */
        single_count += cycle_count_qft(MAX(frac_register, cap_register), COPPERSMITH, 0);
        for (int i = 0; i < item; ++i) {
            single_count += 2 * num_bits(frac_register) + 4;
        }
        single_count += cycle_count_qft(MAX(frac_register, cap_register), COPPERSMITH, 0);

        /* apply fractional greedy.
         * we assume, we can compute fractional greedy for both assignments in parallel
         * only the longer circuit accounts to the total cycle count */
        if (item < k->size - 1)
            /* fractional greedy will only be applied, if there is at least one non-assigned variable */
            single_count += MAX(cycle_count_quantum_fractional_greedy(k, item, 0),
                                cycle_count_quantum_fractional_greedy(k, item, 1));
        cycle_count = MAX(cycle_count, single_count);

    }

    /* In the diffusion operator the upper bound has to be reverted */
    return 2 * cycle_count;
}


count_t cycle_count_quantum_fractional_greedy(knapsack_t *k, int first_item, int assignment) {
    /* if the first item is assigned to 0 we can already skip to the next one */
    int item = first_item + (1 - assignment);
    int b = break_item(k);
    int cost_reg = cost_reg_size(k);

    count_t cycle_count = 0;

    for (int i = item; i < b; i++) {
        /* Check, if item can be included */
        cycle_count += cycle_count_comp_optimal(cost_reg, k->items[item].cost);

        /* Subtract the weight */
        cycle_count += 2 * cycle_count_qft(cost_reg, COPPERSMITH, 0) + 3;
    }

    return cycle_count;
}