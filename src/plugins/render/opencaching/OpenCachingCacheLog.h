// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Daniel Marth <danielmarth@gmx.at>
//

#ifndef OPENCACHINGCACHELOG_H
#define OPENCACHINGCACHELOG_H

#include "OpenCachingCacheLogEntry.h"

#include <QList>

namespace Marble
{

/**
 * All log entries for a cache.
 */
class OpenCachingCacheLog
{
public:
    OpenCachingCacheLog();

    OpenCachingCacheLogEntry &operator[](int index);

    void setCacheId(unsigned long long cacheId);

    unsigned long long cacheId() const;

    void addLogEntry(const OpenCachingCacheLogEntry &logEntry);

    void removeLogEntry(int index);

    int size() const;

    void clear();

    int currentIndex() const;

private:
    unsigned long long m_cacheId; ///< Unique ID of the cache.

    QList<OpenCachingCacheLogEntry> m_logEntries; ///< All log entries.
};

}
#endif // OPENCACHINGCACHELOG_H
