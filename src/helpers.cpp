#include "helpers.h"
#include <stdexcept>

// Implementation of helper functions for nuclear transport calculations

// Linear interpolation function
// Uses the formula: σ(E) = ((σ_{j+1} - σ_j) / (E_{j+1} - E_j)) * (E - E_j) + σ_j
double linearInterpolate(const EnergyCrossSectionPair& point1, 
                        const EnergyCrossSectionPair& point2, 
                        double energy) {
    
    // Check if energy is within the interpolation range
    if (energy < point1.energy || energy > point2.energy) {
        throw std::runtime_error("Energy is outside interpolation range");
    }
    
    // Avoid division by zero
    if (point2.energy == point1.energy) {
        return point1.cross_section;
    }
    
    // Linear interpolation formula
    double slope = (point2.cross_section - point1.cross_section) / (point2.energy - point1.energy);
    double interpolated_value = slope * (energy - point1.energy) + point1.cross_section;
    
    return interpolated_value;
}

// Find interpolated cross section value for given energy from MTData
double interpolatedValue(const MTData& mt_data, double energy) {
    
    // Check if MTData has any points
    if (mt_data.efunc.empty()) {
        throw std::runtime_error("MTData contains no energy-cross section pairs");
    }
    
    // If only one point, return its cross section
    if (mt_data.efunc.size() == 1) {
        return mt_data.efunc[0].cross_section;
    }
    
    // Find the two points to interpolate between
    for (size_t i = 0; i < mt_data.efunc.size() - 1; ++i) {
        const EnergyCrossSectionPair& point1 = mt_data.efunc[i];
        const EnergyCrossSectionPair& point2 = mt_data.efunc[i + 1];
        
        // Check if energy falls between these two points
        if (energy >= point1.energy && energy <= point2.energy) {
            return linearInterpolate(point1, point2, energy);
        }
    }
    
    // If energy is below the lowest point, return the lowest cross section
    if (energy < mt_data.efunc[0].energy) {
        return mt_data.efunc[0].cross_section;
    }
    
    // If energy is above the highest point, return the highest cross section
    if (energy > mt_data.efunc.back().energy) {
        return mt_data.efunc.back().cross_section;
    }
    
    // This should never be reached, but just in case
    throw std::runtime_error("Could not find appropriate interpolation points");
}
