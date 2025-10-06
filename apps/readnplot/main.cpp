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
        
        // Create water compound (H2O) at 4°C density (0.999975 g/cm³)
        Substance water;
        water.species.push_back(h1_data);        // Hydrogen (H)
        water.stoichiometric_coeffs.push_back(2); // 2 hydrogen atoms
        water.species.push_back(o16_data);       // Oxygen (O)
        water.stoichiometric_coeffs.push_back(1); // 1 oxygen atom
        
        Compound water_compound(0.999975); // Density at 4°C
        water_compound.substances.push_back(water);
        water_compound.molar_fractions.push_back(1.0); // 100% water
        
        // Calculate macroscopic cross section for water
        MTData water_macroscopic = calculateTotalMacroscopicCrossSection(water_compound);
        
        // Interpolate water macroscopic cross sections
        std::vector<double> water_macroscopic_cross_sections = interpolatedValueS(water_macroscopic, log_energies);
        
        // Output water macroscopic data
        std::string water_output = "../plot/water_macroscopic.dat";
        clearFileIfExists(water_output);
        for (size_t i = 0; i < log_energies.size(); ++i) {
            writeTable(water_output, "energy", log_energies[i], "macroscopic_cross_section", water_macroscopic_cross_sections[i]);
        }
        
        // Create uranium compound (0.72% U235, 99.28% U238)
        Substance u235_substance;
        u235_substance.species.push_back(u235_data);     // Uranium-235
        u235_substance.stoichiometric_coeffs.push_back(1); // 1 atom
        
        Substance u238_substance;
        u238_substance.species.push_back(u238_data);     // Uranium-238
        u238_substance.stoichiometric_coeffs.push_back(1); // 1 atom
        
        // Natural uranium density: 19.1 g/cm³
        Compound uranium_compound(19.1);
        uranium_compound.substances.push_back(u235_substance);
        uranium_compound.molar_fractions.push_back(0.0072); // 0.72% U235
        uranium_compound.substances.push_back(u238_substance);
        uranium_compound.molar_fractions.push_back(0.9928); // 99.28% U238
        
        // Calculate macroscopic cross section for uranium compound
        MTData uranium_macroscopic = calculateTotalMacroscopicCrossSection(uranium_compound);
        
        // Interpolate uranium macroscopic cross sections
        std::vector<double> uranium_macroscopic_cross_sections = interpolatedValueS(uranium_macroscopic, log_energies);
        
        // Output uranium macroscopic data
        std::string uranium_output = "../plot/uranium_macroscopic.dat";
        clearFileIfExists(uranium_output);
        for (size_t i = 0; i < log_energies.size(); ++i) {
            writeTable(uranium_output, "energy", log_energies[i], "macroscopic_cross_section", uranium_macroscopic_cross_sections[i]);
        }
        
        std::cout << "Generated " << log_energies.size() << " interpolated points for O16, H1, U235, U238, water, and uranium compound" << std::endl;
        std::cout << "O16 data: " << o16_output << std::endl;
        std::cout << "H1 data: " << h1_output << std::endl;
        std::cout << "U235 fission+capture data: " << u235_output << std::endl;
        std::cout << "U238 fission+capture data: " << u238_output << std::endl;
        std::cout << "U238 inelastic data: " << u238_inelastic_output << std::endl;
        std::cout << "Water macroscopic cross section data: " << water_output << std::endl;
        std::cout << "Uranium macroscopic cross section data: " << uranium_output << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
