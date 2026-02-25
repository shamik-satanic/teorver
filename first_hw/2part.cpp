#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <string>
#include <cmath>

using namespace std;

random_device rd;
mt19937 gen(rd());

// ==================== Задача 1: Выбор символов из алфавита ====================
void task1(int N, int C) {
    cout << "\n=== Задача 1: Алфавит из N символов, выборка без возвращения ===\n";
    
    // Аналитика
    int четных = N / 2; // номера 2,4,6,... ≤ N
    double p1_analytical = (double)четных / N; // первый раз чётный
    
    // Второй раз чётный (без изменения индексов)
    double p2a_analytical = (double)(четных - 1) / (N - 1); // если первый был чётный
    
    // Второй раз чётный (с изменением индексов) — зависит от первого выбора, но тут общая формула:
    // P(оба чётные) = C(чётных,2)/C(N,2)
    double p_both_analytical = (четных >= 2) ? 
        (double)(четных * (четных - 1)) / (N * (N - 1)) : 0.0;
    
    // Эмпирика
    vector<int> alphabet(N);
    iota(alphabet.begin(), alphabet.end(), 1);
    
    int count_first_even = 0;
    int count_second_even_a = 0; // индексы не меняются (старый порядок)
    int count_second_even_b = 0; // индексы меняются
    int count_both_even = 0;
    
    uniform_int_distribution<> dist(0, N-1);
    
    for (int exp = 0; exp < C; ++exp) {
        vector<int> temp = alphabet;
        shuffle(temp.begin(), temp.end(), gen);
        
        // а) первый раз чётный
        if (temp[0] % 2 == 0) count_first_even++;
        
        // б) второй раз чётный (индексы не меняются) — просто смотрим temp[1]
        if (temp[1] % 2 == 0) count_second_even_a++;
        
        // в) оба чётные
        if (temp[0] % 2 == 0 && temp[1] % 2 == 0) count_both_even++;
        
        // Для случая с изменением индексов после первого выбора:
        // После выбора первого символа оставшиеся индексы сдвигаются.
        // В эмпирике это не нужно моделировать отдельно — вероятность такая же, как (б),
        // потому что в случайной перестановке порядок равновероятен.
        // Но для демонстрации: вычислим условно при условии первого чётного:
        if (temp[0] % 2 == 0) {
            // Ищем второй символ
            int second = temp[1];
            if (second % 2 == 0) count_second_even_b++;
        }
    }
    
    double p1_empirical = (double)count_first_even / C;
    double p2a_empirical = (double)count_second_even_a / C;
    double p2b_empirical = (double)count_second_even_b / count_first_even; // условная
    double p_both_empirical = (double)count_both_even / C;
    
    cout << "а) Первый раз чётный:\n";
    cout << "   Аналитически: " << p1_analytical << "\n";
    cout << "   Эмпирически:  " << p1_empirical << "\n";
    
    cout << "б) Второй раз чётный (индексы не меняются):\n";
    cout << "   Аналитически: " << (double)(четных - 1)/(N-1) << "\n";
    cout << "   Эмпирически:  " << p2a_empirical << "\n";
    
    cout << "в) Второй раз чётный (индексы меняются):\n";
    cout << "   Аналитически: " << (double)(четных - 1)/(N-1) << " (та же формула)\n";
    cout << "   Эмпирически (условная): " << p2b_empirical << "\n";
    
    cout << "г) Оба раза чётные:\n";
    cout << "   Аналитически: " << p_both_analytical << "\n";
    cout << "   Эмпирически:  " << p_both_empirical << "\n";
}

