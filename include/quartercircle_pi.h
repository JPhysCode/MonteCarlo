#pragma once
#include <cstdint>

// Result structure for quarter circle pi estimation with timing
struct QuarterCircleResult {
    double piEstimate;
    double runtime;
};

// Run quarter circle pi estimation algorithm
QuarterCircleResult runQuarterCircleSimulation(std::uint64_t numSamples, std::uint64_t seed);
