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

typedef struct node {
    num_t tot_profit;
    num_t capacity;
    num_t ub;
    mpz_t vector;
    double amplitude;
} node_t;

typedef enum branch {
    COMPARE,
    SINGLE,
} branch_t;

std::vector<node> breadth_first_search(knapsack_instance data, num_t, num_t, \
                                       double, branch_t, mpz_t);

#endif
