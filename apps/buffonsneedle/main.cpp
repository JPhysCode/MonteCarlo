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
    
    // Output using variadic template including runtime
    printAndSave("output.txt", "samples=", numSamples, " seed=", seed, 
                 " needle=", needleLength, " spacing=", lineSpacing,
                 " pi≈", result.piEstimate, " runtime=", result.runtime, "s");
    
    return 0;
}
