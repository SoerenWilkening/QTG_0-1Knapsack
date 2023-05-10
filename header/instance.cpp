//
// Created by Sören Wilkening on 10.05.23.
//

#include "instance.h"
#include <cstdio>

knapsack_instance read_instance(char *filename){
    knapsack_instance instance;

    printf("name : %s\n", filename);

    int a, b, c;

    FILE *file = fopen(filename, "r");

    fscanf(file, "%d", &instance.n);
    instance.p.resize(instance.n);
    instance.z.resize(instance.n);

    for(int i = 0; i < instance.n; i++) fscanf(file, "%d %d %d", &a, &instance.p[i], &instance.z[i]);
    fscanf(file, "%d", &instance.Z);
    fclose(file);
    for(int i = 0; i < instance.n; i++){
        for(int j = 0; j < instance.n - 1; j++){
            if ((double) instance.p[j] / instance.z[j] < (double) instance.p[j + 1] / instance.z[j + 1]  ){
                b = instance.p[j];
                c = instance.z[j];
                instance.p[j] = instance.p[j + 1];
                instance.z[j] = instance.z[j + 1];
                instance.p[j + 1] = b;
                instance.z[j + 1] = c;
            }
        }
    }

    return instance;
}