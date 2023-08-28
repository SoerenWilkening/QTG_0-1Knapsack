#include "common/knapsack.h"
#include "common/utils.hpp"
#include <cstdint>
#include <memory>

namespace generator {
    std::unique_ptr<utils::cpp_knapsack>
    generate(int n, int cap, int classes, double frac, double eps, int small);
}
