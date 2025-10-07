#include "random_sampling.h"
#include "helpers.h"
#include "physics.h"
#include <stdexcept>
#include <cmath>

// Constructor
Random::Random(std::uint64_t seed) : engine_(seed) {}

// Reseed the generator
void Random::reseed(std::uint64_t seed) { engine_.seed(seed); }

// Uniform [0,1)
double Random::uniform01() {
    static thread_local std::uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(engine_);
}

// Uniform [min,max)
double Random::uniformxy(double min, double max) {
    static thread_local std::uniform_real_distribution<double> dist;
    return std::uniform_real_distribution<double>(min, max)(engine_);
}

// Sample a random interval from a collection, weighted by interval size
int intervalSampling(Random& rng, const std::vector<double>& interval_sizes) {
    if (interval_sizes.empty()) {
        throw std::runtime_error("Cannot select from empty interval collection");
    }
    
    // Calculate total weight (sum of all interval sizes)
    double total_weight = 0.0;
    for (double size : interval_sizes) {
        if (size < 0.0) {
            throw std::runtime_error("Interval size must be non-negative");
        }
        total_weight += size;
    }
    
    if (total_weight <= 0.0) {
        throw std::runtime_error("Total interval weight must be positive");
    }
    
    // Generate random number in [0, total_weight)
    double random_value = rng.uniformxy(0.0, total_weight);
    
    // Find the interval containing this random value
    double cumulative_weight = 0.0;
    for (size_t i = 0; i < interval_sizes.size(); ++i) {
        cumulative_weight += interval_sizes[i];
        if (random_value < cumulative_weight) {
            return static_cast<int>(i);
        }
    }
    
    // This should never happen due to floating point precision, but just in case
    return static_cast<int>(interval_sizes.size() - 1);
}

// Sample a target species from a compound at a specific energy
NuclearData targetSampling(Random& rng, const Compound& compound, double energy) {
    // Create a mutable copy of the compound to calculate atom number densities
    Compound compound_copy = compound;
    
    // Calculate atom number densities for all species
    calculateAtomNumberDensities(compound_copy);
    
    // Collect all species and their macroscopic cross sections at the given energy
    std::vector<NuclearData> all_species;
    std::vector<double> macroscopic_cross_sections;
    
    // Iterate through each substance in the compound
    for (size_t i = 0; i < compound_copy.substances.size(); ++i) {
        const Substance& substance = compound_copy.substances[i];
        
        // Iterate through each species in the substance
        for (size_t j = 0; j < substance.species.size(); ++j) {
            const NuclearData& species_data = substance.species[j];
            double atom_number_density = substance.atom_number_densities[j];
            
            // Interpolate nuclear data at the specific energy
            NuclearData species_at_energy = NuclearDataAtEnergy(species_data, energy);
            
            // Calculate macroscopic cross section for this species at this energy
            MTData species_macroscopic = calculateSpeciesMacroscopicCrossSection(species_at_energy, atom_number_density);
            
            // Get the total macroscopic cross section (should be single-point MTData)
            if (species_macroscopic.efunc.empty()) {
                continue; // Skip if no data available
            }
            
            double species_macroscopic_cross_section = species_macroscopic.efunc[0].cross_section;
            
            // Store the species and its macroscopic cross section
            all_species.push_back(species_at_energy);
            macroscopic_cross_sections.push_back(species_macroscopic_cross_section);
        }
    }
    
    // Check if we found any species
    if (all_species.empty()) {
        throw std::runtime_error("No species found in compound for target sampling");
    }
    
    // Use interval sampling to select a target species based on macroscopic cross sections
    int selected_index = intervalSampling(rng, macroscopic_cross_sections);
    
    // Return the nuclear data of the selected target species
    return all_species[selected_index];
}

