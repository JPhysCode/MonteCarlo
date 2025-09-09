#include "io.h"
#include "quartercircle_pi.h"
#include <iostream>
#include <cstdint>
#include <string>

int main(int argc, char** argv) {
    // Read input using I/O module
    QuarterCircleInput input = readQuarterCircleInput("input.txt");
    std::uint64_t numSamples = input.samples;
    std::uint64_t seed = input.seed;

    // Start timing the computation
    Timer timer;
    timer.start();

    // Run the quarter circle simulation
    double piEstimate = runQuarterCircleSimulation(numSamples, seed);
    
    // Stop timing after all computation is complete
    timer.stop();
    double runtime = timer.getElapsedSeconds();
    
    // Output using variadic template including runtime
    printAndSave("output.txt", "samples=", numSamples, " seed=", seed, " pi≈", piEstimate, " runtime=", runtime, "s");
    
    return 0;
}
