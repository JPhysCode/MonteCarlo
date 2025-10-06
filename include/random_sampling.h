#pragma once
#include <cstdint>
#include <random>
#include <vector>
#include "physics.h"

// Random number generator and sampling utilities
class Random {
public:
    explicit Random(std::uint64_t seed = 5489u);
    void reseed(std::uint64_t seed);
    double uniform01(); // [0,1)
    double uniformxy(double min, double max); // [min,max)
private:
    std::mt19937_64 engine_;
};

// Sample a random interval from a collection, weighted by interval size
// Takes a vector of interval sizes and returns the index of the selected interval
int intervalSampling(Random& rng, const std::vector<double>& interval_sizes);

// Sample a target species from a compound at a specific energy
// Uses macroscopic cross sections as weights for species selection
// Returns the nuclear data of the selected target species
NuclearData targetSampling(Random& rng, const Compound& compound, double energy);

// Sample a nuclear reaction (MT channel) for a species
// Uses cross section values as weights for reaction selection
// Input nuclear data must already be boiled down to a single energy point
// Returns the Q-value of the selected reaction
double reactionSampling(Random& rng, const NuclearData& nuclear_data);

// Sample energy from a Maxwellian distribution
// T is the nuclear temperature parameter
// Returns energy sampled from Maxwellian distribution (eV)
double maxwellianSampling(Random& rng, double T);
