//
// Created by Sören Wilkening on 10.05.23.
//

#include "cpp_combo.h"

long cpp_combo_wrap(int n, int *p, int *w, stype c, double *timerecord, int first_item, int define_sol){

    state_node gr = greedy(n, c, p, w, first_item);
    if (gr.P == 0) return 0;
    if (greedy_is_one(gr.vector, n - first_item)) return gr.P;

    return combo_wrap(n, p, w, c, timerecord, first_item, define_sol);

}