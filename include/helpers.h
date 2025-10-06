#ifndef HELPERS_H
#define HELPERS_H

#include "io.h"
#include <vector>

// Helper functions for nuclear transport calculations

// Linear interpolation function
// Interpolates between two EnergyCrossSectionPair points
// Returns interpolated cross section value at given energy
double linearInterpolate(const EnergyCrossSectionPair& point1, 
                        const EnergyCrossSectionPair& point2, 
                        double energy);

// Find interpolated cross section value for given energy from MTData
// Automatically finds the appropriate two points and interpolates
double interpolatedValue(const MTData& mt_data, double energy);

// Interpolate cross-section values for an array of energies
// Takes a vector of energies and returns a vector of corresponding interpolated cross-sections
std::vector<double> interpolatedValueS(const MTData& mt_data, const std::vector<double>& energies);

// Generate n points equidistributed on a log scale between min_val and max_val
// Returns a vector of n values where each value is log-distributed
std::vector<double> logSpace(double min_val, double max_val, int n);

// Interpolate all MT values at a specific energy for given nuclear data
// Returns NuclearData with single-point MTData containing the interpolated values at the given energy
NuclearData NuclearDataAtEnergy(const NuclearData& nuclear_data, double energy);

// Convert temperature from Kelvin to energy in eV
// Uses the relationship: E = k_B * T where k_B = 8.617333262e-5 eV/K
double temperatureToEnergy(double temperature_K);

// Stationary neutron slowing down calculation
// Takes initial energy, nuclear data, and number of steps, returns array with energy at each step
// Uses the equations: ΔE/E₀ = (1-α)/2 where α = ((1-A)/(1+A))² and A = M/m
std::vector<double> stationarySlowingDown(double initial_energy, const NuclearData& nuclear_data, int num_steps = 1);

#endif // HELPERS_H
