#include <iostream>
#include "header/instance.h"
#include "header/cpp_combo.h"
#include "header/greedy.h"
#include <gmpxx.h>
#include <chrono>
#include "header/simulator.h"
#include <filesystem>
#include <unistd.h>
#include <string>

namespace fs = std::filesystem;

int main(int argc, char *argv[]) {
    /*
     * PPQA simulator written in pure C/C++
     *
     *
     * */
    srand(time(nullptr));

    /* change working directory to main.cpp level - for consistency with other platforms */
    std::filesystem::path pathToSource(__FILE__);
    std::filesystem::path pathToDir = pathToSource.parent_path();
    chdir(pathToDir.c_str());

    knapsack_instance data;

//    std::string filename = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_400_c_1000000_g_2_f_0.2_eps_0.001_s_300/test.in";

    std::string filename = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_1200_c_10000000000_g_2_f_0.3_eps_0.0001_s_300/test.in";
//    std::string filename = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_400_c_10000000000_g_2_f_0.2_eps_0.0001_s_100/test.in";

//    std::string filename = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_400_c_100000000_g_10_f_0.2_eps_0.01_s_100/test.in";
//    std::string filename = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_400_c_100000000_g_14_f_0.2_eps_0.001_s_300/test.in";
    data = read_instance(filename);
//
//    data.n = 7;
//    data.Z = 9;
//    data.p = {3, 7, 6, 9, 8, 5, 6};
//    data.z = {4, 9, 5, 7, 6, 3, 2};
//    std::reverse(data.p.begin(), data.p.end());
//    std::reverse(data.z.begin(), data.z.end());
//    data.name = ".calculations/sandor";

    auto *t = static_cast<double *>(calloc(1, sizeof(double)));
    auto start = std::chrono::high_resolution_clock::now();

    long zzz = cpp_combo_wrap(data.n, data.p, data.z, data.Z, data.name, t, 0, 0, false);
    std::vector<state_node> gr = greedy(data.n, data.Z, data.p, data.z, 0);

    std::cout << zzz << std::endl;
    std::cout << gr[0].P << std::endl;

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    std::cout << (double) duration.count() / 1000000 << " " << *t << std::endl;
    QMaxSearch search {data, zzz};
    gr = search.execute(5);

    std::cout << gr[0].P << std::endl;
    std::cout << search.M_tot << std::endl;

    return 0;
}
