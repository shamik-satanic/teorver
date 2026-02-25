#include <iostream>
#include <vector>
#include <random>
#include <algorithm>
#include <numeric>
#include <iomanip>

using namespace std;

random_device rd;
mt19937 gen(rd());
uniform_real_distribution<> dist01(0.0, 1.0);
uniform_real_distribution<> dist_neg1_1(-1.0, 1.0);

// -------------------- Задача 1: куб 10×10×10, окрашенные грани --------------------
void task6(int trials = 1000000) {
    cout << "\n=== Задача 1: куб 10x10x10, окрашенные грани ===\n";
    
    // Аналитика
    int total = 1000;
    int three = 8;          
    int two = 8 * 12 - 24;   
    
    two = 96;
    int one = 8 * 8 * 6;    
    int zero = 8*8*8;       
    
    // Эмпирика
    uniform_int_distribution<> dist(0, total - 1);
    int count1 = 0, count2 = 0, count3 = 0;
    
    for (int t = 0; t < trials; ++t) {
        int idx = dist(gen);
        // координаты в кубе 10×10×10 (0..9)
        int x = idx % 10;
        int y = (idx / 10) % 10;
        int z = idx / 100;
        
        int painted = 0;
        if (x == 0 || x == 9) painted++;
        if (y == 0 || y == 9) painted++;
        if (z == 0 || z == 9) painted++;
        
        if (painted == 1) count1++;
        else if (painted == 2) count2++;
        else if (painted == 3) count3++;
    }
    
    cout << "Теоретически:\n";
    cout << "  1 грань: " << one << "/1000 = " << (double)one/1000 << "\n";
    cout << "  2 грани: " << two << "/1000 = " << (double)two/1000 << "\n";
    cout << "  3 грани: " << three << "/1000 = " << (double)three/1000 << "\n";
    cout << "Эмпирически (trials=" << trials << "):\n";
    cout << "  1 грань: " << (double)count1/trials << "\n";
    cout << "  2 грани: " << (double)count2/trials << "\n";
    cout << "  3 грани: " << (double)count3/trials << "\n";
}

// -------------------- Задача 2: кубики с номерами, возрастающий порядок --------------------
void task8(int trials = 1000000) {
    cout << "\n=== Задача 2: номера в возрастающем порядке (6 кубиков) ===\n";
    
    // Аналитика
    double theory = 1.0 / 720.0; // 6! = 720
    
    // Эмпирика
    vector<int> cubes(6);
    iota(cubes.begin(), cubes.end(), 1);
    int success = 0;
    
    for (int t = 0; t < trials; ++t) {
        shuffle(cubes.begin(), cubes.end(), gen);
        if (is_sorted(cubes.begin(), cubes.end())) success++;
    }
    
    cout << "Теоретически: " << theory << "\n";
    cout << "Эмпирически: " << (double)success/trials << "\n";
}

// -------------------- Задача 3: 100 фото, 1 нужная, выбрать 10 --------------------
void task13(int trials = 1000000) {
    cout << "\n=== Задача 3: 100 фото, 1 нужная, выбрать 10 ===\n";
    
    // Аналитика: P = 1 - C(99,10)/C(100,10) = 10/100 = 0.1
    double theory = 0.1;
    
    // Эмпирика
    vector<int> photos(100);
    iota(photos.begin(), photos.end(), 0); // 0..99, пусть нужная = 0
    uniform_int_distribution<> dist(0, 99);
    int success = 0;
    
    for (int t = 0; t < trials; ++t) {
        vector<bool> selected(100, false);
        for (int i = 0; i < 10; ++i) {
            int idx;
            do {
                idx = dist(gen);
            } while (selected[idx]);
            selected[idx] = true;
        }
        if (selected[0]) success++; // 0 - нужная
    }
    
    cout << "Теоретически: " << theory << "\n";
    cout << "Эмпирически: " << (double)success/trials << "\n";
}

// -------------------- Задача 4: 5 элементов, 2 изношены, выбрать 2, оба неизношенные --------------------
void task15(int trials = 1000000) {
    cout << "\n=== Задача 4: 5 элементов (2 изношены), выбрать 2, оба неизношенные ===\n";
    
    // Аналитика: C(3,2)/C(5,2) = 3/10 = 0.3
    double theory = 3.0 / 10.0;
    
    // Эмпирика
    vector<int> elements = {0, 0, 1, 1, 1}; // 0 - изношен, 1 - неизношен
    int success = 0;
    
    for (int t = 0; t < trials; ++t) {
        shuffle(elements.begin(), elements.end(), gen);
        if (elements[0] == 1 && elements[1] == 1) success++;
    }
    
    cout << "Теоретически: " << theory << "\n";
    cout << "Эмпирически: " << (double)success/trials << "\n";
}

