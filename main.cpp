#include <iostream>
#include "header/RNG.h"
#include "header/instance.h"
#include "header/cpp_combo.h"
#include "header/state_generator.h"
#include "header/greedy.h"
#include <gmpxx.h>
#include <chrono>

int main() {
    /*
     * PPQA simulator written in pure C/C++
     *
     *
     * */
//    probability_distribution_sampling();
    knapsack_instance data;

    char filename[] = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_400_c_100000000_g_2_f_0.3_eps_0.001_s_100/test.in";
//    char filename[] = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_400_c_100000000_g_10_f_0.2_eps_0.01_s_100/test.in";
//    char filename[] = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_400_c_100000000_g_14_f_0.2_eps_0.001_s_300/test.in";
    data = read_instance(filename);

//    data.n = 7;
//    data.Z = 9;
//    data.p = {3, 7, 6, 9, 8, 5, 6};
//    data.z = {4, 9, 5, 7, 6, 3, 2};
//    std::reverse(data.p.begin(), data.p.end());
//    std::reverse(data.z.begin(), data.z.end());

    double *t = static_cast<double *>(calloc(1, sizeof(double)));

    auto start = std::chrono::high_resolution_clock::now();
    long zzz = cpp_combo_wrap(data.n, data.p, data.z, data.Z, t, 0, 1);
    state_node gr = greedy(data.n, data.Z, data.p, data.z, 0);

    std::cout << *t << std::endl;
    std::cout << zzz << std::endl;
    std::cout << gr.P << std::endl;

    std::vector<state_node> bnb = breadth_first_search(data, gr.P, zzz);

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);

    std::cout << "Time taken: " << duration.count() << " milliseconds" << std::endl;
    return 0;
}
