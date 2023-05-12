//
// Created by Sören Wilkening on 11.05.23.
//

#include "simulator.h"


std::vector<state_node> QMaxSearch::amplitude_amplification(long threshold, int calls) {

    /* at this point bnb is always empty, but in a later stage maybe not */
    if (bnb.empty()) bnb = breadth_first_search(std::move(data), threshold, exact);

    std::vector<state_node> sample; // result will contain a single state -> the result of simulate

    double total, factor;

    total = 0;
    for (auto &i: bnb) if (threshold < i.P) total += pow(i.amplitude, 2);

    std::vector<double> probabilities(bnb.size() + 1, 0);

    factor = pow(sin((2 * calls + 1) * asin(sqrt(total))), 2) / total;

    int i;
    total = 0;
    for (i = 0; i < bnb.size(); ++i) {
        if (threshold < bnb[i].P) {
            probabilities[i] = pow(bnb[i].amplitude, 2) * factor;
            total += probabilities[i];
        }
    }
    probabilities[i] = 1. - total;

    /* executing a quantum measurement */
    int measurement = sampling(&probabilities); // sampling from the probability distribution
    if (measurement == bnb.size()) sample.push_back({0, 0, 0, 0, 0});
    else {
        sample.push_back({bnb[measurement].P, 0, 0, 0, 1});
        mpz_set(sample[0].vector, bnb[measurement].vector);
    }

    probabilities.clear();
    return sample;
}

std::vector<state_node> QMaxSearch::QSearch(long threshold, int M) {
    int l = 0, m_tot = 0, m, j;
    double c = 6. / 5;

    std::vector<state_node> sample;

    while(m_tot < M){
        m = ceil(pow(c, l));
        j = rand() % m;
        m_tot += 2 * j + 1;
        M_tot += m_tot;

        sample = QMaxSearch::amplitude_amplification(threshold, j);

        if (sample[0].P > threshold) return sample;

        l++;
    }
    return std::vector<state_node>(1, {0, 0});
}

std::vector<state_node> QMaxSearch::execute(int M) {
    M_tot = 0;
    std::vector<state_node> sample = greedy(data.n, data.Z, data.p, data.z, 0);
    std::vector<state_node> measured;

    while (true) {
        measured = QSearch(sample[0].P, M);
        if (measured[0].P > sample[0].P) sample[0] = measured[0];
        else return sample;
        measured.clear();
    }
}