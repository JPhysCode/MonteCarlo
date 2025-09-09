#include "statistics.h"
#include <cmath>

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

StatisticalResult Statistics::calculateAll(const std::vector<double>& values, const std::vector<double>& runtimes) {
    double mean = calculateMean(values);
    double stdDev = calculateStdDev(values, mean);
    double avgRuntime = calculateAvgRuntime(runtimes);
    double fom = calculateFom(mean, stdDev, avgRuntime);
    
    return {mean, stdDev, avgRuntime, fom};
}
