#include "io.h"
#include "buffon_pi.h"
#include "statistics.h"
#include <iostream>
#include <cstdint>
#include <vector>

int main() {
    // Clear output file if it exists
    clearFileIfExists("output.txt");
    
    // Count number of data rows
    std::size_t numRows = countDataRows("input.txt");
    
    // Run simulation for each data row
    for (std::size_t row = 0; row < numRows; ++row) {
        // Read input for current row
        GenericInput input = readKeyValueInput("input.txt", row);
        std::uint64_t numSamples = input.getUInt64("samples");
        std::uint64_t seed = input.getUInt64("seed");
        double needleLength = input.getDouble("needle");
        double lineSpacing = input.getDouble("spacing");
        std::uint64_t numRuns = input.getUInt64("runs");

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
            // Write each pi estimate to pi_estimates.txt (header once, rows appended)
            writeTable("pi_estimates.txt", "pi", result.piEstimate);
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
    }
    
    return 0;
}
