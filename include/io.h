#pragma once
#include <cstdint>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>

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

QuarterCircleInput readQuarterCircleInput(const std::string& filename);
BuffonInput readBuffonInput(const std::string& filename);

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
