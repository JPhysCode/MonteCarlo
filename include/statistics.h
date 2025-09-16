#pragma once
#include <vector>
#include <cstdint>

// Statistical result structure (for calculateAll method)
struct StatisticalResult {
    double mean;
    double stdDev;
    double avgRuntime;
    double fom;
    double normalityPValue;
};

// Statistics calculation class
class Statistics {
public:
    // Individual statistical calculation methods
    static double calculateMean(const std::vector<double>& values);
    static double calculateStdDev(const std::vector<double>& values, double mean);
    static double calculateAvgRuntime(const std::vector<double>& runtimes);
    // Figure of merit using precomputed stats: FOM = 1 / (variance * avgRuntime)
    static double calculateFom(double mean, double stdDev, double avgRuntime);
    
    // Shapiro–Francia normality test.
    // Returns the statistic W' in (0,1]. If the input size n < 5, returns NaN.
    static double shapiroFranciaW(const std::vector<double>& values);
    
    // Shapiro–Francia p-value using Royston calibration (valid roughly for 5 <= n <= 5000).
    // If n > 5000, computes p-value on a uniform random subsample of size 5000.
    // If n < 5, returns NaN.
    static double shapiroFranciaPValue(const std::vector<double>& values);
    
    // Calculate all statistics
    static StatisticalResult calculateAll(const std::vector<double>& values, const std::vector<double>& runtimes);
};
