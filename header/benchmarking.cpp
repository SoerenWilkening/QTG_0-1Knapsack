//
// Created by Sören Wilkening on 22.05.23.
//

#include "iostream"
#include "instance.h"
#include "cpp_combo.h"

int main(int argc, char *argv[]){

    boolean exec_combo = true;
    if ((std::string) argv[2] == "0") exec_combo = false;

    knapsack_instance data = read_instance(argv[1]);

    long *t = (long*) calloc(0, sizeof(long));

    bool define_sol = true;
//    bool define_sol = false;

    long zzz = cpp_combo_wrap(data.n, data.p, data.z, data.Z, data.name, t, 0, define_sol, false, exec_combo);

    if ((std::string) argv[2] == "0") return 0;

    fs::create_directories(data.name + "/benchmark/");

    if (define_sol) {
        std::ofstream myfile(data.name + "/benchmark/combo_runtime_with_sol.txt", std::ios::app);
        myfile << *t << std::endl;
        myfile.close();
    }else{
        std::ofstream myfile(data.name + "/benchmark/combo_runtime.txt", std::ios::app);
        myfile << *t << std::endl;
        myfile.close();
    }

    return 0;
}