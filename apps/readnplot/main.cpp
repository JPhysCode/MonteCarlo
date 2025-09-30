#include <iostream>
#include <fstream>
#include <string>
#include "../../include/io.h"
#include "../../include/physics.h"

int main() {
    std::cout << "Read and Plot Application" << std::endl;
    
    try {
        // Read O16.dat from data folder
        std::string data_file = "../data/O16.dat";
        std::cout << "Reading nuclear data from: " << data_file << std::endl;
        
        NuclearData data = readNuclearDataFile(data_file);
        
        std::cout << "Loaded data for: " << data.symbol << std::endl;
        std::cout << "Z=" << data.pnumber << ", A=" << data.mnumber << ", AW=" << data.aweight << ", T=" << data.temp << "K" << std::endl;
        std::cout << "Number of MT sections: " << data.mt_data.size() << std::endl;
        
        // Calculate total cross section (MT1)
        std::cout << "Calculating total cross section..." << std::endl;
        calculateTotalCrossSection(data);
        
        // Check if MT1 exists (total cross section)
        if (data.mt_data.find(1) == data.mt_data.end()) {
            std::cout << "MT1 not found after calculation!" << std::endl;
            return 1;
        }
        
        const MTData& mt1 = data.mt_data.at(1);
        std::cout << "MT1 data: Q=" << mt1.qval << ", Number of points=" << mt1.num_ec_pairs << std::endl;
        
        // Output MT1 data to plot file using writeTable
        std::string output_file = "../plot/O16_MT1.dat";
        
        // Clear the file first
        clearFileIfExists(output_file);
        
        // Write each energy-cross section pair as a separate row
        for (const auto& pair : mt1.efunc) {
            writeTable(output_file, "energy", pair.energy, "cross_section", pair.cross_section);
        }
        
        std::cout << "MT1 data written to: " << output_file << std::endl;
        std::cout << "Total data points: " << mt1.efunc.size() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
