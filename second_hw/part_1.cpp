#include <iostream>
#include <iomanip>
#include <vector>
#include <cmath>
#include <algorithm>
#include <random>
#include <chrono>
#include <map>
#include <set>
#include <cstdlib>
#include <ctime>

using namespace std;

double factorial(int n) {
    double res = 1.0;
    for (int i = 2; i <= n; i++) res *= i;
    return res;
}

double combination(int n, int k) {
    if (k < 0 || k > n) return 0;
    return factorial(n) / (factorial(k) * factorial(n - k));
}

void task57() {
    double p1 = 0.6, p2 = 0.7, p3 = 0.8, p4 = 0.9;
    double a_teor = 1.0 - (p1 * p2 * p3 * p4);
    double all_four = p1 * p2 * p3 * p4;
    double exactly_three = 
        p1 * p2 * p3 * (1 - p4) +
        p1 * p2 * (1 - p3) * p4 +
        p1 * (1 - p2) * p3 * p4 +
        (1 - p1) * p2 * p3 * p4;
    double b_teor = exactly_three + all_four;
    
    // Эмпирическая проверка
    int experiments = 1000000;
    int count_a = 0, count_b = 0;
    
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 1);
    
    for (int i = 0; i < experiments; i++) {
        bool in1 = dis(gen) < p1;
        bool in2 = dis(gen) < p2;
        bool in3 = dis(gen) < p3;
        bool in4 = dis(gen) < p4;
        
        int count = in1 + in2 + in3 + in4;
        if (count <= 3) count_a++;
        if (count >= 2) count_b++;
    }
    
    double a_emp = (double)count_a / experiments;
    double b_emp = (double)count_b / experiments;
    
    cout << "Задача 57:\n";
    cout << "  а) не более чем в трех:\n";
    cout << "     Теоретическая: " << a_teor << "\n";
    cout << "     Эмпирическая:  " << a_emp << "\n";
    cout << "  б) не менее чем в двух:\n";
    cout << "     Теоретическая: " << b_teor << "\n";
    cout << "     Эмпирическая:  " << b_emp << "\n\n";
}

void task58() {
    double a_teor = 1.0 / (6 * 6 * 6);
    double b_teor = 6.0 / (6 * 6 * 6);
    
    // Эмпирическая проверка
    int experiments = 1000000;
    int count_a = 0, count_b = 0;
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 6);
    
    for (int i = 0; i < experiments; i++) {
        int d1 = dis(gen);
        int d2 = dis(gen);
        int d3 = dis(gen);
        
        if (d1 == 5 && d2 == 5 && d3 == 5) count_a++;
        if (d1 == d2 && d2 == d3) count_b++;
    }
    
    double a_emp = (double)count_a / experiments;
    double b_emp = (double)count_b / experiments;
    
    cout << "Задача 58:\n";
    cout << "  а) на каждой пять очков:\n";
    cout << "     Теоретическая: " << a_teor << "\n";
    cout << "     Эмпирическая:  " << a_emp << "\n";
    cout << "  б) одинаковое число:\n";
    cout << "     Теоретическая: " << b_teor << "\n";
    cout << "     Эмпирическая:  " << b_emp << "\n\n";
}

void task59() {
    double a_teor = (6.0 * 5) / (6 * 6 * 6);
    double b_teor = (6.0 * 5 * 3) / (6 * 6 * 6);
    double c_teor = 1.0 / (6 * 6);
    
    // Эмпирическая проверка
    int experiments = 1000000;
    int count_a = 0, count_b = 0, count_c = 0;
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 6);
    
    for (int i = 0; i < experiments; i++) {
        int d1 = dis(gen);
        int d2 = dis(gen);
        int d3 = dis(gen);
        
        if ((d1 == d2 && d2 != d3) || (d1 == d3 && d1 != d2) || (d2 == d3 && d2 != d1)) count_b++;
        if (d1 == d2 && d2 == d3) count_c++;
    }
    count_a = experiments - count_b - count_c;
    
    double a_emp = (double)count_a / experiments;
    double b_emp = (double)count_b / experiments;
    double c_emp = (double)count_c / experiments;
    
    cout << "Задача 59:\n";
    cout << "  а) на двух по одному, на третьей другое:\n";
    cout << "     Теоретическая: " << a_teor << "\n";
    cout << "     Эмпирическая:  " << a_emp << "\n";
    cout << "  б) на двух одинаковое, на третьей другое:\n";
    cout << "     Теоретическая: " << b_teor << "\n";
    cout << "     Эмпирическая:  " << b_emp << "\n";
    cout << "  в) на всех одинаковое:\n";
    cout << "     Теоретическая: " << c_teor << "\n";
    cout << "     Эмпирическая:  " << c_emp << "\n\n";
}

