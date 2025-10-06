#ifndef FISSION_H
#define FISSION_H

#include "physics.h"
#include "random_sampling.h"
#include <vector>

// Fission cross-sections and calculations

// Generate new neutrons from fission reaction
// Takes neutron and nuclear data, marks the neutron as captured, returns vector of new neutrons
std::vector<Neutron> fission(Neutron& neutron, const NuclearData& nuclear_data, Random& rng);

#endif // FISSION_H
