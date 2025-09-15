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

// Kolmogorov–Smirnov test against N(mean, stdDev) using sample mean/stdDev.
// Lilliefors-type setting: parameters estimated from data; p-values use
// asymptotic Kolmogorov distribution as an approximation.
static double normalCdf(double z) {
    return 0.5 * (1.0 + std::erf(z / std::sqrt(2.0)));
}

static double ksPvalue(double d, std::size_t n) {
    // Asymptotic Kolmogorov distribution approximation
    double sum = 0.0;
    for (int k = 1; k <= 100; ++k) {
        double term = std::exp(-2.0 * k * k * d * d * static_cast<double>(n));
        sum += ((k % 2 == 1) ? 1.0 : -1.0) * term;
        if (term < 1e-12) break;
    }
    double p = 2.0 * sum;
    if (p < 0.0) p = 0.0; if (p > 1.0) p = 1.0;
    return p;
}

double Statistics::ksNormalTest(const std::vector<double>& values) {
    std::size_t n = values.size();
    if (n < 3) return 1.0;
    double mean = calculateMean(values);
    double stdDev = calculateStdDev(values, mean);
    if (stdDev == 0.0) return 1.0;
    
    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    
    double d = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        double x = sorted[i];
        double z = (x - mean) / stdDev;
        double F = normalCdf(z);
        double Fn_after = static_cast<double>(i + 1) / static_cast<double>(n);
        double Fn_before = static_cast<double>(i) / static_cast<double>(n);
        double d_plus = Fn_after - F;   // sup over (F_n(x) - F(x))
        double d_minus = F - Fn_before; // sup over (F(x) - F_n(x-))
        if (d_plus > d) d = d_plus;
        if (d_minus > d) d = d_minus;
    }
    return ksPvalue(d, n);
}

StatisticalResult Statistics::calculateAll(const std::vector<double>& values, const std::vector<double>& runtimes) {
    double mean = calculateMean(values);
    double stdDev = calculateStdDev(values, mean);
    double avgRuntime = calculateAvgRuntime(runtimes);
    double fom = calculateFom(mean, stdDev, avgRuntime);
    double normalityPValue = ksNormalTest(values);
    
    return {mean, stdDev, avgRuntime, fom, normalityPValue};
}
