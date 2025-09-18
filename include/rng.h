#pragma once
#include <cstdint>
#include <random>

// Random number generator
class Random {
public:
    explicit Random(std::uint64_t seed = 5489u);
    void reseed(std::uint64_t seed);
    double uniform01(); // [0,1)
    double uniformxy(double min, double max); // [min,max)
private:
    std::mt19937_64 engine_;
};
