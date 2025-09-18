#include "buffon_pi.h"
#include "rng.h"
#include "io.h"
#include <cmath>

BuffonResult runBuffonSimulation(std::uint64_t numSamples, std::uint64_t seed, double needleLength, double lineSpacing) {
    Random rng(seed);
    std::uint64_t hits = 0;

    // Start timing the computation
    Timer timer;
    timer.start();

    // Buffon's needle simulation loop
    for (std::uint64_t i = 0; i < numSamples; ++i) {
        // Random needle position (distance from nearest line)
        double y = rng.uniformxy(0.0, lineSpacing / 2.0);
        
        // Random needle angle
        double theta = rng.uniformxy(0.0, M_PI / 2.0);
        
        // Check if needle crosses a line
        if (y <= (needleLength / 2.0) * std::sin(theta)) {
            ++hits;
        }
    }

    // Buffon's formula
    double piEstimate = (2.0 * needleLength * static_cast<double>(numSamples)) / 
                       (lineSpacing * static_cast<double>(hits));
    
    // Stop timing after all computation is complete
    timer.stop();
    double runtime = timer.getElapsedSeconds();
    
    return {piEstimate, runtime};
}
