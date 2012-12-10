/*
 This file is part of the Marble Virtual Globe.

 This program is free software licensed under the GNU LGPL. You can
 find a copy of this license in LICENSE.txt in the top directory of
 the source code.

 Copyright (C) 2008 Torsten Rahn <rahn@kde.org>

 Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

 Copyright 2012 Ander Pijoan <ander.pijoan@deusto.es>
*/

#ifndef MARBLE_GEOSCENETILED_H
#define MARBLE_GEOSCENETILED_H

#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QVector>
#include <QtCore/QSize>

#include "GeoSceneLayer.h"
#include "MarbleGlobal.h"

/**
 * @short Tiled dataset stored in a layer. TextureTile and VectorTile layes inherit from this class.
 */

/* In order to make Marble able to manage vector tiles,
 * now there is GeoSceneTiled and then GeoSceneTiled
 * (for the tag <texture> in dgml) or GeoSceneVectorTile
 * (for <vectortile>) are created, which inherit from this class */

namespace Marble
{
class Blending;
class DownloadPolicy;
class ServerLayout;
class TileId;

class GeoSceneTiled : public GeoSceneAbstractDataset
{
 public:
    enum StorageLayout { Marble, OpenStreetMap, TileMapService };
    enum Projection { Equirectangular, Mercator };

    explicit GeoSceneTiled( const QString& name );
    ~GeoSceneTiled();
    virtual const char* nodeType() const;

    QString sourceDir() const;
    void setSourceDir( const QString& sourceDir );

    QString installMap() const;
    void setInstallMap( const QString& installMap );

    StorageLayout storageLayout() const;
    void setStorageLayout( const StorageLayout );

    void setServerLayout( const ServerLayout * );
    const ServerLayout *serverLayout() const;

    int levelZeroColumns() const;
    void setLevelZeroColumns( const int );

    int levelZeroRows() const;
    void setLevelZeroRows( const int );

    bool hasMaximumTileLevel() const;
    int maximumTileLevel() const;
    void setMaximumTileLevel( const int );
    
    QVector<QUrl> downloadUrls() const;

    const QSize tileSize() const;
    void setTileSize( const QSize &tileSize );

    Projection projection() const;
    void setProjection( const Projection );

    QString blending() const;
    void setBlending( const QString &name );

    /**
     * Creates a download URL for the given tile id.
     *
     * It implements the round robin for the tile servers.
     * On each invocation the next url is returned.
     */
    QUrl downloadUrl( const TileId & ) const;
    void addDownloadUrl( const QUrl & );

    QString relativeTileFileName( const TileId & ) const;

    QString themeStr() const;

    QList<const DownloadPolicy *> downloadPolicies() const;
    void addDownloadPolicy( const DownloadUsage usage, const int maximumConnections );

    virtual QString type();

 private:
    Q_DISABLE_COPY( GeoSceneTiled )
    QStringList hostNames() const;

    QString m_sourceDir;
    QString m_installMap;
    StorageLayout m_storageLayoutMode;
    const ServerLayout *m_serverLayout;
    int m_levelZeroColumns;
    int m_levelZeroRows;
    int m_maximumTileLevel;
    mutable QSize m_tileSize;
    Projection m_projection;
    QString m_blending;

    /// List of Urls which are used in a round robin fashion
    QVector<QUrl> m_downloadUrls;

    /// Points to next Url for the round robin algorithm
    mutable QVector<QUrl>::const_iterator m_nextUrl;
    QList<const DownloadPolicy *> m_downloadPolicies;
};

inline bool GeoSceneTiled::hasMaximumTileLevel() const
{
    return m_maximumTileLevel != -1;
}

inline QString GeoSceneTiled::blending() const
{
    return m_blending;
}

inline void GeoSceneTiled::setBlending( const QString &name )
{
    m_blending = name;
}

}

#endif
