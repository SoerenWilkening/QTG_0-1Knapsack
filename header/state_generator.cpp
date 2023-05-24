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

std::vector<state_node> breadth_first_search(knapsack_instance data,
                                             long threshold,
                                             long exact,
                                             double bias,
                                             std::string states,
                                             mpz_t previous_sol,
                                             std::string ub) {
    /**
     * TODO:
     *  - include calculations of amplitudes
     *  - enable multiprocessing
     * */
//    std::cout << "executing bnb" << std::endl;

    int number_of_states = 1, a;
    std::vector<state_node> parent = initialize_states(number_of_states, data.n);
    parent[0].ub = exact;
    parent[0].Z = data.Z;
    long zzz;
    double discarded = 0;
    long *t = static_cast<long *>(calloc(1, sizeof(long)));
    for (int item = 0; item < data.n; item++) {
        a = 0;
        std::vector<state_node> children = initialize_states(2 * number_of_states, data.n);

        for (int k = 0; k < number_of_states; k++) {
            /* checking if item cannot be included */
            if (parent[k].Z - data.z[item] < 0) children[a++] = parent[k];
            else {
                /* item can be included */
                zzz = cpp_combo_wrap(data.n, data.p, data.z, parent[k].Z, data.name, t, item + 1);

                int statement = zzz + parent[k].P > threshold;
                int statement2 = zzz == parent[k].ub;

                if (statement) {
                    children[a] = parent[k];
                    children[a].ub = zzz;
                    /* amplitudes have to be changed */
                    if (states == "comp") {
                        children[a++].amplitude = parent[k].amplitude * sqrt((1 + (1 - mpz_tstbit(previous_sol, data.n - item - 1)) * bias) / (bias + 2));
                    } // need to check how to work with the integer (right now double) representation
                    else if (states == "single") children[a++].amplitude = parent[k].amplitude * sqrt(1. / (bias + 2));

                    if (statement2) {
                        /* other state was upper bound of parent node, so this has also to be evaluated */
                        zzz = cpp_combo_wrap(data.n, data.p, data.z, parent[k].Z - data.z[item], data.name, t, item + 1);
                        if (zzz + parent[k].P + data.p[item] > threshold) {
                            children[a].ub = zzz;
                            children[a].Z = parent[k].Z - data.z[item];
                            children[a].P = parent[k].P + data.p[item];

                            mpz_set(children[a].vector, parent[k].vector);
                            mpz_setbit(children[a].vector, data.n - 1 - item);

//                            if (states == "comp") children[a++].amplitude = 1; // need to check how to work with the integer (right now double) representation
                            if (states == "comp") {
                                children[a++].amplitude = parent[k].amplitude * sqrt((1 + mpz_tstbit(previous_sol, data.n - item - 1) * bias) / (bias + 2));
                            } // need to check how to work with the integer (right now double) representation
                            else if (states == "single") children[a++].amplitude = parent[k].amplitude * sqrt((1. + bias) / (bias + 2));
                        }
                    } else {
                        children[a].ub = parent[k].ub - data.p[item];
                        children[a].Z = parent[k].Z - data.z[item];
                        children[a].P = parent[k].P + data.p[item];

                        mpz_set(children[a].vector, parent[k].vector);
                        mpz_setbit(children[a].vector, data.n - 1 - item);

                        if (states == "comp") {
                            children[a++].amplitude = parent[k].amplitude * sqrt((1 + mpz_tstbit(previous_sol, data.n - item - 1) * bias) / (bias + 2));
                        } // need to check how to work with the integer (right now double) representation
                        else if (states == "single") children[a++].amplitude = parent[k].amplitude * sqrt((1. + bias) / (bias + 2));
                    }
                } else {
                    children[a].ub = parent[k].ub - data.p[item];
                    children[a].Z = parent[k].Z - data.z[item];
                    children[a].P = parent[k].P + data.p[item];

                    mpz_set(children[a].vector, parent[k].vector);
                    mpz_setbit(children[a].vector, data.n - 1 - item);

                    if (states == "comp") {
                        children[a++].amplitude = parent[k].amplitude * sqrt((1 + mpz_tstbit(previous_sol, data.n - item - 1) * bias) / (bias + 2));
                    } // need to check how to work with the integer (right now double) representation
                    else if (states == "single") children[a++].amplitude = parent[k].amplitude * sqrt((1. + bias) / (bias + 2));
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
////        std::cout << item << " " << a << " " << total << " " << count << " " << discarded << " " << discarded * pow(2, - (double) (data.n - item)) << std::endl;
//        parent.clear();
//        parent.resize(a - count);
//        int d = 0;
//        for(int i = 0; i < a; i++) {
//            if (children[i].amplitude >= thr or children[i].ub + children[i].P == exact) parent[d++] = children[i];
//        }

//        printf("item = %d d = %d prob = %f, max_amp = %f, thr = %f\n", item, d, total, max_amp, thr);
//        number_of_states = d;

//        std::cout << item << " " << a << std::endl;
        std::swap(parent, children);
        children.clear();
        children.shrink_to_fit();
        number_of_states = a;

    }
    parent.resize(number_of_states);
//    std::cout << "total discarded = " << discarded << std::endl;

    return parent;
}