//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
//

#include "OpenCachingCache.h"

namespace Marble
{

OpenCachingCache::OpenCachingCache( const QHash<QString, QVariant>& properties )
{
    m_id =  properties["id"].toULongLong();
    m_dateHidden = properties["datehidden"].toDateTime();
    m_dateCreated = properties["datecreated"].toDateTime();
    m_dateLastModified = properties["lastmodified"].toDateTime();
    m_userName = properties["userid"].toString();
    m_cacheName = properties["name"].toString();
    m_cacheType = properties["type"].toString();
    m_status = properties["status"].toString();
    m_country = properties["country"].toString();
    m_sizeString = properties["size"].toString();
    m_difficulty = properties["difficulty"].toReal();
    m_terrain = properties["terrain"].toReal();
    m_longitude = properties["longitude"].toReal();
    m_latitude = properties["latitude"].toReal();
}

void OpenCachingCache::setId( unsigned long long id )
{
    m_id = id;
}

unsigned long long OpenCachingCache::id() const
{
    return m_id;
}

void OpenCachingCache::setDateHidden( const QDateTime& dateHidden )
{
    m_dateHidden = dateHidden;
}

const QDateTime& OpenCachingCache::dateHidden() const
{
    return m_dateHidden;
}

void OpenCachingCache::setDateCreated( const QDateTime& dateCreated )
{
    m_dateCreated = dateCreated;
}

const QDateTime& OpenCachingCache::dateCreated() const
{
    return m_dateCreated;
}

void OpenCachingCache::setDateLastModified( const QDateTime& dateLastModified )
{
    m_dateLastModified = dateLastModified;
}

const QDateTime& OpenCachingCache::dateLastModified() const
{
    return m_dateLastModified;
}

void OpenCachingCache::setUserName( const QString& userName )
{
    m_userName = userName;
}

const QString& OpenCachingCache::userName() const
{
    return m_userName;
}

void OpenCachingCache::setCacheName( const QString& cacheName )
{
    m_cacheName = cacheName;
}

const QString& OpenCachingCache::cacheName() const
{
    return m_cacheName;
}

void OpenCachingCache::setCacheType( const QString& cacheType )
{
    m_cacheType = cacheType;
}

const QString& OpenCachingCache::cacheType() const
{
    return m_cacheType;
}

void OpenCachingCache::setStatus( const QString& status )
{
    m_status = status;
}

const QString& OpenCachingCache::status() const
{
    return m_status;
}

void OpenCachingCache::setCountry( const QString& country )
{
    m_country = country;
}

const QString& OpenCachingCache::country() const
{
    return m_country;
}

void OpenCachingCache::setSizeString( const QString& sizeString )
{
    m_sizeString = sizeString;
}

const QString& OpenCachingCache::sizeString() const
{
    return m_sizeString;
}

void OpenCachingCache::setDifficulty( qreal difficulty )
{
    m_difficulty = difficulty;
}

qreal OpenCachingCache::difficulty() const
{
    return m_difficulty;
}

void OpenCachingCache::setTerrain( qreal terrain )
{
    m_terrain = terrain;
}

qreal OpenCachingCache::terrain() const
{
    return m_terrain;
}

void OpenCachingCache::setLongitude( qreal longitude )
{
    m_longitude = longitude;
}

qreal OpenCachingCache::longitude() const
{
    return m_longitude;
}

void OpenCachingCache::setLatitude( qreal latitude )
{
    m_latitude = latitude;
}

qreal OpenCachingCache::latitude() const
{
    return m_latitude;
}

void OpenCachingCache::setDescription( const QHash<QString, OpenCachingCacheDescription>& description )
{
    m_description = description;
}

const QHash<QString, OpenCachingCacheDescription>& OpenCachingCache::description() const
{
    return m_description;
}

void OpenCachingCache::setLog( const OpenCachingCacheLog& log )
{
    m_log = log;
}

const OpenCachingCacheLog& OpenCachingCache::log() const
{
    return m_log;
}

}
