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

#endif // HELPERS_H
