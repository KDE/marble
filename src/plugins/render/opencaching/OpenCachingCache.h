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

class OpenCachingCache
{
public:
    OpenCachingCache( const QHash<QString, QVariant>& properties = QHash<QString, QVariant>() );

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

    unsigned long long m_id;

    QDateTime m_dateHidden;

    QDateTime m_dateCreated;

    QDateTime m_dateLastModified;

    QString m_userName;

    QString m_cacheName;

    QString m_cacheType;

    QString m_status;

    QString m_country;

    QString m_sizeString;

    qreal m_difficulty;

    qreal m_terrain;

    qreal m_longitude;

    qreal m_latitude;

    QHash<QString, OpenCachingCacheDescription> m_description;

    OpenCachingCacheLog m_log;
};

}
#endif // OPENCACHINGCACHE_H
