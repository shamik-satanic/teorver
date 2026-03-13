#pragma once
#include <vector>
#include <random>
#include <cstdint>

namespace ReturnOrigin {

struct SimParams {
    double   p          = 0.5;
    double   s          = 1.0;
    int      N_max      = 32;
    uint64_t runs       = 100000;
    uint64_t max_steps  = 10000;
};

struct NResult {
    int      N;
    double   p_empirical;
    double   p_theoretical;
    uint64_t return_count;
    uint64_t total_runs;
};

// Теоретическая вероятность первого возврата в 0 за ровно N шагов
double theoreticalFirstReturn(int N, double p);

// Один прогон: true = первый возврат в 0 произошёл именно на шаге N
bool simulateFirstReturn(int N, double p, double s, std::mt19937_64& rng);

// Симуляция для всех чётных N от 2 до N_max
std::vector<NResult> runSimulation(const SimParams& params, std::mt19937_64& rng);

} // namespace ReturnOrigin
