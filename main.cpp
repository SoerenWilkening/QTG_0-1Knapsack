#include <iostream>
#include "header/RNG.h"
#include "header/instance.h"
#include "header/combo.h"

int main() {
    std::cout << "Hello, World!" << std::endl;

//    probability_distribution_sampling();

//    char filename[] = "/Users/sorenwilkening/Desktop/Algorithms/PPQA_CPP/instances_01_KP/knapsackProblemInstances/problemInstances/n_400_c_100000000_g_10_f_0.2_eps_0.01_s_100/test.in";
    char filename[] = "/Users/sorenwilkening/Desktop/Algorithms/PPQA_CPP/instances_01_KP/knapsackProblemInstances/problemInstances/n_400_c_100000000_g_2_f_0.2_eps_0.01_s_100/test.in";

    knapsack_instance data = read_instance(filename);

    double *t = static_cast<double *>(calloc(1, sizeof(double)));

    std::vector<int> x(data.n, 0);

    long long zzz = combo_wrap(data.n, data.p.data(), data.z.data(), x.data(), data.Z, t, 0, 0);

    std::cout << zzz << std::endl;
    std::cout << *t << std::endl;

    return 0;
}
