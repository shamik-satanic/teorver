#include "ConfigManager.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iostream>

namespace ReturnOrigin {

SimParams ConfigManager::defaults()
{
    SimParams p;
    p.p         = 0.5;
    p.s         = 1.0;
    p.N_max     = 32;
    p.runs      = 100000;
    p.max_steps = 10000;
    return p;
}

// -------------------------------------------------------
// Минимальный JSON-парсер: ищем "key": value
// Поддерживаем double и integer значения
// -------------------------------------------------------
static std::string readFile(const std::string& path)
{
    std::ifstream f(path);
    if (!f.is_open())
        throw std::runtime_error("Cannot open file: " + path);
    std::ostringstream ss;
    ss << f.rdbuf();
    return ss.str();
}

// Найти значение по ключу в JSON-строке (только верхний уровень)
static std::string findValue(const std::string& json, const std::string& key)
{
    std::string search = "\"" + key + "\"";
    auto pos = json.find(search);
    if (pos == std::string::npos) return "";

    pos = json.find(':', pos + search.size());
    if (pos == std::string::npos) return "";
    ++pos;

    // Skip whitespace
    while (pos < json.size() && std::isspace(json[pos])) ++pos;

    // Read until , or } or whitespace
    std::string val;
    while (pos < json.size()
           && json[pos] != ','
           && json[pos] != '}'
           && json[pos] != '\n'
           && !std::isspace(json[pos])) {
        val += json[pos++];
    }
    return val;
}

static double toDouble(const std::string& s, double def)
{
    if (s.empty()) return def;
    try { return std::stod(s); }
    catch (...) { return def; }
}

static uint64_t toUInt64(const std::string& s, uint64_t def)
{
    if (s.empty()) return def;
    try { return static_cast<uint64_t>(std::stoull(s)); }
    catch (...) { return def; }
}

// -------------------------------------------------------
SimParams ConfigManager::loadFromFile(const std::string& path)
{
    SimParams p = defaults();
    std::string json;
    try {
        json = readFile(path);
    } catch (const std::exception& e) {
        std::cerr << "[Config] " << e.what()
                  << " — using defaults.\n";
        return p;
    }

    p.p         = toDouble(findValue(json, "p"),         p.p);
    p.s         = toDouble(findValue(json, "s"),         p.s);
    p.N_max     = static_cast<int>(
                    toUInt64(findValue(json, "N_max"),   p.N_max));
    p.runs      = toUInt64(findValue(json, "runs"),      p.runs);
    p.max_steps = toUInt64(findValue(json, "max_steps"), p.max_steps);

    // Validate
    p.p     = std::clamp(p.p, 1e-6, 1.0 - 1e-6);
    p.s     = std::max(p.s, 1e-9);
    p.N_max = std::max(2, p.N_max);
    if (p.N_max % 2 != 0) p.N_max++;   // округлить до чётного
    p.runs  = std::max(uint64_t(1), p.runs);

    return p;
}

// -------------------------------------------------------
void ConfigManager::saveToFile(const std::string& path,
                                const SimParams& p)
{
    std::ofstream f(path);
    if (!f.is_open()) {
        std::cerr << "[Config] Cannot write: " << path << "\n";
        return;
    }
    f << "{\n"
      << "    \"p\":         " << p.p         << ",\n"
      << "    \"s\":         " << p.s         << ",\n"
      << "    \"N_max\":     " << p.N_max     << ",\n"
      << "    \"runs\":      " << p.runs      << ",\n"
      << "    \"max_steps\": " << p.max_steps << "\n"
      << "}\n";
}

} // namespace ReturnOrigin
