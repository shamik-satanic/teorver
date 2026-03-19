#include <iostream>
#include <iomanip>
#include <random>
#include <string>
#include <cmath>

static constexpr long long N = 10'000'000;
static std::mt19937_64 rng(42);

void sep(const std::string& t) {
    std::cout << "\n┌─────────────────────────────────────────────────────────┐\n";
    std::cout << "│  " << std::left << std::setw(55) << t << "│\n";
    std::cout << "└─────────────────────────────────────────────────────────┘\n";
}

void show(const std::string& label, double a, double e) {
    std::cout << "  " << label << "\n"
              << "    Аналитически : " << a << "\n"
              << "    Эмпирически  : " << e << "\n"
              << "    |Δ|          : " << std::abs(a-e) << "\n";
}

void task92() {
    sep("92. Пять винтовок (3 с прицелом p=0.95, 2 без p=0.70)");
    double analyt = 3.0/5*0.95 + 2.0/5*0.70;
    std::uniform_int_distribution<int> rifle(1,5);
    std::uniform_real_distribution<double> u(0,1);
    long long hits=0;
    for(long long i=0;i<N;++i){
        double p=(rifle(rng)<=3)?0.95:0.70;
        if(u(rng)<p)++hits;
    }
    show("P(мишень поражена)", analyt, (double)hits/N);
}

void task93() {
    sep("93. Три завода: 12+20+18 деталей, p=0.9/0.6/0.9");
    double analyt = 12.0/50*0.9 + 20.0/50*0.6 + 18.0/50*0.9;
    std::uniform_int_distribution<int> det(1,50);
    std::uniform_real_distribution<double> u(0,1);
    long long good=0;
    for(long long i=0;i<N;++i){
        int d=det(rng);
        double p=(d<=12)?0.9:(d<=32)?0.6:0.9;
        if(u(rng)<p)++good;
    }
    show("P(деталь отличного качества)", analyt, (double)good/N);
}

void task95() {
    sep("95. Три урны: 6 чёрных + 4 белых в каждой");
    // Аналитика: дерево событий
    double pw1=4.0/10, pb1=6.0/10;
    double pw2=pw1*5.0/11 + pb1*4.0/11;
    double pb2=1-pw2;
    double analyt=pw2*5.0/11 + pb2*4.0/11;

    std::uniform_real_distribution<double> u(0,1);
    long long w3=0;
    for(long long i=0;i<N;++i){
        bool bw1=(u(rng)<4.0/10);
        bool bw2=(u(rng)<(bw1?5.0/11:4.0/11));
        if(u(rng)<(bw2?5.0/11:4.0/11))++w3;
    }
    show("P(белый из третьей урны)", analyt, (double)w3/N);
}

void task98() {
    sep("98. 10 винтовок (4 с прицелом). Попал — Байес");
    double po=4.0/10, pn=6.0/10, pho=0.95, phn=0.80;
    double ph=po*pho+pn*phn;
    double p_opt=po*pho/ph, p_no=pn*phn/ph;
    std::cout<<std::fixed<<std::setprecision(6);
    std::cout<<"  Аналитически:\n"
             <<"    P(с прицелом | попал)  = "<<p_opt<<"\n"
             <<"    P(без прицела | попал) = "<<p_no<<"\n"
             <<"  Вывод: вероятнее "<<(p_opt>p_no?"С прицелом":"БЕЗ прицела")<<"\n";
    std::uniform_int_distribution<int> r(1,10);
    std::uniform_real_distribution<double> u(0,1);
    long long ho=0,hn=0;
    for(long long i=0;i<N;++i){
        bool opt=(r(rng)<=4);
        if(u(rng)<(opt?0.95:0.80)){if(opt)++ho;else++hn;}
    }
    long long th=ho+hn;
    std::cout<<"  Эмпирически:\n"
             <<"    P(с прицелом | попал)  = "<<(double)ho/th<<"\n"
             <<"    P(без прицела | попал) = "<<(double)hn/th<<"\n";
}

