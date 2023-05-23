//
// Created by Sören Wilkening on 10.05.23.
//

#include "greedy.h"

std::vector<state_node> greedy(int n, long Z, std::vector<long> p, std::vector<long> z, int first_item, int *break_item, double *ub) {
    std::vector<state_node> result(1);
    long Z_ = Z;
    result[0].P = 0;
    mpz_init2(result[0].vector, n - first_item);

    for (int i = first_item; i < n; i++) {
        if (Z_ - z[i] >= 0) {
            result[0].P = result[0].P + p[i];
            Z_ = Z_ - z[i];
            mpz_setbit(result[0].vector, n - first_item - 1 - i);
        }
        else if(*break_item == 0) {
            *break_item = i;
            *ub = result[0].P + (double) Z_ / z[i] * p[i];
        }
    }

    return result;
}