// ==================== Задача 2: Перестановки, проверка теоремы сложения ====================
void task2(int N) {
    cout << "\n=== Задача 2: Перестановки, проверка P(A_i + A_j) = P(A_i) + P(A_j) - P(A_i A_j) ===\n";
    
    if (N < 2) {
        cout << "N должно быть >= 2\n";
        return;
    }
    
    vector<int> elements(N);
    iota(elements.begin(), elements.end(), 1);
    
    vector<vector<int>> all_permutations;
    do {
        all_permutations.push_back(elements);
    } while (next_permutation(elements.begin(), elements.end()));
    
    int total = all_permutations.size();
    cout << "Всего перестановок: " << total << "\n";
    
    // Для примера возьмём i=1, j=2
    int i = 1, j = 2;
    
    int count_Ai = 0, count_Aj = 0, count_AiAj = 0, count_AiUAj = 0;
    
    cout << "\nБлагоприятствующие исходы для A_i + A_j (i=" << i << ", j=" << j << "):\n";
    for (const auto& perm : all_permutations) {
        bool Ai = (perm[i-1] == i);
        bool Aj = (perm[j-1] == j);
        
        if (Ai) count_Ai++;
        if (Aj) count_Aj++;
        if (Ai && Aj) count_AiAj++;
        if (Ai || Aj) {
            count_AiUAj++;
            // вывод перестановки
            for (int x : perm) cout << x << " ";
            cout << "\n";
        }
    }
    
    double pAi = (double)count_Ai / total;
    double pAj = (double)count_Aj / total;
    double pAiAj = (double)count_AiAj / total;
    double pAiUAj = (double)count_AiUAj / total;
    
    cout << "\nСтатистика:\n";
    cout << "P(Ai) = " << pAi << "\n";
    cout << "P(Aj) = " << pAj << "\n";
    cout << "P(AiAj) = " << pAiAj << "\n";
    cout << "P(Ai+Aj) эмпирически = " << pAiUAj << "\n";
    cout << "P(Ai)+P(Aj)-P(AiAj) = " << pAi + pAj - pAiAj << "\n";
    
    if (fabs(pAiUAj - (pAi + pAj - pAiAj)) < 1e-10) {
        cout << "Теорема сложения подтверждается.\n";
    } else {
        cout << "Ошибка! (не должно происходить)\n";
    }
}

// ==================== Задача 3: Бросание монеты до двух одинаковых подряд ====================
void task3(int k) {
    cout << "\n=== Задача 3: Монета бросается до двух одинаковых подряд ===\n";
    
    if (k < 2) {
        cout << "k должно быть >= 2\n";
        return;
    }
    
    // Аналитика: вероятности
    // Пространство элементарных событий: последовательности орлов (1) и решек (0) длины n ≥ 2,
    // где первые n-1 бросаний: чередуются, n-ое равно n-1-му.
    // Количество исходов длины n: 2 (выбор первой стороны, дальше чередование фиксировано)
    
    // а) опыт закончится до k-ого бросания: n ≤ k-1
    double p_less_than_k = 0.0;
    for (int n = 2; n <= k-1; ++n) {
        p_less_than_k += 2.0 / pow(2, n); // каждый исход длины n имеет вес 1/2^n
    }
    
    // б) чётное число бросаний: n чётное, n ≥ 2
    double p_even = 0.0;
    for (int n = 2; n <= 1000; n += 2) { // до разумного предела
        p_even += 2.0 / pow(2, n);
    }
    
    cout << "Аналитически:\n";
    cout << "а) P(опыт закончится до " << k << "-ого бросания) = " << p_less_than_k << "\n";
    cout << "б) P(чётное число бросаний) = " << p_even << "\n";
    
    // Эмпирика
    int experiments = 1000000;
    uniform_int_distribution<> coin(0, 1);
    
    int count_less_than_k = 0;
    int count_even = 0;
    
    for (int exp = 0; exp < experiments; ++exp) {
        int last = coin(gen);
        int current;
        int n = 1;
        while (true) {
            n++;
            current = coin(gen);
            if (current == last) break;
            last = current;
        }
        
        if (n < k) count_less_than_k++;
        if (n % 2 == 0) count_even++;
    }
    
    cout << "\nЭмпирически (" << experiments << " экспериментов):\n";
    cout << "а) " << (double)count_less_than_k / experiments << "\n";
    cout << "б) " << (double)count_even / experiments << "\n";
}

// ==================== main: выбор задачи по аргументам командной строки ====================
int main(int argc, char* argv[]) {
    cout << fixed << setprecision(6);
    
    if (argc < 2) {
        cout << "Использование:\n";
        cout << "  " << argv[0] << " 1 <N> <C>   - задача 1 (N символов, C экспериментов)\n";
        cout << "  " << argv[0] << " 2 <N>        - задача 2 (N элементов, вывод перестановок)\n";
        cout << "  " << argv[0] << " 3 <k>        - задача 3 (k - порог)\n";
        return 1;
    }
    
    int task = stoi(argv[1]);
    
    if (task == 1) {
        if (argc != 4) {
            cout << "Для задачи 1 нужно: " << argv[0] << " 1 <N> <C>\n";
            return 1;
        }
        int N = stoi(argv[2]);
        int C = stoi(argv[3]);
        task1(N, C);
    }
    else if (task == 2) {
        if (argc != 3) {
            cout << "Для задачи 2 нужно: " << argv[0] << " 2 <N>\n";
            return 1;
        }
        int N = stoi(argv[2]);
        task2(N);
    }
    else if (task == 3) {
        if (argc != 3) {
            cout << "Для задачи 3 нужно: " << argv[0] << " 3 <k>\n";
            return 1;
        }
        int k = stoi(argv[2]);
        task3(k);
    }
    else {
        cout << "Неизвестный номер задачи: " << task << "\n";
        return 1;
    }
    
    return 0;
}