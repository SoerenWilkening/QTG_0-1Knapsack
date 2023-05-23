//
// Created by Sören Wilkening on 10.05.23.
//

#ifndef PPQA_CPP_GREEDY_H
#define PPQA_CPP_GREEDY_H

#include "instance.h"
#include <math.h>

std::vector<state_node> greedy(int n, long Z, std::vector<long> p, std::vector<long> z, int first_item, int *break_item, double *ub);

#endif //PPQA_CPP_GREEDY_H
