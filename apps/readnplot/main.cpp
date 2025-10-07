#include <iostream>
#include <fstream>
#include <string>
#include "../../include/io.h"
#include "../../include/physics.h"
#include "../../include/helpers.h"

int main() {
    try {
        // Create log-spaced energy grid (1e-11 MeV to 20 MeV -> 1e-5 eV to 2e7 eV)
        std::vector<double> log_energies = logSpace(1e-5, 2e7, 500);
        
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
        
        // Output combined H1 and O16 total cross-sections
        std::string h1_o16_output = "../plot/H1_O16_total.dat";
        clearFileIfExists(h1_o16_output);
        for (size_t i = 0; i < log_energies.size(); ++i) {
            writeTable(h1_o16_output, "energy", log_energies[i], 
                      "h1_total", h1_cross_sections[i],
                      "o16_total", o16_cross_sections[i]);
        }
        
        // Process U235 data
        NuclearData u235_data = readNuclearDataFile("../data/U235.dat");
        
        // Get fission and capture cross-sections for U235
        MTData u235_fission = getTotalFissionCrossSection(u235_data);
        MTData u235_capture = getRadiativeCaptureCrossSection(u235_data);
        
        // Interpolate U235 fission and capture cross-sections separately
        std::vector<double> u235_fission_cross_sections = interpolatedValueS(u235_fission, log_energies);
        std::vector<double> u235_capture_cross_sections = interpolatedValueS(u235_capture, log_energies);
        
        // Process U238 data
        NuclearData u238_data = readNuclearDataFile("../data/U238.dat");
        
        // Get fission and capture cross-sections for U238
        MTData u238_fission = getTotalFissionCrossSection(u238_data);
        MTData u238_capture = getRadiativeCaptureCrossSection(u238_data);
        
        // Interpolate U238 fission and capture cross-sections separately
        std::vector<double> u238_fission_cross_sections = interpolatedValueS(u238_fission, log_energies);
        std::vector<double> u238_capture_cross_sections = interpolatedValueS(u238_capture, log_energies);
        
        // Output combined uranium fission and capture data
        std::string uranium_fission_capture_output = "../plot/uranium_fission_capture.dat";
        clearFileIfExists(uranium_fission_capture_output);
        for (size_t i = 0; i < log_energies.size(); ++i) {
            writeTable(uranium_fission_capture_output, "energy", log_energies[i], 
                      "u235_fission", u235_fission_cross_sections[i],
                      "u235_capture", u235_capture_cross_sections[i],
                      "u238_fission", u238_fission_cross_sections[i],
                      "u238_capture", u238_capture_cross_sections[i]);
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
        
        // Output combined macroscopic cross-sections
        std::string macroscopic_output = "../plot/water_uranium_macroscopic.dat";
        clearFileIfExists(macroscopic_output);
        for (size_t i = 0; i < log_energies.size(); ++i) {
            writeTable(macroscopic_output, "energy", log_energies[i], 
                      "water_macroscopic", water_macroscopic_cross_sections[i],
                      "uranium_macroscopic", uranium_macroscopic_cross_sections[i]);
        }
        
        std::cout << "Generated " << log_energies.size() << " interpolated points for O16, H1, U235, U238, water, and uranium compound" << std::endl;
        std::cout << "H1 and O16 total cross-sections: " << h1_o16_output << std::endl;
        std::cout << "Uranium fission and capture data: " << uranium_fission_capture_output << std::endl;
        std::cout << "U238 inelastic data: " << u238_inelastic_output << std::endl;
        std::cout << "Water and uranium macroscopic cross-sections: " << macroscopic_output << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