// -------------------- Задача 5: 15 кинескопов (10 Львовских), выбрать 5, ровно 3 Львовских --------------------
void task19(int trials = 1000000) {
    cout << "\n=== Задача 5: 15 кинескопов (10 Львовских), выбрать 5, ровно 3 Львовских ===\n";
    
    // Аналитика: C(10,3)*C(5,2)/C(15,5)
    double theory = (120.0 * 10.0) / 3003.0; // 1200/3003 ≈ 0.3996
    
    // Эмпирика
    vector<int> kines = {}; 
    for (int i = 0; i < 10; ++i) kines.push_back(1);
    for (int i = 0; i < 5; ++i) kines.push_back(0);
    int success = 0;
    
    for (int t = 0; t < trials; ++t) {
        shuffle(kines.begin(), kines.end(), gen);
        int львовских = 0;
        for (int i = 0; i < 5; ++i) if (kines[i] == 1) львовских++;
        if (львовских == 3) success++;
    }
    
    cout << "Теоретически: " << theory << "\n";
    cout << "Эмпирически: " << (double)success/trials << "\n";
}

// -------------------- Задача 6: 12 студентов (8 отличников), выбрать 9, ровно 5 отличников --------------------
void task20(int trials = 1000000) {
    cout << "\n=== Задача 6: 12 студентов (8 отличников), выбрать 9, ровно 5 отличников ===\n";
    
    // Аналитика: C(8,5)*C(4,4)/C(12,9) = C(8,5)*1 / C(12,9) ; C(12,9)=C(12,3)=220, C(8,5)=56
    double theory = 56.0 / 220.0; // ≈0.2545
    
    // Эмпирика
    vector<int> students = {}; 
    for (int i = 0; i < 8; ++i) students.push_back(1);
    for (int i = 0; i < 4; ++i) students.push_back(0);
    int success = 0;
    
    for (int t = 0; t < trials; ++t) {
        shuffle(students.begin(), students.end(), gen);
        int otl = 0;
        for (int i = 0; i < 9; ++i) if (students[i] == 1) otl++;
        if (otl == 5) success++;
    }
    
    cout << "Теоретически: " << theory << "\n";
    cout << "Эмпирически: " << (double)success/trials << "\n";
}

// -------------------- Задача 7: 5 изделий (3 окрашены), выбрать 2 --------------------
void task21(int trials = 1000000) {
    cout << "\n=== Задача 7: 5 изделий (3 окрашены), выбрать 2 ===\n";
    
    // Аналитика:
    // а) одно окрашено: C(3,1)*C(2,1)/C(5,2) = 3*2/10 = 6/10 = 0.6
    // б) два окрашены: C(3,2)*C(2,0)/10 = 3/10 = 0.3
    // в) хотя бы одно: 1 - C(2,2)/10 = 1 - 1/10 = 0.9
    
    // Эмпирика
    vector<int> items = {}; 
    for (int i = 0; i < 3; ++i) items.push_back(1);
    for (int i = 0; i < 2; ++i) items.push_back(0);
    
    int count1 = 0, count2 = 0, countAtLeast1 = 0;
    
    for (int t = 0; t < trials; ++t) {
        shuffle(items.begin(), items.end(), gen);
        int okr = (items[0] == 1) + (items[1] == 1);
        if (okr == 1) count1++;
        if (okr == 2) count2++;
        if (okr >= 1) countAtLeast1++;
    }
    
    cout << "Теоретически:\n";
    cout << "  а) одно окрашенное: 0.6\n";
    cout << "  б) два окрашенных: 0.3\n";
    cout << "  в) хотя бы одно: 0.9\n";
    cout << "Эмпирически:\n";
    cout << "  а) одно окрашенное: " << (double)count1/trials << "\n";
    cout << "  б) два окрашенных: " << (double)count2/trials << "\n";
    cout << "  в) хотя бы одно: " << (double)countAtLeast1/trials << "\n";
}

