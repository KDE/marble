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

#ifndef GEOSCENETEXTURE_H
#define GEOSCENETEXTURE_H

#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtCore/QUrl>
#include <QtCore/QVector>

#include "GeoSceneLayer.h"
#include "global.h"

/**
 * @short Texture dataset stored in a layer.
 */

namespace Marble
{
class DownloadPolicy;

class GeoSceneTexture : public GeoSceneAbstractDataset
{
 public:
    enum StorageLayoutMode { Marble, OpenStreetMap, Custom };
    enum Projection { Equirectangular, Mercator };

    explicit GeoSceneTexture( const QString& name );
    ~GeoSceneTexture();

    QString sourceDir() const;
    void setSourceDir( const QString& sourceDir );

    QString installMap() const;
    void setInstallMap( const QString& installMap );

    StorageLayoutMode storageLayoutMode() const;
    void setStorageLayoutMode( StorageLayoutMode const );

    QString customStorageLayout()const;
    void setCustomStorageLayout( const QString& );

    int levelZeroColumns() const;
    void setLevelZeroColumns( const int );

    int levelZeroRows() const;
    void setLevelZeroRows( const int );

    Projection projection() const;
    void setProjection( const Projection );

    // this method is a little more than just a stupid getter,
    // it implements the round robin for the tile servers.
    // on each invocation the next url is returned
    QUrl downloadUrl();
    void addDownloadUrl( const QUrl & );

    QList<DownloadPolicy *> downloadPolicies() const;
    void addDownloadPolicy( const DownloadUsage usage, const int maximumConnections );

    virtual QString type();

 private:
    Q_DISABLE_COPY( GeoSceneTexture )
    QStringList hostNames() const;

    QString m_sourceDir;
    QString m_installMap;
    StorageLayoutMode m_storageLayoutMode;
    QString m_customStorageLayout;
    int m_levelZeroColumns;
    int m_levelZeroRows;
    Projection m_projection;

    /// List of Urls which are used in a round robin fashion
    QVector<QUrl> m_downloadUrls;

    /// Points to next Url for the round robin algorithm
    QVector<QUrl>::const_iterator m_nextUrl;
    QList<DownloadPolicy *> m_downloadPolicies;
};

}

#endif // GEOSCENETEXTURE_H
