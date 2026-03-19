#include "../include/cipher_model.h"
#include "../include/ciphers.h"
#include "../include/attacks.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <map>

using namespace cipher;

// ─────────────────────────────────────────────────────────────────────────────
void sep(const std::string& t) {
    std::cout << "\n┌─────────────────────────────────────────────────────────┐\n";
    std::cout << "│  " << std::left << std::setw(55) << t << "│\n";
    std::cout << "└─────────────────────────────────────────────────────────┘\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Часть I.  Вероятностная модель шифра на маленьком примере
// ─────────────────────────────────────────────────────────────────────────────
void demo_probability_model() {
    sep("I. Вероятностная модель шифра (Виженер, алфавит A-C)");

    VigenereCipher vc;

    // Пространство открытых текстов M = {AB, BA, AA}
    ProbSpace<std::string> M;
    M.add("AB", 0.5);
    M.add("BA", 0.3);
    M.add("AA", 0.2);

    // Пространство ключей K = {AA, AB, BA} (равновероятны)
    ProbSpace<std::string> K;
    K.add("AA", 1.0/3);
    K.add("AB", 1.0/3);
    K.add("BA", 1.0/3);

    CipherModel model(vc, M, K);

    // Все шифротексты
    auto ct_dist = model.cipher_distribution();

    std::cout << "\n  P(Cipher = c_l):\n";
    for (auto& [cl, p] : ct_dist)
        std::cout << "    c_l = " << cl << "   P = " << std::fixed
                  << std::setprecision(4) << p << "\n";

    std::cout << "\n  P(Cipher = c_l | M = mj):\n";
    for (auto& mj : M.elements) {
        std::cout << "    M = " << mj << ":";
        for (auto& [cl, _] : ct_dist) {
            double p = model.prob_cipher_given_plain(cl, mj);
            if (p > 1e-9)
                std::cout << "  P(C=" << cl << "|M=" << mj
                          << ")=" << std::setprecision(4) << p;
        }
        std::cout << "\n";
    }

    std::cout << "\n  P(M = mj | Cipher = c_l) — апостериорные вероятности:\n";
    for (auto& [cl, pc] : ct_dist) {
        std::cout << "    C = " << cl << "  [P(C)=" << std::setprecision(4) << pc << "]:";
        for (auto& mj : M.elements) {
            double post = model.prob_plain_given_cipher(mj, cl);
            if (post > 1e-9)
                std::cout << "  P(M=" << mj << "|C=" << cl
                          << ")=" << std::setprecision(4) << post;
        }
        std::cout << "\n";
    }

    std::cout << "\n  Совершенная стойкость: "
              << (model.is_perfectly_secret() ? "ДА" : "НЕТ") << "\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Часть II.  Шифр Вернама: совершенная стойкость
// ─────────────────────────────────────────────────────────────────────────────
void demo_vernam_perfect_secrecy() {
    sep("II. Шифр Вернама — совершенная стойкость");

    VernamCipher vr;

    // M = {A, B, C}  (однобуквенные сообщения)
    // При |K|=26 и любом распределении M -> совершенная стойкость
    ProbSpace<std::string> M;
    M.add("A", 0.5);
    M.add("B", 0.3);
    M.add("C", 0.2);

    // K = все 26 ключей длины 1 (равновероятны)
    // Условие совершенной стойкости: |K| >= |M_space|^len = 26^1 = 26
    ProbSpace<std::string> K;
    for (int ki = 0; ki < 26; ++ki)
        K.add(std::string(1,(char)('A'+ki)), 1.0/26);

    CipherModel model(vr, M, K);

    auto ct_dist = model.cipher_distribution();
    std::cout << "\n  P(Cipher = c_l):\n";
    for (auto& [cl, p] : ct_dist)
        std::cout << "    " << cl << "  →  " << std::setprecision(4) << p << "\n";

    std::cout << "\n  P(M=mj|C=cl) vs P(M=mj):\n";
    for (auto& [cl, _] : ct_dist)
        for (auto& mj : M.elements) {
            double post = model.prob_plain_given_cipher(mj, cl);
            double prio = M.prob(mj);
            std::cout << "    P(M=" << mj << "|C=" << cl
                      << ") = " << std::setprecision(6) << post
                      << "   P(M=" << mj << ") = " << prio
                      << "   |Δ| = " << std::abs(post-prio) << "\n";
        }

    std::cout << "\n  Совершенная стойкость: "
              << (model.is_perfectly_secret() ? "ДА ✓" : "НЕТ") << "\n";
    std::cout << "  → Шифротекст не даёт никакой информации об открытом тексте.\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Часть III.  Атака на шифр Виженера (частотный анализ + IC)
// ─────────────────────────────────────────────────────────────────────────────
void demo_vigenere_attack() {
    sep("III. Статистическая атака на шифр Виженера");

    VigenereCipher vc;

    // Достаточно длинный открытый текст (200+ символов) для надёжного IC
    const std::string PLAINTEXT =
        "THEQUICKBROWNFOXJUMPSOVERTHELAZYDOG"
        "CRYPTOGRAPHYISTHEPRACTICEANDSTUDYOFTECHNIQUESFOR"
        "SECURECOMMUNICATIONINTHEPRESENCEOFADVERSARIALBEHA"
        "VIORMOREGENERALLYISABOUTCONSTRUCTINGANDANALYZINGP"
        "ROTOCOLSTHATPREVENTTHIRDPARTIESORADVERSARIESFROMR"
        "EADINGPRIVATEMESSAGESVARIOUSSIDESINFORMATIONSECUR"
        "ITYAREUSEDINTERCHANGEABLYWITHCRYPTOGRAPHY";

    const std::string KEY = "SECRET";

    std::string ct = vc.encrypt(PLAINTEXT, KEY);

    std::cout << "\n  Ключ (неизвестен атакующему): " << KEY << "\n";
    std::cout << "  Длина открытого текста: " << PLAINTEXT.size() << " символов\n";
    std::cout << "  IC шифротекста: "
              << std::setprecision(5) << index_of_coincidence(ct)
              << "  (случайный ≈0.0385, английский ≈0.0655)\n";

    // Оценка длины ключа
    std::cout << "\n  IC по длинам ключа:\n";
    for (int kl = 1; kl <= 10; ++kl) {
        double ic_sum = 0;
        for (int r = 0; r < kl; ++r) {
            std::string sub;
            for (int i = r; i < (int)ct.size(); i += kl) sub += ct[i];
            ic_sum += index_of_coincidence(sub);
        }
        double avg = ic_sum / kl;
        std::cout << "    len=" << kl << "  avg_IC=" << std::setprecision(5) << avg
                  << (std::abs(avg - 0.0655) < 0.008 ? "  ← вероятная длина" : "") << "\n";
    }

    auto res = attack_vigenere(ct);
    std::cout << "\n  Найденная длина ключа : " << res.key_len << "\n";
    std::cout << "  Найденный ключ        : " << res.key << "\n";
    std::cout << "  Ожидаемый ключ        : " << KEY << "\n";
    std::cout << "  Ключ угадан правильно : "
              << (res.key == KEY ? "ДА ✓" : "НЕТ — частичное совпадение") << "\n";
    std::cout << "  Chi² расшифровки      : "
              << std::setprecision(4) << chi_squared(res.plaintext) << "\n";
    std::cout << "\n  Первые 80 символов расшифровки:\n  "
              << res.plaintext.substr(0, 80) << "\n";
    std::cout << "  Первые 80 символов оригинала:\n  "
              << PLAINTEXT.substr(0, 80) << "\n";
}

// ─────────────────────────────────────────────────────────────────────────────
// Часть IV.  Атака на Вернам при повторном использовании ключа (many-time pad)
// ─────────────────────────────────────────────────────────────────────────────
void demo_vernam_reused_key_attack() {
    sep("IV. Атака на Вернам при повторном ключе (many-time pad)");

    VernamCipher vr;

    const std::string M1 = "ATTACKATDAWN";
    const std::string M2 = "RETREATATDUSK";
    const std::string KEY = "SECRETKEYABC"; // длина = max(|m1|,|m2|)

    // Обрезаем до минимальной длины
    int n = std::min({M1.size(), M2.size(), KEY.size()});
    std::string m1 = M1.substr(0, n);
    std::string m2 = M2.substr(0, n);
    std::string k  = KEY.substr(0, n);

    std::string c1 = vr.encrypt(m1, k);
    std::string c2 = vr.encrypt(m2, k);

    std::cout << "\n  M1 = " << m1 << "\n  M2 = " << m2 << "\n";
    std::cout << "  Ключ (один и тот же!): " << k << "\n";
    std::cout << "  C1 = " << c1 << "\n  C2 = " << c2 << "\n";

    // XOR двух шифротекстов = XOR двух открытых текстов
    std::string xors(n, ' ');
    for (int i = 0; i < n; ++i)
        xors[i] = (char)('A' + (c1[i] - c2[i] + 26) % 26);
    std::cout << "\n  C1 ⊕ C2 = M1 ⊕ M2 = " << xors << "\n";
    std::cout << "  (зная одно сообщение, мгновенно находим второе)\n";

    // Демонстрация: знаем m1, находим m2
    std::string recovered_k(n, ' ');
    for (int i = 0; i < n; ++i)
        recovered_k[i] = (char)('A' + (c1[i] - m1[i] + 26) % 26);
    std::string recovered_m2 = vr.decrypt(c2, recovered_k);

    std::cout << "\n  Если знаем M1 = " << m1 << ":\n";
    std::cout << "    → Восстановленный ключ: " << recovered_k << "\n";
    std::cout << "    → Восстановленный M2:   " << recovered_m2 << "\n";
    std::cout << "    → Истинный M2:           " << m2 << "\n";
    std::cout << "    → Совпадение: " << (recovered_m2 == m2 ? "ДА ✓" : "НЕТ") << "\n";

    std::cout << "\n  ВЫВОД: Шифр Вернама АБСОЛЮТНО СТОЕК только при\n";
    std::cout << "         однократном использовании ключа (OTP).\n";
    std::cout << "         Повторное использование полностью разрушает стойкость.\n";
}

// ─────────────────────────────────────────────────────────────────────────────
int main() {


    demo_probability_model();
    demo_vernam_perfect_secrecy();
    demo_vigenere_attack();
    demo_vernam_reused_key_attack();

    return 0;
}