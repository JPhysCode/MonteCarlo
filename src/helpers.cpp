#include "helpers.h"
#include <stdexcept>
#include <cmath>
#include <vector>

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

// Interpolate cross-section values for an array of energies
std::vector<double> interpolatedValueS(const MTData& mt_data, const std::vector<double>& energies) {
    
    // Create the result vector
    std::vector<double> result;
    result.reserve(energies.size());
    
    // Interpolate for each energy in the input vector
    for (double energy : energies) {
        double interpolated_value = interpolatedValue(mt_data, energy);
        result.push_back(interpolated_value);
    }
    
    return result;
}

// Generate n points equidistributed on a log scale between min_val and max_val
std::vector<double> logSpace(double min_val, double max_val, int n) {
    
    // Input validation
    if (n <= 0) {
        throw std::runtime_error("Number of points must be positive");
    }
    
    if (min_val <= 0 || max_val <= 0) {
        throw std::runtime_error("Both min_val and max_val must be positive for log scale");
    }
    
    if (min_val >= max_val) {
        throw std::runtime_error("min_val must be less than max_val");
    }
    
    // Create the result vector
    std::vector<double> result;
    result.reserve(n);
    
    // Special case: if n == 1, return the midpoint on log scale
    if (n == 1) {
        double log_mid = (std::log10(min_val) + std::log10(max_val)) / 2.0;
        result.push_back(std::pow(10.0, log_mid));
        return result;
    }
    
    // Calculate log-spaced points
    // Formula: x_i = 10^(log10(min) + i * (log10(max) - log10(min)) / (n-1))
    double log_min = std::log10(min_val);
    double log_max = std::log10(max_val);
    double log_step = (log_max - log_min) / (n - 1);
    
    for (int i = 0; i < n; ++i) {
        double log_value = log_min + i * log_step;
        double linear_value = std::pow(10.0, log_value);
        result.push_back(linear_value);
    }
    
    return result;
}

// Interpolate all MT values at a specific energy for given nuclear data
NuclearData NuclearDataAtEnergy(const NuclearData& nuclear_data, double energy) {
    // Create a copy of the nuclear data with the same basic properties
    NuclearData result_data;
    result_data.symbol = nuclear_data.symbol;
    result_data.pnumber = nuclear_data.pnumber;
    result_data.mnumber = nuclear_data.mnumber;
    result_data.aweight = nuclear_data.aweight;
    result_data.temp = nuclear_data.temp;
    
    // Interpolate each MT value at the given energy
    for (const auto& mt_pair : nuclear_data.mt_data) {
        int mt_number = mt_pair.first;
        const MTData& mt_data = mt_pair.second;
        
        try {
            // Get the interpolated cross section value at the given energy
            double interpolated_cross_section = interpolatedValue(mt_data, energy);
            
            // Create a single-point MTData with the interpolated value
            MTData single_point_mt;
            single_point_mt.qval = mt_data.qval;
            single_point_mt.num_ec_pairs = 1;
            
            // Create the single energy-cross section pair
            EnergyCrossSectionPair single_point;
            single_point.energy = energy;
            single_point.cross_section = interpolated_cross_section;
            single_point_mt.efunc.push_back(single_point);
            
            // Store the interpolated MT data
            result_data.mt_data[mt_number] = single_point_mt;
            
        } catch (const std::exception& e) {
            // Skip MT values that cannot be interpolated at this energy
            // This allows the function to continue with other MT values
            continue;
        }
    }
    
    return result_data;
}

// Convert temperature from Kelvin to energy in eV
double temperatureToEnergy(double temperature_K) {
    // Boltzmann constant in eV/K
    const double k_B = 8.617333262e-5;  // eV/K
    
    return k_B * temperature_K;
}

// Stationary neutron slowing down calculation
std::vector<double> stationarySlowingDown(double initial_energy, const NuclearData& nuclear_data, int num_steps) {
    // Constants
    const double neutron_mass = 939.56542052e6;  // Neutron mass in eV/c²
    
    // Calculate target mass from atomic weight
    double target_mass = nuclear_data.aweight * 931.49410242e6;  // Target mass in eV/c²
    
    // Calculate mass ratio A = M/m
    double A = target_mass / neutron_mass;
    
    // Calculate α = ((1-A)/(1+A))²
    double alpha = ((1.0 - A) / (1.0 + A)) * ((1.0 - A) / (1.0 + A));
    
    // Calculate ΔE/E₀ = (1-α)/2
    double delta_E_over_E0 = (1.0 - alpha) / 2.0;
    
    // Initialize result array with initial energy
    std::vector<double> result;
    result.push_back(initial_energy);
    
    // Calculate energy after each step
    double current_energy = initial_energy;
    for (int step = 0; step < num_steps; ++step) {
        // Calculate energy loss ΔE for this step
        double delta_E = delta_E_over_E0 * current_energy;
        
        // Calculate new energy after collision
        current_energy = current_energy - delta_E;
        
        // Add to result array
        result.push_back(current_energy);
    }
    
    return result;
}
