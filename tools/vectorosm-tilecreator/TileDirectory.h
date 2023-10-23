// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_TILEDIRECTORY_H
#define MARBLE_TILEDIRECTORY_H

#include "VectorClipper.h"
#include "TagsFilter.h"
#include <TileId.h>
#include <GeoDataLinearRing.h>
#include <ParsingRunnerManager.h>
#include <GeoSceneMercatorTileProjection.h>

#include <QNetworkAccessManager>
#include <QSharedPointer>
#include <QObject>
#include <QFile>

class QNetworkReply;

namespace Marble {

class Download : public QObject
{
    Q_OBJECT

public:
    QString target;
    QNetworkReply* reply;
    qint64 received;
    qint64 total;

public Q_SLOTS:
    void updateProgress(qint64 received, qint64 total);

private:
    QFile m_file;
};

class TileDirectory : public QObject
{
    Q_OBJECT

public:

    enum TileType
    {
        Landmass,
        OpenStreetMap
    };

    TileDirectory(TileType tileType, const QString &cacheDir, ParsingRunnerManager &manager, int maxZoomLevel);
    /** Create a tile directory for loading data from an OSMX file.
     *  @param maxZoomLevel The output zoom level.
     *  @param loadZoomLevel The zoom level at which the input data should be loaded.
     *  This must be smaller or equal to maxZoomLevel. Using a smaller value can be more efficient when
     *  generating a larger batch of tiles that fall within a lower zoom level tile, but comes at a greater
     *  cost for memory and clipping operations.
     */
    TileDirectory(const QString &cacheDir, const QString &osmxFile, ParsingRunnerManager &manager, int maxZoomLevel, int loadZoomLevel);

    QSharedPointer<GeoDataDocument> load(int zoomLevel, int tileX, int tileY);
    void setInputFile(const QString &filename);

    TileId tileFor(int zoomLevel, int tileX, int tileY) const;
    GeoDataDocument *clip(int zoomLevel, int tileX, int tileY);
    QString name() const;

    static QSharedPointer<GeoDataDocument> open(const QString &filename, ParsingRunnerManager &manager);
    GeoDataLatLonBox boundingBox(const QString &filename) const;
    GeoDataLatLonBox boundingBox() const;
    void setBoundingBox(const GeoDataLatLonBox &boundingBox);
    void setBoundingPolygon(const QString &filename);
    void createTiles();
    void createOsmTiles() const;
    int innerNodes(const TileId &tile) const;

    static void printProgress(double progress, int barWidth=40);

private Q_SLOTS:
    void updateProgress();
    void handleFinishedDownload(const QString &filename, const QString &id);

private:
    TagsFilter::Tags tagsFilteredIn(int zoomLevel) const;
    void setTagZoomLevel(int zoomLevel);
    void download(const QString &url, const QString &target);
    QString osmFileFor(const TileId &tileId) const;

    QString m_cacheDir;
    QString m_baseDir;
    QString m_osmxFile;
    ParsingRunnerManager &m_manager;
    QSharedPointer<GeoDataDocument> m_landmass;
    int m_zoomLevel = -1;
    int m_tileX = -1;
    int m_tileY = -1;
    int m_tagZoomLevel = -1;
    QSharedPointer<VectorClipper> m_clipper;
    QSharedPointer<TagsFilter> m_tagsFilter;
    TileType m_tileType;
    QString m_inputFile;
    GeoDataLatLonBox m_boundingBox;
    QVector<GeoDataLinearRing> m_boundingPolygon;
    QNetworkAccessManager m_downloadManager;
    GeoSceneMercatorTileProjection m_tileProjection;
    QString m_landmassFile;
    QSharedPointer<Download> m_download;
    int m_maxZoomLevel;
    static QMap<int, TagsFilter::Tags> m_tags;
};

}

#endif
