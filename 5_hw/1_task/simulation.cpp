#include <iostream>
#include <iomanip>
#include <cmath>
#include <vector>
#include <random>
#include <string>
#include <numeric>

// ── Параметры ──────────────────────────────────────────────────────────────
static constexpr double LAMBDA     = 1.94;
static constexpr long long N_SIM   = 10'000'000;  // число симулируемых семей
static constexpr int MAX_M         = 5;            // проверяем m = 1..MAX_M

// ── Аналитика ──────────────────────────────────────────────────────────────

// P(K = k) по Пуассону
double poisson_pmf(int k, double lam) {
    return std::exp(-lam) * std::pow(lam, k) / std::tgamma(k + 1);
}

// Хвост Пуассона: P(K >= m, нет девочек) = e^{-lam} * sum_{k=m}^{inf} (lam/2)^k / k!
// Считаем через усечённую сумму (сходится быстро)
double tail_no_girls(int m, double lam, int max_k = 200) {
    double lam2 = lam / 2.0;
    double s = 0.0;
    for (int k = m; k <= max_k; ++k)
        s += std::pow(lam2, k) / std::tgamma(k + 1);
    return std::exp(-lam) * s;
}

// P(нет девочек) = e^{-lam/2}
double prob_no_girls(double lam) {
    return std::exp(-lam / 2.0);
}

// P(K >= m | нет девочек) — аналитически
double cond_prob_analytical(int m, double lam) {
    return tail_no_girls(m, lam) / prob_no_girls(lam);
}

// P(K >= m) — безусловная
double uncond_prob(int m, double lam, int max_k = 200) {
    double s = 0.0;
    for (int k = m; k <= max_k; ++k)
        s += poisson_pmf(k, lam);
    return s;
}

// ── Симуляция ──────────────────────────────────────────────────────────────
struct SimResult {
    double cond_prob;    // P(K >= m | нет девочек)
    double uncond_prob;  // P(K >= m)
};

std::vector<SimResult> simulate(long long n_families, int max_m, double lam,
                                unsigned seed = 42) {
    std::mt19937_64 rng(seed);
    std::poisson_distribution<int> pois(lam);
    std::bernoulli_distribution coin(0.5);  // пол ребёнка

    // Счётчики для каждого m
    std::vector<long long> cnt_cond_num(max_m + 1, 0);  // K>=m И нет девочек
    std::vector<long long> cnt_cond_den(max_m + 1, 0);  // нет девочек
    std::vector<long long> cnt_uncond(max_m + 1, 0);     // K>=m

    for (long long i = 0; i < n_families; ++i) {
        int k = pois(rng);

        // нет ли в семье девочек?
        bool no_girls = true;
        for (int j = 0; j < k; ++j)
            if (!coin(rng)) { no_girls = false; break; }

        for (int m = 1; m <= max_m; ++m) {
            if (k >= m) cnt_uncond[m]++;
            if (no_girls) {
                cnt_cond_den[m]++;
                if (k >= m) cnt_cond_num[m]++;
            }
        }
    }

    std::vector<SimResult> res(max_m + 1);
    for (int m = 1; m <= max_m; ++m) {
        res[m].cond_prob   = (cnt_cond_den[m] > 0)
                             ? (double)cnt_cond_num[m] / cnt_cond_den[m]
                             : 0.0;
        res[m].uncond_prob = (double)cnt_uncond[m] / n_families;
    }
    return res;
}

// ── Вывод ──────────────────────────────────────────────────────────────────
int main() {
    std::cout << std::fixed << std::setprecision(6);

    std::cout << "╔══════════════════════════════════════════════════════════════════╗\n";
    std::cout << "║  Пуассоновская модель семьи  λ=" << LAMBDA
              << "   N=" << N_SIM << "               ║\n";
    std::cout << "╚══════════════════════════════════════════════════════════════════╝\n\n";

    // Распределение числа детей
    std::cout << "── Распределение P(K=k) ─────────────────────────────────────────\n";
    std::cout << "  k   P(K=k)\n";
    for (int k = 0; k <= 8; ++k)
        std::cout << "  " << k << "   " << poisson_pmf(k, LAMBDA) << "\n";
    std::cout << "  P(нет девочек) = e^{-λ/2} = " << prob_no_girls(LAMBDA) << "\n\n";

    // Аналитические вероятности
    std::cout << "── Аналитика ────────────────────────────────────────────────────\n";
    std::cout << "  m   P(K≥m|нет дев.) [аналит.]   P(K≥m) [безусл.]   отношение\n";
    std::cout << "  " << std::string(62, '-') << "\n";
    std::vector<double> a_cond(MAX_M + 1), a_uncond(MAX_M + 1);
    for (int m = 1; m <= MAX_M; ++m) {
        a_cond[m]   = cond_prob_analytical(m, LAMBDA);
        a_uncond[m] = uncond_prob(m, LAMBDA);
        std::cout << "  " << m
                  << "   " << std::setw(26) << a_cond[m]
                  << "   " << std::setw(17) << a_uncond[m]
                  << "   " << std::setw(8) << a_cond[m] / a_uncond[m] << "\n";
    }

    // Симуляция
    std::cout << "\n── Симуляция (" << N_SIM << " семей) ────────────────────────────────\n";
    auto sim = simulate(N_SIM, MAX_M, LAMBDA);

    std::cout << "  m   P(K≥m|нет дев.) [эмпир.]   P(K≥m) [эмпир.]   |Δ| усл.\n";
    std::cout << "  " << std::string(62, '-') << "\n";
    for (int m = 1; m <= MAX_M; ++m) {
        std::cout << "  " << m
                  << "   " << std::setw(26) << sim[m].cond_prob
                  << "   " << std::setw(17) << sim[m].uncond_prob
                  << "   " << std::setw(8)
                  << std::abs(sim[m].cond_prob - a_cond[m]) << "\n";
    }

    // Сравнение: насколько условные отличаются от безусловных
    std::cout << "\n── Разница: условная vs безусловная вероятность ────────────────\n";
    std::cout << "  m   аналит. разница   эмпир. разница   интерпретация\n";
    std::cout << "  " << std::string(62, '-') << "\n";
    for (int m = 1; m <= MAX_M; ++m) {
        double diff_a = a_cond[m] - a_uncond[m];
        double diff_e = sim[m].cond_prob - sim[m].uncond_prob;
        std::string interp = diff_a > 0.001 ? "условная ВЫШЕ" :
                             diff_a < -0.001 ? "условная НИЖЕ" : "примерно равны";
        std::cout << "  " << m
                  << "   " << std::setw(15) << diff_a
                  << "   " << std::setw(15) << diff_e
                  << "   " << interp << "\n";
    }

    
    return 0;
}