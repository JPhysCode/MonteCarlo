#pragma once
#include <cstdint>
#include <string>
#include <sstream>
#include <iostream>
#include <fstream>
#include <chrono>
#include <vector>
#include <stdexcept>

// Generic key-value input
struct GenericInput {
    std::vector<std::string> names;
    std::vector<std::string> values; // raw strings; convert in mains as needed
    // Helper accessors
    std::string getString(const std::string& key) const {
        for (std::size_t i = 0; i < names.size(); ++i) if (names[i] == key) return values[i];
        throw std::runtime_error("Key not found: " + key);
    }
    std::uint64_t getUInt64(const std::string& key) const {
        std::stringstream ss(getString(key)); std::uint64_t v{}; ss >> v; return v;
    }
    double getDouble(const std::string& key) const {
        std::stringstream ss(getString(key)); double v{}; ss >> v; return v;
    }
};

// Read key-value style input: first line = names (tab-separated),
// subsequent lines = values rows. Select which values row to read
// with valuesRowIndex (0 = first values row after header).
GenericInput readKeyValueInput(const std::string& filename, std::size_t valuesRowIndex = 0);

// Count the number of data rows (excluding header) in a key-value input file
std::size_t countDataRows(const std::string& filename);

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

// Clear file if it exists
void clearFileIfExists(const std::string& filename);

// Core table writer: two-row table (header names, then values), tab-separated
void writeTable(const std::string& filename,
                const std::vector<std::string>& names,
                const std::vector<std::string>& values);

// Convenience: variadic name/value pairs. Usage: writeTable("out.txt", "a", 1, "b", 2.5)
// Accepts arbitrary streamable value types; converts values to strings and forwards to core writer
namespace detail {
    inline void collectPairs(std::vector<std::string>& /*names*/,
                             std::vector<std::string>& /*values*/) {}

    template<typename T, typename... Rest>
    void collectPairs(std::vector<std::string>& names,
                      std::vector<std::string>& values,
                      const std::string& name,
                      const T& value,
                      Rest... rest) {
        std::stringstream ss; ss << value;
        names.push_back(name);
        values.push_back(ss.str());
        detail::collectPairs(names, values, rest...);
    }
}

template<typename... Rest>
void writeTable(const std::string& filename, Rest... rest) {
    static_assert((sizeof...(rest) % 2) == 0, "writeTable expects name/value pairs");
    std::vector<std::string> names;
    std::vector<std::string> values;
    names.reserve(sizeof...(rest) / 2);
    values.reserve(sizeof...(rest) / 2);
    detail::collectPairs(names, values, rest...);
    writeTable(filename, names, values);
}
