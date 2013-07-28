//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
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

    OpenCachingCacheLogEntry& operator[]( int index );

    void setCacheId( unsigned long long cacheId );

    unsigned long long cacheId() const;

    void addLogEntry( const OpenCachingCacheLogEntry& logEntry );

    void removeLogEntry( int index );

    int size() const;

    void clear();

    int currentIndex() const;

private:
    unsigned long long m_cacheId;                   ///< Unique ID of the cache.

    QList<OpenCachingCacheLogEntry> m_logEntries;   ///< All log entries.
};

}
#endif // OPENCACHINGCACHELOG_H
