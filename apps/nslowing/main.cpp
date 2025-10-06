#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "../../include/physics.h"
#include "../../include/random_sampling.h"
#include "../../include/helpers.h"

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
        
        std::cout << "Initial neutron energy: " << initial_neutron.energy << " eV" << std::endl;
        
        // Run H1 simulations using physics function
        std::cout << "Running " << NUM_RUNS << " simulations for H1..." << std::endl;
        std::vector<double> h1_energy_history = averageEnergyOfSingleNeutron(initial_neutron, hydrogen_h1_compound, NUM_RUNS, MAX_STEPS);
        
        // Run H2 simulations using physics function
        std::cout << "Running " << NUM_RUNS << " simulations for H2..." << std::endl;
        std::vector<double> h2_energy_history = averageEnergyOfSingleNeutron(initial_neutron, hydrogen_h2_compound, NUM_RUNS, MAX_STEPS);
        
        std::cout << "All simulations completed!" << std::endl;
        std::cout << "H1 average energy history computed with " << h1_energy_history.size() << " steps" << std::endl;
        std::cout << "H2 average energy history computed with " << h2_energy_history.size() << " steps" << std::endl;
        
        // Output H1 energy history to file
        std::string h1_output_file = "../plot/neutron_slowing_h1.dat";
        clearFileIfExists(h1_output_file);
        
        for (size_t i = 0; i < h1_energy_history.size(); ++i) {
            writeTable(h1_output_file, "step", static_cast<int>(i), "energy", h1_energy_history[i]);
        }
        
        std::cout << "H1 average energy history saved to: " << h1_output_file << std::endl;
        
        // Output H2 energy history to file
        std::string h2_output_file = "../plot/neutron_slowing_h2.dat";
        clearFileIfExists(h2_output_file);
        
        for (size_t i = 0; i < h2_energy_history.size(); ++i) {
            writeTable(h2_output_file, "step", static_cast<int>(i), "energy", h2_energy_history[i]);
        }
        
        std::cout << "H2 average energy history saved to: " << h2_output_file << std::endl;
        
        // Calculate stationary slowing down for H1 and H2
        std::cout << "\nCalculating stationary slowing down..." << std::endl;
        
        // Calculate H1 stationary slowing (use same number of steps as Monte Carlo)
        std::vector<double> h1_stationary = stationarySlowingDown(2.0e6, h1_data, MAX_STEPS);  // 2 MeV initial energy
        
        // Calculate H2 stationary slowing (use same number of steps as Monte Carlo)
        std::vector<double> h2_stationary = stationarySlowingDown(2.0e6, h2_data, MAX_STEPS);  // 2 MeV initial energy
        
        // Output H1 stationary slowing to file (only first and last entries for straight line)
        std::string h1_stationary_file = "../plot/neutron_slowing_h1_stationary.dat";
        clearFileIfExists(h1_stationary_file);
        
        // Write first entry
        writeTable(h1_stationary_file, "step", 0, "energy", h1_stationary[0]);
        // Write last entry
        writeTable(h1_stationary_file, "step", static_cast<int>(h1_stationary.size()-1), "energy", h1_stationary[h1_stationary.size()-1]);
        
        std::cout << "H1 stationary slowing saved to: " << h1_stationary_file << std::endl;
        
        // Output H2 stationary slowing to file (only first and last entries for straight line)
        std::string h2_stationary_file = "../plot/neutron_slowing_h2_stationary.dat";
        clearFileIfExists(h2_stationary_file);
        
        // Write first entry
        writeTable(h2_stationary_file, "step", 0, "energy", h2_stationary[0]);
        // Write last entry
        writeTable(h2_stationary_file, "step", static_cast<int>(h2_stationary.size()-1), "energy", h2_stationary[h2_stationary.size()-1]);
        
        std::cout << "H2 stationary slowing saved to: " << h2_stationary_file << std::endl;
        
        std::cout << "\nComparison complete! All datasets saved to plot directory." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}