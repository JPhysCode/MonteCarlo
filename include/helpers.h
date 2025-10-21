#ifndef HELPERS_H
#define HELPERS_H

#include "io.h"
#include "physics.h"
#include <vector>
#include <string>
#include <map>

// Helper functions for nuclear transport calculations

// Structure to store a single collision event
struct CollisionEvent {
    std::string target_symbol;    // Target nucleus symbol (e.g., "U235", "H1")
    int reaction_mt;              // Reaction MT number (e.g., 2, 18, 102)
    double neutron_energy;        // Neutron energy at collision (eV)
    double timestamp;            // Optional: simulation time or step number
    
    CollisionEvent(const std::string& symbol, int mt, double energy, double time = 0.0)
        : target_symbol(symbol), reaction_mt(mt), neutron_energy(energy), timestamp(time) {}
};

// Global reaction logger class - singleton pattern
class ReactionLogger {
private:
    static ReactionLogger* instance_;
    std::vector<CollisionEvent> events_;
    bool enabled_;
    
    ReactionLogger() : enabled_(false) {}
    
public:
    // Get singleton instance
    static ReactionLogger& getInstance();
    
    // Enable/disable logging
    void enable() { enabled_ = true; }
    void disable() { enabled_ = false; }
    bool isEnabled() const { return enabled_; }
    
    // Log a collision event
    void logCollision(const std::string& target_symbol, int reaction_mt, double neutron_energy, double timestamp = 0.0);
    
    // Get all logged events
    const std::vector<CollisionEvent>& getEvents() const { return events_; }
    
    // Clear all logged events
    void clear() { events_.clear(); }
    
    // Get count of events
    size_t getEventCount() const { return events_.size(); }
    
    // Get statistics about logged reactions
    std::map<std::string, std::map<int, int>> getReactionStatistics() const;
    
    // Get events for a specific target
    std::vector<CollisionEvent> getEventsForTarget(const std::string& target_symbol) const;
    
    // Get events for a specific reaction type
    std::vector<CollisionEvent> getEventsForReaction(int reaction_mt) const;
    
    // Get events within energy range
    std::vector<CollisionEvent> getEventsInEnergyRange(double min_energy, double max_energy) const;
};

// Helper functions for nuclear transport calculations

// Linear interpolation function
// Interpolates between two EnergyCrossSectionPair points
// Returns interpolated cross section value at given energy
double linearInterpolate(const EnergyCrossSectionPair& point1, 
                        const EnergyCrossSectionPair& point2, 
                        double energy);

// Find interpolated cross section value for given energy from MTData
// Automatically finds the appropriate two points and interpolates
double interpolatedValue(const MTData& mt_data, double energy);

// Interpolate cross-section values for an array of energies
// Takes a vector of energies and returns a vector of corresponding interpolated cross-sections
std::vector<double> interpolatedValueS(const MTData& mt_data, const std::vector<double>& energies);

// Generate n points equidistributed on a log scale between min_val and max_val
// Returns a vector of n values where each value is log-distributed
std::vector<double> logSpace(double min_val, double max_val, int n);

// Interpolate all MT values at a specific energy for given nuclear data
// Returns NuclearData with single-point MTData containing the interpolated values at the given energy
NuclearData NuclearDataAtEnergy(const NuclearData& nuclear_data, double energy);

// Convert temperature from Kelvin to energy in eV
// Uses the relationship: E = k_B * T where k_B = 8.617333262e-5 eV/K
double temperatureToEnergy(double temperature_K);

// Stationary neutron slowing down calculation
// Takes initial energy, nuclear data, and number of steps, returns array with energy at each step
// Uses the equations: ΔE/E₀ = (1-α)/2 where α = ((1-A)/(1+A))² and A = M/m
std::vector<double> stationarySlowingDown(double initial_energy, const NuclearData& nuclear_data, int num_steps = 1);

// Count the number of active (non-captured) neutrons in a neutron bank
// Takes a vector of neutrons and returns the count of neutrons where captured == false
int activeNeutronCount(const std::vector<Neutron>& neutronbank);

#endif // HELPERS_H
