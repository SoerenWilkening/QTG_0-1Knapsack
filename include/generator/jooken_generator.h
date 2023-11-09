#include "common/utils.hpp"
#include <cstdint>
#include <memory>

namespace generator {
    using capacity_type = long long;

    std::unique_ptr<utils::cpp_knapsack>
    generate(capacity_type n, capacity_type cap, capacity_type classes, double frac, double eps, capacity_type small);
}
