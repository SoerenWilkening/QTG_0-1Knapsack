#ifndef SIMULATE_H
#define SIMULATE_H

/* 
 * =============================================================================
 *                            includes
 * =============================================================================
 */

#include "common/stategen.h"
#include "qtgcount.h"

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
size_t sampling(const double[], size_t, const gsl_rng *);

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
path_t *ampl_amp(const node_t[], size_t, size_t, const gsl_rng *);

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
path_t *q_search(const node_t[], size_t, size_t *, size_t *, size_t, \
                 const gsl_rng *);

#ifdef __cplusplus
}
#endif

#endif

