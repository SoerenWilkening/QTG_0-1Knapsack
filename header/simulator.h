//
// Created by Sören Wilkening on 11.05.23.
//

#ifndef PPQA_CPP_SIMULATOR_H
#define PPQA_CPP_SIMULATOR_H
#include <cstdlib>



class QMaxSearch{
public:
    knapsack_instance data;
    long exact;

    double bias = 0;
    std::string states = "single";
    mpz_t previous_sol;
    std::string ub = "no";


    std::vector<long> qtg, gates, qtg2, gates2;
    int M_tot = 0;
    std::vector<node_t> bnb;

    knapsack_t* k = create_empty_knapsack(data.n, data.Z);

    std::vector<node_t> amplitude_amplification(long threshold, int M);
    std::vector<node_t> QSearch(long threshold, int M);
    std::vector<node_t> execute(int M);
};

std::vector<node_t> simulate(knapsack_instance data, long exact, long threshold, int M);

#endif //PPQA_CPP_SIMULATOR_H
