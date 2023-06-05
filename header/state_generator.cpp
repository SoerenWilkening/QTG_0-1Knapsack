//
// Created by Sören Wilkening on 10.05.23.
//

#include "state_generator.h"

std::vector<state_node> initialize_states(int number, int bits) {
    std::vector<state_node> result(number);

//    result.states = calloc(number, sizeof(state_node));

    for (int i = 0; i < number; i++) {
        result[i].amplitude = 1.;
        result[i].P = 0;
        result[i].Z = 0;
        mpz_init2(result[i].vector, bits);
//        result[i].vector = 0.;
        result[i].ub = 0;
    }
    return result;
}

std::vector<state_node> breadth_first_search(knapsack_t* k,
                                             num_t threshold,
                                             num_t exact,
                                             double bias,
                                             std::string states,
                                             mpz_t previous_sol) {
    /**
     * TODO:
     *  - include calculations of amplitudes
     *  - enable multiprocessing
     * */
//    std::cout << "executing bnb" << std::endl;

    size_t num_states = 1, a;
    std::vector<state_node> parent = initialize_states(num_states, k->size);
    parent[0].ub = exact;
    parent[0].capacity = k->capacity;
    mpz_setbit(parent[0].vector, 1);
    num_t zzz;
    double discarded = 0;
    long* t = static_cast<long *>(calloc(1, sizeof(long)));
    for (bit_t i = 0; i < k->size; ++i) {
        a = 0;
        std::vector<state_node> children = initialize_states(2 * num_states, k->size);

        for (size_t j = 0; j < num_states; ++j) {
            /* checking if item cannot be included */
            if (parent[j].capacity < k->items[i].cost) children[a++] = parent[j];
            else {
                /* item can be included */
                zzz = cpp_combo_wrap(k->size, k->items, k->items + (k->size - 1), parent[j].capacity, data.name, t, i + 1); //TODO: Change data.name

                int statement = zzz + parent[j].tot_profit > threshold;
                int statement2 = zzz == parent[j].ub;

                if (statement) {
                    children[a] = parent[j];
                    children[a].ub = zzz;
                    /* amplitudes have to be changed */
                    if (states == "comp") {
                        children[a++].amplitude = parent[j].amplitude * sqrt((1 + (1 - mpz_tstbit(previous_sol, k->size - i - 1)) * bias) / (bias + 2));
                    } // need to check how to work with the integer (right now double) representation
                    else if (states == "single") children[a++].amplitude = parent[k].amplitude * sqrt(1. / (bias + 2));

                    if (statement2) {
                        /* other state was upper bound of parent node, so this has also to be evaluated */
                        zzz = cpp_combo_wrap(k->size, k->items, k->items + (k->size - 1), parent[j].capacity - k->items[i].cost, data.name, t, i + 1); //TODO: Change data.name
                        if (zzz + parent[j].tot_profit + k->items[i].profit > threshold) {
                            children[a].ub = zzz;
                            children[a].capacity = parent[j].capacity - k->items[i].cost;
                            children[a].tot_profit = parent[j].tot_profit + k->items[i].profit;

                            mpz_set(children[a].vector, parent[j].vector);
                            mpz_setbit(children[a].vector, k->size - 1 - i);

//                            if (states == "comp") children[a++].amplitude = 1; // need to check how to work with the integer (right now double) representation
                            if (states == "comp") {
                                children[a++].amplitude = parent[j].amplitude * sqrt((1 + mpz_tstbit(previous_sol, k->size - i - 1) * bias) / (bias + 2));
                            } // need to check how to work with the integer (right now double) representation
                            else if (states == "single") children[a++].amplitude = parent[j].amplitude * sqrt((1. + bias) / (bias + 2));
                        }
                    } else {
                        children[a].ub = parent[j].ub - k->items[i].profit;
                        children[a].capacity = parent[j].capacity - k->items[i].cost;
                        children[a].tot_profit = parent[j].tot_profit + k->items[i].profit;

                        mpz_set(children[a].vector, parent[j].vector);
                        mpz_setbit(children[a].vector, k->size - 1 - i);

                        if (states == "comp") {
                            children[a++].amplitude = parent[j].amplitude * sqrt((1 + mpz_tstbit(previous_sol, k->size - i - 1) * bias) / (bias + 2));
                        } // need to check how to work with the integer (right now double) representation
                        else if (states == "single") children[a++].amplitude = parent[j].amplitude * sqrt((1. + bias) / (bias + 2));
                    }
                } else {
                    children[a].ub = parent[j].ub - k->items[i].profit;
                    children[a].Z = parent[j].capacity - k->items[i].cost;
                    children[a].P = parent[j].tot_profit + k->items[i].profit;

                    mpz_set(children[a].vector, parent[j].vector);
                    mpz_setbit(children[a].vector, k->size - 1 - i);

                    if (states == "comp") {
                        children[a++].amplitude = parent[j].amplitude * sqrt((1 + mpz_tstbit(previous_sol, k->size - i - 1) * bias) / (bias + 2));
                    } // need to check how to work with the integer (right now double) representation
                    else if (states == "single") children[a++].amplitude = parent[j].amplitude * sqrt((1. + bias) / (bias + 2));
                }
            }
        }

//        double max_amp = 0, factor = 0.000075;
//        for(int i = 0; i < a; i++) {
//            if (children[i].amplitude > max_amp) max_amp = children[i].amplitude;
//        }
//        double total = 0, thr = 0;
////        if (max_amp / 500 < factor) thr = max_amp / 500;
////        else thr = factor;
//
//        int count = 0;
//
//        for(int i = 0; i < a; i++) {
//            total += pow(children[i].amplitude, 2);
//            if (children[i].amplitude < thr and children[i].ub + children[i].P != exact) {
//                discarded += pow(children[i].amplitude, 2);
//                count++;
//            }
//        }
////        std::cout << i << " " << a << " " << total << " " << count << " " << discarded << " " << discarded * pow(2, - (double) (k->size - i)) << std::endl;
//        parent.clear();
//        parent.resize(a - count);
//        int d = 0;
//        for(int i = 0; i < a; i++) {
//            if (children[i].amplitude >= thr or children[i].ub + children[i].P == exact) parent[d++] = children[i];
//        }

//        printf("item = %d d = %d prob = %f, max_amp = %f, thr = %f\n", i, d, total, max_amp, thr);
//        num_states = d;

        std::swap(parent, children);
        children.clear();
        children.shrink_to_fit();
        num_states = a;

    }
    parent.resize(num_states);
//    std::cout << "total discarded = " << discarded << std::endl;

    return parent;
}