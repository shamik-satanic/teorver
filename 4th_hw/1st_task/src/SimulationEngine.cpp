#include "SimulationEngine.h"
#include <cmath>
#include <cassert>

namespace RandomWalk {

// -------------------------------------------------------
WalkOutcome simulateWalk(int startPos, double p, uint64_t maxSteps,
                         std::mt19937_64& rng)
{
    assert(startPos > 0);
    assert(p > 0.0 && p < 1.0);

    std::uniform_real_distribution<double> dist(0.0, 1.0);

    int      pos   = startPos;
    uint64_t steps = 0;

    while (steps < maxSteps) {
        if (pos == 0)                          return { WalkResult::Cliff, steps };
        if (pos == startPos && steps > 0)      return { WalkResult::Cafe,  steps };

        pos += (dist(rng) < p) ? 1 : -1;
        ++steps;
    }
    return { WalkResult::Timeout, steps };
}

// -------------------------------------------------------
double theoreticalCliffProb(int B, double p)
{
    // Классическая задача о разорении:
    // При N → ∞ (нет верхнего поглощающего барьера)
    //   p > q:  P(ruin | B) = (q/p)^B
    //   p < q:  P(ruin | B) = 1
    //   p = q:  P(ruin | B) = 1 (случайное блуждание рекуррентно в 1D)
    const double q = 1.0 - p;
    const double eps = 1e-9;

    if (std::fabs(p - q) < eps) {
        // Симметричный случай: P = 1 (но сходится медленно)
        return 1.0;
    }
    if (p > q) {
        return std::pow(q / p, static_cast<double>(B));
    }
    // p < q
    return 1.0;
}

// -------------------------------------------------------
std::vector<int> recordTrajectory(int startPos, double p,
                                  uint64_t maxSteps,
                                  std::mt19937_64& rng)
{
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    std::vector<int> traj;
    traj.reserve(std::min(maxSteps, static_cast<uint64_t>(10000)));
    traj.push_back(startPos);

    int      pos   = startPos;
    uint64_t steps = 0;

    while (steps < maxSteps) {
        if (pos == 0)                     break;
        if (pos == startPos && steps > 0) break;

        pos += (dist(rng) < p) ? 1 : -1;
        traj.push_back(pos);
        ++steps;
    }
    return traj;
}

// -------------------------------------------------------
std::vector<BStats> runFullSimulation(const SimParams& params,
                                      std::mt19937_64& rng)
{
    std::vector<BStats> result;
    result.reserve(static_cast<size_t>(params.bMax - params.bMin + 1));

    for (int B = params.bMin; B <= params.bMax; ++B) {
        int      cliffCount = 0;
        int      cafeCount  = 0;
        uint64_t totalSteps = 0;

        for (int i = 0; i < params.simCount; ++i) {
            auto outcome = simulateWalk(B, params.p, params.maxSteps, rng);
            if (outcome.result == WalkResult::Cliff) ++cliffCount;
            else if (outcome.result == WalkResult::Cafe) ++cafeCount;
            totalSteps += outcome.steps;
        }

        BStats s;
        s.B        = B;
        s.pCliff   = static_cast<double>(cliffCount) / params.simCount;
        s.pCafe    = static_cast<double>(cafeCount)  / params.simCount;
        s.pTheory  = theoreticalCliffProb(B, params.p);
        s.avgSteps = static_cast<double>(totalSteps) / params.simCount;
        s.simCount = params.simCount;
        result.push_back(s);
    }
    return result;
}

} // namespace RandomWalk
