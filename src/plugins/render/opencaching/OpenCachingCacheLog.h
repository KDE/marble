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

#include <QtCore/QList>

namespace Marble
{

class OpenCachingCacheLog
{
public:
    OpenCachingCacheLog();

    OpenCachingCacheLogEntry& operator[]( int index );

    void setCacheId( int cacheId );

    int cacheId() const;

    void addLogEntry( const OpenCachingCacheLogEntry& logEntry );

    void removeLogEntry( int index );

    int size() const;

    void clear();

private:
    long m_cacheId;

    QList<OpenCachingCacheLogEntry> m_logEntries;
};

}
#endif // OPENCACHINGCACHELOG_H
