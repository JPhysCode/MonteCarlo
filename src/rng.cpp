#include "rng.h"

// Constructor
Random::Random(std::uint64_t seed) : engine_(seed) {}

// Reseed the generator
void Random::reseed(std::uint64_t seed) { engine_.seed(seed); }

// Uniform [0,1)
double Random::uniform01() {
    static thread_local std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(engine_);
}

// Uniform [min,max)
double Random::uniformxy(double min, double max) {
    static thread_local std::uniform_real_distribution<double> dist;
    return std::uniform_real_distribution<double>(min, max)(engine_);
}
