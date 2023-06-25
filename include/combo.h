#ifndef COMBO_H
#define COMBO_H
/* 
 * =============================================================================
 *                                  C++ check
 * =============================================================================
 */

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

typedef int           boolean; /* logical variable         */
typedef int           ntype;   /* number of states/items   */
typedef long          itype;   /* item profits and weights */
typedef long          stype;   /* sum of profit or weight  */

/* item record */
typedef struct {
  itype   p;              /* profit                  */
  itype   w;              /* weight                  */
  boolean x;              /* solution variable       */
} item;

/* ======================================================================
			      forward declarations
   ====================================================================== */

stype combo(item *f, item *l, stype c, stype lb, stype ub,
                   boolean def, boolean relx);
/* f,l : first, last item                                               */
/* c   : capacity of knapsack                                           */
/* lb  : lower bound. Solution vector is only updated if better z found */
/* ub  : upper bound. When upper bound is reached terminate immediately */
/* def : should solution vector be defined or just find objective value */
/* relx: relaxed problem is solved (no more relaxations will be made)   */
/* returns the objective value of the problem                           */

#ifdef __cplusplus
}
#endif

#endif
