// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Daniel Marth <danielmarth@gmx.at>
//

#include "OpenCachingCacheLog.h"

namespace Marble
{

OpenCachingCacheLog::OpenCachingCacheLog()
    : m_cacheId( 0 )
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
