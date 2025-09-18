#pragma once
#include <cstdint>

// Result structure for Buffon's needle pi estimation with timing
struct BuffonResult {
    double piEstimate;
    double runtime;
};

// Run Buffon's needle pi estimation algorithm
BuffonResult runBuffonSimulation(std::uint64_t numSamples, std::uint64_t seed, double needleLength, double lineSpacing);
