#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "../../include/physics.h"
#include "../../include/random_sampling.h"

// Function to run simulations for a given compound
std::vector<double> runSimulations(const Compound& compound, const std::string& isotope_name, int num_runs, int max_steps) {
    std::cout << "Running " << num_runs << " simulations for " << isotope_name << "..." << std::endl;
    
    // Store all energy histories
    std::vector<std::vector<double>> all_energy_histories;
    std::vector<int> run_step_counts;
    
    // Run multiple simulations
    for (int run = 0; run < num_runs; ++run) {
        if ((run + 1) % 50 == 0) {
            std::cout << "Completed " << (run + 1) << " runs for " << isotope_name << "..." << std::endl;
        }
        
        // Create random number generator with different seed for each run
        Random rng(12345 + run);
        
        // Create neutron array with one 2 MeV neutron
        std::vector<Neutron> neutrons;
        
        // Create initial neutron with exactly 2 MeV energy and random direction
        std::vector<double> random_direction = sampleRandomDirection(rng);
        Neutron initial_neutron(2.0e6, random_direction[0], random_direction[1], random_direction[2], false);  // 2 MeV
        neutrons.push_back(initial_neutron);
        
        // Array to track energy at each step for this run
        std::vector<double> energy_history;
        energy_history.push_back(neutrons[0].energy);  // Initial energy
        
        int step_count = 0;
        
        // Process collisions until the first neutron is captured or max steps reached
        while (!neutrons[0].captured && step_count < max_steps) {
            step_count++;
            
            // Process neutron collision
            processNeutronCollision(neutrons, compound, rng);
            
            // Record energy of the first neutron
            energy_history.push_back(neutrons[0].energy);
        }
        
        // Store this run's data
        all_energy_histories.push_back(energy_history);
        run_step_counts.push_back(step_count);
    }
    
    std::cout << "All " << isotope_name << " simulations completed!" << std::endl;
    
    // Find the maximum number of steps across all runs
    int max_steps_actual = *std::max_element(run_step_counts.begin(), run_step_counts.end());
    std::cout << "Maximum steps in any " << isotope_name << " run: " << max_steps_actual << std::endl;
    
    // Compute average energy array
    std::vector<double> average_energy_history;
    
    for (int step = 0; step <= max_steps_actual; ++step) {
        double energy_sum = 0.0;
        int valid_runs = 0;
        
        // Sum energies from all runs that have data for this step
        for (size_t run = 0; run < all_energy_histories.size(); ++run) {
            if (step < static_cast<int>(all_energy_histories[run].size())) {
                energy_sum += all_energy_histories[run][step];
                valid_runs++;
            }
        }
        
        // Compute average energy for this step
        double average_energy = (valid_runs > 0) ? energy_sum / valid_runs : 0.0;
        average_energy_history.push_back(average_energy);
    }
    
    std::cout << "Average " << isotope_name << " energy history computed with " << average_energy_history.size() << " steps" << std::endl;
    
    // Print statistics
    std::cout << "\n" << isotope_name << " Simulation Statistics:" << std::endl;
    std::cout << "Total runs: " << num_runs << std::endl;
    std::cout << "Average steps per run: " << std::accumulate(run_step_counts.begin(), run_step_counts.end(), 0) / num_runs << std::endl;
    std::cout << "Min steps in a run: " << *std::min_element(run_step_counts.begin(), run_step_counts.end()) << std::endl;
    std::cout << "Max steps in a run: " << max_steps_actual << std::endl;
    
    return average_energy_history;
}

int main() {
    std::cout << "Neutron Slowing Down Application - H1 vs H2 Comparison" << std::endl;
    
    try {
        const int NUM_RUNS = 1000;
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
        
        // Run simulations for H1
        std::vector<double> h1_energy_history = runSimulations(hydrogen_h1_compound, "H1", NUM_RUNS, MAX_STEPS);
        
        // Run simulations for H2
        std::vector<double> h2_energy_history = runSimulations(hydrogen_h2_compound, "H2", NUM_RUNS, MAX_STEPS);
        
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
        
        std::cout << "\nComparison complete! Both datasets saved to plot directory." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}