//
// Created by Sören Wilkening on 10.05.23.
//

#ifndef PPQA_CPP_CPP_COMBO_H
#define PPQA_CPP_CPP_COMBO_H

#include "combo.h"
#include "instance.h"
#include <fstream>
#include <filesystem>
#include <cstdint>

namespace fs = std::filesystem;

long cpp_combo_wrap(int n, std::vector<long> p, std::vector<long> w, long c,
                    std::string name,
                    double *timerecord,
                    int first_item,
                    int define_sol = 0,
                    bool read = true);

#endif //PPQA_CPP_CPP_COMBO_H
