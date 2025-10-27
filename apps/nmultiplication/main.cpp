#include <iostream>
#include <vector>
#include <map>
#include "../../include/io.h"
#include "../../include/physics.h"
#include "../../include/random_sampling.h"
#include "../../include/helpers.h"
#include "../../include/statistics.h"

int main() {
    std::cout << "Neutron Multiplication Application" << std::endl;
    
    // Load nuclear data
    NuclearData u235_data = readNuclearDataFile("../data/U235.dat");
    NuclearData u238_data = readNuclearDataFile("../data/U238.dat");
    NuclearData o16_data = readNuclearDataFile("../data/O16.dat");
    NuclearData h1_data = readNuclearDataFile("../data/H1.dat");
    
    // Create substances
    // Pure U238O2 substance
    Substance uo2_substance;
    uo2_substance.species = {u238_data, o16_data};
    uo2_substance.stoichiometric_coeffs = {1, 2};
    uo2_substance.atom_number_densities.resize(2);
    
    // U235O2 substance
    Substance u235o2_substance;
    u235o2_substance.species = {u235_data, o16_data};
    u235o2_substance.stoichiometric_coeffs = {1, 2};
    u235o2_substance.atom_number_densities.resize(2);
    
    // U238O2 substance  
    Substance u238o2_substance;
    u238o2_substance.species = {u238_data, o16_data};
    u238o2_substance.stoichiometric_coeffs = {1, 2};
    u238o2_substance.atom_number_densities.resize(2);
    
    // H2O substance
    Substance h2o_substance;
    h2o_substance.species = {h1_data, o16_data};
    h2o_substance.stoichiometric_coeffs = {2, 1};
    h2o_substance.atom_number_densities.resize(2);
    
    // Create compounds
    // Case 1: Pure U238 UO2
    Compound uo2_compound;
    uo2_compound.substances = {uo2_substance};
    uo2_compound.molar_fractions = {1.0};
    uo2_compound.density = 10.97; // g/cm³ (typical UO2 density)
    
    // Case 2: Natural uranium UO2 (0.72% U235, 99.28% U238)
    Compound natural_uo2_compound;
    natural_uo2_compound.substances = {u235o2_substance, u238o2_substance};
    natural_uo2_compound.molar_fractions = {0.0072, 0.9928}; // Natural abundance
    natural_uo2_compound.density = 10.97; // g/cm³ (typical UO2 density)
    
    // Case 3: 50/50 molar mixture of H2O and natural uranium UO2
    Compound mixed_compound;
    mixed_compound.substances = {h2o_substance, u235o2_substance, u238o2_substance};
    mixed_compound.molar_fractions = {0.5, 0.0036, 0.4964}; // 50% H2O, 0.5*0.72% U235O2, 0.5*99.28% U238O2
    mixed_compound.density = 6.72; // Approximate density for 50/50 H2O/UO2 mixture
    
    // Calculate atom number densities for all compounds
    calculateAtomNumberDensities(uo2_compound);
    calculateAtomNumberDensities(natural_uo2_compound);
    calculateAtomNumberDensities(mixed_compound);
    
    // Create random number generator
    Random rng;
    
    // Create a 1 MeV neutron with random direction
    std::vector<double> direction = sampleRandomDirection(rng);
    Neutron neutron(1e6, direction[0], direction[1], direction[2]); // 1 MeV = 1e6 eV
    
    // Simulation parameters
    const int NUM_RUNS = 10000;
    
    // Case 1: Pure U238 UO2
    std::cout << "Running " << NUM_RUNS << " simulations for pure U238 UO2..." << std::endl;
    
    // Enable logging and clear any previous events
    ReactionLogger::getInstance().enable();
    ReactionLogger::getInstance().clear();
    
    double total_neutrons = 0.0;
    
    for (int run = 0; run < NUM_RUNS; ++run) {
        std::vector<Neutron> neutronbank = {neutron};
        processNeutronCollisionLog(neutronbank, uo2_compound, rng);
        total_neutrons += activeNeutronCount(neutronbank);
    }
    
    double pure_u238_average = total_neutrons / NUM_RUNS;
    std::cout << "Pure U238 UO2 - Average number of neutrons after one collision: " << pure_u238_average << std::endl;
    
    // Print and save reaction statistics
    Statistics::saveReactionStatistics("../apps/nmultiplication/pure_u238_reaction_stats.stat", "Pure U238 UO2 Target-Reaction Statistics", NUM_RUNS);
    
    // Case 2: Natural uranium UO2
    std::cout << "\nRunning " << NUM_RUNS << " simulations for natural uranium UO2..." << std::endl;
    
    // Clear previous events for this case
    ReactionLogger::getInstance().clear();
    
    total_neutrons = 0.0;
    
    for (int run = 0; run < NUM_RUNS; ++run) {
        std::vector<Neutron> neutronbank = {neutron};
        processNeutronCollisionLog(neutronbank, natural_uo2_compound, rng);
        total_neutrons += activeNeutronCount(neutronbank);
    }
    
    double natural_average = total_neutrons / NUM_RUNS;
    std::cout << "Natural uranium UO2 - Average number of neutrons after one collision: " << natural_average << std::endl;
    
    // Print and save reaction statistics
    Statistics::saveReactionStatistics("../apps/nmultiplication/natural_uranium_reaction_stats.stat", "Natural Uranium UO2 Target-Reaction Statistics", NUM_RUNS);
    
    // Case 3: Mixed H2O/natural uranium UO2
    std::cout << "\nRunning " << NUM_RUNS << " simulations for mixed H2O/natural uranium UO2..." << std::endl;
    
    // Clear previous events for this case
    ReactionLogger::getInstance().clear();
    
    total_neutrons = 0.0;
    
    for (int run = 0; run < NUM_RUNS; ++run) {
        std::vector<Neutron> neutronbank = {neutron};
        processNeutronCollisionLog(neutronbank, mixed_compound, rng);
        total_neutrons += activeNeutronCount(neutronbank);
    }
    
    double mixed_average = total_neutrons / NUM_RUNS;
    std::cout << "Mixed H2O/natural uranium UO2 - Average number of neutrons after one collision: " << mixed_average << std::endl;
    
    // Print and save reaction statistics
    Statistics::saveReactionStatistics("../apps/nmultiplication/mixed_h2o_uranium_reaction_stats.stat", "Mixed H2O/Natural Uranium UO2 Target-Reaction Statistics", NUM_RUNS);
    
    return 0;
}