// -------------------- Задача 8: замок, 4 диска по 5 цифр --------------------
void task22(int trials = 1000000) {
    cout << "\n=== Задача 8: замок, 4 диска по 5 цифр ===\n";
    
    double theory = 1.0 / 625.0;
    
    // Эмпирика
    uniform_int_distribution<> dist(0, 4);
    int code[4] = {2, 3, 1, 4}; 
    int success = 0;
    
    for (int t = 0; t < trials; ++t) {
        bool match = true;
        for (int i = 0; i < 4; ++i) {
            if (dist(gen) != code[i]) {
                match = false;
                break;
            }
        }
        if (match) success++;
    }
    
    cout << "Теоретически: " << theory << "\n";
    cout << "Эмпирически: " << (double)success/trials << "\n";
}

// -------------------- Задача 26: отрезок 20 см, меньший 10 см --------------------
void task26(int trials = 1000000) {
    cout << "\n=== Задача 26: отрезок 20 см, внутри отрезок 10 см ===\n";
    
    // Аналитика: длина меньшего / длина большего = 10/20 = 0.5
    double theory = 0.5;
    
    // Эмпирика: большой отрезок [0,20], меньший расположим в центре [5,15] для определённости
    uniform_real_distribution<> dist(0.0, 20.0);
    int success = 0;
    
    for (int t = 0; t < trials; ++t) {
        double x = dist(gen);
        if (x >= 5.0 && x <= 15.0) success++;
    }
    
    cout << "Теоретически: " << theory << "\n";
    cout << "Эмпирически: " << (double)success/trials << "\n";
}

// -------------------- Задача 27: отрезок OA длины L, точка B, меньший из OB и BA > L/3 --------------------
void task27(int trials = 1000000) {
    cout << "\n=== Задача 27: отрезок длины L, точка B, меньший отрезок > L/3 ===\n";
    
    double L = 1.0; // для простоты
    // Аналитика: x из [0, L], меньший = min(x, L-x) > L/3
    // Условие: x > L/3 и L-x > L/3  =>  x in (L/3, 2L/3)
    // Длина интервала = L/3, вероятность = (L/3)/L = 1/3
    double theory = 1.0/3.0;
    
    uniform_real_distribution<> dist(0.0, L);
    int success = 0;
    
    for (int t = 0; t < trials; ++t) {
        double x = dist(gen);
        double menor = min(x, L - x);
        if (menor > L/3.0) success++;
    }
    
    cout << "Теоретически: " << theory << "\n";
    cout << "Эмпирически: " << (double)success/trials << "\n";
}

// -------------------- Задача 28: круг радиуса R, внутри круг радиуса r --------------------
void task28(int trials = 1000000) {
    cout << "\n=== Задача 28: круг R, внутри круг r ===\n";
    
    double R = 10.0, r = 3.0; // пример
    // Аналитика: площадь малого / площадь большого = (π r^2) / (π R^2) = (r/R)^2
    double theory = (r*r) / (R*R);
    
    // Эмпирика: бросаем точки в квадрат [-R, R]x[-R, R], проверяем попадание в большой круг и малый
    uniform_real_distribution<> dist(-R, R);
    int success = 0, total_in_big = 0;
    
    for (int t = 0; t < trials; ++t) {
        double x = dist(gen);
        double y = dist(gen);
        double dist_to_center = sqrt(x*x + y*y);
        if (dist_to_center <= R) { // попал в большой круг
            total_in_big++;
            if (dist_to_center <= r) success++; // попал и в малый
        }
    }
    
    // Условная вероятность P(в малый | в большой) ≈ (число в оба)/(число в большой)
    double empirical = (total_in_big > 0) ? (double)success / total_in_big : 0.0;
    
    cout << "Теоретически: " << theory << "\n";
    cout << "Эмпирически: " << empirical << "\n";
    cout << "Всего бросков: " << trials << ", попаданий в большой круг: " << total_in_big << "\n";
}

