//
// Created by Sören Wilkening on 10.05.23.
//

#include "instance.h"
#include <cstdio>
#include <string>
#include <regex>

knapsack_instance read_instance(std::string filename) {
    knapsack_instance instance;


    int a;
    long b, c;

    FILE *file = fopen(filename.c_str(), "r");
    printf("name : %s\n", filename.c_str());

    fscanf(file, "%d", &instance.n);
    instance.p.resize(instance.n);
    instance.z.resize(instance.n);

    for (int i = 0; i < instance.n; i++) fscanf(file, "%d %ld %ld", &a, &instance.p[i], &instance.z[i]);
    fscanf(file, "%ld", &instance.Z);
    fclose(file);
    for (int i = 0; i < instance.n; i++) {
        for (int j = 0; j < instance.n - 1; j++) {
            if (static_cast<double>(instance.p[j]) / static_cast<double>(instance.z[j]) < static_cast<double>(instance.p[j + 1]) / static_cast<double>(instance.z[j + 1])) {
                b = instance.p[j];
                c = instance.z[j];
                instance.p[j] = instance.p[j + 1];
                instance.z[j] = instance.z[j + 1];
                instance.p[j + 1] = b;
                instance.z[j + 1] = c;
            }
        }
    }

    std::string sub = "instances_01_KP";
    instance.name = std::regex_replace(filename.substr(filename.find(sub)), std::regex(sub), ".calculations");

    return instance;
}


int knapsack_instance_is_trivial(std::vector<long> p, std::vector<long> z, long Z, long *P, int first_item) {
    long Z_ = 0;
    int counter = 0;
    for (int i = first_item; i < p.size(); ++i) {
        Z_ += z[i];
        if (Z >= z[i]) {
            counter++;
            *P += p[i];
        }
    }
    if (Z_ < Z) return 1;
    if (counter == 0) return 1;
    return 0;
}