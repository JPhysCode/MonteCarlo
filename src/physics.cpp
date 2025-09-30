#include "physics.h"
#include "helpers.h"
#include <algorithm>
#include <stdexcept>

// Implementation of physics constants and calculations for nuclear transport

// Calculate total cross section by summing all MT data (except MT0) pointwise
void calculateTotalCrossSection(NuclearData& nuclear_data) {
    
    // Check if MT1 already exists
    if (nuclear_data.mt_data.find(1) != nuclear_data.mt_data.end()) {
        return; // Do nothing if MT1 already exists
    }
    
    // Find MT with the most energy-cross section pairs (excluding MT0)
    int max_points = 0;
    int reference_mt = -1;
    
    for (const auto& mt_pair : nuclear_data.mt_data) {
        int mt_number = mt_pair.first;
        if (mt_number != 0) { // Skip MT0
            int num_points = mt_pair.second.num_ec_pairs;
            if (num_points > max_points) {
                max_points = num_points;
                reference_mt = mt_number;
            }
        }
    }
    
    // Check if we found any MT data (excluding MT0)
    if (reference_mt == -1) {
        throw std::runtime_error("No MT data found (excluding MT0) to calculate total cross section");
    }
    
    // Get the reference MT data (the one with most points)
    const MTData& reference_data = nuclear_data.mt_data.at(reference_mt);
    
    // Create MT1 data structure
    MTData total_cross_section;
    total_cross_section.qval = 0.0; // Total cross section has Q=0
    total_cross_section.num_ec_pairs = max_points;
    total_cross_section.efunc.reserve(max_points);
    
    // Calculate total cross section at each energy point
    for (const auto& energy_point : reference_data.efunc) {
        double energy = energy_point.energy;
        double total_sigma = 0.0;
        
        // Sum cross sections from all MT data (excluding MT0)
        for (const auto& mt_pair : nuclear_data.mt_data) {
            int mt_number = mt_pair.first;
            if (mt_number != 0) { // Skip MT0
                const MTData& mt_data = mt_pair.second;
                try {
                    double sigma = interpolatedValue(mt_data, energy);
                    total_sigma += sigma;
                } catch (const std::exception& e) {
                    // If interpolation fails, skip this MT for this energy point
                    continue;
                }
            }
        }
        
        // Store the total cross section at this energy
        EnergyCrossSectionPair total_point;
        total_point.energy = energy;
        total_point.cross_section = total_sigma;
        total_cross_section.efunc.push_back(total_point);
    }
    
    // Store MT1 in the nuclear data
    nuclear_data.mt_data[1] = total_cross_section;
}
