#include "physics.h"
#include "helpers.h"
#include <algorithm>
#include <stdexcept>

// Avogadro's number (atoms/mol)
const double AVOGADRO_NUMBER = 6.02214076e23;

// Implementation of physics constants and calculations for nuclear transport

// Calculate molar mass of a substance using atomic weights from nuclear data
double calculateSubstanceMolarMass(const Substance& substance) {
    double total_molar_mass = 0.0;
    
    // Check that species and stoichiometric coefficients arrays have the same size
    if (substance.species.size() != substance.stoichiometric_coeffs.size()) {
        throw std::runtime_error("Species and stoichiometric coefficients arrays must have the same size");
    }
    
    // Calculate molar mass by summing (atomic_weight * stoichiometric_coefficient) for each species
    for (size_t i = 0; i < substance.species.size(); ++i) {
        double atomic_weight = substance.species[i].aweight;
        int stoichiometric_coeff = substance.stoichiometric_coeffs[i];
        
        total_molar_mass += atomic_weight * stoichiometric_coeff;
    }
    
    return total_molar_mass; // Returns molar mass in g/mol
}

// Calculate mean molar mass of a compound using substance molar masses and molar fractions
double calculateCompoundMolarMass(const Compound& compound) {
    double mean_molar_mass = 0.0;
    
    // Check that substances and molar fractions arrays have the same size
    if (compound.substances.size() != compound.molar_fractions.size()) {
        throw std::runtime_error("Substances and molar fractions arrays must have the same size");
    }
    
    // Calculate mean molar mass by summing (substance_molar_mass * molar_fraction) for each substance
    for (size_t i = 0; i < compound.substances.size(); ++i) {
        double substance_molar_mass = calculateSubstanceMolarMass(compound.substances[i]);
        double molar_fraction = compound.molar_fractions[i];
        
        mean_molar_mass += substance_molar_mass * molar_fraction;
    }
    
    return mean_molar_mass; // Returns mean molar mass in g/mol
}

// Calculate atom number densities for all species in all substances of a compound
void calculateAtomNumberDensities(Compound& compound) {
    // Calculate the mean molar mass of the compound
    double mean_molar_mass = calculateCompoundMolarMass(compound);
    
    // Calculate the total molar density of the compound (mol/cm³)
    double molar_density = compound.density / mean_molar_mass;
    
    // Iterate through each substance in the compound
    for (size_t i = 0; i < compound.substances.size(); ++i) {
        Substance& substance = compound.substances[i];
        double molar_fraction = compound.molar_fractions[i];
        
        // Calculate the molar density of this substance (mol/cm³)
        double substance_molar_density = molar_density * molar_fraction;
        
        // Clear and resize the atom number densities array
        substance.atom_number_densities.clear();
        substance.atom_number_densities.resize(substance.species.size());
        
        // Calculate atom number density for each species in this substance
        for (size_t j = 0; j < substance.species.size(); ++j) {
            int stoichiometric_coeff = substance.stoichiometric_coeffs[j];
            
            // Atom number density = substance_molar_density * stoichiometric_coefficient * Avogadro_number
            substance.atom_number_densities[j] = substance_molar_density * stoichiometric_coeff * AVOGADRO_NUMBER;
        }
    }
}

// Calculate total macroscopic cross section for a compound
MTData calculateTotalMacroscopicCrossSection(const Compound& compound) {
    // Create a mutable copy of the compound to calculate atom number densities
    Compound compound_copy = compound;
    
    // Calculate atom number densities for all species
    calculateAtomNumberDensities(compound_copy);
    
    std::vector<MTData> weighted_mt_data;
    
    // Iterate through each substance in the compound
    for (size_t i = 0; i < compound_copy.substances.size(); ++i) {
        const Substance& substance = compound_copy.substances[i];
        
        // Iterate through each species in the substance
        for (size_t j = 0; j < substance.species.size(); ++j) {
            const NuclearData& species_data = substance.species[j];
            double atom_number_density = substance.atom_number_densities[j];
            
            // Calculate total cross section (MT1) for this species
            NuclearData species_copy = species_data;  // Make a copy to avoid modifying original
            calculateTotalCrossSection(species_copy);
            
            // Get the total cross section (MT1) for this species
            auto mt1_it = species_copy.mt_data.find(1);
            if (mt1_it == species_copy.mt_data.end()) {
                throw std::runtime_error("Failed to calculate MT1 (total cross section) for species " + species_data.symbol);
            }
            
            const MTData& mt1_data = mt1_it->second;
            
            // Create weighted MTData by multiplying cross sections by atom number density
            MTData weighted_mt;
            weighted_mt.qval = mt1_data.qval;
            weighted_mt.num_ec_pairs = mt1_data.num_ec_pairs;
            weighted_mt.efunc.reserve(mt1_data.efunc.size());
            
            // Weight each energy-cross section pair by atom number density
            for (const auto& energy_point : mt1_data.efunc) {
                EnergyCrossSectionPair weighted_point;
                weighted_point.energy = energy_point.energy;
                weighted_point.cross_section = energy_point.cross_section * atom_number_density;
                weighted_mt.efunc.push_back(weighted_point);
            }
            
            weighted_mt_data.push_back(weighted_mt);
        }
    }
    
    // Check if we found any MT data
    if (weighted_mt_data.empty()) {
        throw std::runtime_error("No MT data found in compound for macroscopic cross section calculation");
    }
    
    // Sum all weighted MT data to get total macroscopic cross section
    return sumMTData(weighted_mt_data);
}

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

