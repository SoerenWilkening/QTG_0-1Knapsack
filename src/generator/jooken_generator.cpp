#include "generator/jooken_generator.h"
#include <iostream>
#include <random>
#include <memory>
#include <string>
#include <stdexcept>

namespace generator {


    template<typename ... Args>
    std::string string_format( const std::string& format, Args ... args )
    {
        int size_s = std::snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
        if( size_s <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
        auto size = static_cast<size_t>( size_s );
        std::unique_ptr<char[]> buf( new char[ size ] );
        std::snprintf( buf.get(), size, format.c_str(), args ... );
        return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
    }

    std::unique_ptr<utils::cpp_knapsack> generate(int n, int cap,
                                       int classes, double frac, double eps, int small) {
        auto instance = std::make_unique<utils::cpp_knapsack>();

        std::random_device device;
        std::mt19937 generator(device());
        std::uniform_int_distribution<int> distribution(1, small);

        instance->size = n;

        int amountSmall = static_cast<int>(n * frac);
        int am1 = (n - amountSmall) / classes;
        double denominator = 2.0;
        int amountCtr = 0;
        for (int j = 0; j < classes; j++) {
            for (int i = 0; i < am1; i++) {
                int num1 = distribution(generator);
                int num2 = distribution(generator);

                instance->items.emplace_back((int) ((1 / denominator + eps) * cap + num1),
                                             (int) ((1 / denominator + eps) * cap + num2));
                amountCtr++;
            }
            denominator *= 2;
        }
        for (int i = amountCtr; i < n; i++) {
            int num1 = distribution(generator);
            int num2 = distribution(generator);
            instance->items.emplace_back(num1, num2);
        }

        instance->capacity = cap;

        instance->name = string_format("n_%d_c_%d_g_%d_f_%.2f_e_%.2f_s_%d",
                                       n, cap, classes, frac, eps, small);

        return instance;
    }
}
