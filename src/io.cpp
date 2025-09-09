#include "io.h"
#include <fstream>
#include <iostream>

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
