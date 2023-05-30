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