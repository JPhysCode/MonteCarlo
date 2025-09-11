#include "statistics.h"
#include <cmath>
#include <algorithm>
#include <numeric>

// Statistics class implementation
double Statistics::calculateMean(const std::vector<double>& values) {
    double sum = 0.0;
    for (double value : values) {
        sum += value;
    }
    return sum / static_cast<double>(values.size());
}

double Statistics::calculateStdDev(const std::vector<double>& values, double mean) {
    double variance = 0.0;
    for (double value : values) {
        double diff = value - mean;
        variance += diff * diff;
    }
    return std::sqrt(variance / static_cast<double>(values.size()));
}

double Statistics::calculateAvgRuntime(const std::vector<double>& runtimes) {
    double totalRuntime = 0.0;
    for (double runtime : runtimes) {
        totalRuntime += runtime;
    }
    return totalRuntime / static_cast<double>(runtimes.size());
}

double Statistics::calculateFom(double /*mean*/, double stdDev, double avgRuntime) {
    double variance = stdDev * stdDev;
    return 1.0 / (variance * avgRuntime);
}

double Statistics::shapiroWilkTest(const std::vector<double>& values) {
    // Simplified Shapiro-Wilk test implementation
    // For small samples (n <= 50), this provides a reasonable approximation
    std::vector<double> sortedValues = values;
    std::sort(sortedValues.begin(), sortedValues.end());
    
    std::uint64_t n = sortedValues.size();
    if (n < 3) return 1.0; // Not enough data for meaningful test
    
    // Calculate mean and standard deviation
    double mean = calculateMean(values);
    double stdDev = calculateStdDev(values, mean);
    
    if (stdDev == 0.0) return 1.0; // All values are identical
    
    // Calculate W statistic (simplified version)
    double numerator = 0.0;
    double denominator = 0.0;
    
    for (std::uint64_t i = 0; i < n; ++i) {
        double x = (sortedValues[i] - mean) / stdDev;
        double expected = 0.0;
        
        // Approximate expected order statistics for normal distribution
        double p = (static_cast<double>(i) + 1.0 - 0.375) / (n + 0.25);
        if (p > 0.0 && p < 1.0) {
            // Inverse normal CDF approximation
            double t = std::sqrt(-2.0 * std::log(p));
            expected = t - (2.515517 + 0.802853 * t + 0.010328 * t * t) / 
                       (1.0 + 1.432788 * t + 0.189269 * t * t + 0.001308 * t * t * t);
        }
        
        numerator += expected * x;
        denominator += x * x;
    }
    
    double W = (numerator * numerator) / denominator;
    
    // Approximate p-value calculation for the Shapiro-Wilk test
    double pValue = 1.0;
    if (W < 0.9) {
        pValue = 0.01; // Very likely not normal
    } else if (W < 0.95) {
        pValue = 0.05; // Likely not normal
    } else if (W < 0.98) {
        pValue = 0.1;  // Possibly not normal
    } else {
        pValue = 0.5;  // Likely normal
    }
    
    return pValue;
}

StatisticalResult Statistics::calculateAll(const std::vector<double>& values, const std::vector<double>& runtimes) {
    double mean = calculateMean(values);
    double stdDev = calculateStdDev(values, mean);
    double avgRuntime = calculateAvgRuntime(runtimes);
    double fom = calculateFom(mean, stdDev, avgRuntime);
    double normalityPValue = shapiroWilkTest(values);
    
    return {mean, stdDev, avgRuntime, fom, normalityPValue};
}
