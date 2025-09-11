#include "io.h"
#include "buffon_pi.h"
#include <iostream>
#include <cstdint>

int main() {
    // Read input using I/O module
    BuffonInput input = readBuffonInput("input.txt");
    std::uint64_t numSamples = input.samples;
    std::uint64_t seed = input.seed;
    double needleLength = input.needleLength;
    double lineSpacing = input.lineSpacing;

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
    
    return 0;
}
