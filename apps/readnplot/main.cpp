#include <iostream>
#include <fstream>
#include <string>
#include "../../include/io.h"
#include "../../include/physics.h"
#include "../../include/helpers.h"

int main() {
    try {
        // Read nuclear data and calculate total cross section
        NuclearData data = readNuclearDataFile("../data/O16.dat");
        calculateTotalCrossSection(data);
        
        const MTData& mt1 = data.mt_data.at(1);
        
        // Create log-spaced energy grid and interpolate cross-sections
        std::vector<double> log_energies = logSpace(1e-11, 20.0, 500);
        std::vector<double> interpolated_cross_sections = interpolatedValueS(mt1, log_energies);
        
        // Output interpolated data to file
        std::string output_file = "../plot/O16_MT1.dat";
        clearFileIfExists(output_file);
        
        for (size_t i = 0; i < log_energies.size(); ++i) {
            writeTable(output_file, "energy", log_energies[i], "cross_section", interpolated_cross_sections[i]);
        }
        
        std::cout << "Generated " << log_energies.size() << " interpolated points in " << output_file << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
