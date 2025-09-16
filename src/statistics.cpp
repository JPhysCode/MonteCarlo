#include "statistics.h"
#include "rng.h"
#include <cmath>
#include <algorithm>
#include <numeric>
#include <limits>

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

static double normalCdf(double z) {
    return 0.5 * (1.0 + std::erf(z / std::sqrt(2.0)));
}

// Inverse standard normal CDF using Acklam's approximation.
static double inverseNormalCdf(double p) {
    const double a1 = -3.969683028665376e+01;
    const double a2 =  2.209460984245205e+02;
    const double a3 = -2.759285104469687e+02;
    const double a4 =  1.383577518672690e+02;
    const double a5 = -3.066479806614716e+01;
    const double a6 =  2.506628277459239e+00;
    const double b1 = -5.447609879822406e+01;
    const double b2 =  1.615858368580409e+02;
    const double b3 = -1.556989798598866e+02;
    const double b4 =  6.680131188771972e+01;
    const double b5 = -1.328068155288572e+01;
    const double c1 = -7.784894002430293e-03;
    const double c2 = -3.223964580411365e-01;
    const double c3 = -2.400758277161838e+00;
    const double c4 = -2.549732539343734e+00;
    const double c5 =  4.374664141464968e+00;
    const double c6 =  2.938163982698783e+00;
    const double d1 =  7.784695709041462e-03;
    const double d2 =  3.224671290700398e-01;
    const double d3 =  2.445134137142996e+00;
    const double d4 =  3.754408661907416e+00;
    const double plow  = 0.02425;
    const double phigh = 1.0 - plow;

    if (p <= 0.0) return -INFINITY;
    if (p >= 1.0) return INFINITY;

    if (p < plow) {
        double q = std::sqrt(-2.0 * std::log(p));
        return (((((c1*q + c2)*q + c3)*q + c4)*q + c5)*q + c6) /
               ((((d1*q + d2)*q + d3)*q + d4)*q + 1.0);
    }
    if (p > phigh) {
        double q = std::sqrt(-2.0 * std::log(1.0 - p));
        return -(((((c1*q + c2)*q + c3)*q + c4)*q + c5)*q + c6) /
                 ((((d1*q + d2)*q + d3)*q + d4)*q + 1.0);
    }
    double q = p - 0.5;
    double r = q * q;
    return (((((a1*r + a2)*r + a3)*r + a4)*r + a5)*r + a6) * q /
           (((((b1*r + b2)*r + b3)*r + b4)*r + b5)*r + 1.0);
}

// Reservoir sampling to choose k values uniformly without replacement
static std::vector<double> sampleUniformSubset(const std::vector<double>& values, std::size_t k) {
    if (values.size() <= k) return values;
    std::vector<double> reservoir(values.begin(), values.begin() + static_cast<std::ptrdiff_t>(k));
    Random rng(12345u); // Fixed seed for reproducible subsampling
    for (std::size_t i = k; i < values.size(); ++i) {
        double uniform = rng.uniformxy(0.0, static_cast<double>(i + 1));
        std::size_t j = static_cast<std::size_t>(uniform);
        if (j < k) reservoir[j] = values[i];
    }
    return reservoir;
}

double Statistics::shapiroFranciaW(const std::vector<double>& values) {
    const std::size_t n = values.size();
    if (n < 5) return std::numeric_limits<double>::quiet_NaN();

    std::vector<double> sorted = values;
    std::sort(sorted.begin(), sorted.end());
    const double mean = calculateMean(sorted);

    // Expected normal quantiles m_i = Phi^{-1}(i/(n+1))
    std::vector<double> m(n, 0.0);
    for (std::size_t i = 0; i < n; ++i) {
        double pi = (static_cast<double>(i) + 1.0) / (static_cast<double>(n) + 1.0);
        m[i] = inverseNormalCdf(pi);
    }
    double sumM2 = 0.0;
    for (double mi : m) sumM2 += mi * mi;
    const double norm = std::sqrt(sumM2);
    if (norm == 0.0) return std::numeric_limits<double>::quiet_NaN();

    double lin = 0.0;
    for (std::size_t i = 0; i < n; ++i) {
        double ai = m[i] / norm;
        lin += ai * sorted[i];
    }
    double denom = 0.0;
    for (double x : sorted) { double d = x - mean; denom += d * d; }
    if (denom == 0.0) return std::numeric_limits<double>::quiet_NaN();
    return (lin * lin) / denom;
}

double Statistics::shapiroFranciaPValue(const std::vector<double>& values) {
    std::size_t n = values.size();
    if (n < 5) return std::numeric_limits<double>::quiet_NaN();
    std::vector<double> data = (n > 5000) ? sampleUniformSubset(values, 5000) : values;
    n = data.size();
    double w = shapiroFranciaW(data);
    if (!(w >= 0.0) || w >= 1.0) {
        if (w >= 1.0) return 1.0; // perfect normality -> p ~ 1
        return std::numeric_limits<double>::quiet_NaN();
    }
    const double g = std::log1p(-w);
    const double v = std::log(static_cast<double>(n));
    const double beta0 = -1.2725;
    const double beta1 =  1.0521;
    const double gamma0 =  1.0308;
    const double gamma1 = -0.26758;
    const double mu = beta0 + beta1 * (std::log(v) - v);
    const double sigma = gamma0 + gamma1 * (std::log(v) + 2.0 / v);
    const double z = (g - mu) / sigma;
    const double p = 1.0 - normalCdf(z);
    return p;
}

StatisticalResult Statistics::calculateAll(const std::vector<double>& values, const std::vector<double>& runtimes) {
    double mean = calculateMean(values);
    double stdDev = calculateStdDev(values, mean);
    double avgRuntime = calculateAvgRuntime(runtimes);
    double fom = calculateFom(mean, stdDev, avgRuntime);
    double normalityPValue = shapiroFranciaPValue(values);
    
    return {mean, stdDev, avgRuntime, fom, normalityPValue};
}
