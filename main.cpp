#include <iostream>
#include "header/instance.h"
#include "header/cpp_combo.h"
#include "header/greedy.h"
#include <chrono>
#include "header/simulator.h"
#include <filesystem>
#include <unistd.h>
#include <string>

namespace fs = std::filesystem;


int benching(std::string filename) {
    for (int x = 0; x < 10; ++x) {
        long peak_ram = 0;
        for (int i = 0; i <= 1; ++i) {

            std::string command = "echo \"$(/usr/bin/time -l ./cmake-build-debug/bench " + filename +
                                  " " + std::to_string(i) + " 2>&1)\" | grep \"maximum resident set size\"";

            FILE *pipe = popen(command.c_str(), "r");
            if (!pipe) {
                std::cerr << "Error opening pipe." << std::endl;
                return -1;
            }

            // Read the command output line by line
            char buffer[128];
            long res;

            fscanf(pipe, "%ld %s\n", &res, buffer);
            peak_ram += pow(-1, 1 - i) * res;

            // Close the pipe
            pclose(pipe);
        }
        std::cout << "peak ram " << (double) peak_ram / 1000000 * 8 << "Mbit" << std::endl;
    }

    return 0;
}


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

//    std::string filename = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_1200_c_10000000000_g_2_f_0.3_eps_0.0001_s_200/test.in";
    std::string filename = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_400_c_100000000_g_2_f_0.3_eps_0.0001_s_300/test.in";
//    std::string filename = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_400_c_100000000_g_6_f_0.3_eps_0.0001_s_300/test.in";

//    std::string filename = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_400_c_100000000_g_10_f_0.2_eps_0.01_s_100/test.in";
//    std::string filename = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_400_c_100000000_g_14_f_0.2_eps_0.001_s_300/test.in";
//    data = read_instance(filename);

//    data.n = 7;
//    data.Z = 9;
//    data.p = {3, 7, 6, 9, 8, 5, 6};
//    data.z = {4, 9, 5, 7, 6, 3, 2};
//    std::reverse(data.p.begin(), data.p.end());
//    std::reverse(data.z.begin(), data.z.end());
//    data.name = ".calculations/sandor";

//    auto *t = static_cast<double *>(calloc(1, sizeof(double)));
//
//    auto start = std::chrono::high_resolution_clock::now();
//
//
//    int break_item = 0;
//    std::vector<state_node> gr = greedy(data.n, data.Z, data.p, data.z, 0, &break_item);
//    break_item = 0;
//    std::cout << break_item << std::endl;
//
//    long zzz = cpp_combo_wrap(data.n, data.p, data.z, data.Z, data.name, t, 0, false, false);
//
//    std::cout << zzz << std::endl;
//    std::cout << greedy(data.n, data.Z, data.p, data.z, 0, &break_item)[0].P << std::endl;
//
//
//    auto stop = std::chrono::high_resolution_clock::now();
//    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
//
//    std::cout << (double) duration.count() / 1000000 << " " << *t << std::endl;


    benching(filename);

//
//    knapsack_instance new_data = subinstance(data, break_item);
//
//    std::cout << greedy(new_data.n, new_data.Z, new_data.p, new_data.z, 0, &break_item)[0].P << std::endl;
//    std::cout << cpp_combo_wrap(new_data.n, new_data.p, new_data.z, new_data.Z, new_data.name, t, 0, 0, false)
//              << std::endl;
//    zzz = cpp_combo_wrap(new_data.n, new_data.p, new_data.z, new_data.Z, new_data.name, t, 0, 0, false);
//
//    int count = 0, mean_m = 0;
//    for (int i = 0; i < 1; ++i) {
//        QMaxSearch search{new_data, zzz, 75, "comp"};
//        gr = search.execute(200);
//
//        std::cout << gr[0].P << " " << search.M_tot << std::endl;
//        if (gr[0].P == zzz) {
//            count++;
//            mean_m += search.M_tot;
//        }
//    }
//
//    std::cout << (double) count / 1 << "% " << (double) mean_m / count << std::endl;

    return 0;
}
