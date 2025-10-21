#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "../../include/physics.h"
#include "../../include/random_sampling.h"
#include "../../include/helpers.h"
#include "../../include/statistics.h"

int main() {
    std::cout << "Neutron Slowing Down Application - H1 vs H2 Comparison" << std::endl;
    
    try {
        const int NUM_RUNS = 2000;
        const int MAX_STEPS = 80;  
        
        // Create H1 compound
        Substance hydrogen_h1;
        NuclearData h1_data = readNuclearDataFile("../data/H1.dat");
        hydrogen_h1.species.push_back(h1_data);
        hydrogen_h1.stoichiometric_coeffs.push_back(1);  // 1 hydrogen atom
        
        Compound hydrogen_h1_compound(0.08988);  // Hydrogen gas density at STP (g/cm³)
        hydrogen_h1_compound.substances.push_back(hydrogen_h1);
        hydrogen_h1_compound.molar_fractions.push_back(1.0);  // 100% H1
        
        // Create H2 compound
        Substance hydrogen_h2;
        NuclearData h2_data = readNuclearDataFile("../data/H2.dat");
        hydrogen_h2.species.push_back(h2_data);
        hydrogen_h2.stoichiometric_coeffs.push_back(1);  // 1 deuterium atom
        
        Compound hydrogen_h2_compound(0.08988);  // Same density for fair comparison
        hydrogen_h2_compound.substances.push_back(hydrogen_h2);
        hydrogen_h2_compound.molar_fractions.push_back(1.0);  // 100% H2
        
        // Create initial neutron with exactly 2 MeV energy and random direction
        Random rng_for_direction(12345);
        std::vector<double> random_direction = sampleRandomDirection(rng_for_direction);
        Neutron initial_neutron(2.0e6, random_direction[0], random_direction[1], random_direction[2], false);  // 2 MeV
        
        // Enable logging for reaction statistics
        ReactionLogger::getInstance().enable();
        
        // Run H1 simulation with logging
        std::cout << "Running H1 simulation with logging..." << std::endl;
        ReactionLogger::getInstance().clear();
        std::vector<double> h1_energy_history = averageEnergyOfSingleNeutron(initial_neutron, hydrogen_h1_compound, NUM_RUNS, MAX_STEPS);
        
        // Print and save H1 reaction statistics
        Statistics::saveReactionStatistics("../plot/h1_reaction_stats.dat", "H1 Reaction Statistics", NUM_RUNS * MAX_STEPS);
        
        // Run H2 simulation with logging
        std::cout << "\nRunning H2 simulation with logging..." << std::endl;
        ReactionLogger::getInstance().clear();
        std::vector<double> h2_energy_history = averageEnergyOfSingleNeutron(initial_neutron, hydrogen_h2_compound, NUM_RUNS, MAX_STEPS);
        
        // Print and save H2 reaction statistics
        Statistics::saveReactionStatistics("../plot/h2_reaction_stats.dat", "H2 Reaction Statistics", NUM_RUNS * MAX_STEPS);
        
        // Output combined H1 and H2 data to file
        std::string nslowing_output_file = "../plot/nslowing_h1_h2.dat";
        clearFileIfExists(nslowing_output_file);
        
        for (size_t i = 0; i < h1_energy_history.size(); ++i) {
            writeTable(nslowing_output_file, "step", static_cast<int>(i), 
                      "h1_energy", h1_energy_history[i],
                      "h2_energy", h2_energy_history[i]);
        }
        
        // Calculate stationary slowing down for H1 and H2
        
        // Calculate H1 stationary slowing (use same number of steps as Monte Carlo)
        std::vector<double> h1_stationary = stationarySlowingDown(2.0e6, h1_data, MAX_STEPS);  // 2 MeV initial energy
        
        // Calculate H2 stationary slowing (use same number of steps as Monte Carlo)
        std::vector<double> h2_stationary = stationarySlowingDown(2.0e6, h2_data, MAX_STEPS);  // 2 MeV initial energy
        
        // Output combined reference data to file (only first and last entries for straight lines)
        std::string reference_output_file = "../plot/nslowing_h1_h2_reference.dat";
        clearFileIfExists(reference_output_file);
        
        // Write first entries
        writeTable(reference_output_file, "step", 0, 
                  "h1_energy", h1_stationary[0],
                  "h2_energy", h2_stationary[0]);
        
        // Write last entries
        writeTable(reference_output_file, "step", static_cast<int>(h1_stationary.size()-1), 
                  "h1_energy", h1_stationary[h1_stationary.size()-1],
                  "h2_energy", h2_stationary[h2_stationary.size()-1]);
        
        std::cout << "Data saved to: " << nslowing_output_file << " and " << reference_output_file << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}