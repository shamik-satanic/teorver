#pragma once
#include <QObject>
#include <random>
#include <cstdint>
#include "SimulationEngine.h"

namespace PointMotion {

class SimulationWorker : public QObject
{
    Q_OBJECT
public:
    explicit SimulationWorker(QObject* parent = nullptr);

public slots:
    void runFull(PointMotion::SimParams params);
    void runSingle(PointMotion::SimParams params);

signals:
    void singleReady(PointMotion::SimResult result);
    void fullReady(PointMotion::SimResult sample, PointMotion::EmpiricalResult emp);
    void errorOccurred(QString msg);

private:
    std::mt19937_64 m_rng;
};

} // namespace PointMotion

Q_DECLARE_METATYPE(PointMotion::SimParams)
Q_DECLARE_METATYPE(PointMotion::SimResult)
Q_DECLARE_METATYPE(PointMotion::EmpiricalResult)
