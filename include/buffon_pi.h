#pragma once
#include <cstdint>

// Run Buffon's needle π estimation algorithm
double runBuffonSimulation(std::uint64_t numSamples, std::uint64_t seed, double needleLength, double lineSpacing);
