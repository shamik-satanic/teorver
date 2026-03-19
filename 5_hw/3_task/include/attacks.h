#pragma once
#include "ciphers.h"
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>

namespace cipher {

// ── Английские частоты букв (ETAOIN...) ──────────────────────────────────────
static const double EN_FREQ[26] = {
    0.08167,0.01492,0.02782,0.04253,0.12702,0.02228,0.02015,0.06094,
    0.06966,0.00153,0.00772,0.04025,0.02406,0.06749,0.07507,0.01929,
    0.00095,0.05987,0.06327,0.09056,0.02758,0.00978,0.02360,0.00150,
    0.01974,0.00074
};

// ── Индекс совпадений (Index of Coincidence) ─────────────────────────────────
inline double index_of_coincidence(const std::string& text) {
    if (text.size() < 2) return 0.0;
    int freq[26] = {};
    for (char c : text) if (c >= 'A' && c <= 'Z') freq[c-'A']++;
    double n = 0;
    for (int f : freq) n += f;
    if (n < 2) return 0.0;
    double ic = 0;
    for (int f : freq) ic += f * (f - 1);
    return ic / (n * (n - 1));
}

// ── Оценка длины ключа Виженера через IC ─────────────────────────────────────
inline int estimate_key_length(const std::string& ctext, int max_len = 20) {
    int best_len = 1;
    double best_ic = 0.0;
    for (int kl = 1; kl <= max_len; ++kl) {
        double avg_ic = 0.0;
        for (int r = 0; r < kl; ++r) {
            std::string sub;
            for (int i = r; i < (int)ctext.size(); i += kl) sub += ctext[i];
            avg_ic += index_of_coincidence(sub);
        }
        avg_ic /= kl;
        if (avg_ic > best_ic) { best_ic = avg_ic; best_len = kl; }
    }
    return best_len;
}

// ── Частотный анализ одного сдвига (ключевой символ) ─────────────────────────
// Возвращает наиболее вероятный сдвиг для потока букв
inline int guess_shift(const std::string& sub) {
    int freq[26] = {};
    for (char c : sub) if (c >= 'A' && c <= 'Z') freq[c-'A']++;
    double n = sub.size();

    int best_shift = 0;
    double best_score = -1e18;
    for (int s = 0; s < 26; ++s) {
        double score = 0;
        for (int c = 0; c < 26; ++c)
            score += freq[c] / n * EN_FREQ[(c - s + 26) % 26];
        if (score > best_score) { best_score = score; best_shift = s; }
    }
    return best_shift;
}

// ── Атака на шифр Виженера ────────────────────────────────────────────────────
struct VigenereAttackResult {
    int         key_len;
    std::string key;
    std::string plaintext;
    double      ic_avg;
};

inline VigenereAttackResult attack_vigenere(const std::string& ctext_raw,
                                             int forced_len = 0) {
    VigenereCipher vc;
    std::string ct;
    for (char c : ctext_raw)
        if (c >= 'A' && c <= 'Z') ct += c;
        else if (c >= 'a' && c <= 'z') ct += (char)(c - 32);

    int kl = (forced_len > 0) ? forced_len : estimate_key_length(ct);

    std::string key;
    double ic_sum = 0;
    for (int r = 0; r < kl; ++r) {
        std::string sub;
        for (int i = r; i < (int)ct.size(); i += kl) sub += ct[i];
        ic_sum += index_of_coincidence(sub);
        key += (char)('A' + guess_shift(sub));
    }

    std::string pt = vc.decrypt(ct, key);
    return {kl, key, pt, ic_sum / kl};
}

// ── Атака на шифр Вернама при повторном использовании ключа ──────────────────
// Если два шифротекста зашифрованы одним ключом:
//   c1 XOR c2 = m1 XOR m2  (в алфавите A-Z: (c1-c2+26)%26 = (m1-m2+26)%26)
// Частотный анализ XOR-а позволяет восстановить открытые тексты.

struct VernamReusedKeyResult {
    std::string xor_stream;     // c1 ⊕ c2 (в алфавите)
    std::string guess_m1;
    std::string guess_m2;
    std::string guessed_key;
};

inline VernamReusedKeyResult attack_vernam_reused(const std::string& ct1_raw,
                                                   const std::string& ct2_raw) {
    auto clean = [](const std::string& s) {
        std::string r;
        for (char c : s)
            if (c >= 'A' && c <= 'Z') r += c;
            else if (c >= 'a' && c <= 'z') r += (char)(c-32);
        return r;
    };
    std::string c1 = clean(ct1_raw), c2 = clean(ct2_raw);
    std::size_t n = std::min(c1.size(), c2.size());

    // XOR двух шифротекстов = XOR двух открытых текстов
    std::string xors(n, ' ');
    for (std::size_t i = 0; i < n; ++i)
        xors[i] = (char)('A' + (c1[i] - c2[i] + 26) % 26);

    // Угадываем сдвиг для m1 относительно m2
    int shift = guess_shift(xors);

    // Восстанавливаем ключ из c1 и предполагаемого m1
    // Предположим, что m1 начинается с наиболее частой буквы 'E'
    // (упрощённая эвристика crib dragging)
    std::string key(n, ' ');
    for (std::size_t i = 0; i < n; ++i)
        key[i] = (char)('A' + (c1[i] - 'A' - shift + 26) % 26);

    VernamCipher vc;
    std::string g_m1 = vc.decrypt(c1, key);
    std::string g_m2 = vc.decrypt(c2, key);

    return {xors, g_m1, g_m2, key};
}

// ── Мера расстояния Хи-квадрат (качество угаданного открытого текста) ────────
inline double chi_squared(const std::string& text) {
    int freq[26] = {};
    double n = 0;
    for (char c : text) if (c >= 'A' && c <= 'Z') { freq[c-'A']++; n++; }
    if (n == 0) return 1e9;
    double chi = 0;
    for (int i = 0; i < 26; ++i) {
        double expected = EN_FREQ[i] * n;
        double diff = freq[i] - expected;
        chi += diff * diff / expected;
    }
    return chi;
}

} // namespace cipher