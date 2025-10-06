#include "scattering.h"
#include "helpers.h"
#include <cmath>
#include <stdexcept>

// Implementation of neutron scattering cross-sections and calculations

// Handle elastic scattering collision
void elasticScattering(Neutron& neutron, const NuclearData& species_data, Random& rng, double temperature_K) {
    // Constants
    const double neutron_mass = 939.56542052e6;  // Neutron mass in eV/c²
    const double E_fg_threshold = 200.0;  // Free-gas model threshold in eV
    const double room_temperature = 293.15;  // Room temperature in K
    
    // Convert temperature to energy
    double T_energy = temperatureToEnergy(temperature_K);
    
    // Get target mass from atomic weight (convert from amu to eV/c²)
    double target_mass = species_data.aweight * 931.49410242e6;  // amu to eV/c²
    double A = target_mass / neutron_mass;  // Mass ratio
    
    // Determine target motion
    double V_L_x = 0.0, V_L_y = 0.0, V_L_z = 0.0;  // Target velocity in lab frame
    
    if (neutron.energy < E_fg_threshold) {
        // Free-gas model: sample target energy from Maxwellian distribution
        double E_t = maxwellianSampling(rng, T_energy);
        
        // Calculate target speed and sample direction randomly
        double V_L_magnitude = std::sqrt(2.0 * E_t / target_mass);
        std::vector<double> target_direction = sampleRandomDirection(rng);
        
        V_L_x = V_L_magnitude * target_direction[0];
        V_L_y = V_L_magnitude * target_direction[1];
        V_L_z = V_L_magnitude * target_direction[2];
    }
    // If E_n >= E_fg_threshold, target is stationary (V_L = 0)
    
    // Calculate neutron speed in lab frame
    double v_L_magnitude = std::sqrt(2.0 * neutron.energy / neutron_mass);
    
    // Calculate center-of-mass velocity
    double V_CM_x = (v_L_magnitude * neutron.direction_x + A * V_L_x) / (1.0 + A);
    double V_CM_y = (v_L_magnitude * neutron.direction_y + A * V_L_y) / (1.0 + A);
    double V_CM_z = (v_L_magnitude * neutron.direction_z + A * V_L_z) / (1.0 + A);
    
    // Coordinate transformation from L- to C-frame
    double v_C_x = v_L_magnitude * neutron.direction_x - V_CM_x;
    double v_C_y = v_L_magnitude * neutron.direction_y - V_CM_y;
    double v_C_z = v_L_magnitude * neutron.direction_z - V_CM_z;
    
    // Sample direction isotropically in C-frame
    std::vector<double> Omega_C = sampleRandomDirection(rng);
    
    // Calculate scattered velocity in C-frame (magnitude unchanged in elastic scattering)
    double v_C_magnitude = std::sqrt(v_C_x * v_C_x + v_C_y * v_C_y + v_C_z * v_C_z);
    double v_prime_C_x = v_C_magnitude * Omega_C[0];
    double v_prime_C_y = v_C_magnitude * Omega_C[1];
    double v_prime_C_z = v_C_magnitude * Omega_C[2];
    
    // Coordinate transformation from C- to L-frame
    double v_prime_L_x = v_prime_C_x + V_CM_x;
    double v_prime_L_y = v_prime_C_y + V_CM_y;
    double v_prime_L_z = v_prime_C_z + V_CM_z;
    
    // Calculate new neutron energy and direction
    double v_prime_L_magnitude = std::sqrt(v_prime_L_x * v_prime_L_x + v_prime_L_y * v_prime_L_y + v_prime_L_z * v_prime_L_z);
    double new_energy = 0.5 * neutron_mass * v_prime_L_magnitude * v_prime_L_magnitude;
    
    // Update neutron
    neutron.energy = new_energy;
    neutron.direction_x = v_prime_L_x / v_prime_L_magnitude;
    neutron.direction_y = v_prime_L_y / v_prime_L_magnitude;
    neutron.direction_z = v_prime_L_z / v_prime_L_magnitude;
}
