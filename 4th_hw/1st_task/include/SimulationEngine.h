#pragma once

#include <vector>
#include <random>
#include <cstdint>

namespace RandomWalk {

/// Результат одного прогона блуждания
enum class WalkResult {
    Cliff,    ///< Упал в обрыв (позиция 0)
    Cafe,     ///< Вернулся в кафе (позиция B)
    Timeout   ///< Превышен лимит шагов
};

struct WalkOutcome {
    WalkResult result;
    uint64_t   steps;
};

/// Агрегированная статистика для одного значения B
struct BStats {
    int      B;
    double   pCliff;       ///< Эмпирическая P(обрыв)
    double   pCafe;        ///< Эмпирическая P(кафе)
    double   pTheory;      ///< Теоретическая P(обрыв)
    double   avgSteps;     ///< Среднее число шагов
    int      simCount;
};

/// Параметры симуляции
struct SimParams {
    double   p         = 0.40;   ///< Вероятность шага вперёд
    int      bMin      = 1;
    int      bMax      = 15;
    int      simCount  = 2000;   ///< Симуляций на точку B
    uint64_t maxSteps  = 50000;  ///< Лимит шагов в одной симуляции
};

// -------------------------------------------------------
/// Чистая функция: один прогон случайного блуждания
/// startPos — начальная позиция B (обрыв=0, кафе=startPos)
// -------------------------------------------------------
WalkOutcome simulateWalk(int startPos, double p, uint64_t maxSteps,
                         std::mt19937_64& rng);

/// Теоретическая вероятность попасть в обрыв из B
/// (задача о разорении игрока, N→∞)
double theoreticalCliffProb(int B, double p);

/// Записать полную траекторию позиций (для отрисовки)
std::vector<int> recordTrajectory(int startPos, double p,
                                  uint64_t maxSteps,
                                  std::mt19937_64& rng);

/// Прогнать симуляцию по всем B и вернуть вектор статистик
std::vector<BStats> runFullSimulation(const SimParams& params,
                                      std::mt19937_64& rng);

} // namespace RandomWalk
