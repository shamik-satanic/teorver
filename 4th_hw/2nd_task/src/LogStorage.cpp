#include "LogStorage.h"

namespace PointMotion {

LogStorage::LogStorage(int maxEntries, QObject* parent)
    : QObject(parent), m_max(maxEntries) {}

void LogStorage::setMaxEntries(int k)
{
    m_max = std::max(1, k);
    while (m_entries.size() > m_max)
        m_entries.removeFirst();
    emit entriesChanged();
}

void LogStorage::addEntry(const LogEntry& e)
{
    m_entries.append(e);
    while (m_entries.size() > m_max)
        m_entries.removeFirst();
    emit entriesChanged();
}

void LogStorage::clear()
{
    m_entries.clear();
    emit entriesChanged();
}

} // namespace PointMotion
