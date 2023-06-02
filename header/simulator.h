//
// Created by Sören Wilkening on 11.05.23.
//

#ifndef PPQA_CPP_SIMULATOR_H
#define PPQA_CPP_SIMULATOR_H
#include "instance.h"
#include "state_generator.h"
#include "RNG.h"
#include "greedy.h"
#include "gate_count.h"
#include "KnapsackQTG/qtg.h"
#include <cstdlib>



class QMaxSearch{
public:
    knapsack_instance data;
    long exact;

    double bias = 0;
    std::string states = "single";
    mpz_t presious_sol;
    std::string ub = "no";


    std::vector<long> qtg, gates, qtg2, gates2;
    int M_tot = 0;
    std::vector<state_node> bnb;

    knapsack_t *k = create_empty_knapsack(data.n, data.Z);


    std::vector<state_node> amplitude_amplification(long threshold, int M);
    std::vector<state_node> QSearch(long threshold, int M);
    std::vector<state_node> execute(int M);
};

std::vector<state_node> simulate(knapsack_instance data, long exact, long threshold, int M);

#endif //PPQA_CPP_SIMULATOR_H
