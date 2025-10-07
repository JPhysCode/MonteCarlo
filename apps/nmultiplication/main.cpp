#include <iostream>
#include <vector>
#include "../../include/io.h"
#include "../../include/physics.h"
#include "../../include/random_sampling.h"
#include "../../include/helpers.h"

int main() {
    std::cout << "Neutron Multiplication Application" << std::endl;
    
    // Load nuclear data
    NuclearData u238_data = readNuclearDataFile("../data/U238.dat");
    NuclearData o16_data = readNuclearDataFile("../data/O16.dat");
    
    // Create UO2 substance (1 U238 + 2 O16)
    Substance uo2_substance;
    uo2_substance.species = {u238_data, o16_data};
    uo2_substance.stoichiometric_coeffs = {1, 2};
    uo2_substance.atom_number_densities.resize(2);
    
    // Create compound with 100% UO2
    Compound uo2_compound;
    uo2_compound.substances = {uo2_substance};
    uo2_compound.molar_fractions = {1.0};
    uo2_compound.density = 10.97; // g/cm³ (typical UO2 density)
    
    // Calculate atom number densities
    calculateAtomNumberDensities(uo2_compound);
    
    // Create random number generator
    Random rng;
    
    // Create a 1 MeV neutron with random direction
    std::vector<double> direction = sampleRandomDirection(rng);
    Neutron neutron(1e6, direction[0], direction[1], direction[2]); // 1 MeV = 1e6 eV
    
    // Process neutron collisions multiple times and track neutron count
    const int NUM_RUNS = 10000;
    
    double total_neutrons = 0.0;
    
    std::cout << "Running " << NUM_RUNS << " simulations..." << std::endl;
    
    for (int run = 0; run < NUM_RUNS; ++run) {
        // Create fresh neutronbank for each run with a copy of the original neutron
        std::vector<Neutron> neutronbank = {neutron};
        
        // Process one collision
        processNeutronCollision(neutronbank, uo2_compound, rng);
        
        // Count active neutrons in the bank
        total_neutrons += activeNeutronCount(neutronbank);
    }
    
    // Compute average number of neutrons after one collision
    double average_neutrons = total_neutrons / NUM_RUNS;
    
    std::cout << "Pure UO2 - Average number of neutrons after one collision: " << average_neutrons << std::endl;
    
    // Now test 50/50 molar mixture of water and uranium
    std::cout << "\nCreating 50/50 molar mixture of H2O and UO2..." << std::endl;
    
    // Load hydrogen data for water
    NuclearData h1_data = readNuclearDataFile("../data/H1.dat");
    
    // Create H2O substance (2 H1 + 1 O16)
    Substance h2o_substance;
    h2o_substance.species = {h1_data, o16_data};
    h2o_substance.stoichiometric_coeffs = {2, 1};
    h2o_substance.atom_number_densities.resize(2);
    
    // Create mixed compound: 50% H2O, 50% UO2
    Compound mixed_compound;
    mixed_compound.substances = {h2o_substance, uo2_substance};
    mixed_compound.molar_fractions = {0.5, 0.5};
    mixed_compound.density = 6.72; // Approximate density for 50/50 H2O/UO2 mixture
    
    // Calculate atom number densities for mixed compound
    calculateAtomNumberDensities(mixed_compound);
    
    // Reset for mixed compound simulation
    total_neutrons = 0.0;
    
    std::cout << "Running " << NUM_RUNS << " simulations for mixed compound..." << std::endl;
    
    for (int run = 0; run < NUM_RUNS; ++run) {
        // Create fresh neutronbank for each run with a copy of the original neutron
        std::vector<Neutron> neutronbank = {neutron};
        
        // Process one collision
        processNeutronCollision(neutronbank, mixed_compound, rng);
        
        // Count active neutrons in the bank
        total_neutrons += activeNeutronCount(neutronbank);
    }
    
    // Compute average number of neutrons after one collision for mixed compound
    double mixed_average_neutrons = total_neutrons / NUM_RUNS;
    
    std::cout << "Mixed H2O/UO2 - Average number of neutrons after one collision: " << mixed_average_neutrons << std::endl;
    
    return 0;
}
