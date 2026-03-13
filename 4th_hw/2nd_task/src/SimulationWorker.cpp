#include "SimulationWorker.h"
#include <chrono>

namespace PointMotion {

SimulationWorker::SimulationWorker(QObject* parent)
    : QObject(parent)
{
    uint64_t seed = static_cast<uint64_t>(
        std::chrono::steady_clock::now().time_since_epoch().count());
    m_rng.seed(seed ^ std::random_device{}());
}

void SimulationWorker::runFull(SimParams params)
{
    if (params.n <= 0 || params.runs <= 0) {
        emit errorOccurred("n и runs должны быть > 0");
        return;
    }
    SimResult    sample = runOnce(params, m_rng);
    EmpiricalResult emp = runMany(params, m_rng);
    emit fullReady(sample, emp);
}

void SimulationWorker::runSingle(SimParams params)
{
    if (params.n <= 0) {
        emit errorOccurred("n должно быть > 0");
        return;
    }
    SimResult res = runOnce(params, m_rng);
    emit singleReady(res);
}

} // namespace PointMotion
