#include "io.h"
#include "quartercircle_pi.h"
#include <iostream>
#include <cstdint>
#include <vector>
#include <cmath>

int main() {
    // Read input using I/O module
    StatQuarterCircleInput input = readStatQuarterCircleInput("input.txt");
    std::uint64_t numSamples = input.samples;
    std::uint64_t seed = input.seed;
    std::uint64_t numRuns = input.numRuns;

    // Run multiple simulations
    std::vector<double> piValues;
    piValues.reserve(numRuns);
    
    for (std::uint64_t run = 0; run < numRuns; ++run) {
        // Use different seed for each run to ensure independence
        std::uint64_t runSeed = seed + run;
        QuarterCircleResult result = runQuarterCircleSimulation(numSamples, runSeed);
        piValues.push_back(result.piEstimate);
    }

    // Calculate statistics
    double sum = 0.0;
    for (double pi : piValues) {
        sum += pi;
    }
    double mean = sum / static_cast<double>(numRuns);
    
    // Calculate standard deviation
    double variance = 0.0;
    for (double pi : piValues) {
        double diff = pi - mean;
        variance += diff * diff;
    }
    double stdDev = std::sqrt(variance / static_cast<double>(numRuns));
    
    // Output using variadic template including statistics
    printAndSave("output.txt", "runs=", numRuns, " samples=", numSamples, " seed=", seed,
                 " mean=", mean, " stddev=", stdDev);
    
    return 0;
}
