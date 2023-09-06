//
// Created by Sören Wilkening on 10.05.23.
//

#ifndef EXPKNAPWRP_H
#define EXPKNAPWRP_H

/* 
 * =============================================================================
 *                            includes
 * =============================================================================
 */

#include "knapsack.h"
#include "expknap.h"

/* 
 * =============================================================================
 *                            C++ check
 * =============================================================================
 */

#ifdef __cplusplus
extern "C" {
#endif

num_t
expkap_wrap(const knapsack_t *k, bit_t first_item, num_t capacity, bool_t def, \
           bool_t relx, bool_t exe_combo, bool_t save);

#ifdef __cplusplus
}
#endif

#endif
