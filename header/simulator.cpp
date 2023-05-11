//
// Created by Sören Wilkening on 11.05.23.
//

#include "simulator.h"

#include <utility>

std::vector<state_node> simulate(knapsack_instance data, long exact, long threshold, int calls){

    std::vector<state_node> sample;
    std::vector<state_node> bnb;

    double total, factor;
    long updated_T = threshold;

    /* at this point bnb is always empty, but in a later stage maybe not */
    if (bnb.empty()) bnb = breadth_first_search(std::move(data), threshold, exact);

    total = 0;
    for (auto & i : bnb) {
        if(updated_T < i.P) total += pow(i.amplitude, 2);
    }

    std::vector<double> probabilities(bnb.size() + 1, 0);

    factor = pow(sin( (2 * calls + 1) * asin(sqrt(total)) ), 2) / total;

    int i;
    total = 0;
    for (i = 0; i < bnb.size(); ++i) {
        if(updated_T < bnb[i].P) {
            probabilities[i] = pow(bnb[i].amplitude, 2) * factor;
            total += probabilities[i];
        }
    }
    probabilities[i] = 1.- total;

    int measurement = sampling(&probabilities);
    if (measurement == bnb.size()) sample.push_back({0, 0, 0, 0, 0});
    else {
        sample.push_back({bnb[measurement].P, 0, 0, 0, 1});
        mpz_set(sample[0].vector, bnb[measurement].vector);
    }

    probabilities.clear();

    return sample;
}