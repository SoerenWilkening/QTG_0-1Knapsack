//
// Created by Sören Wilkening on 10.05.23.
//

#ifndef PPQA_CPP_INSTANCE_H
#define PPQA_CPP_INSTANCE_H
#include <vector>
#include <iostream>
#include <gmpxx.h>
#include <string>

typedef struct {
    int n;
    long Z;
    std::vector<long> p, z;
    std::string name;
} knapsack_instance;

typedef struct {
    long P;
    long Z;
    long ub;
    mpz_t vector;
    double amplitude;
} state_node;

knapsack_instance read_instance(std::string filename);

int knapsack_instance_is_trivial(std::vector<long> p, std::vector<long> z, long Z, long *P, int first_item);

knapsack_instance subinstance(knapsack_instance data, int first_item);

long total_Z(knapsack_instance data, int last_item);

long total_P(knapsack_instance data, int last_item);

#endif //PPQA_CPP_INSTANCE_H
