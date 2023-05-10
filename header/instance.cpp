//
// Created by Sören Wilkening on 10.05.23.
//

#include "instance.h"
#include <cstdio>

knapsack_instance read_instance(char *filename) {
    knapsack_instance instance;

    printf("name : %s\n", filename);

    int a, b, c;

    FILE *file = fopen(filename, "r");

    fscanf(file, "%d", &instance.n);
    instance.p.resize(instance.n);
    instance.z.resize(instance.n);

    for (int i = 0; i < instance.n; i++) fscanf(file, "%d %d %d", &a, &instance.p[i], &instance.z[i]);
    fscanf(file, "%d", &instance.Z);
    fclose(file);
    for (int i = 0; i < instance.n; i++) {
        for (int j = 0; j < instance.n - 1; j++) {
            if ((double) instance.p[j] / instance.z[j] < (double) instance.p[j + 1] / instance.z[j + 1]) {
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


int knapsack_instance_is_trivial(std::vector<int> p, std::vector<int> z, int Z, long *P, int first_item) {
    long Z_ = 0;
    int counter = 0;
    for (int i = first_item; i < p.size(); ++i) {
        Z_ += z[i];
        if (Z >= z[i]) {
            counter++;
            *P += p[i];
        }
    }
//    std::cout << "trivial checking " << Z_ << " " << Z << " " << counter << std::endl;
    if (Z_ < Z) return 1;
    if (counter == 0) return 1;
    return 0;
}