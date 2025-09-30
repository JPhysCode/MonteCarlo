#include "physics.h"
#include "helpers.h"
#include <algorithm>
#include <stdexcept>

// Implementation of physics constants and calculations for nuclear transport

// Generic function to sum multiple MTData objects
MTData sumMTData(const std::vector<MTData>& mt_data_array) {
    
    // Check if array is empty
    if (mt_data_array.empty()) {
        throw std::runtime_error("Cannot sum empty MTData array");
    }
    
    // Find the MTData with the most energy-cross section pairs
    size_t max_points = 0;
    size_t reference_index = 0;
    
    for (size_t i = 0; i < mt_data_array.size(); ++i) {
        size_t num_points = mt_data_array[i].num_ec_pairs;
        if (num_points > max_points) {
            max_points = num_points;
            reference_index = i;
        }
    }
    
    // Get the reference MTData (the one with most points)
    const MTData& reference_data = mt_data_array[reference_index];
    
    // Create summed MTData structure
    MTData summed_data;
    summed_data.qval = 0.0; // Will be set to sum of Q values
    summed_data.num_ec_pairs = max_points;
    summed_data.efunc.reserve(max_points);
    
    // Calculate total Q value
    double total_qval = 0.0;
    for (const auto& mt_data : mt_data_array) {
        total_qval += mt_data.qval;
    }
    summed_data.qval = total_qval;
    
    // Extract energy grid from reference data
    std::vector<double> energy_grid;
    energy_grid.reserve(max_points);
    for (const auto& energy_point : reference_data.efunc) {
        energy_grid.push_back(energy_point.energy);
    }
    
    // Calculate summed cross section using interpolatedValueS
    std::vector<double> summed_sigma_values(energy_grid.size(), 0.0);
    
    // Sum cross sections from all MTData
    for (const auto& mt_data : mt_data_array) {
        try {
            std::vector<double> mt_sigma_values = interpolatedValueS(mt_data, energy_grid);
            for (size_t i = 0; i < summed_sigma_values.size(); ++i) {
                summed_sigma_values[i] += mt_sigma_values[i];
            }
        } catch (const std::exception& e) {
            // If interpolation fails, skip this MTData entirely
            continue;
        }
    }
    
    // Store the summed cross section at each energy
    for (size_t i = 0; i < energy_grid.size(); ++i) {
        EnergyCrossSectionPair summed_point;
        summed_point.energy = energy_grid[i];
        summed_point.cross_section = summed_sigma_values[i];
        summed_data.efunc.push_back(summed_point);
    }
    
    return summed_data;
}

// Calculate total cross section by summing all MT data (except MT0) pointwise
void calculateTotalCrossSection(NuclearData& nuclear_data) {
    
    // Check if MT1 already exists
    if (nuclear_data.mt_data.find(1) != nuclear_data.mt_data.end()) {
        return; // Do nothing if MT1 already exists
    }
    
    // Collect all MT data (excluding MT0)
    std::vector<MTData> all_mt_data;
    for (const auto& mt_pair : nuclear_data.mt_data) {
        int mt_number = mt_pair.first;
        if (mt_number != 0) { // Skip MT0
            all_mt_data.push_back(mt_pair.second);
        }
    }
    
    // Check if we found any MT data (excluding MT0)
    if (all_mt_data.empty()) {
        throw std::runtime_error("No MT data found (excluding MT0) to calculate total cross section");
    }
    
    // Use sumMTData to calculate total cross section
    MTData total_cross_section = sumMTData(all_mt_data);
    
    // Store MT1 in the nuclear data
    nuclear_data.mt_data[1] = total_cross_section;
}

// Get total cross section by calculating and returning MTData
MTData getTotalCrossSection(NuclearData& nuclear_data) {
    // Use the existing calculateTotalCrossSection function
    calculateTotalCrossSection(nuclear_data);
    
    // Return the calculated MT1 data
    return nuclear_data.mt_data.at(1);
}

// Calculate radiative capture cross section by summing MT 102-107
MTData getRadiativeCaptureCrossSection(const NuclearData& nuclear_data) {
    
    // Define the MT numbers for radiative capture (102-107)
    std::vector<int> capture_mt_numbers = {102, 103, 104, 105, 106, 107};
    
    // Collect capture MT data that exists
    std::vector<MTData> capture_mt_data;
    for (int mt_number : capture_mt_numbers) {
        auto it = nuclear_data.mt_data.find(mt_number);
        if (it != nuclear_data.mt_data.end()) {
            capture_mt_data.push_back(it->second);
        }
    }
    
    // Check if we found any capture MT data
    if (capture_mt_data.empty()) {
        throw std::runtime_error("No radiative capture MT data (102-107) found");
    }
    
    // Use sumMTData to calculate radiative capture cross section
    return sumMTData(capture_mt_data);
}

// Get total fission cross section (MT18)
MTData getTotalFissionCrossSection(const NuclearData& nuclear_data) {
    
    // Check if MT18 exists in the nuclear data
    auto it = nuclear_data.mt_data.find(18);
    if (it == nuclear_data.mt_data.end()) {
        throw std::runtime_error("MT18 (total fission cross section) not found in nuclear data");
    }
    
    // Return the MT18 data
    return it->second;
}

// Get total inelastic cross section by summing MT 51-91
MTData getTotalInelasticCrossSection(const NuclearData& nuclear_data) {
    
    // Define the MT numbers for inelastic scattering (51-91)
    std::vector<int> inelastic_mt_numbers;
    for (int mt = 51; mt <= 91; ++mt) {
        inelastic_mt_numbers.push_back(mt);
    }
    
    // Collect inelastic MT data that exists
    std::vector<MTData> inelastic_mt_data;
    for (int mt_number : inelastic_mt_numbers) {
        auto it = nuclear_data.mt_data.find(mt_number);
        if (it != nuclear_data.mt_data.end()) {
            inelastic_mt_data.push_back(it->second);
        }
    }
    
    // Check if we found any inelastic MT data
    if (inelastic_mt_data.empty()) {
        throw std::runtime_error("No inelastic MT data (51-91) found");
    }
    
    // Use sumMTData to calculate total inelastic cross section
    return sumMTData(inelastic_mt_data);
}

