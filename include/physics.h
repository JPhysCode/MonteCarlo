#ifndef PHYSICS_H
#define PHYSICS_H

#include "io.h"

// Physics constants and calculations for nuclear transport

// Substance structure: contains nuclear data for species and their stoichiometric coefficients
struct Substance {
    std::vector<NuclearData> species;           // Array of nuclear data for each species
    std::vector<int> stoichiometric_coeffs;     // Corresponding stoichiometric multipliers
    std::vector<double> atom_number_densities;  // Atom number densities (atoms/cm³) for each species
    
    // Constructor
    Substance() = default;
};

// Compound structure: contains multiple substances with molar fractions and density
struct Compound {
    std::vector<Substance> substances;          // Array of substances
    std::vector<double> molar_fractions;        // Corresponding molar fractions
    double density;                             // Density of the compound (g/cm³)
    
    // Constructor
    Compound(double compound_density = 0.0) : density(compound_density) {}
};

// Calculate molar mass of a substance using atomic weights from nuclear data
// Returns molar mass in g/mol
double calculateSubstanceMolarMass(const Substance& substance);

// Calculate mean molar mass of a compound using substance molar masses and molar fractions
// Returns mean molar mass in g/mol
double calculateCompoundMolarMass(const Compound& compound);

// Calculate atom number densities for all species in all substances of a compound
// Uses substance molar masses, mean compound molar mass, and compound density
// Stores results in the atom_number_densities array of each substance
void calculateAtomNumberDensities(Compound& compound);

// Calculate macroscopic cross section for an individual species
// Takes nuclear data and atom number density, returns macroscopic cross section (cm⁻¹)
MTData calculateSpeciesMacroscopicCrossSection(const NuclearData& species_data, double atom_number_density);

// Calculate total macroscopic cross section for a compound
// Computes atom number densities and sums weighted microscopic cross sections
// Returns MTData containing energy-dependent macroscopic cross section (cm⁻¹)
MTData calculateTotalMacroscopicCrossSection(const Compound& compound);

// Generic function to sum multiple MTData objects
// Finds the MTData with most energy points, uses its energy grid as base
// Interpolates all other MTData onto this grid and sums them up
MTData sumMTData(const std::vector<MTData>& mt_data_array);

// Calculate total cross section by summing all MT data (except MT0) pointwise
// Uses the energy grid from the MT with most data points
// Stores result as MT1 in the nuclear data
void calculateTotalCrossSection(NuclearData& nuclear_data);

// Get total cross section by calculating and returning MTData
// Uses calculateTotalCrossSection internally
MTData getTotalCrossSection(NuclearData& nuclear_data);

// Calculate radiative capture cross section by summing MT 102-107
// Uses the energy grid from the MT with most data points among 102-107
// Returns the MTData for radiative capture cross section
MTData getRadiativeCaptureCrossSection(const NuclearData& nuclear_data);

// Get total fission cross section (MT18)
// Returns the MTData for total fission cross section
MTData getTotalFissionCrossSection(const NuclearData& nuclear_data);

// Get total inelastic cross section by summing MT 51-91
// Returns the MTData for total inelastic cross section
MTData getTotalInelasticCrossSection(const NuclearData& nuclear_data);

#endif // PHYSICS_H
