#pragma once
#include "cipher_model.h"
#include <stdexcept>
#include <algorithm>
#include <cctype>

namespace cipher {

// ── Вспомогательные функции ───────────────────────────────────────────────────

static std::string to_upper_alpha(const std::string& s) {
    std::string r;
    for (char c : s)
        if (std::isalpha((unsigned char)c))
            r += (char)std::toupper((unsigned char)c);
    return r;
}

// ── Шифр Виженера ─────────────────────────────────────────────────────────────
// E_{key}(m)[i] = (m[i] + key[i % len(key)]) mod 26
// D_{key}(c)[i] = (c[i] - key[i % len(key)] + 26) mod 26

class VigenereCipher final : public ICipher {
public:
    std::string encrypt(const std::string& plaintext,
                        const std::string& key) const override {
        auto pt = to_upper_alpha(plaintext);
        auto k  = to_upper_alpha(key);
        if (k.empty()) throw std::invalid_argument("Vigenere: empty key");
        std::string ct;
        ct.reserve(pt.size());
        for (std::size_t i = 0; i < pt.size(); ++i)
            ct += (char)('A' + (pt[i] - 'A' + k[i % k.size()] - 'A') % 26);
        return ct;
    }

    std::string decrypt(const std::string& ciphertext,
                        const std::string& key) const override {
        auto ct = to_upper_alpha(ciphertext);
        auto k  = to_upper_alpha(key);
        if (k.empty()) throw std::invalid_argument("Vigenere: empty key");
        std::string pt;
        pt.reserve(ct.size());
        for (std::size_t i = 0; i < ct.size(); ++i)
            pt += (char)('A' + (ct[i] - 'A' - (k[i % k.size()] - 'A') + 26) % 26);
        return pt;
    }

    std::string name() const override { return "Vigenere"; }
};

// ── Шифр Вернама (One-Time Pad) ───────────────────────────────────────────────
// Ключ должен быть той же длины, что и открытый текст.
// E_{key}(m)[i] = m[i] XOR key[i]   (побайтово)
// Для текстовых демонстраций работаем в алфавите A-Z:
// E_{key}(m)[i] = (m[i] + key[i]) mod 26

class VernamCipher final : public ICipher {
public:
    std::string encrypt(const std::string& plaintext,
                        const std::string& key) const override {
        auto pt = to_upper_alpha(plaintext);
        auto k  = to_upper_alpha(key);
        if (k.size() != pt.size())
            throw std::invalid_argument(
                "Vernam: key length (" + std::to_string(k.size()) +
                ") != plaintext length (" + std::to_string(pt.size()) + ")");
        std::string ct;
        ct.reserve(pt.size());
        for (std::size_t i = 0; i < pt.size(); ++i)
            ct += (char)('A' + (pt[i] - 'A' + k[i] - 'A') % 26);
        return ct;
    }

    std::string decrypt(const std::string& ciphertext,
                        const std::string& key) const override {
        auto ct = to_upper_alpha(ciphertext);
        auto k  = to_upper_alpha(key);
        if (k.size() != ct.size())
            throw std::invalid_argument("Vernam: key/ciphertext length mismatch");
        std::string pt;
        pt.reserve(ct.size());
        for (std::size_t i = 0; i < ct.size(); ++i)
            pt += (char)('A' + (ct[i] - 'A' - (k[i] - 'A') + 26) % 26);
        return pt;
    }

    std::string name() const override { return "Vernam (OTP)"; }
};

} // namespace cipher