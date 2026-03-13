#include "SimulationEngine.h"
#include <cmath>
#include <cassert>
#include <algorithm>
#include <numeric>

namespace PointMotion {

// -------------------------------------------------------
std::string distName(Distribution d)
{
    switch (d) {
    case Distribution::Uniform:         return "Uniform";
    case Distribution::Binomial:        return "Binomial";
    case Distribution::FiniteGeometric: return "FiniteGeometric";
    case Distribution::DiscreteTriangle:return "DiscreteTriangle";
    }
    return "Unknown";
}

// -------------------------------------------------------
double sampleS(const DistParams& p, std::mt19937_64& rng)
{
    const int imin = static_cast<int>(std::ceil(p.s_min));
    const int imax = static_cast<int>(std::floor(p.s_max));
    const int range = imax - imin;   // >= 0 assumed

    switch (p.dist) {

    case Distribution::Uniform: {
        // Равномерное дискретное на [imin, imax]
        if (range <= 0) return imin;
        std::uniform_int_distribution<int> d(imin, imax);
        return static_cast<double>(d(rng));
    }

    case Distribution::Binomial: {
        // Bin(n_trials, prob) сдвинутое на s_min
        int nt = std::max(1, p.n_trials);
        double pr = std::clamp(p.prob, 0.001, 0.999);
        std::binomial_distribution<int> d(nt, pr);
        // Нормируем в [s_min, s_max]: k / n_trials * (s_max - s_min) + s_min
        int k = d(rng);
        return p.s_min + (nt > 0 ? static_cast<double>(k) / nt : 0.0)
               * (p.s_max - p.s_min);
    }

    case Distribution::FiniteGeometric: {
        // Геометрическое, обрезанное на [0, range]: P(k) ~ (1-prob)^k * prob / Z
        // Сэмплируем rejection sampling
        if (range <= 0) return imin;
        double pr = std::clamp(p.prob, 0.001, 0.999);
        std::geometric_distribution<int> geo(pr);
        int k;
        do { k = geo(rng); } while (k > range);
        return static_cast<double>(imin + k);
    }

    case Distribution::DiscreteTriangle: {
        // Дискретное треугольное: P(k) ~ (range/2 + 1) - |k - range/2|
        if (range <= 0) return imin;
        int N = range + 1;
        int mid = range / 2;
        // Строим CDF
        std::vector<double> weights(static_cast<size_t>(N));
        double total = 0.0;
        for (int i = 0; i < N; ++i) {
            weights[static_cast<size_t>(i)] = static_cast<double>(mid + 1 - std::abs(i - mid));
            total += weights[static_cast<size_t>(i)];
        }
        std::uniform_real_distribution<double> ud(0.0, total);
        double r = ud(rng);
        double cum = 0.0;
        for (int i = 0; i < N; ++i) {
            cum += weights[static_cast<size_t>(i)];
            if (r <= cum) return static_cast<double>(imin + i);
        }
        return static_cast<double>(imin + mid);
    }
    }
    return 0.0;
}

// -------------------------------------------------------
int countCrossings(const std::vector<Point2D>& traj)
{
    int count = 0;
    for (size_t i = 1; i < traj.size(); ++i) {
        double y0 = traj[i-1].y;
        double y1 = traj[i].y;
        // Пересечение: смена знака (строгое), или точное попадание в 0
        if (y1 == 0.0) {
            ++count;
        } else if ((y0 > 0.0 && y1 < 0.0) || (y0 < 0.0 && y1 > 0.0)) {
            ++count;
        }
    }
    return count;
}

// -------------------------------------------------------
SimResult runOnce(const SimParams& params, std::mt19937_64& rng)
{
    SimResult res;
    res.trajectory.reserve(static_cast<size_t>(params.n + 1));

    double x = 0.0;
    double y = params.Y0;
    res.trajectory.push_back({x, y});

    for (int i = 0; i < params.n; ++i) {
        double s = sampleS(params.distParams, rng);
        x += params.h;
        y += s;
        res.trajectory.push_back({x, y});
    }

    res.crossings   = countCrossings(res.trajectory);
    res.reachedZero = (res.crossings > 0);
    return res;
}

// -------------------------------------------------------
EmpiricalResult runMany(const SimParams& params, std::mt19937_64& rng)
{
    EmpiricalResult emp;
    int maxC = 0;

    std::vector<int> crossCounts;
    crossCounts.reserve(static_cast<size_t>(params.runs));

    for (int r = 0; r < params.runs; ++r) {
        int c = runOnce(params, rng).crossings;
        crossCounts.push_back(c);
        if (c > maxC) maxC = c;
    }

    emp.maxCrossings = maxC;
    emp.histogram.assign(static_cast<size_t>(maxC + 1), 0);

    int atLeastL = 0, exactL = 0, zero = 0;
    double sumC = 0.0;

    for (int c : crossCounts) {
        emp.histogram[static_cast<size_t>(c)]++;
        sumC += c;
        if (c == 0)              ++zero;
        if (c == params.l_target) ++exactL;
        if (c >= params.l_target) ++atLeastL;
    }

    double R = static_cast<double>(params.runs);
    emp.pAtLeastL     = atLeastL / R;
    emp.pExactL       = exactL   / R;
    emp.pZero         = zero     / R;
    emp.meanCrossings = sumC     / R;
    return emp;
}

} // namespace PointMotion
