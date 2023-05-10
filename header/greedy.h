//
// Created by Sören Wilkening on 10.05.23.
//

#ifndef PPQA_CPP_GREEDY_H
#define PPQA_CPP_GREEDY_H

#include "instance.h"
#include <math.h>

state_node greedy(int n, int Z, int *p, int *z, int first_item);
int greedy_is_one(double vector, int n);

#endif //PPQA_CPP_GREEDY_H
