//
// Created by Sören Wilkening on 10.05.23.
//

#include <iostream>
#include <vector>
#include <random>
#include <stdexcept>
#include "RNG.h"
using namespace std;

int sampling(std::vector<double> *probabilities) {

    double total = 0;
    for (int i = 0; i < probabilities->size(); ++i) total = total + probabilities->at(i);
    if (total != 1.) throw std::invalid_argument("\n\nProbabilities don't sum to 1");

    std::discrete_distribution<int> dist(probabilities->begin(), probabilities->end());

    std::mt19937 generator(std::random_device{}());  // initialize the Mersenne Twister generator
    return dist(generator);
}

int probability_distribution_sampling() {
    std::vector<double> probabilities = {0.100001, 0.3944, 0.3056, 0.199999};

    int n_samples = 100000;
    int outcomes[n_samples];  // array to store the outcomes
    std::vector<int> counts(4, 0);  // initialize counts with 4 zeros
    for (int a = 0; a < n_samples; ++a) {
        outcomes[a] = sampling(&probabilities);
        counts[outcomes[a]]++;
    }

    for (int i = 0; i < 4; ++i) std::printf("%d %d\n", i, counts[i]);

    probabilities.clear();
    if (probabilities.empty()) printf("cleared");
    else std::printf("not cleared");
    return 0;
}


