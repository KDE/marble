//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
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

    TileDirectory(TileType tileType, const QString &cacheDir, ParsingRunnerManager &manager, const QString &extension, int maxZoomLevel);

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
    void createTiles() const;
    int innerNodes(const TileId &tile) const;

    static void printProgress(double progress, int barWidth=40);

private Q_SLOTS:
    void updateProgress();
    void handleFinishedDownload(const QString &filename, const QString &id);

private:
    QStringList tagsFilteredIn(int zoomLevel) const;
    void setTagZoomLevel(int zoomLevel);
    void download(const QString &url, const QString &target);

    QString m_cacheDir;
    QString m_baseDir;
    ParsingRunnerManager &m_manager;
    QSharedPointer<GeoDataDocument> m_landmass;
    int m_zoomLevel;
    int m_tileX;
    int m_tileY;
    int m_tagZoomLevel;
    QString m_extension;
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
};

}

#endif