// Sample a nuclear reaction (MT channel) for a species
int reactionSampling(Random& rng, const NuclearData& nuclear_data) {
    // Validate that nuclear data is boiled down to a single energy point
    for (const auto& mt_pair : nuclear_data.mt_data) {
        const MTData& mt_data = mt_pair.second;
        if (mt_data.efunc.size() != 1) {
            throw std::runtime_error("Nuclear data must be boiled down to a single energy point. MT" + 
                                   std::to_string(mt_pair.first) + " has " + 
                                   std::to_string(mt_data.efunc.size()) + " energy points");
        }
    }
    
    // Collect cross section values for all MT channels (excluding MT0 and MT1)
    std::vector<double> cross_sections;
    std::vector<int> mt_numbers;
    
    // Iterate through all MT data
    for (const auto& mt_pair : nuclear_data.mt_data) {
        int mt_number = mt_pair.first;
        
        // Skip MT0 (nu-bar data) and MT1 (total cross section)
        if (mt_number == 0 || mt_number == 1) {
            continue;
        }
        
        const MTData& mt_data = mt_pair.second;
        
        // Get the cross section value (should be single-point MTData)
        if (mt_data.efunc.empty()) {
            continue; // Skip if no data available
        }
        
        double cross_section = mt_data.efunc[0].cross_section;
        
        // Store the cross section and MT number
        cross_sections.push_back(cross_section);
        mt_numbers.push_back(mt_number);
    }
    
    // Check if we found any reaction channels
    if (cross_sections.empty()) {
        throw std::runtime_error("No reaction channels found for species " + nuclear_data.symbol);
    }
    
    // Use interval sampling to select a reaction based on cross section weights
    int selected_index = intervalSampling(rng, cross_sections);
    
    // Return the MT number of the selected reaction
    return mt_numbers[selected_index];
}

// Sample energy from a Maxwellian distribution
double maxwellianSampling(Random& rng, double T) {
    // Input validation
    if (T <= 0.0) {
        throw std::runtime_error("Nuclear temperature T must be positive");
    }
    
    double xi1, xi2, R;
    
    do {
        // Sample two random numbers on the unit interval
        xi1 = rng.uniform01();
        xi2 = rng.uniform01();
        
        // Calculate R = xi1^2 + xi2^2
        R = xi1 * xi1 + xi2 * xi2;
        
    } while (R > 1.0);  // Continue until we're inside the unit circle
    
    // Sample two more random numbers on the unit interval
    double xi3 = rng.uniform01();
    double xi4 = rng.uniform01();
    
    // Calculate final energy using the Maxwellian sampling formula
    double E = -T * (xi1 * xi1 * std::log(xi3) / R + std::log(xi4));
    
    return E;  // Energy sampled from Maxwellian distribution
}

// Sample a random direction vector uniformly distributed on the unit sphere
std::vector<double> sampleRandomDirection(Random& rng) {
    // Generate two random numbers
    double xi1 = rng.uniform01();
    double xi2 = rng.uniform01();
    
    // Generate random angles
    double theta = 2.0 * M_PI * xi1;        // Azimuthal angle [0, 2π)
    double phi = std::acos(1.0 - 2.0 * xi2); // Polar angle [0, π]
    
    // Convert spherical to Cartesian coordinates
    double x = std::sin(phi) * std::cos(theta);
    double y = std::sin(phi) * std::sin(theta);
    double z = std::cos(phi);
    
    // Return direction vector
    return {x, y, z};
}

// Sample a new neutron with random direction and Maxwellian energy distribution
Neutron sampleNewNeutron(Random& rng, double T) {
    // Input validation
    if (T <= 0.0) {
        throw std::runtime_error("Nuclear temperature T must be positive");
    }
    
    // Sample random direction
    std::vector<double> direction = sampleRandomDirection(rng);
    
    // Sample energy from Maxwellian distribution
    double energy = maxwellianSampling(rng, T);
    
    // Create and return neutron with captured = false
    return Neutron(energy, direction[0], direction[1], direction[2], false);
}
