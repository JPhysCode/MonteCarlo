#include "buffon_pi.h"
#include "rng.h"
#include <cmath>

double runBuffonSimulation(std::uint64_t numSamples, std::uint64_t seed, double needleLength, double lineSpacing) {
    Random rng(seed);
    std::uint64_t hits = 0;

    // Buffon's needle simulation loop
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

    // Buffon's formula: π ≈ (2 * needleLength * numSamples) / (lineSpacing * hits)
    double piEstimate = (2.0 * needleLength * static_cast<double>(numSamples)) / 
                       (lineSpacing * static_cast<double>(hits));
    
    return piEstimate;
}
