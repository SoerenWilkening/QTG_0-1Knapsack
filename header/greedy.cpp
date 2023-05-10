//
// Created by Sören Wilkening on 10.05.23.
//

#include "greedy.h"

state_node greedy(int n, int Z, int *p, int *z, int first_item) {
    state_node result;
    int Z_ = Z, a = first_item;
    result.P = 0;
    result.vector = 0;

    for (int i = first_item; i < n; i++) {
        if (Z_ - z[i] >= 0) {
            result.P = result.P + p[i];
            Z_ = Z_ - z[i];
            result.vector = result.vector + pow(2, n - a - 1);
        }
        a++;
    }

    return result;
}

int greedy_is_one(double vector, int n) {

    double step = vector;

    for (int i = 0; i < n; i++) {
        if ((int) fmod(step, 2) == 0) return 0;
        step = step / 2;
    }
    return 1;
}