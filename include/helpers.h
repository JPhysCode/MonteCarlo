#ifndef HELPERS_H
#define HELPERS_H

#include "io.h"

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

#endif // HELPERS_H
