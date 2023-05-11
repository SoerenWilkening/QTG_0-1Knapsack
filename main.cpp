#include <iostream>
//#include "header/RNG.h"
//#include "header/instance.h"
//#include "header/cpp_combo.h"
//#include "header/state_generator.h"
//#include "header/greedy.h"
#include <gmpxx.h>

int main() {
    /*
     * PPQA simulator written in pure C/C++
     *
     *
     * */
//    probability_distribution_sampling();
//    knapsack_instance data;
//
////    char filename[] = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_400_c_100000000_g_10_f_0.2_eps_0.01_s_100/test.in";
//    char filename[] = "/Users/sorenwilkening/Desktop/Algorithms/instances_01_KP/knapsackProblemInstances/problemInstances/n_400_c_100000000_g_2_f_0.2_eps_0.0001_s_100/test.in";
//    data = read_instance(filename);
//
//    data.n = 7;
//    data.Z = 9;
//    data.p = {3, 7, 6, 9, 8, 5, 6};
//    data.z = {4, 9, 5, 7, 6, 3, 2};
//    std::reverse(data.p.begin(), data.p.end());
//    std::reverse(data.z.begin(), data.z.end());
//
//    double *t = static_cast<double *>(calloc(1, sizeof(double)));
//
//    std::vector<int> x(data.n, 0);
//
//    long zzz = cpp_combo_wrap(data.n, data.p, data.z, data.Z, t, 0);
//    state_node gr = greedy(data.n, data.Z, data.p.data(), data.z.data(), 0);
//
//    std::cout << zzz << std::endl;
//    std::cout << gr.P << std::endl;
//
//    std::vector<state_node> bnb = breadth_first_search(data, gr.P, zzz);
//
//
//    for (int i = 0; i < bnb.size(); ++i) {
//        std::cout << bnb[i].P << " ";
//        std::cout << bnb[i].Z << " ";
//        std::cout << bnb[i].amplitude << " ";
//        std::cout << bnb[i].vector << std::endl;
//    }

    mpz_t n;
    mpz_init2(n, 400);
    mpz_setbit(n, 145);



    std::cout << n << " " << mpz_tstbit(n, 145) << " " << mpz_tstbit(n, 146) << std::endl;

    return 0;

//    int num = 6;
//
//    std::cout << ((num & (1 << 2)) != 0) << std::endl;
//
//    return 0;
}
