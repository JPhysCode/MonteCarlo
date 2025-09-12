#include "io.h"
#include "quartercircle_pi.h"
#include <iostream>
#include <cstdint>
#include <string>

int main(int argc, char** argv) {
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

        // Run the quarter circle simulation
        QuarterCircleResult result = runQuarterCircleSimulation(numSamples, seed);
        
        // Tabular two-row output
        writeTable("output.txt",
                   "samples", numSamples,
                   "seed", seed,
                   "pi", result.piEstimate,
                   "runtime_s", result.runtime);
    }
    
    return 0;
}
