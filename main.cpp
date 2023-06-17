#include <iostream>
#include <chrono>
#include <filesystem>
#include <unistd.h>
#include <string>
#include <limits>
#include <fstream>
#include <sstream>

#include "header/instance.h"
#include "header/cpp_combo.h"
#include "header/greedy.h"
#include "header/simulator.h"
#include "header/gate_count.h"

namespace fs = std::filesystem;

int benching(std::string filename, int reps, int def_sol) {
    knapsack_instance data = read_instance(filename);

    for (int x = 0; x < reps; ++x) {
        long peak_ram = 0;
        for (int i = 0; i <= 1; ++i) {

            std::string command = "echo \"$(/usr/bin/time -l ./bench " + filename +
                                  " " + std::to_string(i) + " " +
                                  std::to_string(def_sol) + " 2>&1)\" | grep \"maximum resident set size\"";

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

    std::string filename;
    int M;
    double bias;
    knapsack_instance data;

    std::ifstream file("benchmark_parameters.txt");
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string parameter;

        iss >> parameter;
        filename = parameter;
        iss >> parameter;
        M = std::stoi(parameter);
        iss >> parameter;
        bias = std::stod(parameter);
        iss >> parameter;

        std::cout << filename << " M = " << M << " bias = " << bias << " sim = " << parameter << std::endl;
        std::cout << "benchmarking" << std::endl;

        data = read_instance(filename);

        break_item = 0;
        long zzz = cpp_combo_wrap(data.n, data.p, data.z, data.Z, data.name, &t, 0, false, false);
        std::vector<state_node> gr = greedy(data.n, data.Z, data.p, data.z, 0, &break_item, &ub);

        std::ofstream myfile(data.name + "/benchmark/fractional_greedy.txt");
        myfile << (long) ceil(ub) << std::endl;
        myfile.close();

        std::ofstream myfile1(data.name + "/benchmark/greedy.txt");
        myfile1 << gr[0].P << std::endl;
        myfile1.close();

        benching(filename, 1, 0); // benchmarking classical algorithm
        benching(filename, 1, 1); // benchmarking classical algorithm

        if (parameter == "yes") {
            std::cout << "simulating" << std::endl;
            int count = 0;

            QMaxSearch search{data, zzz, bias, "comp"};
            for (int i = 0; i < data.n; ++i) {
                search.k->items[i].cost = data.z[i];
                search.k->items[i].profit = data.p[i];
            }

            for (int i = 0; i < 3; ++i) {
                std::cout << i << std::endl;
                gr = search.execute(M);

                if (gr[0].P == zzz) count++;
            }

            mpz_clear(search.previous_sol);
            search.gates2.clear();
            search.qtg2.clear();
            std::cout << (double) count / 5 << "% " << std::endl;
        }
    }

    return 0;
}
