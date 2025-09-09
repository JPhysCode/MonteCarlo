#include "rng.h"
#include "io.h"
#include <iostream>
#include <cstdint>
#include <cmath>

int main() {
    // Read input using I/O module
    BuffonInput input = readBuffonInput("input.txt");
    std::uint64_t numSamples = input.samples;
    std::uint64_t seed = input.seed;
    double needleLength = input.needleLength;
    double lineSpacing = input.lineSpacing;

    Random rng(seed);
    std::uint64_t hits = 0;

    // Start timing the computation
    Timer timer;
    timer.start();

    for (std::uint64_t i = 0; i < numSamples; ++i) {
        // Random needle position (distance from nearest line)
        double y = rng.uniformxy(0.0, lineSpacing / 2.0);
        
        // Random needle angle
        double theta = rng.uniformxy(0.0, M_PI / 2.0);
        
        // Check if needle crosses a line
        // Needle crosses if: y <= (needleLength/2) * sin(theta)
        if (y <= (needleLength / 2.0) * std::sin(theta)) {
            ++hits;
        }
    }

    // Calculate results
    // Buffon's formula: π ≈ (2 * needleLength * numSamples) / (lineSpacing * hits)
    double piEstimate = (2.0 * needleLength * static_cast<double>(numSamples)) / 
                       (lineSpacing * static_cast<double>(hits));
    
    // Stop timing after all computation is complete
    timer.stop();
    double runtime = timer.getElapsedSeconds();
    
    // Output using variadic template including runtime
    printAndSave("output.txt", "samples=", numSamples, " seed=", seed, 
                 " needle=", needleLength, " spacing=", lineSpacing,
                 " hits=", hits, " pi≈", piEstimate, " runtime=", runtime, "s");
    
    return 0;
}