void task100() {
    sep("100. Две перфораторщицы p=0.05/0.10 — Байес (ошибка)");
    double pe=0.5*0.05+0.5*0.10;
    double p1=0.5*0.05/pe, p2=0.5*0.10/pe;
    std::cout<<std::fixed<<std::setprecision(6);
    std::cout<<"  P(ошибка) = "<<pe<<"\n"
             <<"  Аналитически:\n"
             <<"    P(ошиблась 1-я | ошибка) = "<<p1<<"\n"
             <<"    P(ошиблась 2-я | ошибка) = "<<p2<<"\n";
    std::uniform_real_distribution<double> u(0,1);
    long long e1=0,e2=0;
    for(long long i=0;i<N;++i){
        bool first=(u(rng)<0.5);
        if(u(rng)<(first?0.05:0.10)){if(first)++e1;else++e2;}
    }
    long long te=e1+e2;
    std::cout<<"  Эмпирически:\n"
             <<"    P(ошиблась 1-я | ошибка) = "<<(double)e1/te<<"\n"
             <<"    P(ошиблась 2-я | ошибка) = "<<(double)e2/te<<"\n";
}

void task102() {
    sep("102. Два товароведа P(1)=0.55/P(2)=0.45, ps=0.90/0.98");
    double ps=0.55*0.90+0.45*0.98;
    double p1=0.55*0.90/ps, p2=0.45*0.98/ps;
    std::cout<<std::fixed<<std::setprecision(6);
    std::cout<<"  P(признано стандартным) = "<<ps<<"\n"
             <<"  Аналитически:\n"
             <<"    P(проверял 1-й | стандарт) = "<<p1<<"\n"
             <<"    P(проверял 2-й | стандарт) = "<<p2<<"\n";
    std::uniform_real_distribution<double> u(0,1);
    long long s1=0,s2=0;
    for(long long i=0;i<N;++i){
        bool first=(u(rng)<0.55);
        if(u(rng)<(first?0.90:0.98)){if(first)++s1;else++s2;}
    }
    long long ts=s1+s2;
    std::cout<<"  Эмпирически:\n"
             <<"    P(проверял 1-й | стандарт) = "<<(double)s1/ts<<"\n"
             <<"    P(проверял 2-й | стандарт) = "<<(double)s2/ts<<"\n";
}

void task104() {
    sep("104. Три гипотезы B1,B2,B3 — найти P_A(B3)");
    double pa_b3=1.0-0.6-0.3;
    std::cout<<std::fixed<<std::setprecision(6);
    std::cout<<"  P_A(B1)+P_A(B2)+P_A(B3)=1  =>  P_A(B3) = "<<pa_b3<<"\n"
             <<"  (Чистая алгебра, симуляция не требуется)\n";
}

void task107() {
    sep("107. Три стрелка p=0.6/0.5/0.4 — залп, ровно 2 попали");
    double p1=0.6,p2=0.5,p3=0.4,q1=0.4,q2=0.5,q3=0.6;
    double p2h = p1*p2*q3 + p1*q2*p3 + q1*p2*p3;
    double p3_and_2 = p1*q2*p3 + q1*p2*p3;
    double analyt = p3_and_2/p2h;
    std::cout<<std::fixed<<std::setprecision(6);
    std::cout<<"  P(ровно 2 попали)             = "<<p2h<<"\n"
             <<"  P(3-й попал И ровно 2 попали) = "<<p3_and_2<<"\n";

    std::uniform_real_distribution<double> u(0,1);
    long long c2=0,c3=0;
    for(long long i=0;i<N;++i){
        bool h1=(u(rng)<p1),h2=(u(rng)<p2),h3=(u(rng)<p3);
        if(h1+h2+h3==2){++c2;if(h3)++c3;}
    }
    show("P(3-й попал | ровно 2 из 3)", analyt, (double)c3/c2);
}

int main(){
    std::cout<<std::fixed<<std::setprecision(6);
    std::cout<<"╔══════════════════════════════════════════════════════════╗\n"
             <<"║  Теория вероятностей — задачи 92,93,95,98,100,102,107   ║\n"
             <<"║  N = 10 000 000 симуляций                                ║\n"
             <<"╚══════════════════════════════════════════════════════════╝\n";
    task92(); task93(); task95(); task98();
    task100(); task102(); task104(); task107();

    return 0;
}