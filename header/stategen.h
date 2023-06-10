#ifndef STATEGEN_H
#define STATEGEN_H

/* 
 * =============================================================================
 *                                  includes
 * =============================================================================
 */

#include "combowrp.h"
#include <gmp.h>


/* 
 * =============================================================================
 *                                  C++ check
 * =============================================================================
 */

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * =============================================================================
 *                              type definitions
 * =============================================================================
 */

typedef struct state {
    num_t tot_profit;
    mpz_t vector;
} state_t;

typedef struct node {
    state_t state;
    num_t capacity;
    num_t ub;
    double prob;
} node_t;

typedef enum branch {
    COMPARE,
    SINGLE,
} branch_t;

/* 
 * =============================================================================
 *                                 enum names
 * =============================================================================
 */

/*
 * Function:    get_branch_time
 * ----------------------------
 * Description: This function returns a branching method's name as a string.
 * Parameter:   Branching method.
 * Returns:     Branching method's name as a string.
 */
const char* get_branch_name(branch_t);

/* 
 * =============================================================================
 *                              update probability
 * =============================================================================
 */

void update_prob(knapsack_t*, bit_t, double, bool_t, branch_t, mpz_t, double*);

/* 
 * =============================================================================
 *                              breadth-first search
 * =============================================================================
 */

node_t* breadth_first_search(knapsack_t*, num_t, num_t, double, branch_t, \
							 mpz_t);

#ifdef __cplusplus
}
#endif

#endif
