//
// Created by Sören Wilkening on 10.05.23.
//

#ifndef STATE_GENERATOR_H
#define STATE_GENERATOR_H

#include "cpp_combo.h"
#include <vector>
#include <iostream>
#include <gmpxx.h>
#include <string>


typedef struct {
    num_t tot_profit;
    num_t capacity;
    num_t ub;
    mpz_t vector;
    double amplitude;
} state_node;

std::vector<state_node> breadth_first_search(knapsack_instance data,
                                             long threshold,
                                             long exact,
                                             double bias = 0,
                                             std::string states = "single",
                                             mpz_t previous_sol = 0,
                                             std::string ub = "no");

#endif