void task61() {
    double p = 0.8;
    double target = 0.4;
    int n_teor = 1;
    while (pow(p, n_teor) >= target) {
        n_teor++;
    }
    
    // Эмпирическая проверка
    int experiments = 100000;
    vector<int> results;
    
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 1);
    
    for (int n = 1; n <= 10; n++) {
        int success_count = 0;
        for (int exp = 0; exp < experiments; exp++) {
            bool all_hit = true;
            for (int shot = 0; shot < n; shot++) {
                if (dis(gen) >= p) {
                    all_hit = false;
                    break;
                }
            }
            if (all_hit) success_count++;
        }
        double prob = (double)success_count / experiments;
        if (prob < target) {
            cout << "Задача 61:\n";
            cout << "  Теоретическое n: " << n_teor << "\n";
            cout << "  Эмпирическое n:  " << n << " (вероятность " << prob << " < " << target << ")\n\n";
            break;
        }
    }
}

void task62() {
    double R = 1.0;
    double area_circle = M_PI * R * R;
    double side = sqrt(3) * R;
    double area_triangle = (sqrt(3) / 4) * side * side;
    double area_segment = (area_circle - area_triangle) / 3.0;
    double p_triangle = area_triangle / area_circle;
    double p_segment = area_segment / area_circle;
    double a_teor = p_triangle * p_triangle * p_triangle * p_triangle;
    double ways = 4.0 * 3.0 * 2.0 * 1.0;
    double b_teor = ways * p_triangle * p_segment * p_segment * p_segment;
    
    // Эмпирическая проверка
    int experiments = 1000000;
    int count_a = 0, count_b = 0;
    
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(-R, R);
    
    for (int i = 0; i < experiments; i++) {
        vector<pair<double, double>> points;
        for (int p = 0; p < 4; p++) {
            double x, y;
            do {
                x = dis(gen);
                y = dis(gen);
            } while (x*x + y*y > R*R);
            points.push_back({x, y});
        }
        
        // Проверка попадания в треугольник (приближенно)
        int in_triangle = 0;
        vector<int> in_segments(3, 0);
        
        for (auto& point : points) {
            double x = point.first, y = point.second;
            double angle = atan2(y, x);
            if (angle < 0) angle += 2*M_PI;
            
            // Приближенная проверка попадания в треугольник (повернутый на 90 градусов)
            bool in_t = (abs(x) <= side/2 && y >= -side/(2*sqrt(3)) && y <= side/sqrt(3) - abs(x)*sqrt(3));
            if (in_t) {
                in_triangle++;
            } else {
                int segment = floor(angle / (2*M_PI/3));
                if (segment >= 0 && segment < 3) in_segments[segment]++;
            }
        }
        
        if (in_triangle == 4) count_a++;
        
        bool b_cond = (in_triangle == 1 && 
                      in_segments[0] == 1 && 
                      in_segments[1] == 1 && 
                      in_segments[2] == 1);
        if (b_cond) count_b++;
    }
    
    double a_emp = (double)count_a / experiments;
    double b_emp = (double)count_b / experiments;
    
    cout << "Задача 62:\n";
    cout << "  а) все четыре в треугольник:\n";
    cout << "     Теоретическая: " << a_teor << "\n";
    cout << "     Эмпирическая:  " << a_emp << "\n";
    cout << "  б) одна в треугольник, по одной в сегменты:\n";
    cout << "     Теоретическая: " << b_teor << "\n";
    cout << "     Эмпирическая:  " << b_emp << "\n\n";
}

void task63() {
    double a_teor = (3.0 * 2.0 * 1.0) / (3.0 * 3.0 * 3.0);
    
    // Эмпирическая проверка
    int experiments = 1000000;
    int count = 0;
    
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 3);
    
    for (int i = 0; i < experiments; i++) {
        double p1 = dis(gen);
        double p2 = dis(gen);
        double p3 = dis(gen);
        
        int part1 = floor(p1);
        int part2 = floor(p2);
        int part3 = floor(p3);
        
        set<int> parts = {part1, part2, part3};
        if (parts.size() == 3) count++;
    }
    
    double a_emp = (double)count / experiments;
    
    cout << "Задача 63:\n";
    cout << "  По одной точке на каждую часть:\n";
    cout << "     Теоретическая: " << a_teor << "\n";
    cout << "     Эмпирическая:  " << a_emp << "\n\n";
}

