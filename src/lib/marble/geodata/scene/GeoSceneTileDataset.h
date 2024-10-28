/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>
    SPDX-FileCopyrightText: 2008 Jens-Michael Hoffmann <jensmh@gmx.de>
    SPDX-FileCopyrightText: 2012 Ander Pijoan <ander.pijoan@deusto.es>

    SPDX-License-Identifier: LGPL-2.1-or-later
*/

#ifndef MARBLE_GEOSCENETILEDATASET_H
#define MARBLE_GEOSCENETILEDATASET_H

#include <QList>
#include <QSize>
#include <QStringList>
#include <QUrl>

#include "GeoDataLatLonBox.h"
#include "GeoSceneAbstractDataset.h"
#include "GeoSceneAbstractTileProjection.h"
#include "MarbleGlobal.h"

/**
 * @short Tiled dataset stored in a layer. TextureTile and VectorTile layes inherit from this class.
 */

/* In order to make Marble able to manage vector tiles,
 * now there is GeoSceneTileDataset and then GeoSceneTextureTileDataset
 * (for the tag <texture> in dgml) or GeoSceneVectorTileDataset
 * (for <vectortile>) are created, which inherit from this class */

namespace Marble
{
class DownloadPolicy;
class ServerLayout;
class TileId;

class GEODATA_EXPORT GeoSceneTileDataset : public GeoSceneAbstractDataset
{
public:
    enum StorageLayout {
        Marble,
        OpenStreetMap,
        TileMapService
    };

    explicit GeoSceneTileDataset(const QString &name);
    ~GeoSceneTileDataset() override;
    const char *nodeType() const override;

    QString sourceDir() const;
    void setSourceDir(const QString &sourceDir);

    QString installMap() const;
    void setInstallMap(const QString &installMap);

    StorageLayout storageLayout() const;
    void setStorageLayout(const StorageLayout);

    void setServerLayout(const ServerLayout *);
    const ServerLayout *serverLayout() const;

    int levelZeroColumns() const;
    void setLevelZeroColumns(const int);

    int levelZeroRows() const;
    void setLevelZeroRows(const int);

    bool hasMaximumTileLevel() const;
    int maximumTileLevel() const;
    void setMaximumTileLevel(const int);

    int minimumTileLevel() const;
    void setMinimumTileLevel(int level);

    void setTileLevels(const QString &tileLevels);
    QList<int> tileLevels() const;

    QList<QUrl> downloadUrls() const;

    const QSize tileSize() const;
    void setTileSize(const QSize &tileSize);

    /**
     * @brief set bounds for the texture. Tiles outside of these bounds won't be searched in this texture.
     * Null box means no bounds.
     */
    void setLatLonBox(const GeoDataLatLonBox &box);
    GeoDataLatLonBox latLonBox() const;

    void setTileProjection(GeoSceneAbstractTileProjection::Type projectionType);

    const GeoSceneAbstractTileProjection *tileProjection() const;
    GeoSceneAbstractTileProjection::Type tileProjectionType() const;

    QString blending() const;
    void setBlending(const QString &name);

    /**
     * Creates a download URL for the given tile id.
     *
     * It implements the round robin for the tile servers.
     * On each invocation the next url is returned.
     */
    QUrl downloadUrl(const TileId &) const;
    void addDownloadUrl(const QUrl &);

    QString relativeTileFileName(const TileId &) const;

    QString themeStr() const;

    QList<const DownloadPolicy *> downloadPolicies() const;
    void addDownloadPolicy(const DownloadUsage usage, const int maximumConnections);

private:
    Q_DISABLE_COPY(GeoSceneTileDataset)
    QStringList hostNames() const;

    QString m_sourceDir;
    QString m_installMap;
    StorageLayout m_storageLayoutMode;
    const ServerLayout *m_serverLayout;
    int m_levelZeroColumns;
    int m_levelZeroRows;
    int m_minimumTileLevel;
    int m_maximumTileLevel;
    QList<int> m_tileLevels;
    mutable QSize m_tileSize;
    GeoDataLatLonBox m_latLonBox;
    GeoSceneAbstractTileProjection *m_tileProjection;
    QString m_blending;

    /// List of Urls which are used in a round robin fashion
    QList<QUrl> m_downloadUrls;

    /// Points to next Url for the round robin algorithm
    mutable QList<QUrl>::const_iterator m_nextUrl;
    QList<const DownloadPolicy *> m_downloadPolicies;
};

inline bool GeoSceneTileDataset::hasMaximumTileLevel() const
{
    return m_maximumTileLevel != -1;
}

inline QString GeoSceneTileDataset::blending() const
{
    return m_blending;
}

inline void GeoSceneTileDataset::setBlending(const QString &name)
{
    m_blending = name;
}

}

#endif
