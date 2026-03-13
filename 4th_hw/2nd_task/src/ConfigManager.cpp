#include "ConfigManager.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <stdexcept>

namespace PointMotion {

static Distribution distFromString(const QString& s)
{
    if (s == "Uniform")          return Distribution::Uniform;
    if (s == "Binomial")         return Distribution::Binomial;
    if (s == "FiniteGeometric")  return Distribution::FiniteGeometric;
    if (s == "DiscreteTriangle") return Distribution::DiscreteTriangle;
    return Distribution::Uniform;
}

static QString distToString(Distribution d)
{
    switch (d) {
    case Distribution::Uniform:          return "Uniform";
    case Distribution::Binomial:         return "Binomial";
    case Distribution::FiniteGeometric:  return "FiniteGeometric";
    case Distribution::DiscreteTriangle: return "DiscreteTriangle";
    }
    return "Uniform";
}

// -------------------------------------------------------
SimParams ConfigManager::defaults()
{
    SimParams p;
    p.Y0       = 10.0;
    p.h        =  1.0;
    p.n        = 100;
    p.runs     = 1000;
    p.l_target =  3;
    p.distParams.dist     = Distribution::Uniform;
    p.distParams.s_min    = -3.0;
    p.distParams.s_max    =  3.0;
    p.distParams.n_trials =  6;
    p.distParams.prob     =  0.5;
    return p;
}

// -------------------------------------------------------
SimParams ConfigManager::fromJson(const QJsonObject& obj)
{
    SimParams p = defaults();

    if (obj.contains("Y0"))       p.Y0       = obj["Y0"].toDouble(p.Y0);
    if (obj.contains("h"))        p.h        = obj["h"].toDouble(p.h);
    if (obj.contains("n"))        p.n        = obj["n"].toInt(p.n);
    if (obj.contains("runs"))     p.runs     = obj["runs"].toInt(p.runs);
    if (obj.contains("l_target")) p.l_target = obj["l_target"].toInt(p.l_target);

    if (obj.contains("distribution")) {
        QJsonObject d = obj["distribution"].toObject();
        if (d.contains("type"))     p.distParams.dist     = distFromString(d["type"].toString());
        if (d.contains("s_min"))    p.distParams.s_min    = d["s_min"].toDouble(p.distParams.s_min);
        if (d.contains("s_max"))    p.distParams.s_max    = d["s_max"].toDouble(p.distParams.s_max);
        if (d.contains("n_trials")) p.distParams.n_trials = d["n_trials"].toInt(p.distParams.n_trials);
        if (d.contains("prob"))     p.distParams.prob     = d["prob"].toDouble(p.distParams.prob);
    }
    return p;
}

// -------------------------------------------------------
QJsonObject ConfigManager::toJson(const SimParams& p)
{
    QJsonObject dist;
    dist["type"]     = distToString(p.distParams.dist);
    dist["s_min"]    = p.distParams.s_min;
    dist["s_max"]    = p.distParams.s_max;
    dist["n_trials"] = p.distParams.n_trials;
    dist["prob"]     = p.distParams.prob;

    QJsonObject obj;
    obj["Y0"]           = p.Y0;
    obj["h"]            = p.h;
    obj["n"]            = p.n;
    obj["runs"]         = p.runs;
    obj["l_target"]     = p.l_target;
    obj["distribution"] = dist;
    return obj;
}

// -------------------------------------------------------
SimParams ConfigManager::loadFromFile(const QString& path)
{
    QFile f(path);
    if (!f.open(QIODevice::ReadOnly))
        return defaults();

    QJsonParseError err;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &err);
    if (doc.isNull() || !doc.isObject())
        return defaults();

    return fromJson(doc.object());
}

// -------------------------------------------------------
void ConfigManager::saveToFile(const QString& path, const SimParams& params)
{
    QFile f(path);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate))
        return;

    QJsonDocument doc(toJson(params));
    f.write(doc.toJson(QJsonDocument::Indented));
}

} // namespace PointMotion
