#include "io.h"
#include "quartercircle_pi.h"
#include <iostream>
#include <cstdint>
#include <string>

int main(int argc, char** argv) {
    // Read input using I/O module
    QuarterCircleInput input = readQuarterCircleInput("input.txt");
    std::uint64_t numSamples = input.samples;
    std::uint64_t seed = input.seed;

    // Run the quarter circle simulation
    QuarterCircleResult result = runQuarterCircleSimulation(numSamples, seed);
    
    // Output using variadic template including runtime
    printAndSave("output.txt", "samples=", numSamples, " seed=", seed, " pi≈", result.piEstimate, " runtime=", result.runtime, "s");
    
    return 0;
}
