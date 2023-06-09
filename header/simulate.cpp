#include <cstdlib>
#include <iostream>
#include <vector>
#include <random>
#include <stdexcept>
using namespace std;

int sampling(std::vector<double>* probabilities) {

    double total = 0;
    for (size_t i = 0; i < probabilities->size(); ++i) {
        total = total + probabilities->at(i);
    }
    if (total != 1.) {
        throw std::invalid_argument("\n\nProbabilities don't sum to 1");
    }

    std::discrete_distribution<int> dist(probabilities->begin(), probabilities->end());

    std::mt19937 generator(std::random_device{}());  // initialize the Mersenne Twister generator
    return dist(generator);
}

int main() {
    std::vector<double> vect{ 0.1, 0.4, 0.5};

    printf("Output: %d\n", sampling(&vect));

    return 0;
}