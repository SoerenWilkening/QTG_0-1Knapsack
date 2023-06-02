//
// Created by Sören Wilkening on 07.05.23.
//

#ifndef UNTITLED_COMBO_H
#define UNTITLED_COMBO_H

typedef long          stype;   /* sum of profit or weight  */
#ifdef __cplusplus
extern "C" {
#endif

typedef int boolean; /* logical variable         */
typedef long itype;   /* item profits and weights */

/* item record internal */
typedef struct {
    itype p;              /* profit                  */
    itype w;              /* weight                  */
    boolean x;              /* pointer solution variable       */
} item;
extern stype combo(item *f, item *l, stype c, stype lb, stype ub, boolean def, boolean relx, int *out);
#ifdef __cplusplus
}
#endif


#endif //UNTITLED_COMBO_H
