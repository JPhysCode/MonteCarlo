#include "io.h"
#include "buffon_pi.h"
#include "statistics.h"
#include <iostream>
#include <cstdint>
#include <vector>

int main() {
    // Read input using I/O module
    StatBuffonInput input = readStatBuffonInput("input.txt");
    std::uint64_t numSamples = input.samples;
    std::uint64_t seed = input.seed;
    double needleLength = input.needleLength;
    double lineSpacing = input.lineSpacing;
    std::uint64_t numRuns = input.numRuns;

    // Run multiple simulations
    std::vector<double> piValues;
    std::vector<double> runtimes;
    piValues.reserve(numRuns);
    runtimes.reserve(numRuns);
    
    for (std::uint64_t run = 0; run < numRuns; ++run) {
        // Use different seed for each run to ensure independence
        std::uint64_t runSeed = seed + run;
        BuffonResult result = runBuffonSimulation(numSamples, runSeed, needleLength, lineSpacing);
        piValues.push_back(result.piEstimate);
        runtimes.push_back(result.runtime);
    }

    // Calculate all statistics
    StatisticalResult stats = Statistics::calculateAll(piValues, runtimes);
    
    // Tabular two-row output
    writeTable("output.txt",
               "runs", numRuns,
               "samples", numSamples,
               "seed", seed,
               "needle", needleLength,
               "spacing", lineSpacing,
               "mean", stats.mean,
               "stddev", stats.stdDev,
               "avg_runtime_s", stats.avgRuntime,
               "fom", stats.fom,
               "normality_p", stats.normalityPValue);
    
    return 0;
}
