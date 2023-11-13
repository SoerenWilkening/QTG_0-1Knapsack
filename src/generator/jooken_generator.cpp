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

    std::unique_ptr<utils::cpp_knapsack> generate(capacity_type n, capacity_type cap,
                                                  capacity_type classes, double frac, double eps, capacity_type small) {
        auto instance = std::make_unique<utils::cpp_knapsack>();

        std::random_device device;
        std::mt19937 generator(device());
        std::uniform_int_distribution<capacity_type> distribution(1, small);

        instance->size = n;

        auto amountSmall = static_cast<capacity_type>((double) n * frac);
        capacity_type am1 = (n - amountSmall) / classes;
        double denominator = 2.0;
        capacity_type amountCtr = 0;
        for (capacity_type j = 0; j < classes; j++) {
            for (capacity_type i = 0; i < am1; i++) {
                capacity_type num1 = distribution(generator);
                capacity_type num2 = distribution(generator);

                instance->items.emplace_back((capacity_type) ((1 / denominator + eps) * (double)cap + (double)num1),
                                             (capacity_type) ((1 / denominator + eps) * (double)cap + (double)num2));
                amountCtr++;
            }
            denominator *= 2;
        }
        for (capacity_type i = amountCtr; i < n; i++) {
            capacity_type num1 = distribution(generator);
            capacity_type num2 = distribution(generator);
            instance->items.emplace_back(num1, num2);
        }

        instance->capacity = cap;
        instance->name = string_format("n_%lld_c_%lld_g_%lld_f_%.2f_eps_%.5f_s_%lld",
                                       n, cap, classes, frac, eps, small);

        return instance;
    }
}
