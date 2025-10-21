#pragma once
#include <vector>
#include <string>
#include <map>

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
