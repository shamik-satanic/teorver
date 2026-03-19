#pragma once
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <numeric>
#include <cmath>

namespace cipher {

// ── Пространство вероятностей ─────────────────────────────────────────────────
// Хранит элементы X и их вероятности p(X=x)

template<typename T>
struct ProbSpace {
    std::vector<T>      elements;
    std::vector<double> probs;     // probs[i] = P(X = elements[i])

    ProbSpace() = default;

    void add(const T& elem, double p) {
        elements.push_back(elem);
        probs.push_back(p);
    }

    double prob(const T& elem) const {
        for (std::size_t i = 0; i < elements.size(); ++i)
            if (elements[i] == elem) return probs[i];
        return 0.0;
    }

    void normalize() {
        double s = 0;
        for (double p : probs) s += p;
        if (s <= 0) throw std::runtime_error("zero total probability");
        for (double& p : probs) p /= s;
    }

    std::size_t size() const { return elements.size(); }
};

// ── Абстрактный шифр ──────────────────────────────────────────────────────────
struct ICipher {
    virtual ~ICipher() = default;

    // E_{key}(plaintext) = ciphertext
    virtual std::string encrypt(const std::string& plaintext,
                                const std::string& key) const = 0;
    // D_{key}(ciphertext) = plaintext
    virtual std::string decrypt(const std::string& ciphertext,
                                const std::string& key) const = 0;

    virtual std::string name() const = 0;
};

// ── Вероятностная модель шифра ────────────────────────────────────────────────
//
//  P(C = c_l)              = Σ_{k_s : E_{k_s}(m_j) = c_l} p_{k_s} · p_{m_j}
//  P(C = c_l | M = m_j)   = Σ_{k_s : E_{k_s}(m_j) = c_l} p_{k_s}
//  P(M = m_j | C = c_l)   = P(C=c_l|M=m_j) · p_{m_j} / P(C=c_l)

struct CipherModel {
    const ICipher&            cipher;
    ProbSpace<std::string>    plaintext_space;   // M
    ProbSpace<std::string>    key_space;          // Key

    CipherModel(const ICipher& c,
                ProbSpace<std::string> M,
                ProbSpace<std::string> K)
        : cipher(c), plaintext_space(std::move(M)), key_space(std::move(K)) {}

    // P(C = cl)
    double prob_cipher(const std::string& cl) const {
        double total = 0.0;
        for (std::size_t j = 0; j < plaintext_space.size(); ++j) {
            for (std::size_t s = 0; s < key_space.size(); ++s) {
                std::string ct = cipher.encrypt(plaintext_space.elements[j],
                                                key_space.elements[s]);
                if (ct == cl)
                    total += key_space.probs[s] * plaintext_space.probs[j];
            }
        }
        return total;
    }

    // P(C = cl | M = mj)
    double prob_cipher_given_plain(const std::string& cl,
                                   const std::string& mj) const {
        double total = 0.0;
        for (std::size_t s = 0; s < key_space.size(); ++s) {
            std::string ct = cipher.encrypt(mj, key_space.elements[s]);
            if (ct == cl) total += key_space.probs[s];
        }
        return total;
    }

    // P(M = mj | C = cl)  — формула Байеса
    double prob_plain_given_cipher(const std::string& mj,
                                   const std::string& cl) const {
        double pc   = prob_cipher(cl);
        if (pc <= 0) return 0.0;
        double pcm  = prob_cipher_given_plain(cl, mj);
        double pm   = plaintext_space.prob(mj);
        return pcm * pm / pc;
    }

    // Полное распределение P(C = ·)
    std::map<std::string, double> cipher_distribution() const {
        std::map<std::string, double> dist;
        for (std::size_t j = 0; j < plaintext_space.size(); ++j)
            for (std::size_t s = 0; s < key_space.size(); ++s) {
                auto ct = cipher.encrypt(plaintext_space.elements[j],
                                         key_space.elements[s]);
                dist[ct] += key_space.probs[s] * plaintext_space.probs[j];
            }
        return dist;
    }

    // Совершенная стойкость: P(M=mj | C=cl) == P(M=mj) для всех mj, cl
    bool is_perfectly_secret(double eps = 1e-9) const {
        auto dist = cipher_distribution();
        for (auto& [cl, _] : dist) {
            for (std::size_t j = 0; j < plaintext_space.size(); ++j) {
                double post = prob_plain_given_cipher(plaintext_space.elements[j], cl);
                double prio = plaintext_space.probs[j];
                if (std::abs(post - prio) > eps) return false;
            }
        }
        return true;
    }
};

} // namespace cipher