//
// Created by Sören Wilkening on 10.05.23.
//

#ifndef COMBOWRP_H
#define COMBOWRP_H

/* 
 * =============================================================================
 *                            includes
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
 *                            Combo wrapper
 * =============================================================================
 */

/*
 * Function:        combo_wrap
 * ---------------------------
 * Description:     This function yields an implementation of the original Combo
 *                  algorithm, adapted to the knapsack structure of this
 *                  package.
 * Parameters:
 *      parameter1: Pointer to the knapsack that should be considered.
 *      parameter2: Index of the first item that should be considered.
 *      parameter3: Capacity the knapsack should be considered with.
 *      parameter4: Determines whether Combo should calculate solution state.
 *      parameter5: Determines whether relaxed problem should be solved.
 *      parameter6: Determines whether Combo is actually executed or not.
 * Returns:         Optimal solution found by Combo.
 */
num_t combo_wrap(const knapsack_t*, bit_t, num_t, bool_t, bool_t, bool_t, bool_t);

/* 
 * =============================================================================
 *                            Combo data
 * =============================================================================
 */

/*
 * Function:        combo_data
 * ---------------------------
 * Description:     This function reads and collects data for the execution of
 *                  the Combo wrapper provided in this package.
 * Parameters:
 *      parameter1: Pointer to the knapsack that should be considered.
 *      parameter2: Index of the first item that should be considered.
 *      parameter3: Capacity the knapsack should be considered with.
 *      parameter4: Determines whether Combo should calculate solution state.
 *      parameter5: Determines whether relaxed problem should be solved.
 *      parameter6: Determines whether data is read-out instead.
 * Returns:         Optimal solution found by Combo.
 */
num_t combo_data(const knapsack_t*, bit_t, num_t, bool_t, bool_t, bool_t);

/* 
 * =============================================================================
 *                            measure Combo
 * =============================================================================
 */

void measure_combo(const knapsack_t*);

#ifdef __cplusplus
}
#endif

#endif
