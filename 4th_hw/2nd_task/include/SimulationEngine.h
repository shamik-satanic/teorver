#pragma once
#include <vector>
#include <string>
#include <random>
#include <cstdint>

namespace PointMotion {

enum class Distribution {
    Uniform,
    Binomial,
    FiniteGeometric,
    DiscreteTriangle
};

struct DistParams {
    Distribution dist     = Distribution::Uniform;
    double       s_min    = -3.0;
    double       s_max    =  3.0;
    int          n_trials =  6;
    double       prob     =  0.5;
};

struct SimParams {
    double     Y0       = 10.0;
    double     h        =  1.0;
    int        n        = 100;
    int        runs     = 1000;
    int        l_target =  3;
    DistParams distParams;
};

struct Point2D { double x, y; };

struct SimResult {
    std::vector<Point2D> trajectory;
    int  crossings   = 0;
    bool reachedZero = false;
};

struct EmpiricalResult {
    double pAtLeastL    = 0.0;
    double pExactL      = 0.0;
    double pZero        = 0.0;
    double meanCrossings = 0.0;
    std::vector<int> histogram;
    int maxCrossings    = 0;
};

double        sampleS(const DistParams& p, std::mt19937_64& rng);
SimResult     runOnce(const SimParams& params, std::mt19937_64& rng);
EmpiricalResult runMany(const SimParams& params, std::mt19937_64& rng);
std::string   distName(Distribution d);
int           countCrossings(const std::vector<Point2D>& traj);

} // namespace PointMotion
