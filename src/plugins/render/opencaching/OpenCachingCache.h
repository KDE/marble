//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Daniel Marth <danielmarth@gmx.at>
//

#ifndef OPENCACHINGCACHE_H
#define OPENCACHINGCACHE_H

#include "OpenCachingCacheLog.h"
#include "OpenCachingCacheDescription.h"

#include <QtCore/QString>
#include <QtCore/QDateTime>

namespace Marble
{

/**
 * Contains all information about a cache, including logs and descriptions in all
 * available languages.
 */
class OpenCachingCache
{
public:
    explicit OpenCachingCache( const QHash<QString, QVariant>& properties = QHash<QString, QVariant>() );

    void setId( unsigned long long id );

    unsigned long long id() const;

    void setDateHidden( const QDateTime& dateHidden );

    const QDateTime& dateHidden() const;

    void setDateCreated( const QDateTime& dateCreated );

    const QDateTime& dateCreated() const;

    void setDateLastModified( const QDateTime& dateLastModified );

    const QDateTime& dateLastModified() const;

    void setUserName( const QString& userName );

    const QString& userName() const;

    void setCacheName( const QString& cacheName );

    const QString& cacheName() const;

    void setCacheType( const QString& cacheType );

    const QString& cacheType() const;

    void setStatus( const QString& status );

    const QString& status() const;

    void setCountry( const QString& country );

    const QString& country() const;

    void setSizeString( const QString& size );

    const QString& sizeString() const;

    void setDifficulty( qreal difficulty );

    qreal difficulty() const;

    void setTerrain( qreal terrain );

    qreal terrain() const;

    void setLongitude( qreal longitude );

    qreal longitude() const;

    void setLatitude( qreal latitude );

    qreal latitude() const;

    void setDescription( const QHash<QString, OpenCachingCacheDescription>& description );

    const QHash<QString, OpenCachingCacheDescription>& description() const;

    void setLog( const OpenCachingCacheLog& log );

    const OpenCachingCacheLog& log() const;

private:
    void updateTooltip();

    unsigned long long m_id;             ///< Unique ID of the cache.

    QDateTime m_dateHidden;              ///< Date the cache was hidden.

    QDateTime m_dateCreated;             ///< Date the cache was created.

    QDateTime m_dateLastModified;        ///< Date the cache was last modified.

    QString m_userName;                  ///< Name of the user that hid the cache.

    QString m_cacheName;                 ///< Name of the cache.

    QString m_cacheType;                 ///< Type of the cache. @todo Possible types?

    QString m_status;                    ///< Status of the cache. @todo Possible states?

    QString m_country;                   ///< Country in which the cache is located.

    QString m_sizeString;                ///< Size of the cache as a string.

    qreal m_difficulty;                  ///< Difficulty of the cache from 1 to 5.

    qreal m_terrain;                     ///< @todo ?

    qreal m_longitude;                   ///< Longitude of the cache.

    qreal m_latitude;                    ///< Latitude of the cache.

    QHash<QString, OpenCachingCacheDescription> m_description;  ///< Descriptions in all available languages. Languages are keys of the map.

    OpenCachingCacheLog m_log;           ///< Log entries of the cache.
};

}
#endif // OPENCACHINGCACHE_H
