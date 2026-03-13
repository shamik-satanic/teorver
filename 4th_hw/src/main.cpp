#include <fstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <chrono>
#include <cmath>
#include <algorithm>

#include "SimulationEngine.h"
#include "ConfigManager.h"

using namespace ReturnOrigin;

// -------------------------------------------------------
// Консольная гистограмма (ASCII-bar)
// -------------------------------------------------------
static void printBar(double value, double maxVal, int width = 30)
{
    int filled = (maxVal > 0)
        ? static_cast<int>(value / maxVal * width)
        : 0;
    filled = std::clamp(filled, 0, width);
    std::string bar(static_cast<size_t>(filled), '#');
    bar += std::string(static_cast<size_t>(width - filled), ' ');
    std::cout << "[" << bar << "]";
}

// -------------------------------------------------------
static void printResults(const std::vector<NResult>& results,
                         const SimParams& params)
{
    // Найти макс для нормировки гистограммы
    double maxEmp = 0.0, maxTh = 0.0;
    for (auto& r : results) {
        maxEmp = std::max(maxEmp, r.p_empirical);
        maxTh  = std::max(maxTh,  r.p_theoretical);
    }
    double maxVal = std::max(maxEmp, maxTh);

    // Заголовок
    std::cout << "\n";
    std::cout << "========================================================="
                 "===================\n";
    std::cout << "  ЗАДАЧА О ВОЗВРАТЕ В НАЧАЛО КООРДИНАТ\n";
    std::cout << "  p = " << params.p
              << "   q = " << (1.0 - params.p)
              << "   s = " << params.s
              << "   runs = " << params.runs
              << "   N_max = " << params.N_max << "\n";
    std::cout << "========================================================="
                 "===================\n\n";

    std::cout << std::left
              << std::setw(6)  << "N"
              << std::setw(14) << "P_эмпир."
              << std::setw(14) << "P_теория"
              << std::setw(12) << "Δ (абс.)"
              << std::setw(10) << "Возвратов"
              << "  Гистограмма\n";
    std::cout << std::string(100, '-') << "\n";

    for (auto& r : results) {
        double delta = std::abs(r.p_empirical - r.p_theoretical);

        std::cout << std::left
                  << std::setw(6)  << r.N
                  << std::setw(14) << std::fixed << std::setprecision(6)
                                   << r.p_empirical
                  << std::setw(14) << std::fixed << std::setprecision(6)
                                   << r.p_theoretical
                  << std::setw(12) << std::fixed << std::setprecision(6)
                                   << delta
                  << std::setw(10) << r.return_count
                  << "  ";
        printBar(r.p_empirical, maxVal, 28);
        std::cout << "\n";
    }

    std::cout << "\n";

    // Суммарная статистика
    double sumEmp = 0.0, sumTh = 0.0;
    for (auto& r : results) {
        sumEmp += r.p_empirical;
        sumTh  += r.p_theoretical;
    }

    std::cout << "  Сумма P_эмпир. (N=2..N_max): "
              << std::fixed << std::setprecision(6) << sumEmp << "\n";
    std::cout << "  Сумма P_теория (N=2..N_max): "
              << std::fixed << std::setprecision(6) << sumTh  << "\n";
    std::cout << "  (Полная сумма → 1 при N_max → ∞ только если p = q)\n\n";

    // Найти N с максимальной вероятностью
    auto maxIt = std::max_element(results.begin(), results.end(),
        [](const NResult& a, const NResult& b) {
            return a.p_empirical < b.p_empirical;
        });
    if (maxIt != results.end()) {
        std::cout << "  Наибольшая вероятность первого возврата: N = "
                  << maxIt->N
                  << "  P = " << std::fixed << std::setprecision(6)
                  << maxIt->p_empirical << "\n";
    }

    // Особый случай p = q = 0.5
    if (std::abs(params.p - 0.5) < 1e-9) {
        std::cout << "\n  [Симметричное блуждание p = q = 0.5]\n";
        std::cout << "  Теорема: частица ВСЕГДА вернётся в 0 (рекуррентность)\n";
        std::cout << "  Среднее время возврата E[T] = ∞\n";
    } else if (params.p > 0.5) {
        std::cout << "\n  [p > q: блуждание уходит вправо]\n";
        std::cout << "  Вероятность когда-либо вернуться = "
                  << std::fixed << std::setprecision(4)
                  << std::pow((1.0 - params.p) / params.p,
                              params.s / params.s)
                  << " (< 1)\n";
    } else {
        std::cout << "\n  [p < q: блуждание уходит влево]\n";
        std::cout << "  Вероятность когда-либо вернуться = "
                  << std::fixed << std::setprecision(4)
                  << std::pow(params.p / (1.0 - params.p),
                              params.s / params.s)
                  << " (< 1)\n";
    }

    std::cout << "\n=========================================================\n";
}

// -------------------------------------------------------
static void printUsage(const char* argv0)
{
    std::cout << "Использование:\n"
              << "  " << argv0 << " [config.json]\n\n"
              << "Если файл не указан — используются значения по умолчанию.\n"
              << "Пример config.json создаётся автоматически как 'config.json'.\n\n";
}

// -------------------------------------------------------
int main(int argc, char* argv[])
{
    printUsage(argv[0]);

    SimParams params;
    std::string configPath = "config.json";

    if (argc >= 2) {
        configPath = argv[1];
        std::cout << "Загружаем конфиг: " << configPath << "\n";
        params = ConfigManager::loadFromFile(configPath);
    } else {
        std::cout << "Конфиг не указан — используем значения по умолчанию.\n";
        params = ConfigManager::defaults();
        // Сохранить дефолтный конфиг рядом с бинарником
        ConfigManager::saveToFile(configPath, params);
        std::cout << "Дефолтный конфиг сохранён в: " << configPath << "\n";
    }

    // Вывести текущие параметры
    std::cout << "\nПараметры:\n"
              << "  p       = " << params.p       << "\n"
              << "  q       = " << (1.0 - params.p) << "\n"
              << "  s       = " << params.s       << "\n"
              << "  N_max   = " << params.N_max   << "\n"
              << "  runs    = " << params.runs    << "\n";

    // Инициализация ГПСЧ
    uint64_t seed = static_cast<uint64_t>(
        std::chrono::steady_clock::now().time_since_epoch().count());
    seed ^= std::random_device{}();
    std::mt19937_64 rng(seed);

    // Запуск симуляции
    std::cout << "\nЗапуск симуляции...\n";
    auto t0 = std::chrono::steady_clock::now();

    std::vector<NResult> results = runSimulation(params, rng);

    auto t1 = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(t1 - t0).count();
    std::cout << "Готово за " << std::fixed << std::setprecision(2)
              << elapsed << " сек.\n";

    // Вывод результатов
    printResults(results, params);

    // Сохранить результаты в CSV
    std::string csvPath = "results.csv";
    {
        std::ofstream csv(csvPath);
        if (csv.is_open()) {
            csv << "N,p_empirical,p_theoretical,delta,return_count,total_runs\n";
            for (auto& r : results) {
                csv << r.N << ","
                    << std::fixed << std::setprecision(8)
                    << r.p_empirical   << ","
                    << r.p_theoretical << ","
                    << std::abs(r.p_empirical - r.p_theoretical) << ","
                    << r.return_count  << ","
                    << r.total_runs    << "\n";
            }
            std::cout << "Результаты сохранены в: " << csvPath << "\n";
        }
    }

    return 0;
}
