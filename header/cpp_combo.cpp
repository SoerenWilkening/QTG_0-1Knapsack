//
// Created by Sören Wilkening on 10.05.23.
//

#include "cpp_combo.h"

long cpp_combo_wrap(int n, std::vector<long> p, std::vector<long> w, long c, double *timerecord, int first_item, int define_sol){
    long P = 0;
    int res = knapsack_instance_is_trivial(p, w, c, &P, first_item);
    if (res) return P;

    return combo_wrap(n, p.data(), w.data(), c, timerecord, first_item, define_sol);
}