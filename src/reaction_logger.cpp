#include "reaction_logger.h"
#include <algorithm>

// Initialize static member
ReactionLogger* ReactionLogger::instance_ = nullptr;

// Get singleton instance
ReactionLogger& ReactionLogger::getInstance() {
    if (instance_ == nullptr) {
        instance_ = new ReactionLogger();
    }
    return *instance_;
}

// Log a collision event
void ReactionLogger::logCollision(const std::string& target_symbol, int reaction_mt, double neutron_energy, double timestamp) {
    if (enabled_) {
        events_.emplace_back(target_symbol, reaction_mt, neutron_energy, timestamp);
    }
}

// Get statistics about logged reactions
std::map<std::string, std::map<int, int>> ReactionLogger::getReactionStatistics() const {
    std::map<std::string, std::map<int, int>> stats;
    
    for (const auto& event : events_) {
        stats[event.target_symbol][event.reaction_mt]++;
    }
    
    return stats;
}

// Get events for a specific target
std::vector<CollisionEvent> ReactionLogger::getEventsForTarget(const std::string& target_symbol) const {
    std::vector<CollisionEvent> result;
    
    for (const auto& event : events_) {
        if (event.target_symbol == target_symbol) {
            result.push_back(event);
        }
    }
    
    return result;
}

// Get events for a specific reaction type
std::vector<CollisionEvent> ReactionLogger::getEventsForReaction(int reaction_mt) const {
    std::vector<CollisionEvent> result;
    
    for (const auto& event : events_) {
        if (event.reaction_mt == reaction_mt) {
            result.push_back(event);
        }
    }
    
    return result;
}

// Get events within energy range
std::vector<CollisionEvent> ReactionLogger::getEventsInEnergyRange(double min_energy, double max_energy) const {
    std::vector<CollisionEvent> result;
    
    for (const auto& event : events_) {
        if (event.neutron_energy >= min_energy && event.neutron_energy <= max_energy) {
            result.push_back(event);
        }
    }
    
    return result;
}
