//
// Created by Sören Wilkening on 10.05.23.
//

#include "state_generator.h"

std::vector<state_node> initialize_states(int number) {
    std::vector<state_node> result(number);

//    result.states = calloc(number, sizeof(state_node));

    for (int i = 0; i < number; i++) {
        result[i].amplitude = 1.;
        result[i].P = 0;
        result[i].Z = 0;
        result[i].vector = 0.;
        result[i].ub = 0;
    }
    return result;
}

std::vector<state_node> breadth_first_search(knapsack_instance data,
                                             long threshold,
                                             long exact,
                                             double bias,
                                             std::string states,
                                             double previous_sol,
                                             std::string ub) {
    /**
     * TODO:
     *  - include calculations of amplitudes
     *  - enable multiprocessing
     * */
    int number_of_states = 1, a;
    std::vector<state_node> parent = initialize_states(number_of_states);
    parent[0].ub = exact;
    parent[0].Z = data.Z;
    long zzz;
    double *t = static_cast<double *>(calloc(1, sizeof(double)));
    for (int item = 0; item < data.n; item++) {
        a = 0;
//        printf("item : %d\n", item);
        std::vector<state_node> children = initialize_states(2 * number_of_states);

        for (int k = 0; k < number_of_states; k++) {
            /* checking if item cannot be included */
            if (parent[k].Z - data.z[item] < 0) children[a++] = parent[k];
            else {
                /* item can be included */
                zzz = cpp_combo_wrap(data.n, data.p, data.z, parent[k].Z, t, item + 1);

                int statement = zzz + parent[k].P > threshold;
                int statement2 = zzz == parent[k].ub;

                if (statement) {
                    children[a] = parent[k];
                    children[a].ub = zzz;
                    /* amplitudes have to be changed */
                    if (states == "comp") children[a++].amplitude = 1; // need to check how to work with the integer (right now double) representation
                    else if (states == "single") children[a++].amplitude = parent[k].amplitude * sqrt(1. / (bias + 2));

                    if (statement2) {
                        /* other state was upper bound of parent node, so this has also to be evaluated */
                        zzz = cpp_combo_wrap(data.n, data.p, data.z, parent[k].Z - data.z[item], t, item + 1);
//                        printf("results: %d %ld\n", item, zzz + parent[k].P + data.p[item]);
                        if (zzz + parent[k].P + data.p[item] > threshold) {
                            children[a].ub = zzz;
                            children[a].Z = parent[k].Z - data.z[item];
                            children[a].P = parent[k].P + data.p[item];
                            children[a].vector = parent[k].vector + pow(2, data.n - item - 1);
                            if (states == "comp") children[a++].amplitude = 1; // need to check how to work with the integer (right now double) representation
                            else if (states == "single") children[a++].amplitude = parent[k].amplitude * sqrt((1. + bias) / (bias + 2));
                        }
                    } else {
                        children[a].ub = parent[k].ub - data.p[item];
                        children[a].Z = parent[k].Z - data.z[item];
                        children[a].P = parent[k].P + data.p[item];
                        children[a].vector = parent[k].vector + pow(2, data.n - item - 1);
                        if (states == "comp") children[a++].amplitude = 1; // need to check how to work with the integer (right now double) representation
                        else if (states == "single") children[a++].amplitude = parent[k].amplitude * sqrt((1. + bias) / (bias + 2));
                    }
                } else {
                    children[a].ub = parent[k].ub - data.p[item];
                    children[a].Z = parent[k].Z - data.z[item];
                    children[a].P = parent[k].P + data.p[item];
                    children[a].vector = parent[k].vector + pow(2, data.n - item - 1);
                    if (states == "comp") children[a++].amplitude = 1; // need to check how to work with the integer (right now double) representation
                    else if (states == "single") children[a++].amplitude = parent[k].amplitude * sqrt((1. + bias) / (bias + 2));
                }
            }
        }
        std::swap(parent, children);
        children.clear();
        number_of_states = a;

//        std::cout << item << " ";
//        std::cout << a << std::endl;
    }
    parent.resize(number_of_states);

    return parent;
}