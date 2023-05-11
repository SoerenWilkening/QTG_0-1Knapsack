//
// Created by Sören Wilkening on 10.05.23.
//

#include "greedy.h"

state_node greedy(int n, long Z, std::vector<long> p, std::vector<long> z, int first_item) {
    state_node result;
    long Z_ = Z;
    result.P = 0;
    mpz_init2(result.vector, n - first_item);

    for (int i = first_item; i < n; i++) {
        if (Z_ - z[i] >= 0) {
            result.P = result.P + p[i];
            Z_ = Z_ - z[i];
            mpz_setbit(result.vector, n - first_item - 1 - i);
        }
    }

    return result;
}