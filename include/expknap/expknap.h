#ifndef EXPKNAP_H
#define EXPKNAP_H
/* 
 * =============================================================================
 *                                  C++ check
 * =============================================================================
 */

#include "../common/knapsack.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ======================================================================
      	     combo.h,    S.Martello, D.Pisinger, P.Toth     feb 1997
   ====================================================================== */

/* This is the header file of the COMBO algorithm described in 
 * S.Martello, D.Pisinger, P.Toth: "Dynamic Programming and Strong
 * Bounds for the 0-1 Knapsack Problem".
 */

/* ======================================================================
				 type declarations
   ====================================================================== */
/*typedef int  boolean;*/ /* boolean variables */
typedef int ntype;   /* number of states/items   */
typedef long itype;   /* item profits and weights */
typedef long  stype;   /* sum of pofit or weight */
typedef int           BOOL; /* logical variable         */
typedef int           boolean; /* logical variable         */

/* item as seen externally */
typedef struct {
    ntype   idx;   /* index */
    itype   p;     /* profit */
    itype   w;     /* weight */
    BOOL x;     /* solution variable */
} exitem_2;

stype expknap(exitem_2 *f, exitem_2 *l, stype cap);


#ifdef __cplusplus
}
#endif

#endif
