#pragma once

#include <cstdint>
#include <random>
#include <limits>

namespace Pip2::Test {
    template<typename T>
    class RandomIntGenerator {
    private:
        std::random_device device_;
        std::mt19937 generator_;
        std::uniform_int_distribution<T> distribution_;

    public:
        RandomIntGenerator(T min = std::numeric_limits<T>::min(), T maximum = std::numeric_limits<T>::max())
                : generator_(device_()), distribution_(min, maximum) {

        }

        T next() {
            return distribution_(generator_);
        }
    };

    template<typename T>
    RandomIntGenerator<T> make_pool_constant_random_generator()
    {
        return RandomIntGenerator<T>(0, 0x80000000);
    }

    template<typename T>
    RandomIntGenerator<T> make_normal_constant_random_generator()
    {
        return RandomIntGenerator<T>(0, 0x10000000);
    }
}