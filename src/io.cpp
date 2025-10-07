#include "io.h"
#include <fstream>
#include <iostream>
#include <chrono>
#include <sstream>
#include <unordered_map>
#include <cstdio> // For std::remove
#include <map>
#include <cctype>

// Split a string by tabs
static std::vector<std::string> splitTabs(const std::string& line) {
    std::vector<std::string> parts;
    std::string token;
    for (char c : line) {
        if (c == '\t') { parts.push_back(token); token.clear(); }
        else { token.push_back(c); }
    }
    parts.push_back(token);
    return parts;
}

// Clear file if it exists
void clearFileIfExists(const std::string& filename) {
    std::ifstream file(filename);
    if (file.good()) {
        file.close();
        std::remove(filename.c_str());
    }
}

// Read key-value style input
GenericInput readKeyValueInput(const std::string& filename, std::size_t valuesRowIndex) {
    std::ifstream in(filename);
    if (!in) return GenericInput{};
    std::string header;
    if (!std::getline(in, header)) return GenericInput{};
    // Skip valuesRowIndex lines (values rows before the desired one)
    std::string line;
    for (std::size_t i = 0; i < valuesRowIndex; ++i) {
        if (!std::getline(in, line)) return GenericInput{}; // not enough rows
    }
    // Read the target values row
    std::string valuesLine;
    if (!std::getline(in, valuesLine)) return GenericInput{};
    GenericInput gi;
    gi.names = splitTabs(header);
    gi.values = splitTabs(valuesLine);
    if (gi.values.size() > gi.names.size()) gi.values.resize(gi.names.size());
    if (gi.values.size() < gi.names.size()) gi.names.resize(gi.values.size());
    return gi;
}

// Count the number of data rows (excluding header) in a key-value input file
std::size_t countDataRows(const std::string& filename) {
    std::ifstream in(filename);
    if (!in) return 0;
    
    std::string line;
    std::size_t count = 0;
    
    // Skip header line
    if (!std::getline(in, line)) return 0;
    
    // Count remaining lines (data rows)
    while (std::getline(in, line)) {
        if (!line.empty()) { // Skip empty lines
            ++count;
        }
    }
    
    return count;
}

// Write a table to a file
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

// Read nuclear data file and populate NuclearData structure
NuclearData readNuclearDataFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    NuclearData data;
    std::string line;

    // First line: SYM Z A AW T
    if (!std::getline(file, line)) {
        throw std::runtime_error("File is empty: " + filename);
    }
    {
        std::istringstream iss(line);
        iss >> data.symbol >> data.pnumber >> data.mnumber >> data.aweight >> data.temp;
    }

    // Second line: NNU (number of E,NU pairs below)
    if (!std::getline(file, line)) {
        throw std::runtime_error("Missing NNU line in file: " + filename);
    }
    int nnu = 0;
    {
        std::istringstream nnu_iss(line);
        nnu_iss >> nnu;
    }

    // Optional NU function under MT=0
    // NOTE: Input energies are in MeV in the data files; convert to eV for internal consistency
    const double MEV_TO_EV = 1.0e6;
    if (nnu < 0) nnu = 0;
    if (nnu > 0) {
        MTData mt0;
        mt0.qval = 0.0;
        mt0.num_ec_pairs = nnu;
        mt0.efunc.reserve(nnu);
        for (int i = 0; i < nnu; ++i) {
            if (!std::getline(file, line)) {
                throw std::runtime_error("Unexpected EOF while reading NU pairs: " + filename);
            }
            std::istringstream ec_iss(line);
            EnergyCrossSectionPair p{};
            ec_iss >> p.energy >> p.cross_section;
            p.energy *= MEV_TO_EV; // convert MeV -> eV
            mt0.efunc.push_back(p);
        }
        data.mt_data[0] = std::move(mt0);
    }

    // Remaining blocks: repeated (MT Q NE) followed by NE lines (E XS)
    while (true) {
        // Seek next non-empty line (header)
        do {
            if (!std::getline(file, line)) {
                file.close();
                return data; // EOF reached cleanly
            }
            bool only_ws = true;
            for (char c : line) {
                if (!std::isspace(static_cast<unsigned char>(c))) { only_ws = false; break; }
            }
            if (!only_ws) break;
        } while (true);

        std::istringstream hdr(line);
        int mt_number = 0;
        double qval = 0.0;
        int ne = 0;
        if (!(hdr >> mt_number >> qval >> ne)) {
            break; // malformed header; stop parsing
        }

        MTData mt_data;
        mt_data.qval = qval;
        mt_data.num_ec_pairs = ne;
        if (ne < 0) ne = 0;
        mt_data.efunc.reserve(ne);

        for (int i = 0; i < ne; ++i) {
            if (!std::getline(file, line)) {
                throw std::runtime_error("Unexpected EOF while reading MT block: " + filename);
            }
            std::istringstream ec_iss(line);
            EnergyCrossSectionPair p{};
            ec_iss >> p.energy >> p.cross_section;
            p.energy *= MEV_TO_EV; // convert MeV -> eV
            mt_data.efunc.push_back(p);
        }

        data.mt_data[mt_number] = std::move(mt_data);
    }
    
    file.close();
    return data;
}


