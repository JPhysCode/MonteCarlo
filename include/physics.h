#ifndef PHYSICS_H
#define PHYSICS_H

#include "io.h"

// Physics constants and calculations for nuclear transport

// Calculate total cross section by summing all MT data (except MT0) pointwise
// Uses the energy grid from the MT with most data points
// Stores result as MT1 in the nuclear data
void calculateTotalCrossSection(NuclearData& nuclear_data);

#endif // PHYSICS_H
