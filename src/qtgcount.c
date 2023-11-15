/* 
 * =============================================================================
 *                            includes
 * =============================================================================
 */

#if defined(_WIN32) || defined(_WIN64)
    #include "..\include\qtgcount.h"
#else
    #include "../include/qtgcount.h"
#endif

/* 
 * =============================================================================
 *                            macros
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
 *                            enum names
 * =============================================================================
 */

const char*
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

const char*
get_add_name(add_t method) {
    switch (method) {
        case DRAPER: {
            return "Draper";
        }

        case DIRECT: {
            return "Direct";
        }

        case COPYDIRECT: {
            return "Copy&Direct"
        }

        default: {
            return "Unspecified adder method!";
        }
    }
}

const char*
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
path_reg_size(const knapsack_t* k) {
    return k->size;
}

bit_t
cost_reg_size(const knapsack_t* k) {
    return num_bits(k->capacity);
}

bit_t
profit_reg_size(const knapsack_t* k, ub_t method) {
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
anc_count_add(bit_t reg_size, num_t summand, add_t method) {
    switch (method) {
        case DRAPER: { /* rotational gates are controlled on summands bits */
            return num_bits(summand); 
        }

        case DIRECT: { /* rotational gates are directly implemented */
            return 0; 
        }

        case COPYDIRECT: {
            return reg_size - lso(summand) - 1;
        }

        default: {
            printf("Unspecified adder method!");
            return -1;
        }
    }
}

count_t
cycle_count_add(bit_t reg_size, num_t summand, add_t method, \
                bool_t tof_decomp) {
    bit_t summand_size = num_bits(summand);
    switch (method) {
        case DRAPER: { /* rotational gates are controlled on summands bits */
            return reg_size; 
        }

        case DIRECT: { /* rotational gates are directly implemented */
            count_t result = 0;
            for (bit_t i = 0; i < summand_size; ++i) {
                result += (summand >> i) & 1;
            }
            return result;
        }

        case COPYDIRECT: {
            return 2 * num_bits(reg_size - lso(summand) - 1) + 1;
        }

        default: {
            printf("Unspecified adder method!");
            return -1;
        }
    }
}

count_t
gate_count_add(bit_t reg_size, num_t summand, add_t method, bool_t tof_decomp) {
    bit_t summand_size = num_bits(summand);
    switch (method) {
        case DRAPER: { /* rotational gates are controlled on summands bits */
            return summand_size * (2 * reg_size + 1 - summand_size) / 2;
        }

        case DIRECT: { /* rotational gates are directly implemented */
            count_t result = 0;
            for (bit_t i = 0; i < summand_size; ++i) {
                result += (reg_size - i) * ((summand >> i) & 1);
            }
            return result;
        }

        case COPYDIRECT: {
            return 3 * (reg_size - lso(summand)) - 2;
        }

        default: {
            printf("Unspecified adder method!");
            return -1;
        }
    }
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
            if (!(((to_compare - 1)>> i) & 1)) {
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

count_t
gate_count_comp(bit_t reg_size, num_t to_compare, mc_t method, \
                bool_t unnegated, bool_t tof_decomp) {
    count_t gate_count;

    if (unnegated) {
        /* check whether being strictly larger than (to_compare - 1) */
        gate_count = 0;
        for (bit_t i = 0; i < num_bits(to_compare); ++i) {
            if (!(((to_compare - 1)>> i) & 1)) {
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
qubit_count_qtg(const knapsack_t* k, ub_t method_ub, qft_t method_qft, \
                add_t method_add, mc_t method_mc) {
    bit_t reg_a = path_reg_size(k);
    bit_t reg_b = cost_reg_size(k);
    bit_t reg_c = profit_reg_size(k, method_ub);

    bit_t reg_anc = MAX(reg_a, reg_b);
    reg_anc = MAX(reg_anc, reg_c);

    return reg_a + reg_b + reg_c + reg_anc - 1;
}

count_t
cycle_count_qtg(const knapsack_t* k, ub_t method_ub, qft_t method_qft, \
                add_t method_add, mc_t method_mc, bool_t tof_decomp) {
    bit_t reg_b = cost_reg_size(k);
    bit_t reg_c = profit_reg_size(k, method_ub);

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
    count_t first_comp = MIN(cycle_count_comp(reg_b, k->items[0].cost, \
                                              method_mc, TRUE, tof_decomp), \
                             cycle_count_comp(reg_b, k->items[0].cost, \
                                              method_mc, FALSE, tof_decomp));
    count_t first_sub;
    count_t first_add;

    switch (method_add) {
        case DRAPER: { /* rotational gates are controlled on summand's bits */
            first_sub = 5 * gate_count_add(reg_b, max_cost(k), DRAPER, \
                                           tof_decomp);
            first_add = 5 * gate_count_add(reg_c, max_profit(k), DRAPER, \
                                           tof_decomp);
            /* 
             * additional control on path register qubit creates two-controlled
             * gates:
             * 1. parallelization is impossible, thus cycles match gates
             * 2. two-controlled gates have to be decomposed into 5
             *    single-controlled gates
             */
            break;
        }

        case DIRECT: { /* rotational gates are directly implemented */
            first_sub = gate_count_add(reg_b, k->items[0].cost, DIRECT, \
                                       tof_decomp);
            first_add = gate_count_add(reg_c, k->items[0].profit, DIRECT, \
                                       tof_decomp);
            /* 
             * additional control on path register qubit creates two-controlled
             * gates: parallelization is impossible, thus cycles match gates
             */
            break;
        }

        case COPYDIRECT: {
            goto first_block_copy;
            break;
        }

        default: {
            return -1;
        }
    }

    if (profit_qft <= first_comp) {
        /* 
         * QFT on profit register is executed in parallel to the first
         * comparison. Since adding on the profit register and QFT-ing the cost
         * register (both QFT and inverse QFT) can be parallelized, their
         * maximum length rather than the sum of their lengths contributes.
         * Both, comparison and subtraction on the cost register, cannot be
         * parallelized further. Thus, their lengths have to be added.
         */
        first_block = first_comp + MAX(2 * cost_qft, first_add) + first_sub;
    } else if (profit_qft <= first_comp + cost_qft) {
        /* 
         * QFT on profit register is executed partially during QFT on the cost
         * register. Since adding on the profit register can be parallelized
         * with the remaining cycles of the QFT on the cost register and the
         * full inverse QFT on the cost register, the maximum length of first
         * executing the profit QFT and than the addition, and first executing
         * the comparison and than both cost QFTs is taken rather than the sum
         * of their lengths. Since the subtraction on the cost register cannot
         * be parallelized further, its full length has to be added to the total
         * length.
         */
        first_block = MAX(profit_qft + first_add, first_comp + 2 * cost_qft) \
                      + first_sub;
    } else if (profit_qft <= first_comp + cost_qft + first_sub) {
        /* 
         * QFT on profit register is executed partially during subtraction on
         * cost register. Since adding on the profit register can only be 
         * parallelized with the remaining inverse QFT on the cost register, the
         * maximum of their lengths rather than the sum of their lengths
         * contributes. Also the first comparison, the QFT on the cost register,
         * and the subtraction on the cost register have to be executed in this
         * order, resulting in adding all their lengths separately.
         */
        first_block = first_comp + cost_qft + first_sub \
                      + MAX(first_add, cost_qft);
    } else {
        /* 
         * QFT on the profit register requires more cycles than first
         * comparison, QFT on the cost register, and first subtraction on the
         * cost register together. The subsequent adding can be parallelized
         * with cycles of the inverse QFT on the cost register, not covered by
         * the QFT on the profit register. Thus, the maximum of the two
         * register's circuits rather than their sum gives the correct result. 
         */
        first_block = MAX(profit_qft + first_add, first_comp + first_sub \
                                                  + 2 * cost_qft);
    }
    goto first_block_noncopy;

first_block_copy:
    if (reg_b > reg_c) {
        first_block = first_comp + cost_qft + profit_qft;
    } else {
        first_block = first_comp + 2 * cost_qft + 1;
    }

    /* 
     * second block consisting of k->size - 2 similar blocks, each consiting of:
     * - comparison on the cost register for controlled operation on the path
     *   register
     * - QFT on the cost register
     * - subtraction on the cost register
     * - inverse QFT on the cost register
     * - inverse QFT on the cost register
     * - addition on the profit register
     */
first_block_noncopy:
    count_t second_block = 0;
    count_t second_comp;
    count_t second_sub;
    count_t second_add;

    for (bit_t i = 1; i < k->size - 1; ++i) {
        /*
         * for each item, check whether unnegated or netaged comparison is more
         * efficient
         */
        second_comp = MIN(cycle_count_comp(reg_b, ((k->items)[i]).cost, \
                                           method_mc, TRUE, tof_decomp), \
                          cycle_count_comp(reg_b, ((k->items)[i]).cost, \
                                           method_mc, FALSE, tof_decomp));

        switch (method_add) {
            case DRAPER: {
                /* rotational gates are controlled on summand's bits */
                second_sub = 5 * gate_count_add(reg_b, max_cost(k), DRAPER, \
                                                tof_decomp);
                second_add = 5 * gate_count_add(reg_c, max_profit(k), DRAPER, \
                                                tof_decomp);
                /* 
                 * additional control on path register qubit creates
                 * two-controlled gates:
                 * 1. parallelization is impossible, thus cycles match gates
                 * 2. two-controlled gates have to be decomposed into 5
                 *    single-controlled gates
                 */
                second_block += second_comp + second_sub + MAX(2 * cost_qft,
                                                               second_add);
                break;
            }

            case DIRECT: {
                /* rotational gates are directly implemented */
                second_sub = gate_count_add(reg_b, k->items[i].cost, DIRECT, \
                                            tof_decomp);
                second_add = gate_count_add(reg_c, k->items[i].profit, DIRECT, \
                                            tof_decomp);
                /* 
                 * additional control on path register qubit creates
                 * two-controlled gates: parallelization is impossible, thus
                 * cycles match gates
                 */
                second_block += second_comp + second_sub + MAX(2 * cost_qft,
                                                               second_add);
                break;
            }

            case COPYDIRECT: {
                second_block += second_comp + 2 * cost_qft + 1;
                break;
            }

            default: {
                return -1;
            }
        }
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
    /* check whether unnegated or netaged last comparison is more efficient */
    count_t last_comp = MIN(cycle_count_comp(reg_b, \
                                             k->items[k->size - 1].cost, \
                                             method_mc, TRUE, tof_decomp), \
                            cycle_count_comp(reg_b, \
                                             k->items[k->size - 1].cost, \
                                             method_mc, FALSE, tof_decomp));
    count_t last_add;

    switch (method_add) {
        case DRAPER: { /* rotational gates are controlled on summand's bits */
            last_add = 5 * gate_count_add(reg_c, max_profit(k), DRAPER, \
                                          tof_decomp);
            /* 
             * additional control on path register qubit creates two-controlled
             * gates:
             * 1. parallelization is impossible, thus cycles match gates
             * 2. two-controlled gates have to be decomposed into 5
             *    single-controlled gates
             */
            break;
        }

        case DIRECT: { /* rotational gates are directly implemented */
            last_add = gate_count_add(reg_c, k->items[k->size - 1].profit, \
                                      DIRECT, tof_decomp);
            /* 
             * additional control on path register qubit creates two-controlled
             * gates: parallelization is impossible, thus cycles match gates
             */
            break;
        }

        case COPYDIRECT: {
            last_add = num_bits(reg_c - lso(k->items[k->size - 1].profit) - 1) + 1;
            break;
        }

        default: {
            return -1;
        }
    }
    /*
     * Since last comparison, last addition, and inverse QFT on the profit
     * register are not parallelizable, all their individual lengths have to be
     * added together.
     */
    third_block = last_comp + last_add + profit_qft;

    return first_block + second_block + third_block;
}

count_t
gate_count_qtg(const knapsack_t* k, ub_t method_ub, qft_t method_qft, \
               add_t method_add, mc_t method_mc, bool_t tof_decomp) {
    bit_t reg_b = cost_reg_size(k);
    bit_t reg_c = profit_reg_size(k, method_ub);
    count_t sub_gates = 0;
    count_t add_gates = 0;
    count_t comp_gates = 0;
    for (bit_t i = 0; i < k->size; ++i) {
        comp_gates += MIN(gate_count_comp(reg_b, k->items[i].cost, method_mc, \
                                          1, tof_decomp), \
                          gate_count_comp(reg_b, k->items[i].cost, method_mc, \
                                          0, tof_decomp));
    }

    switch (method_add) {
        case DRAPER: { /* rotational gates are controlled on summands bits */
            sub_gates += 5 * gate_count_add(reg_b, max_cost(k), DRAPER, \
                                            tof_decomp) \
                         * (((count_t)k->size) - 1);
            add_gates += 5 * gate_count_add(reg_c, max_profit(k), DRAPER, \
                                            tof_decomp) \
                         * ((count_t)(k->size));
            /* 
             * All addition and subtraction cycles have the same length. Since
             * the last subtraction is omitted, the number of subtraction
             * circuits is reduced by one.
             */
            break;
        }

        case DIRECT: { /* rotational gates are directly implemented */
            for (bit_t i = 0; i < k->size - 1; ++i) {
                sub_gates += gate_count_add(reg_b, k->items[i].cost, DIRECT, \
                                            tof_decomp);
                add_gates += gate_count_add(reg_c, k->items[i].cost, DIRECT, \
                                            tof_decomp);
            }
            /* 
             * Since last subtraction is omitted, for the last item, only the
             * addition is considered.
             */
            add_gates += gate_count_add(reg_c, k->items[k->size - 1].profit, \
                                        DIRECT, tof_decomp);
            break;
        }

        case COPYDIRECT: {
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
            add_gates += gate_count_add(reg_c, k->items[k->size - 1].profit, \
                                        COPYDIRECT, tof_decomp);
            break;
        }

        default: {
            return -1;
        }
    }
    return comp_gates /* all comparison gates */ \
           + 2 * gate_count_qft(reg_b, method_qft, tof_decomp) \
             * ((count_t)(k->size) - 1) /* (un)QFT-ing cost register */ \
           + sub_gates /* all subtraction gates */ 
           /* (un)QFT-ing profit register */ \
           + 2 * gate_count_qft(reg_c, method_qft, tof_decomp) \
           + add_gates; // all addition gates
}

void
print_qtg_counts(const knapsack_t* k, ub_t method_ub, qft_t method_qft,
                 add_t method_add, mc_t method_mc, bool_t tof_decomp) {
    printf("The QTG ressources are estimated with the following methods:\n");
    printf("Upper bound on the total profit (for profit register size): %s\n", \
           get_ub_name(method_ub));
    printf("Implementation of the quantum Fourier transform (QFT): %s\n", \
           get_qft_name(method_qft));
    printf("Implementation of the adder/subtractor: %s\n", \
           get_add_name(method_add));
    printf("Decomposition of multi-controlled single-qubit-gates: %s\n", \
           get_mc_name(method_mc));
    printf("Toffoli gates have%s been decomposed.\n--------\n", \
           (tof_decomp) ? "" : " not");
    printf("Number of qubits (main + ancilla registers): %u\n", \
           qubit_count_qtg(k, method_ub, method_qft, method_add, method_mc));
    printf("Number of cycles: %u\n", cycle_count_qtg(k, method_ub, method_qft, \
                                                     method_add, method_mc, \
                                                     tof_decomp));
    printf("Number of gates: %u\n\n", gate_count_qtg(k, method_ub, method_qft, \
                                                     method_add, method_mc, \
                                                     tof_decomp));
}
