#include <iostream>
#include <chrono>
#include <filesystem>
#include <unistd.h>
#include <string>
#include <limits>

#include "header/instance.h"
#include "header/cpp_combo.h"
#include "header/greedy.h"
#include "header/simulator.h"
#include "header/gate_count.h"

namespace fs = std::filesystem;

int benching(std::string filename, int reps) {
    knapsack_instance data = read_instance(filename);

    for (int x = 0; x < reps; ++x) {
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
            if (i == 1) {
                fs::create_directories(data.name + "/benchmark/");
                std::ofstream myfile(data.name + "/benchmark/combo_ram.txt", std::ios::app);
                myfile << res * 8 << std::endl;
                myfile.close();
            }
        }
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
    double ub = 0;
    int break_item = 0;
    long t = 0;

    knapsack_instance data;
//    std::string filename =
//            "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_1200_c_10000000000_g_2_f_" +
//            std::to_string(0.3).substr(0, 3) + "_eps_" +
//            std::to_string(pow(10, -3 - 1)).substr(0, 3 + 3) + "_s_" + std::to_string(200) + "/test.in";
//    data.n = 7;
//    data.Z = 9;
//    data.p = {3, 7, 6, 9, 8, 5, 6};
//    data.z = {4, 9, 5, 7, 6, 3, 2};
//    std::reverse(data.p.begin(), data.p.end());
//    std::reverse(data.z.begin(), data.z.end());

    for (double f = .1; f <= .31; f += .1) {
        for (int x = 100; x <= 300; x += 100) {
            for (int eps = 0; eps < 1; ++eps) {

                std::string filename =
                        "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_1200_c_10000000000_g_6_f_" +
                        std::to_string(f).substr(0, 3) + "_eps_" +
                        "1e-05_s_" + std::to_string(x) + "/test.in";

                std::cout << filename << std::endl;
                std::cout << "benchmarking" << std::endl;
                data = read_instance(filename);

                benching(filename, 10);

//                auto *t = static_cast<long *>(calloc(1, sizeof(long)));
//
//                int break_item = 0;
//                std::vector<state_node> gr = greedy(data.n, data.Z, data.p, data.z, 0, &break_item, &ub);
//
//                std::ofstream myfile(data.name + "/benchmark/fractional_greedy.txt");
//                myfile << (long) ceil(ub) << std::endl;
//                myfile.close();
//
//                std::ofstream myfile1(data.name + "/benchmark/greedy.txt");
//                myfile1 << gr[0].P << std::endl;
//                myfile1.close();
//
//                long zzz = cpp_combo_wrap(data.n, data.p, data.z, data.Z, data.name, t, 0, false, false);
//
//                std::cout << "simulating" << std::endl;
//                int count = 0;
//                for (int i = 0; i < 400; ++i) {
//                    QMaxSearch search{data, zzz, 300, "comp"};
//                    gr = search.execute(200);
//
//                    if (gr[0].P == zzz) count++;
//                    search.bnb.clear();
//                    search.bnb.shrink_to_fit();
//                    search.gates.clear();
//                    search.gates.shrink_to_fit();
//                    search.qtg.clear();
//                    search.qtg.shrink_to_fit();
//                }
//                std::cout << (double) count / 4 << "% " << std::endl;
            }
        }
    }

    return 0;
}
