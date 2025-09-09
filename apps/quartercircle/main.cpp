#include "rng.h"
#include "io.h"
#include <iostream>
#include <cstdint>
#include <string>

int main(int argc, char** argv) {
    // Read input using I/O module
    QuarterCircleInput input = readQuarterCircleInput("input.txt");
    std::uint64_t numSamples = input.samples;
    std::uint64_t seed = input.seed;

    Random rng(seed);
    std::uint64_t inside = 0;

    for (std::uint64_t i = 0; i < numSamples; ++i) {
        double x = rng.uniform01();
        double y = rng.uniform01();
        if (x * x + y * y <= 1.0) {
            ++inside;
        }
    }

    double piEstimate = 4.0 * static_cast<double>(inside) / static_cast<double>(numSamples);
    
    // Output using variadic template
    printAndSave("output.txt", "samples=", numSamples, " seed=", seed, " pi≈", piEstimate);
    
    return 0;
}
