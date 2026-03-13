#pragma once

#include <QObject>
#include <vector>
#include <random>
#include <cstdint>
#include "SimulationEngine.h"

namespace RandomWalk {

class SimulationWorker : public QObject
{
    Q_OBJECT

public:
    explicit SimulationWorker(QObject* parent = nullptr);

public slots:
    void run(RandomWalk::SimParams params);
    void runSingleTrajectory(int B, double p, uint64_t maxSteps);

signals:
    void progressChanged(int current, int total);
    void stepCompleted(RandomWalk::BStats stats);
    void finished(QList<RandomWalk::BStats> allStats);
    void trajectoryReady(QList<int> trajectory, int B, RandomWalk::WalkResult result);
    void errorOccurred(QString message);

private:
    std::mt19937_64 m_rng;
};

} // namespace RandomWalk

Q_DECLARE_METATYPE(RandomWalk::BStats)
Q_DECLARE_METATYPE(RandomWalk::SimParams)
Q_DECLARE_METATYPE(RandomWalk::WalkResult)
Q_DECLARE_METATYPE(QList<RandomWalk::BStats>)
Q_DECLARE_METATYPE(QList<int>)
