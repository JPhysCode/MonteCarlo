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
