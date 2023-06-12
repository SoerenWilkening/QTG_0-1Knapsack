//
// Created by Sören Wilkening on 10.05.23.
//

#ifndef COMBOWRP_H
#define COMBOWRP_H

/* 
 * =============================================================================
 *                                  includes
 * =============================================================================
 */

#include "knapsack.h"
#include "combo.h"

/* 
 * =============================================================================
 *                            C++ check
 * =============================================================================
 */

#ifdef __cplusplus
extern "C" {
#endif

/* 
 * =============================================================================
 *                            combo wrapper
 * =============================================================================
 */

/*
 * Function:        combo_wrap
 * ---------------------------
 * Description:     This function yields an implementation of the original combo
 *                  algorithm, adapted to the knapsack structure of this
 *                  package. Additionally, the time combo needs to solve the
 *                  problem is recorded. For this purpose, an additional boolean
 *                  is required that determines whether combo is actually
 *                  executed or not.
 * Parameters:
 *      parameter1: Pointer to the knapsack that should be considered.
 *      parameter2: Index of the first item that should be considered.
 *      parameter3: Capacity the knapsack should be considered with.
 *      parameter4: Pointer to time record of combo.
 *      parameter5: Determines whether combo should calculate solution state.
 *      parameter6: Determines whether relaxed problem should be solved.
 *      parameter7: Determines whether solution should be read-out instead.
 *      parameter8: Determines whether combo should actually be executed.
 * Returns:         Optimal solution found by combo.
 */
num_t combo_wrap(const knapsack_t*, bit_t, num_t, uint64_t*, bool_t, bool_t, \
                 bool_t, bool_t);

#ifdef __cplusplus
}
#endif

#endif
