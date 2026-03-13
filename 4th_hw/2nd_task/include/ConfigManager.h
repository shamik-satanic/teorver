#pragma once
#include <QString>
#include <QJsonObject>
#include "SimulationEngine.h"

namespace PointMotion {

// Читает/пишет конфиг в JSON
class ConfigManager
{
public:
    static SimParams  loadFromFile(const QString& path);
    static void       saveToFile(const QString& path, const SimParams& params);
    static SimParams  fromJson(const QJsonObject& obj);
    static QJsonObject toJson(const SimParams& params);
    static SimParams  defaults();
};

} // namespace PointMotion
