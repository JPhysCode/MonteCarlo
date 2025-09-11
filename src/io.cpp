#include "io.h"
#include <fstream>
#include <iostream>
#include <chrono>

QuarterCircleInput readQuarterCircleInput(const std::string& filename) {
    QuarterCircleInput input = {1000000, 12345}; // defaults
    
    std::ifstream in(filename);
    if (in) {
        in >> input.samples >> input.seed;
    }
    return input;
}

BuffonInput readBuffonInput(const std::string& filename) {
    BuffonInput input = {1000000, 12345, 1.0, 2.0}; // defaults
    
    std::ifstream in(filename);
    if (in) {
        in >> input.samples >> input.seed >> input.needleLength >> input.lineSpacing;
    }
    return input;
}

StatQuarterCircleInput readStatQuarterCircleInput(const std::string& filename) {
    StatQuarterCircleInput input = {100000, 12345, 10}; // defaults
    
    std::ifstream in(filename);
    if (in) {
        in >> input.samples >> input.seed >> input.numRuns;
    }
    return input;
}

StatBuffonInput readStatBuffonInput(const std::string& filename) {
    StatBuffonInput input = {100000, 12345, 1.0, 2.0, 10}; // defaults
    
    std::ifstream in(filename);
    if (in) {
        in >> input.samples >> input.seed >> input.needleLength >> input.lineSpacing >> input.numRuns;
    }
    return input;
}


void writeTable(const std::string& filename,
                const std::vector<std::string>& names,
                const std::vector<std::string>& values) {
    if (names.size() != values.size()) {
        throw std::runtime_error("Number of names and values must match!");
    }

    // Check if file exists and has a header
    std::ifstream in(filename);
    if (in.good()) {
        std::string header;
        if (std::getline(in, header) && !header.empty()) {
            // Split existing header by tabs
            std::vector<std::string> existingNames;
            std::string token;
            for (std::size_t i = 0; i < header.size(); ++i) {
                char c = header[i];
                if (c == '\t') {
                    existingNames.push_back(token);
                    token.clear();
                } else {
                    token.push_back(c);
                }
            }
            existingNames.push_back(token);

            // Compare with provided names
            if (existingNames.size() != names.size()) {
                throw std::runtime_error("writeTable: header column count mismatch with existing file");
            }
            for (std::size_t i = 0; i < names.size(); ++i) {
                if (existingNames[i] != names[i]) {
                    throw std::runtime_error("writeTable: header names mismatch with existing file");
                }
            }

            // Append values row
            in.close();
            std::ofstream out(filename, std::ios::app);
            if (!out) throw std::runtime_error("Could not open file for appending");
            for (std::size_t i = 0; i < values.size(); ++i) {
                out << values[i];
                if (i + 1 < values.size()) out << "\t";
            }
            out << "\n";
            return;
        }
        // File exists but is empty -> fall through to write header + first row
    }

    // Write new file with header and first row
    std::ofstream out(filename);
    if (!out) throw std::runtime_error("Could not open file");

    for (std::size_t i = 0; i < names.size(); ++i) {
        out << names[i];
        if (i + 1 < names.size()) out << "\t";
    }
    out << "\n";

    for (std::size_t i = 0; i < values.size(); ++i) {
        out << values[i];
        if (i + 1 < values.size()) out << "\t";
    }
    out << "\n";
}




// Timer class implementation
Timer::Timer() : isRunning_(false) {}

void Timer::start() {
    startTime_ = std::chrono::high_resolution_clock::now();
    isRunning_ = true;
}

void Timer::stop() {
    endTime_ = std::chrono::high_resolution_clock::now();
    isRunning_ = false;
}

double Timer::getElapsedSeconds() const {
    auto end = isRunning_ ? std::chrono::high_resolution_clock::now() : endTime_;
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - startTime_);
    return duration.count() / 1000000.0;
}

double Timer::getElapsedMilliseconds() const {
    auto end = isRunning_ ? std::chrono::high_resolution_clock::now() : endTime_;
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - startTime_);
    return duration.count() / 1000.0;
}

void Timer::reset() {
    isRunning_ = false;
    startTime_ = std::chrono::high_resolution_clock::now();
    endTime_ = startTime_;
}
