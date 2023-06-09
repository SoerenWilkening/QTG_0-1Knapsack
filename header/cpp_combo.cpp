//
// Created by Sören Wilkening on 10.05.23.
//

/* 
 * =============================================================================
 *                                  includes
 * =============================================================================
 */

#include "cpp_combo.h"

bool fileExists(const std::string &filename) {
    std::ifstream file(filename);
    return file.good();
}

uint64_t rdtsc() {
    uint32_t lo, hi;
    __asm__ __volatile__ (
            "rdtsc" : "=a" (lo), "=d" (hi)
            );
    return ((uint64_t)hi << 32) | lo;
}

num_t cpp_combo_wrap(knapsack_t* k,
                    std::string name,
                    uint64_t* timerecord,
                    bit_t first_item,
                    bool_t define_sol,
                    bool_t read,
                    bool_t exec_combo) {

    num_t opt_sol;
    
    /* we fist check if the instance is trivial. If it is, combo runs into problems */
    if (is_trivial(k)) {
        return 0;
    }

    /* if it is not trivial, we check, if we have already caclulated and saved the combo output */
    fs::path myFilePath = name + "/combo/size=" + std::to_string(k->size - first_item) + "_capacity=" + std::to_string(capacity) + ".txt";

    if (fileExists(myFilePath) && read) {
        FILE *file = fopen(myFilePath.c_str(), "r");
        fscanf(file, "%"PRIu64, &opt_sol);
        fclose(file);
        return opt_sol;
    }

    //Can I simply define the initial lower bound as zero?
    num_t lbi = 0;
    bool_t def = define_sol;
    bool_t relx = FALSE;
    num_t ubi;
    ubi = 0;

    if(exec_combo) {
        uint64_t start = rdtsc();
        /* type punning: item_t to item */
        opt_sol = combo((item*)(k->items), (item*)(k->items + k->size - 1), k->capacity, lbi, ubi, def, relx); //opt_sol is the solution from the algorithm

        uint64_t end = rdtsc();
        uint64_t elapsed_cycles = end - start;

        *timerecord = elapsed_cycles;

        /* if combo took more than .0003 seconds, we save the result */
        if ((double) *timerecord / (2.6 * pow(10, 9)) > .0003) {
            fs::create_directories(myFilePath.parent_path());
            std::ofstream myfile(myFilePath);
            if (myfile.is_open()) {
                myfile << opt_sol << std::endl;
                myfile.close();
            } else std::cout << "Failed to open file.\n";
        }
    }
    return opt_sol;
}