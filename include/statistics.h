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
    
    // Normality testing (Kolmogorov–Smirnov against Normal with sample mean/std).
    // Note: Since parameters are estimated from the sample, this is a Lilliefors-type
    // approximation; p-values are based on the asymptotic Kolmogorov distribution.
    static double ksNormalTest(const std::vector<double>& values);
    
    // Calculate all statistics
    static StatisticalResult calculateAll(const std::vector<double>& values, const std::vector<double>& runtimes);
};