void task65() {
    double a_teor = (combination(5, 2) * combination(95, 0)) / combination(100, 2);
    
    // Эмпирическая проверка
    int experiments = 1000000;
    int count = 0;
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 100);
    
    for (int i = 0; i < experiments; i++) {
        int ticket1 = dis(gen);
        int ticket2;
        do {
            ticket2 = dis(gen);
        } while (ticket2 == ticket1);
        
        if (ticket1 <= 5 && ticket2 <= 5) count++;
    }
    
    double a_emp = (double)count / experiments;
    
    cout << "Задача 65:\n";
    cout << "  Два выигрышных билета:\n";
    cout << "     Теоретическая: " << a_teor << "\n";
    cout << "     Эмпирическая:  " << a_emp << "\n\n";
}

void task67() {
    double a_teor = combination(6, 4) / combination(10, 4);
    
    // Эмпирическая проверка
    int experiments = 1000000;
    int count = 0;
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 10);
    
    for (int i = 0; i < experiments; i++) {
        vector<int> chosen;
        for (int j = 0; j < 4; j++) {
            int det;
            do {
                det = dis(gen);
            } while (find(chosen.begin(), chosen.end(), det) != chosen.end());
            chosen.push_back(det);
        }
        
        bool all_colored = true;
        for (int d : chosen) {
            if (d > 6) {
                all_colored = false;
                break;
            }
        }
        if (all_colored) count++;
    }
    
    double a_emp = (double)count / experiments;
    
    cout << "Задача 67:\n";
    cout << "  Все четыре окрашенные:\n";
    cout << "     Теоретическая: " << a_teor << "\n";
    cout << "     Эмпирическая:  " << a_emp << "\n\n";
}

void task68() {
    double a_teor = (1.0 / 5) * (1.0 / 4) * (1.0 / 3);
    double b_teor = (3.0 * 2.0 * 1.0) / (5.0 * 4.0 * 3.0);
    
    // Эмпирическая проверка
    int experiments = 1000000;
    int count_a = 0, count_b = 0;
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 5);
    
    for (int i = 0; i < experiments; i++) {
        int b1 = dis(gen);
        int b2;
        do {
            b2 = dis(gen);
        } while (b2 == b1);
        int b3;
        do {
            b3 = dis(gen);
        } while (b3 == b1 || b3 == b2);
        
        if (b1 == 1 && b2 == 4 && b3 == 5) count_a++;
        
        set<int> balls = {b1, b2, b3};
        if (balls.count(1) && balls.count(4) && balls.count(5)) count_b++;
    }
    
    double a_emp = (double)count_a / experiments;
    double b_emp = (double)count_b / experiments;
    
    cout << "Задача 68:\n";
    cout << "  а) последовательно 1,4,5:\n";
    cout << "     Теоретическая: " << a_teor << "\n";
    cout << "     Эмпирическая:  " << a_emp << "\n";
    cout << "  б) любые 1,4,5:\n";
    cout << "     Теоретическая: " << b_teor << "\n";
    cout << "     Эмпирическая:  " << b_emp << "\n\n";
}

void task69() {
    double a_teor = combination(20, 3) / combination(25, 3);
    
    // Эмпирическая проверка
    int experiments = 1000000;
    int count = 0;
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 25);
    
    for (int i = 0; i < experiments; i++) {
        vector<int> chosen;
        for (int j = 0; j < 3; j++) {
            int q;
            do {
                q = dis(gen);
            } while (find(chosen.begin(), chosen.end(), q) != chosen.end());
            chosen.push_back(q);
        }
        
        bool all_known = true;
        for (int q : chosen) {
            if (q > 20) {
                all_known = false;
                break;
            }
        }
        if (all_known) count++;
    }
    
    double a_emp = (double)count / experiments;
    
    cout << "Задача 69:\n";
    cout << "  Знает три вопроса:\n";
    cout << "     Теоретическая: " << a_teor << "\n";
    cout << "     Эмпирическая:  " << a_emp << "\n\n";
}

