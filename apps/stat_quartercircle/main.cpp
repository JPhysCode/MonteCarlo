#include "io.h"
#include "quartercircle_pi.h"
#include "statistics.h"
#include <iostream>
#include <cstdint>
#include <vector>

int main() {
    // Read input using I/O module
    StatQuarterCircleInput input = readStatQuarterCircleInput("input.txt");
    std::uint64_t numSamples = input.samples;
    std::uint64_t seed = input.seed;
    std::uint64_t numRuns = input.numRuns;

    // Run multiple simulations
    std::vector<double> piValues;
    std::vector<double> runtimes;
    piValues.reserve(numRuns);
    runtimes.reserve(numRuns);
    
    for (std::uint64_t run = 0; run < numRuns; ++run) {
        // Use different seed for each run to ensure independence
        std::uint64_t runSeed = seed + run;
        QuarterCircleResult result = runQuarterCircleSimulation(numSamples, runSeed);
        piValues.push_back(result.piEstimate);
        runtimes.push_back(result.runtime);
    }

    // Calculate all statistics
    StatisticalResult stats = Statistics::calculateAll(piValues, runtimes);
    
    // Output using variadic template including statistics
    printAndSave("output.txt", "runs=", numRuns, " samples=", numSamples, " seed=", seed,
                 " mean=", stats.mean, " stddev=", stats.stdDev, 
                 " avg_runtime=", stats.avgRuntime, "s fom=", stats.fom,
                 " normality_p=", stats.normalityPValue);
    
    return 0;
}
