//
// Created by Sören Wilkening on 11.05.23.
//

#ifndef PPQA_CPP_SIMULATOR_H
#define PPQA_CPP_SIMULATOR_H
#include "instance.h"
#include "state_generator.h"
#include "RNG.h"

std::vector<state_node> simulate(knapsack_instance data, long exact, long threshold, int calls);

#endif //PPQA_CPP_SIMULATOR_H
