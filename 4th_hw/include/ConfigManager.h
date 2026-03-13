#pragma once
#include <string>
#include "SimulationEngine.h"

namespace ReturnOrigin {

class ConfigManager {
public:
    static SimParams loadFromFile(const std::string& path);
    static void      saveToFile(const std::string& path, const SimParams& p);
    static SimParams defaults();
};

} // namespace ReturnOrigin
