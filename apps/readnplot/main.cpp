#include <iostream>
#include <fstream>
#include <string>
#include "../../include/io.h"
#include "../../include/physics.h"
#include "../../include/helpers.h"

int main() {
    try {
        // Create log-spaced energy grid
        std::vector<double> log_energies = logSpace(1e-11, 20.0, 500);
        
        // Process O16 data
        NuclearData o16_data = readNuclearDataFile("../data/O16.dat");
        calculateTotalCrossSection(o16_data);
        const MTData& o16_mt1 = o16_data.mt_data.at(1);
        std::vector<double> o16_cross_sections = interpolatedValueS(o16_mt1, log_energies);
        
        // Process H1 data
        NuclearData h1_data = readNuclearDataFile("../data/H1.dat");
        calculateTotalCrossSection(h1_data);
        const MTData& h1_mt1 = h1_data.mt_data.at(1);
        std::vector<double> h1_cross_sections = interpolatedValueS(h1_mt1, log_energies);
        
        // Output O16 interpolated data
        std::string o16_output = "../plot/O16_MT1.dat";
        clearFileIfExists(o16_output);
        for (size_t i = 0; i < log_energies.size(); ++i) {
            writeTable(o16_output, "energy", log_energies[i], "cross_section", o16_cross_sections[i]);
        }
        
        // Output H1 interpolated data
        std::string h1_output = "../plot/H1_MT1.dat";
        clearFileIfExists(h1_output);
        for (size_t i = 0; i < log_energies.size(); ++i) {
            writeTable(h1_output, "energy", log_energies[i], "cross_section", h1_cross_sections[i]);
        }
        
        // Process U235 data
        NuclearData u235_data = readNuclearDataFile("../data/U235.dat");
        
        // Get fission and capture cross-sections for U235
        MTData u235_fission = getTotalFissionCrossSection(u235_data);
        MTData u235_capture = getRadiativeCaptureCrossSection(u235_data);
        
        // Sum fission and capture for U235
        std::vector<MTData> u235_combined = {u235_fission, u235_capture};
        MTData u235_total = sumMTData(u235_combined);
        
        // Interpolate U235 combined cross-sections
        std::vector<double> u235_cross_sections = interpolatedValueS(u235_total, log_energies);
        
        // Process U238 data
        NuclearData u238_data = readNuclearDataFile("../data/U238.dat");
        
        // Get fission and capture cross-sections for U238
        MTData u238_fission = getTotalFissionCrossSection(u238_data);
        MTData u238_capture = getRadiativeCaptureCrossSection(u238_data);
        
        // Sum fission and capture for U238
        std::vector<MTData> u238_combined = {u238_fission, u238_capture};
        MTData u238_total = sumMTData(u238_combined);
        
        // Interpolate U238 combined cross-sections
        std::vector<double> u238_cross_sections = interpolatedValueS(u238_total, log_energies);
        
        // Output U235 combined data
        std::string u235_output = "../plot/U235_fission_capture.dat";
        clearFileIfExists(u235_output);
        for (size_t i = 0; i < log_energies.size(); ++i) {
            writeTable(u235_output, "energy", log_energies[i], "cross_section", u235_cross_sections[i]);
        }
        
        // Output U238 combined data
        std::string u238_output = "../plot/U238_fission_capture.dat";
        clearFileIfExists(u238_output);
        for (size_t i = 0; i < log_energies.size(); ++i) {
            writeTable(u238_output, "energy", log_energies[i], "cross_section", u238_cross_sections[i]);
        }
        
        // Get total inelastic cross-section for U238
        MTData u238_inelastic = getTotalInelasticCrossSection(u238_data);
        
        // Interpolate U238 inelastic cross-sections
        std::vector<double> u238_inelastic_cross_sections = interpolatedValueS(u238_inelastic, log_energies);
        
        // Output U238 inelastic data
        std::string u238_inelastic_output = "../plot/U238_inelastic.dat";
        clearFileIfExists(u238_inelastic_output);
        for (size_t i = 0; i < log_energies.size(); ++i) {
            writeTable(u238_inelastic_output, "energy", log_energies[i], "cross_section", u238_inelastic_cross_sections[i]);
        }
        
        std::cout << "Generated " << log_energies.size() << " interpolated points for O16, H1, U235, and U238" << std::endl;
        std::cout << "O16 data: " << o16_output << std::endl;
        std::cout << "H1 data: " << h1_output << std::endl;
        std::cout << "U235 fission+capture data: " << u235_output << std::endl;
        std::cout << "U238 fission+capture data: " << u238_output << std::endl;
        std::cout << "U238 inelastic data: " << u238_inelastic_output << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
