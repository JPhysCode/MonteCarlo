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

    // Start timing the computation
    Timer timer;
    timer.start();

    // Run the Buffon's needle simulation
    double piEstimate = runBuffonSimulation(numSamples, seed, needleLength, lineSpacing);
    
    // Stop timing after all computation is complete
    timer.stop();
    double runtime = timer.getElapsedSeconds();
    
    // Output using variadic template including runtime
    printAndSave("output.txt", "samples=", numSamples, " seed=", seed, 
                 " needle=", needleLength, " spacing=", lineSpacing,
                 " pi≈", piEstimate, " runtime=", runtime, "s");
    
    return 0;
}