void task70() {
    double a_teor = (1.0 / 10) * (1.0 / 9) * (1.0 / 8);
    double b_teor = (1.0 / 10) * (1.0 / 10) * (1.0 / 10);
    
    // Эмпирическая проверка
    int experiments = 1000000;
    int count_a = 0, count_b = 0;
    
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis(1, 10);
    
    // Без возвращения
    for (int i = 0; i < experiments; i++) {
        int c1 = dis(gen);
        int c2;
        do {
            c2 = dis(gen);
        } while (c2 == c1);
        int c3;
        do {
            c3 = dis(gen);
        } while (c3 == c1 || c3 == c2);
        
        if (c1 == 1 && c2 == 2 && c3 == 3) count_a++;
    }
    
    // С возвращением
    for (int i = 0; i < experiments; i++) {
        int c1 = dis(gen);
        int c2 = dis(gen);
        int c3 = dis(gen);
        
        if (c1 == 1 && c2 == 2 && c3 == 3) count_b++;
    }
    
    double a_emp = (double)count_a / experiments;
    double b_emp = (double)count_b / experiments;
    
    cout << "Задача 70:\n";
    cout << "  а) без возвращения 1,2,3:\n";
    cout << "     Теоретическая: " << a_teor << "\n";
    cout << "     Эмпирическая:  " << a_emp << "\n";
    cout << "  б) с возвращением 1,2,3:\n";
    cout << "     Теоретическая: " << b_teor << "\n";
    cout << "     Эмпирическая:  " << b_emp << "\n\n";
}

void task1() {
    double p_starosta = 0.3;
    double p_student = 0.5;
    double target = 0.9;
    int n_teor = 0;
    double prob = 0.0;
    while (prob <= target) {
        prob = 1.0 - (1.0 - p_starosta) * pow(1.0 - p_student, n_teor);
        if (prob > target) break;
        n_teor++;
    }
    
    // Эмпирическая проверка
    int experiments = 100000;
    int n_emp = 0;
    
    random_device rd;
    mt19937 gen(rd());
    uniform_real_distribution<> dis(0, 1);
    
    for (int n = 0; n <= 10; n++) {
        int success_count = 0;
        for (int exp = 0; exp < experiments; exp++) {
            bool informed = false;
            
            if (dis(gen) < p_starosta) {
                informed = true;
            } else {
                for (int s = 0; s < n; s++) {
                    if (dis(gen) < p_student) {
                        informed = true;
                        break;
                    }
                }
            }
            
            if (informed) success_count++;
        }
        double prob_emp = (double)success_count / experiments;
        if (prob_emp > target) {
            n_emp = n;
            break;
        }
    }
    
    cout << "Задача 1 (доп):\n";
    cout << "  Нужно студентов помимо старосты:\n";
    cout << "     Теоретическое n: " << n_teor << "\n";
    cout << "     Эмпирическое n:  " << n_emp << "\n\n";
}

