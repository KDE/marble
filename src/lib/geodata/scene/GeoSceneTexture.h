/*
    Copyright (C) 2008 Torsten Rahn <rahn@kde.org>
    Copyright (C) 2008 Jens-Michael Hoffmann <jensmh@gmx.de>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef MARBLE_GEOSCENETEXTURE_H
#define MARBLE_GEOSCENETEXTURE_H

#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QVector>
#include <QtCore/QSize>

#include "GeoSceneLayer.h"
#include "global.h"

/**
 * @short Texture dataset stored in a layer.
 */


namespace Marble
{
class Blending;
class DownloadPolicy;
class ServerLayout;
class TileId;

class GeoSceneTexture : public GeoSceneAbstractDataset
{
 public:
    enum StorageLayout { Marble, OpenStreetMap };
    enum Projection { Equirectangular, Mercator };

    explicit GeoSceneTexture( const QString& name );
    ~GeoSceneTexture();

    QString sourceDir() const;
    void setSourceDir( const QString& sourceDir );

    QString installMap() const;
    void setInstallMap( const QString& installMap );

    void setStorageLayout( const StorageLayout );

    void setServerLayout( const ServerLayout * );

    int levelZeroColumns() const;
    void setLevelZeroColumns( const int );

    int levelZeroRows() const;
    void setLevelZeroRows( const int );

    bool hasMaximumTileLevel() const;
    int maximumTileLevel() const;
    void setMaximumTileLevel( const int );
    
    QVector<QUrl> downloadUrls() const;

    const QSize tileSize() const;

    Projection projection() const;
    void setProjection( const Projection );

    Blending const * blending() const;
    void setBlending( Blending const * const );

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

    QList<DownloadPolicy *> downloadPolicies() const;
    void addDownloadPolicy( const DownloadUsage usage, const int maximumConnections );

    virtual QString type();

 private:
    Q_DISABLE_COPY( GeoSceneTexture )
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
    Blending const * m_blending;

    /// List of Urls which are used in a round robin fashion
    QVector<QUrl> m_downloadUrls;

    /// Points to next Url for the round robin algorithm
    mutable QVector<QUrl>::const_iterator m_nextUrl;
    QList<DownloadPolicy *> m_downloadPolicies;
};

inline bool GeoSceneTexture::hasMaximumTileLevel() const
{
    return m_maximumTileLevel != -1;
}

inline Blending const * GeoSceneTexture::blending() const
{
    return m_blending;
}

inline void GeoSceneTexture::setBlending( Blending const * const blending )
{
    m_blending = blending;
}

}

#endif