// -------------------- Задача 29: параллельные прямые через 2a, монета радиуса r --------------------
void task29(int trials = 1000000) {
    cout << "\n=== Задача 29: параллельные прямые через 2a, монета радиуса r ===\n";
    
    double a = 1.0, r = 0.3; // r < a
    // Аналитика: монета не пересекает прямые, если её центр на расстоянии > r от ближайшей прямой
    // Прямые на расстоянии 2a, значит полоса между прямыми шириной 2a
    // Центр может быть в интервале [r, 2a - r] по вертикали — длина 2a - 2r
    // Вероятность = (2a - 2r) / (2a) = 1 - r/a
    double theory = 1.0 - r/a;
    
    // Эмпирика: бросаем центр монеты в полосу [0, 2a] по y
    uniform_real_distribution<> dist(0.0, 2*a);
    int success = 0;
    
    for (int t = 0; t < trials; ++t) {
        double y = dist(gen);
        // Проверяем расстояние до ближайшей прямой (нижняя y=0, верхняя y=2a)
        double dist_to_bottom = y;
        double dist_to_top = 2*a - y;
        if (dist_to_bottom >= r && dist_to_top >= r) success++;
    }
    
    cout << "Теоретически: " << theory << "\n";
    cout << "Эмпирически: " << (double)success/trials << "\n";
}

// -------------------- Задача 30: сетка квадратов со стороной a, монета радиуса r < a/2 --------------------
void task30(int trials = 1000000) {
    cout << "\n=== Задача 30: сетка квадратов a, монета r < a/2 ===\n";
    
    double a = 1.0, r = 0.4; // r < a/2 = 0.5
    // Аналитика: монета не пересекает стороны, если её центр в квадрате [r, a-r]x[r, a-r]
    // Площадь благоприятной области = (a - 2r)^2
    // Вероятность = (a - 2r)^2 / a^2
    double theory = (a - 2*r)*(a - 2*r) / (a*a);
    
    // Эмпирика: бросаем центр в квадрат [0, a]x[0, a]
    uniform_real_distribution<> dist(0.0, a);
    int success = 0;
    
    for (int t = 0; t < trials; ++t) {
        double x = dist(gen);
        double y = dist(gen);
        if (x >= r && x <= a - r && y >= r && y <= a - r) success++;
    }
    
    cout << "Теоретически: " << theory << "\n";
    cout << "Эмпирически: " << (double)success/trials << "\n";
}

// -------------------- Задача 32: кольцо между окружностями радиусов 5 и 10 --------------------
void task32(int trials = 1000000) {
    cout << "\n=== Задача 32: кольцо между окружностями R1=5, R2=10 ===\n";
    
    double R1 = 5.0, R2 = 10.0;
    // Аналитика: площадь кольца / площадь большого круга = (πR2^2 - πR1^2) / (πR2^2) = 1 - (R1/R2)^2
    double theory = 1.0 - (R1*R1)/(R2*R2); // = 1 - 25/100 = 0.75
    
    // Эмпирика: бросаем точки в квадрат [-R2, R2]x[-R2, R2]
    uniform_real_distribution<> dist(-R2, R2);
    int success = 0, total_in_big = 0;
    
    for (int t = 0; t < trials; ++t) {
        double x = dist(gen);
        double y = dist(gen);
        double dist_to_center = sqrt(x*x + y*y);
        if (dist_to_center <= R2) { // попал в большой круг
            total_in_big++;
            if (dist_to_center >= R1) success++; // попал в кольцо
        }
    }
    
    double empirical = (total_in_big > 0) ? (double)success / total_in_big : 0.0;
    
    cout << "Теоретически: " << theory << "\n";
    cout << "Эмпирически: " << empirical << "\n";
    cout << "Всего бросков: " << trials << ", попаданий в большой круг: " << total_in_big << "\n";
}



void task45(int trials = 1000000) {
    cout << "\n=== Задача: x,y in [0,1], P(x+y ≤ 1 и xy ≥ 0.09) ===\n";
    
    int success = 0;
    
    for (int t = 0; t < trials; ++t) {
        double x = dist01(gen);
        double y = dist01(gen);
        
        if (x + y <= 1.0 && x * y >= 0.09) {
            success++;
        }
    }
    
    double empirical = (double)success / trials;
    
    cout << "Эмпирическая вероятность: " << empirical << "\n";
    cout << "Число испытаний: " << trials << "\n";
    cout << "Число успехов: " << success << "\n";
}

int main() {
    cout << fixed << setprecision(6);
    
    int trials = 1000000; 
    
    //task6(trials);
    //task8(trials);
    //task13(trials);
    //task15(trials);
    //task19(trials);
    //task20(trials);
    //task21(trials);
    //task22(trials);
    //task26(trials);
    //task27(trials);
    //task28(trials);
    //task29(trials);
    //task30(trials);
    //task32(trials);
    //task45(trials);

    
    return 0;
}