#include "io.h"
#include "buffon_pi.h"
#include <iostream>
#include <cstdint>

int main() {
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

        // Run the Buffon's needle simulation
        BuffonResult result = runBuffonSimulation(numSamples, seed, needleLength, lineSpacing);
        
        // Tabular two-row output
        writeTable("output.txt",
                   "samples", numSamples,
                   "seed", seed,
                   "needle", needleLength,
                   "spacing", lineSpacing,
                   "pi", result.piEstimate,
                   "runtime_s", result.runtime);
    }
    
    return 0;
}
