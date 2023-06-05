//
// Created by Sören Wilkening on 10.05.23.
//

#ifndef CPP_COMBO_H
#define CPP_COMBO_H

/* 
 * =============================================================================
 *                                  includes
 * =============================================================================
 */

#include "knapsack.h"
#include "combo.h"
#include <fstream>
#include <filesystem>
#include <cstdint>

namespace fs = std::filesystem;

long cpp_combo_wrap(int n, std::vector<long> p, std::vector<long> w, long c,
                    std::string name,
                    long *timerecord,
                    int first_item,
                    int define_sol = 0,
                    bool read = true,
                    bool exec_combo = true);

#endif
