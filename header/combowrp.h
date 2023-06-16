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
 *                            type definitions
 * =============================================================================
 */

 typedef struct combo_args {
    item* f;
    item* l;
    num_t c;
    num_t lb;
    num_t ub;
    bool_t def;
    bool_t relx;
 } combo_args_t;

/* 
 * =============================================================================
 *                            Combo void mask
 * =============================================================================
 */

void combo_void_mask(void*, void*, void*, void*, void*, void*, void*);

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
 *      parameter6: Determines whether solution should be read-out instead.
 * Returns:         Optimal solution found by Combo.
 */
num_t combo_wrap(const knapsack_t*, bit_t, num_t, bool_t, bool_t, bool_t);

/* 
 * =============================================================================
 *                            measure Combo
 * =============================================================================
 */

/*
 * Function:        measure_combo
 * ------------------------------
 * Description:     This function times the execution of Combo on a given
 *                  knapsack instance via elapsed cycles and also measure its
 *                  peak memory usage. The instance is solved two times: In the
 *                  first run, Combo only calculates the optimal profit. In the
 *                  second run, Combo also calculates a optimal solution. All
 *                  counters (cycles, memory) are saved in corresponding files.
 * Parameters:
 *      parameter1: Pointer to the knapsack that should be considered.
 */
void measure_combo(const knapsack_t*);

#ifdef __cplusplus
}
#endif

#endif