void task2() {
    cout << "Задача 2 (доп):\n";
    
    // Теоретические расчеты
    cout << "Колода 36 карт:\n";
    cout << "Без возвращения:\n";
    double total36 = combination(36, 4);
    int values36 = 36 / 4;
    double p1_36_no = (values36 * combination(4, 3) * (values36 - 1) * combination(4, 1)) / total36;
    double p2_36_no = (combination(values36, 2) * combination(4, 2) * combination(4, 2)) / total36;
    double p3_36_no = (combination(values36, 4) * pow(4, 4)) / total36;
    double p4_36_no = values36 / total36;
    cout << "  Теоретические:\n";
    cout << "    1) три одного, одна другого: " << p1_36_no << "\n";
    cout << "    2) две одного, две другого: " << p2_36_no << "\n";
    cout << "    3) все разного значения: " << p3_36_no << "\n";
    cout << "    4) все одного значения: " << p4_36_no << "\n";
    
    cout << "С возвращением:\n";
    double total36_ret = pow(36, 4);
    double p1_36_ret = (36 * 3 * (36 - 1)) / total36_ret;
    double p2_36_ret = (36 * 3 * (36 - 1)) / total36_ret;
    double p3_36_ret = (36 * (36 - 1) * (36 - 2) * (36 - 3)) / total36_ret;
    double p4_36_ret = 36 / total36_ret;
    cout << "  Теоретические:\n";
    cout << "    1) три одного, одна другого: " << p1_36_ret << "\n";
    cout << "    2) две одного, две другого: " << p2_36_ret << "\n";
    cout << "    3) все разного значения: " << p3_36_ret << "\n";
    cout << "    4) все одного значения: " << p4_36_ret << "\n";
    
    cout << "Колода 52 карты:\n";
    cout << "Без возвращения:\n";
    double total52 = combination(52, 4);
    int values52 = 52 / 4;
    double p1_52_no = (values52 * combination(4, 3) * (values52 - 1) * combination(4, 1)) / total52;
    double p2_52_no = (combination(values52, 2) * combination(4, 2) * combination(4, 2)) / total52;
    double p3_52_no = (combination(values52, 4) * pow(4, 4)) / total52;
    double p4_52_no = values52 / total52;
    cout << "  Теоретические:\n";
    cout << "    1) три одного, одна другого: " << p1_52_no << "\n";
    cout << "    2) две одного, две другого: " << p2_52_no << "\n";
    cout << "    3) все разного значения: " << p3_52_no << "\n";
    cout << "    4) все одного значения: " << p4_52_no << "\n";
    
    cout << "С возвращением:\n";
    double total52_ret = pow(52, 4);
    double p1_52_ret = (52 * 3 * (52 - 1)) / total52_ret;
    double p2_52_ret = (52 * 3 * (52 - 1)) / total52_ret;
    double p3_52_ret = (52 * (52 - 1) * (52 - 2) * (52 - 3)) / total52_ret;
    double p4_52_ret = 52 / total52_ret;
    cout << "  Теоретические:\n";
    cout << "    1) три одного, одна другого: " << p1_52_ret << "\n";
    cout << "    2) две одного, две другого: " << p2_52_ret << "\n";
    cout << "    3) все разного значения: " << p3_52_ret << "\n";
    cout << "    4) все одного значения: " << p4_52_ret << "\n";
    
    // Эмпирическая проверка для колоды 36 карт
    int experiments = 1000000;
    
    cout << "\n  Эмпирические (36 карт, 1e6 экспериментов):\n";
    
    // Без возвращения
    int count1_36_no = 0, count2_36_no = 0, count3_36_no = 0, count4_36_no = 0;
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<> dis36(1, 36);
    
    for (int exp = 0; exp < experiments; exp++) {
        vector<int> cards;
        for (int j = 0; j < 4; j++) {
            int c;
            do {
                c = dis36(gen);
            } while (find(cards.begin(), cards.end(), c) != cards.end());
            cards.push_back(c);
        }
        
        vector<int> values(4);
        for (int j = 0; j < 4; j++) {
            values[j] = (cards[j] - 1) / 4;
        }
        sort(values.begin(), values.end());
        
        if (values[0] == values[1] && values[1] == values[2] && values[2] != values[3]) count1_36_no++;
        else if (values[0] == values[1] && values[2] == values[3] && values[1] != values[2]) count2_36_no++;
        else if (values[0] != values[1] && values[1] != values[2] && values[2] != values[3]) count3_36_no++;
        else if (values[0] == values[1] && values[1] == values[2] && values[2] == values[3]) count4_36_no++;
    }
    
    cout << "  Без возвращения:\n";
    cout << "    1) три одного, одна другого: " << (double)count1_36_no/experiments << "\n";
    cout << "    2) две одного, две другого: " << (double)count2_36_no/experiments << "\n";
    cout << "    3) все разного значения: " << (double)count3_36_no/experiments << "\n";
    cout << "    4) все одного значения: " << (double)count4_36_no/experiments << "\n";
    
    // С возвращением
    int count1_36_ret = 0, count2_36_ret = 0, count3_36_ret = 0, count4_36_ret = 0;
    
    for (int exp = 0; exp < experiments; exp++) {
        vector<int> cards(4);
        for (int j = 0; j < 4; j++) {
            cards[j] = dis36(gen);
        }
        
        vector<int> values(4);
        for (int j = 0; j < 4; j++) {
            values[j] = (cards[j] - 1) / 4;
        }
        sort(values.begin(), values.end());
        
        if (values[0] == values[1] && values[1] == values[2] && values[2] != values[3]) count1_36_ret++;
        else if (values[0] == values[1] && values[2] == values[3] && values[1] != values[2]) count2_36_ret++;
        else if (values[0] != values[1] && values[1] != values[2] && values[2] != values[3]) count3_36_ret++;
        else if (values[0] == values[1] && values[1] == values[2] && values[2] == values[3]) count4_36_ret++;
    }
    
    cout << "  С возвращением:\n";
    cout << "    1) три одного, одна другого: " << (double)count1_36_ret/experiments << "\n";
    cout << "    2) две одного, две другого: " << (double)count2_36_ret/experiments << "\n";
    cout << "    3) все разного значения: " << (double)count3_36_ret/experiments << "\n";
    cout << "    4) все одного значения: " << (double)count4_36_ret/experiments << "\n\n";
}

int main() {
    cout << fixed << setprecision(6);
    
    //task57();
    //task58();
    //task59();
    //task61();
    //task62();
    //task63();
    //task65();
    //task67();
    //task68();
    //task69();
    //task70();
    //task1();
    //task2();
    
    return 0;
}