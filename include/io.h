#pragma once
#include <cstdint>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <chrono>

// Specific readers for different app types
struct QuarterCircleInput {
    std::uint64_t samples;
    std::uint64_t seed;
};

struct BuffonInput {
    std::uint64_t samples;
    std::uint64_t seed;
    double needleLength;
    double lineSpacing;
};

struct StatQuarterCircleInput {
    std::uint64_t samples;
    std::uint64_t seed;
    std::uint64_t numRuns;
};

struct StatBuffonInput {
    std::uint64_t samples;
    std::uint64_t seed;
    double needleLength;
    double lineSpacing;
    std::uint64_t numRuns;
};

QuarterCircleInput readQuarterCircleInput(const std::string& filename);
BuffonInput readBuffonInput(const std::string& filename);
StatQuarterCircleInput readStatQuarterCircleInput(const std::string& filename);
StatBuffonInput readStatBuffonInput(const std::string& filename);

// Timing utility class
class Timer {
public:
    Timer();
    void start();
    void stop();
    double getElapsedSeconds() const;
    double getElapsedMilliseconds() const;
    void reset();
    
private:
    std::chrono::high_resolution_clock::time_point startTime_;
    std::chrono::high_resolution_clock::time_point endTime_;
    bool isRunning_;
};

// Variadic template output function
template<typename... Args>
void printAndSave(const std::string& filename, Args... args) {
    std::stringstream ss;
    (ss << ... << args);  // C++17 fold expression
    std::cout << ss.str() << std::endl;
    
    std::ofstream out(filename);
    if (out.is_open()) {
        out << ss.str() << std::endl;
    }
}
