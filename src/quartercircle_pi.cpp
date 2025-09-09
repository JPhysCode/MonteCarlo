#include "quartercircle_pi.h"
#include "rng.h"
#include "io.h"

QuarterCircleResult runQuarterCircleSimulation(std::uint64_t numSamples, std::uint64_t seed) {
    Random rng(seed);
    std::uint64_t inside = 0;

    // Start timing the computation
    Timer timer;
    timer.start();

    // Monte Carlo simulation loop
    for (std::uint64_t i = 0; i < numSamples; ++i) {
        double x = rng.uniform01();
        double y = rng.uniform01();
        if (x * x + y * y <= 1.0) {
            ++inside;
        }
    }

    // Calculate π estimate
    double piEstimate = 4.0 * static_cast<double>(inside) / static_cast<double>(numSamples);
    
    // Stop timing after all computation is complete
    timer.stop();
    double runtime = timer.getElapsedSeconds();
    
    return {piEstimate, runtime};
}
