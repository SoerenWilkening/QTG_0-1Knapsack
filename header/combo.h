//
// Created by Sören Wilkening on 07.05.23.
//

#ifndef UNTITLED_COMBO_H
#define UNTITLED_COMBO_H

typedef long          stype;   /* sum of profit or weight  */
#ifdef __cplusplus
extern "C" {
long long combo_wrap(int n, int *p, int *w, stype c, double *timerecord, int first_item, int define_sol);
}
#endif


#endif //UNTITLED_COMBO_H
