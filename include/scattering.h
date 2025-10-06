#ifndef SCATTERING_H
#define SCATTERING_H

#include "physics.h"
#include "random_sampling.h"

// Neutron scattering cross-sections and calculations

// Handle elastic scattering collision
// Updates the neutron's energy and direction after elastic scattering
void elasticScattering(Neutron& neutron, const NuclearData& species_data, Random& rng, double temperature_K = 293.15);

#endif // SCATTERING_H
