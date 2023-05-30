//
// Created by Sören Wilkening on 24.05.23.
//

#include "gate_count.h"


int gates_integer_comparison(long value, std::vector<long> *gates, int greater_smaller) {

    if (greater_smaller == 1) gates->at(0) += 1;
    for (int i = 0; i < gates->size() - 1; ++i) {
        if (((value >> i) & 1) == greater_smaller) gates->at(gates->size() - 1 - i) += 1;
    }

    return 0;
}

int gates_integer_addition(long value, std::vector<long> *gates) {
    int bits = gates->size() - 1;

    for (int i = 0; i < bits; ++i) if ((value >> i) & 1) gates->at(1) += bits - i;
    return 0;
}

int gates_quantum_fourier_transform(std::vector<long> *gates) {
    int bits = gates->size() - 1;

    gates->at(0) += bits;
//    gates->at(1) += (long) (pow(bits, 2) / 2) - bits / 2 + 1;
    gates->at(1) += (long) (bits * log2(bits));

    return 0;
}

int gates_amplitude_amplification(knapsack_instance *data, std::vector<long> *gates, std::vector<long> *qtg, int calls,
                                  long value) {

    for (int i = 0; i < gates->size(); ++i) gates->at(i) += (2 * calls + 1) * qtg->at(i);
    for (int i = 0; i < calls; i++) gates_integer_comparison(value, gates, classical_smaller);
    gates->at(2) += 2 * (data->n - 1) * calls;
    gates->at(1) += calls;

    return 0;
}


std::vector<long> qtg_gatecount(knapsack_instance data) {
    std::vector<long> gates(ceil(log2(data.Z)) + 1);

    for (int item = 0; item < data.n; ++item) {
        gates_integer_comparison(data.z[item], &gates, classical_greater); // comparison of z[i]

        gates_quantum_fourier_transform(&gates);
        gates_integer_addition(data.z[item], &gates); // subtraction of z[i]
        gates_integer_addition(data.p[item], &gates); // addition of p[i]
        gates_quantum_fourier_transform(&gates);
    }

    return gates;
}