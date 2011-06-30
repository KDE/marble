//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
//

#include "OpenCachingCacheLog.h"

namespace Marble
{

OpenCachingCacheLog::OpenCachingCacheLog()
{
}

OpenCachingCacheLogEntry& OpenCachingCacheLog::operator[]( int index )
{
    return m_logEntries[index];
}

void OpenCachingCacheLog::setCacheId( unsigned long long cacheId )
{
    m_cacheId = cacheId;
}

unsigned long long OpenCachingCacheLog::cacheId() const
{
    return m_cacheId;
}

void OpenCachingCacheLog::addLogEntry( const OpenCachingCacheLogEntry& logEntry )
{
    m_logEntries.append( logEntry );
}

void OpenCachingCacheLog::removeLogEntry( int index )
{
    m_logEntries.removeAt( index );
}

int OpenCachingCacheLog::size() const
{
    return m_logEntries.size();
}

void OpenCachingCacheLog::clear()
{
    m_logEntries.clear();
}

}
