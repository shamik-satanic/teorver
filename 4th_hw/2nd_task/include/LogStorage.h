#pragma once
#include <QObject>
#include <QList>
#include <QDateTime>
#include <QString>
#include "SimulationEngine.h"

namespace PointMotion {

struct LogEntry {
    QDateTime        timestamp;
    SimParams        params;
    EmpiricalResult  empirical;
    SimResult        sampleRun;   // одна траектория для просмотра
    QString          summary;     // краткая строка
};

// Хранит последние K записей, K настраивается
class LogStorage : public QObject
{
    Q_OBJECT
public:
    explicit LogStorage(int maxEntries = 50, QObject* parent = nullptr);

    void        addEntry(const LogEntry& e);
    void        setMaxEntries(int k);
    int         maxEntries() const { return m_max; }
    int         count()      const { return m_entries.size(); }
    const LogEntry& entry(int i) const { return m_entries[i]; }
    void        clear();

signals:
    void entriesChanged();

private:
    QList<LogEntry> m_entries;
    int             m_max;
};

} // namespace PointMotion
