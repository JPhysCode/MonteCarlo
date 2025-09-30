#ifndef PHYSICS_H
#define PHYSICS_H

#include "io.h"

// Physics constants and calculations for nuclear transport

// Generic function to sum multiple MTData objects
// Finds the MTData with most energy points, uses its energy grid as base
// Interpolates all other MTData onto this grid and sums them up
MTData sumMTData(const std::vector<MTData>& mt_data_array);

// Calculate total cross section by summing all MT data (except MT0) pointwise
// Uses the energy grid from the MT with most data points
// Stores result as MT1 in the nuclear data
void calculateTotalCrossSection(NuclearData& nuclear_data);

// Get total cross section by calculating and returning MTData
// Uses calculateTotalCrossSection internally
MTData getTotalCrossSection(NuclearData& nuclear_data);

// Calculate radiative capture cross section by summing MT 102-107
// Uses the energy grid from the MT with most data points among 102-107
// Returns the MTData for radiative capture cross section
MTData getRadiativeCaptureCrossSection(const NuclearData& nuclear_data);

// Get total fission cross section (MT18)
// Returns the MTData for total fission cross section
MTData getTotalFissionCrossSection(const NuclearData& nuclear_data);

// Get total inelastic cross section by summing MT 51-91
// Returns the MTData for total inelastic cross section
MTData getTotalInelasticCrossSection(const NuclearData& nuclear_data);

#endif // PHYSICS_H
