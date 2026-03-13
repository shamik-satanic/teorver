#include "SimulationEngine.h"
#include <cmath>
#include <cassert>
#include <stdexcept>

namespace ReturnOrigin {

// -------------------------------------------------------
// Биномиальный коэффициент C(n,k) через логарифмы
// -------------------------------------------------------
static double logBinom(int n, int k)
{
    if (k < 0 || k > n) return -1e300;
    double res = 0.0;
    for (int i = 0; i < k; ++i)
        res += std::log(static_cast<double>(n - i))
             - std::log(static_cast<double>(i + 1));
    return res;
}

// -------------------------------------------------------
// Теоретическая вероятность ПЕРВОГО возврата в 0 за N шагов
//
// Для случайного блуждания на Z с шагами +s (p) и -s (q=1-p):
//   - Возврат в 0 возможен только за чётное N = 2k
//   - P(первый возврат за 2k шагов) = C(2k,k)*p^k*q^k / (2k-1)
//     (формула Чандрасекара / рекуррентная формула для симметр. блуждания)
//
// Для p != 0.5 используем общую формулу через рекуррентность:
//   u_{2k} = C(2k,k) * p^k * q^k   — вероятность быть в 0 на шаге 2k
//   f_{2k} = u_{2k} / (2k-1)        — первый возврат (точная формула)
//
// Точная формула для первого возврата:
//   f_{2k} = C(2k,k) * p^k * q^k / (2k - 1)
// -------------------------------------------------------
double theoreticalFirstReturn(int N, double p)
{
    // Нечётное N — невозможно вернуться в 0
    if (N % 2 != 0) return 0.0;
    if (N < 2)      return 0.0;

    int k = N / 2;
    double q = 1.0 - p;

    // log P = logC(2k,k) + k*log(p) + k*log(q) - log(2k-1)
    double logP = logBinom(2*k, k)
                + k * std::log(p)
                + k * std::log(q)
                - std::log(static_cast<double>(2*k - 1));
    return std::exp(logP);
}

// -------------------------------------------------------
// Симуляция одного прогона длиной ровно N шагов.
// Возвращает true если:
//   1) позиция на шаге N == 0
//   2) позиция НЕ была 0 ни на одном шаге от 1 до N-1
// -------------------------------------------------------
bool simulateFirstReturn(int N, double p, double s, std::mt19937_64& rng)
{
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    double pos = 0.0;

    for (int step = 1; step <= N; ++step) {
        pos += (dist(rng) < p) ? s : -s;

        // Промежуточный возврат до шага N — не считается
        if (step < N && std::abs(pos) < 1e-9)
            return false;
    }
    return std::abs(pos) < 1e-9;
}

// -------------------------------------------------------
// Полная симуляция по всем чётным N от 2 до N_max
// -------------------------------------------------------
std::vector<NResult> runSimulation(const SimParams& params,
                                   std::mt19937_64& rng)
{
    std::vector<NResult> results;

    for (int N = 2; N <= params.N_max; N += 2) {
        uint64_t count = 0;
        for (uint64_t r = 0; r < params.runs; ++r) {
            if (simulateFirstReturn(N, params.p, params.s, rng))
                ++count;
        }

        NResult res;
        res.N             = N;
        res.return_count  = count;
        res.total_runs    = params.runs;
        res.p_empirical   = static_cast<double>(count) / params.runs;
        res.p_theoretical = theoreticalFirstReturn(N, params.p);
        results.push_back(res);
    }
    return results;
}

} // namespace ReturnOrigin
