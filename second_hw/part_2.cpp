#include <iostream>
#include <iomanip>
#include <vector>
#include <random>
#include <chrono>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <cctype>

using namespace std;

bool isNumber(const string& s) {
    if (s.empty()) return false;

    for (char c : s) {
        if (!isdigit(c)) {
            return false;
        }
    }
    return true;
}

void printUsage(const char* programName) {
    cout << "Использование: " << programName << " K [n]\n";
    cout << "  K - количество экспериментов (целое положительное число, обязательно)\n";
    cout << "  n - количество ключей в связке (целое положительное число, необязательно, по умолчанию 10)\n";
    cout << "\nПримеры:\n";
    cout << "  " << programName << " 1000\n";
    cout << "  " << programName << " 5000 5\n";
    cout << "  " << programName << " 10000 20\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cout << "Ошибка: не указан обязательный аргумент K\n";
        printUsage(argv[0]);
        return 1;
    }
    
    if (!isNumber(argv[1])) {
        cout << "Ошибка: K должно быть целым положительным числом\n";
        cout << "Вы ввели: " << argv[1] << "\n";
        printUsage(argv[0]);
        return 1;
    }
    
    long long K_long = atoll(argv[1]);
    if (K_long <= 0 || K_long > 1000000000) {
        cout << "Ошибка: K должно быть положительным числом не больше 1e9\n";
        return 1;
    }
    int K = (int)K_long;
    
    int n = 10;
    
    if (argc >= 3) {
        if (!isNumber(argv[2])) {
            cout << "Ошибка: n должно быть целым положительным числом\n";
            cout << "Вы ввели: " << argv[2] << "\n";
            printUsage(argv[0]);
            return 1;
        }
        
        long long n_long = atoll(argv[2]);
        if (n_long <= 0 || n_long > 1000000) {
            cout << "Ошибка: n должно быть положительным числом не больше 1e6\n";
            return 1;
        }
        n = (int)n_long;
    }
    
    if (K > 1000000) {
        cout << "Предупреждение: K = " << K << " очень большое. Это может занять много времени.\n";
        cout << "Рекомендуется K <= 1000000 для быстрой работы.\n";
        cout << "Продолжить? (y/n): ";
        
        string answer;
        cin >> answer;
        if (answer != "y" && answer != "Y" && answer != "yes" && answer != "Yes") {
            cout << "Программа завершена.\n";
            return 0;
        }
    }
    
    cout << "Моделирование последовательного испытания ключей\n";
    cout << "================================================\n";
    cout << "Количество ключей в связке: " << n << "\n";
    cout << "Количество экспериментов: " << K << "\n\n";
    
    random_device rd;
    mt19937 gen(rd());

    vector<int> stepCounts(n, 0);
    
    cout << "Результаты экспериментов:\n";
    cout << "------------------------\n";

    int experimentsToShow = min(K, 20);
    
    for (int exp = 1; exp <= K; exp++) {
        uniform_int_distribution<> posDist(0, n - 1);
        int correctKeyPos = posDist(gen);

        vector<int> testOrder(n);
        for (int i = 0; i < n; i++) {
            testOrder[i] = i;
        }
        shuffle(testOrder.begin(), testOrder.end(), gen);
        
        int step = 0;
        bool found = false;
        
        for (int i = 0; i < n && !found; i++) {
            step++;
            if (testOrder[i] == correctKeyPos) {
                found = true;
            }
        }
        stepCounts[step - 1]++;

        if (exp <= experimentsToShow) {
            cout << "Эксперимент " << setw(4) << exp << ": ключ найден на " << step << " испытании\n";
        } else if (exp == experimentsToShow + 1) {
            cout << "... (пропущено " << K - experimentsToShow << " экспериментов)\n";
        }
    }
    
    cout << "------------------------\n\n";
    

    cout << "Статистика:\n";
    cout << "-----------\n";
    cout << "Теоретическая вероятность для каждого исхода: 1/" << n << " = " 
         << fixed << setprecision(6) << 1.0/n << "\n\n";
    
    cout << "Исход | Частота | Эмпирическая вероятность | Отклонение\n";
    cout << "------------------------------------------------------\n";
    
    double theoreticalProb = 1.0 / n;
    
    for (int i = 0; i < n; i++) {
        double empiricalProb = (double)stepCounts[i] / K;
        double deviation = abs(empiricalProb - theoreticalProb);
        
        cout << setw(5) << i + 1 << " | "
             << setw(8) << stepCounts[i] << " | "
             << fixed << setprecision(6) << setw(12) << empiricalProb << "   | "
             << fixed << setprecision(6) << deviation << "\n";
    }
    
    return 0;
}