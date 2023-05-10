//
// Created by Sören Wilkening on 10.05.23.
//

#ifndef PPQA_CPP_INSTANCE_H
#define PPQA_CPP_INSTANCE_H
#include <vector>
#include <iostream>

typedef struct {
    int n, Z;
    std::vector<int> p, z;
    char name[1000];
} knapsack_instance;

typedef struct {
    long P;
    long Z;
    long ub;
    double vector;
    double amplitude;
} state_node;

typedef struct {
    std::vector<state_node> states;
} all_info;

knapsack_instance read_instance(char *filename);

#endif //PPQA_CPP_INSTANCE_H
