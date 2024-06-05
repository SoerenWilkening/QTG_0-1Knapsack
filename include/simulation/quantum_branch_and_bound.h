#ifndef QUANTUM_TREE_GENERATOR_QUANTUM_BRANCH_AND_BOUND_H
#define QUANTUM_TREE_GENERATOR_QUANTUM_BRANCH_AND_BOUND_H

#ifdef __cplusplus
extern "C" {
#endif


#include "common/knapsack.h"

typedef unsigned char qbnb_method;
#define Ambainis 0
#define Montanaro 1

long branch_and_bound(knapsack_t *k, int level, long threshold, size_t *count_nodes, long current_objective,
                      long remaining_capacity, int *exec, qbnb_method method, unsigned long max_node_count);

num_t bnb_greedy(const knapsack_t *k, long cap, int first_item);

#ifdef __cplusplus
}
#endif

#endif //QUANTUM_TREE_GENERATOR_QUANTUM_BRANCH_AND_BOUND_H
