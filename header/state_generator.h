//
// Created by Sören Wilkening on 10.05.23.
//

#ifndef PPQA_CPP_STATE_GENERATOR_H
#define PPQA_CPP_STATE_GENERATOR_H

#include "cpp_combo.h"
#include <string>

std::vector<state_node> breadth_first_search(knapsack_instance data,
                                             long threshold,
                                             long exact,
                                             double bias = 0,
                                             std::string states = "single",
                                             double previous_sol = 0,
                                             std::string ub = "no");

#endif //PPQA_CPP_STATE_GENERATOR_H
