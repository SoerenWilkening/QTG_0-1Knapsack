//
// Created by Sören Wilkening on 10.05.23.
//

#ifndef PPQA_CPP_STATE_GENERATOR_H
#define PPQA_CPP_STATE_GENERATOR_H

#include "cpp_combo.h"

std::vector<state_node> breadth_first_search(knapsack_instance data, long threshold, long exact);

#endif //PPQA_CPP_STATE_GENERATOR_H
