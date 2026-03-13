#include "SimulationWorker.h"
#include <chrono>

namespace RandomWalk {

SimulationWorker::SimulationWorker(QObject* parent)
    : QObject(parent)
{
    // Инициализируем ГПСЧ на основе аппаратной энтропии + времени
    auto seed = static_cast<uint64_t>(
        std::chrono::steady_clock::now().time_since_epoch().count()
    );
    m_rng.seed(seed ^ std::random_device{}());
}

void SimulationWorker::run(SimParams params)
{
    if (params.p <= 0.0 || params.p >= 1.0) {
        emit errorOccurred("Вероятность p должна быть в диапазоне (0, 1)");
        return;
    }
    if (params.bMin < 1 || params.bMax < params.bMin) {
        emit errorOccurred("Некорректный диапазон B");
        return;
    }

    QList<BStats> allStats;
    const int total = params.bMax - params.bMin + 1;
    int current = 0;

    for (int B = params.bMin; B <= params.bMax; ++B) {
        int      cliffCount = 0;
        int      cafeCount  = 0;
        uint64_t totalSteps = 0;

        for (int i = 0; i < params.simCount; ++i) {
            auto outcome = simulateWalk(B, params.p, params.maxSteps, m_rng);
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

        allStats.append(s);
        emit stepCompleted(s);
        emit progressChanged(++current, total);
    }

    emit finished(allStats);
}

void SimulationWorker::runSingleTrajectory(int B, double p, uint64_t maxSteps)
{
    if (B <= 0 || p <= 0.0 || p >= 1.0) {
        emit errorOccurred("Некорректные параметры траектории");
        return;
    }

    auto traj = recordTrajectory(B, p, maxSteps, m_rng);

    QList<int> qtTraj;
    qtTraj.reserve(static_cast<int>(traj.size()));
    for (int v : traj) qtTraj.append(v);

    int last = traj.empty() ? B : traj.back();
    WalkResult res = (last == 0)  ? WalkResult::Cliff
                   : (last == B) ? WalkResult::Cafe
                                  : WalkResult::Timeout;

    emit trajectoryReady(qtTraj, B, res);
}

} // namespace RandomWalk
