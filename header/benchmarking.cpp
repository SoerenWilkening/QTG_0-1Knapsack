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

    long zzz = cpp_combo_wrap(data.n, data.p, data.z, data.Z, data.name, t, 0, false, false, exec_combo);

//    std::string f = "" + data.name;
//    FILE *file = fopen(f.c_str());

    return 0;
}