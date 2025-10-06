#include "fission.h"
#include "capture.h"
#include "helpers.h"
#include <cmath>
#include <stdexcept>

// Implementation of fission cross-sections and calculations

// Generate new neutrons from fission reaction
std::vector<Neutron> fission(Neutron& neutron, const NuclearData& nuclear_data, Random& rng) {
    // Mark the incoming neutron as captured using the capture function
    neutronCapture(neutron);
    // Get MT0 data (nu_bar - average number of fission neutrons)
    auto mt0_it = nuclear_data.mt_data.find(0);
    if (mt0_it == nuclear_data.mt_data.end()) {
        throw std::runtime_error("MT0 (nu_bar) not found for species " + nuclear_data.symbol + " at energy " + std::to_string(neutron.energy) + " eV");
    }
    
    const MTData& mt0_data = mt0_it->second;
    if (mt0_data.efunc.empty()) {
        throw std::runtime_error("MT0 data is empty for species " + nuclear_data.symbol);
    }
    
    // Get nu_bar value interpolated at the neutron's energy
    double nu_bar = interpolatedValue(mt0_data, neutron.energy);
    
    // Calculate number of new fission neutrons
    int num_fission_neutrons = static_cast<int>(nu_bar);  // Truncate nu_bar
    
    // Add one more neutron with probability (nu_bar - truncated_value)
    double xi = rng.uniform01();
    if (xi < (nu_bar - num_fission_neutrons)) {
        num_fission_neutrons += 1;
    }
    
    // Generate new neutrons
    std::vector<Neutron> new_neutrons;
    new_neutrons.reserve(num_fission_neutrons);
    
    for (int i = 0; i < num_fission_neutrons; ++i) {
        // Sample new neutron with random direction and Maxwellian energy
        Neutron new_neutron = sampleNewNeutron(rng);
        new_neutrons.push_back(new_neutron);
    }
    
    return new_neutrons;
}
