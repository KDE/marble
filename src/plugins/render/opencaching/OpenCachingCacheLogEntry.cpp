//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
//

#include "OpenCachingCacheLogEntry.h"

namespace Marble
{

OpenCachingCacheLogEntry::OpenCachingCacheLogEntry( const QHash<QString, QVariant>& properties )
{
    m_cacheId = properties["cacheid"].toULongLong();
    m_userName = properties["userid"].toString();
    m_logType = properties["logtype"].toString();
    m_text = properties["text"].toString();
    m_logDate = properties["date"].toDateTime();
    m_createdDate = properties["datecreated"].toDateTime();
    m_lastModifiedDate = properties["lastmodified"].toDateTime();
}

void OpenCachingCacheLogEntry::setCacheId( int cacheId )
{
    m_cacheId = cacheId;
}

int OpenCachingCacheLogEntry::cacheId() const
{
    return m_cacheId;
}

void OpenCachingCacheLogEntry::setUserName( const QString& userName )
{
    m_userName = userName;
}

const QString& OpenCachingCacheLogEntry::userName() const
{
    return m_userName;
}

void OpenCachingCacheLogEntry::setLogType( const QString& logType )
{
    m_logType = logType;
}

const QString& OpenCachingCacheLogEntry::logType() const
{
    return m_logType;
}

void OpenCachingCacheLogEntry::setText( const QString& text )
{
    m_text = text;
}

const QString& OpenCachingCacheLogEntry::text() const
{
    return m_text;
}

void OpenCachingCacheLogEntry::setLogDate( const QDateTime& logDate )
{
    m_logDate = logDate;
}

const QDateTime& OpenCachingCacheLogEntry::logDate() const
{
    return m_logDate;
}

void OpenCachingCacheLogEntry::setCreatedDate( const QDateTime& createdDate )
{
    m_createdDate = createdDate;
}

const QDateTime& OpenCachingCacheLogEntry::createdDate() const
{
    return m_createdDate;
}

void OpenCachingCacheLogEntry::setLastModifiedDate( const QDateTime& lastModifiedDate )
{
    m_lastModifiedDate = lastModifiedDate;
}

const QDateTime& OpenCachingCacheLogEntry::lastModifiedDate() const
{
    return m_lastModifiedDate;
}

}
