#include "rng.h"

Random::Random(std::uint64_t seed) : engine_(seed) {}

void Random::reseed(std::uint64_t seed) { engine_.seed(seed); }

double Random::uniform01() {
    static thread_local std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(engine_);
}

double Random::uniformxy(double min, double max) {
    static thread_local std::uniform_real_distribution<double> dist;
    return std::uniform_real_distribution<double>(min, max)(engine_);
}
