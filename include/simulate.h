#ifndef SIMULATE_H
#define SIMULATE_H

/* 
 * =============================================================================
 *                            includes
 * =============================================================================
 */

#include "stategen.h"
#include "qtgcount.h"

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
 *                            sampling
 * =============================================================================
 */

/*
 * Function:        sampling
 * -------------------------
 * Description:     This function simulates a quantum measurement in the
 *                  computational basis on a superposition of states represented
 *                  by the given array of probabilities. A random number
 *                  generator from the GNU Scientific Library (GSL) is used to
 *                  simulate the measurement.
 * Parameters:
 *      parameter1: Array of probabilities.
 *      parameter2: Size of array of probabilities.
 *      parameter3: Pointer to GSL's random number generator.
 * Returns:         Index of the state obtained by the simulated measurement.
 */
size_t sampling(const double*, size_t, const gsl_rng*);

/* 
 * =============================================================================
 *                            amplitude amplification
 * =============================================================================
 */

/*
 * Function:        ampl_amp
 * -------------------------
 * Description:     This function simulates the application of amplitude
 *                  amplification to a given set of states. The states handed
 *                  over are assumed to be "good" states, i.e., those whose
 *                  amplitude should be amplified. In the end, a quantum
 *                  measurement in the computational basis is simulated on the
 *                  amplified set of states, returning either a state or a null
 *                  pointer, if measuring any "good" states fails.
 * Parameters:
 *      parameter1: Array of relevant states together with their probabilities.
 *      parameter2: Number of states, i.e. length of the array.
 *      parameter3: Specifies how often the AA operator should be applied.
 *      parameter4: Pointer to GSL's random number generator.
 * Returns:         If successul, pointer to improved state (path) measured.
 *                  Otherwise, a null pointer.
 * Side Effect:     Allocates dynamically; pointer should eventually be freed.
 */
path_t* ampl_amp(const node_t[], size_t, size_t, const gsl_rng*);

/* 
 * =============================================================================
 *                            QSearch
 * =============================================================================
 */

/*
 * Function:        q_search
 * -------------------------
 * Description:     This function simulates the application of the QSearch
 *                  algorithm to a given set of states. The states handed over
 *                  are assumed to be "good" states, i.e., those whose
 *                  amplitude should be amplified. The routine calls the AA
 *                  routine as long as the specified maximum number of QTG calls
 *                  is not surpassed. As soon as the AA subroutine yields an
 *                  improved state, QSearch interrupts and outputs this result.
 *                  If no improved state was found, a null pointer is returned.
 *                  The function writes the number of conducted rounds at the
 *                  counter address handed over.
 * Parameters:
 *      parameter1: Array of relevant states together with their probabilities.
 *      parameter2: Number of states, i.e. length of the array.
 *      parameter3: Pointer to round counter.
 *      parameter4: Pointer to iteration counter.
 *      parameter5: Maximum number of iterations.
 *      parameter6: Pointer to GSL's random number generator.
 * Returns:         If successul, pointer to improved state (path) measured.
 *                  Otherwise, a null pointer.
 * Side Effect:     Allocates dynamically; pointer should eventually be freed.
 */
path_t* q_search(const node_t[], size_t, size_t*, size_t*, size_t, \
                 const gsl_rng*);

/* 
 * =============================================================================
 *                            QMaxSearch
 * =============================================================================
 */

/*
 * Function:        q_max_search
 * -----------------------------
 * Description:     This funtion simulates the application of the QMaxSearch
 *                  algorithm to a given knapsack instance. The routine starts
 *                  with sorting the input knapsack in-place and then calculates
 *                  its integer greedy solution, obtaining the first threshold.
 *                  It also calculates its optimal total profit via Combo.
 *                  Then, the application of the QTG, and subsequently of the
 *                  QSearch is simulated as long as an improved state (path) is
 *                  found. After each application of QSearch, the found state's
 *                  total profit serves as an updated threshold, determining
 *                  the parameter for the next application of QSearch. As soon
 *                  as no improved state is found by QSearch, the entire routine
 *                  terminates and returns the last found state.
 * Parameters:
 *      parameter1: Pointer to knapsack instance that should be considered.
 *      parameter2: Bias towards certain branch.
 *      parameter3: Method that should be used for branching.
 *      parameter4: Specifies how often the QTG should be applied in total per
 *                  QSearch subroutine.
 *      parameter5: Pointer to GSL's random number generator.
 * Returns:         Index of the state obtained by the simulated measurement.
 * Side Effect:     Allocates dynamically; pointer should eventually be freed.
 */
path_t* q_max_search(knapsack_t*, size_t, branch_t, size_t, const gsl_rng*);

#ifdef __cplusplus
}
#endif

#endif

