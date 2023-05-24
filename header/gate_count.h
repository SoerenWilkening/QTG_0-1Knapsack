//
// Created by Sören Wilkening on 24.05.23.
//

#ifndef PPQA_CPP_GATE_COUNT_H
#define PPQA_CPP_GATE_COUNT_H

#define classical_greater 1
#define classical_smaller 0

#include "instance.h"
#include <iostream>


int gates_integer_comparison(long value, std::vector<long> *gates, int greater_smaller);

int gates_quantum_fourier_transform(std::vector<long> *gates);

int gates_integer_addition(long value, std::vector<long> *gates);

int gates_amplitude_amplification(knapsack_instance *data, std::vector<long> *gates, std::vector<long> *qtg, int calls,
                                  long value);

std::vector<long> qtg_gatecount(knapsack_instance data);


#endif //PPQA_CPP_GATE_COUNT_H
