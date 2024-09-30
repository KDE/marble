// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

#include "TileDirectory.h"
#include "MarbleZipReader.h"
#include "PeakAnalyzer.h"
#include "StyleBuilder.h"
#include "TileCoordsPyramid.h"
#include "TileIterator.h"
#include <GeoDataDocumentWriter.h>
#include <GeoDataLatLonAltBox.h>

#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QProcess>
#include <QTemporaryFile>
#include <QThread>
#include <QUrl>

#include <iomanip>
#include <iostream>

using namespace std;

namespace Marble
{

QMap<int, TagsFilter::Tags> TileDirectory::m_tags;

TileDirectory::TileDirectory(TileType tileType, const QString &cacheDir, ParsingRunnerManager &manager, int maxZoomLevel)
    : m_cacheDir(cacheDir)
    , m_manager(manager)
    , m_tileType(tileType)
    , m_landmassFile("land-polygons-split-4326.zip")
    , m_maxZoomLevel(maxZoomLevel)
{
    if (m_tileType == Landmass) {
        m_zoomLevel = 7;
        m_baseDir = QString("%1/landmass/%2").arg(cacheDir).arg(m_zoomLevel);
    } else {
        m_zoomLevel = 10;
        m_baseDir = QString("%1/osm/%2").arg(cacheDir).arg(m_zoomLevel);
    }
    QDir().mkpath(m_baseDir);
}

TileDirectory::TileDirectory(const QString &cacheDir,
                             const QString &osmxFile,
                             ParsingRunnerManager &manager,
                             int maxZoomLevel,
                             int loadZoomLevel,
                             InputType inputType)
    : m_cacheDir(cacheDir)
    , m_osmxFile(osmxFile)
    , m_manager(manager)
    , m_zoomLevel(loadZoomLevel)
    , m_tileType(OpenStreetMap)
    , m_inputType(inputType)
    , m_maxZoomLevel(maxZoomLevel)
{
}

TileId TileDirectory::tileFor(int zoomLevel, int tileX, int tileY) const
{
    int const zoomDiff = zoomLevel - m_zoomLevel;
    int const x = tileX >> zoomDiff;
    int const y = tileY >> zoomDiff;
    return TileId(QString(), m_zoomLevel, x, y);
}

QSharedPointer<GeoDataDocument> TileDirectory::load(int zoomLevel, int tileX, int tileY)
{
    auto const tile = tileFor(zoomLevel, tileX, tileY);
    if (tile.x() == m_tileX && tile.y() == m_tileY) {
        return m_landmass;
    }
    m_tileX = tile.x();
    m_tileY = tile.y();

    if (!m_osmxFile.isEmpty() && m_inputType == OsmxInput) {
        const auto tileBox = m_tileProjection.geoCoordinates(tile);
        const QString bbox = QString::number(tileBox.south(GeoDataCoordinates::Degree)) + QLatin1Char(',')
            + QString::number(tileBox.west(GeoDataCoordinates::Degree)) + QLatin1Char(',') + QString::number(tileBox.north(GeoDataCoordinates::Degree))
            + QLatin1Char(',') + QString::number(tileBox.east(GeoDataCoordinates::Degree));

        // TODO the following could be optimized by directly reading via OSMX API
        QTemporaryFile tempPbfFile(m_cacheDir + "/tmp/XXXXXX.osm.pbf");
        if (!tempPbfFile.open()) {
            qCritical() << "Failed to open temporary file!" << tempPbfFile.errorString() << m_cacheDir;
            return {};
        }

        QProcess osmx;
        osmx.start("osmx", QStringList({"extract", (m_cacheDir + QLatin1Char('/') + m_osmxFile), tempPbfFile.fileName(), "--noUserData", "--bbox", bbox}));
        osmx.waitForFinished(5 * 60 * 1000);
        if (osmx.exitCode() != 0) {
            qWarning() << osmx.readAllStandardError();
            qWarning() << "osmx failed: " << osmx.errorString() << osmx.exitStatus() << osmx.exitCode();
            return {};
        }

        m_landmass = open(tempPbfFile.fileName(), m_manager);
    } else if (!m_osmxFile.isEmpty() && m_inputType == RawInput) {
        m_landmass = open(m_osmxFile, m_manager);
    } else {
        QString const filename = QString("%1/%2/%3.%4").arg(m_baseDir).arg(tile.x()).arg(tile.y()).arg("o5m");
        m_landmass = open(filename, m_manager);
    }

    if (m_landmass) {
        PeakAnalyzer::determineZoomLevel(m_landmass->placemarkList());
    }
    return m_landmass;
}

void TileDirectory::setInputFile(const QString &filename)
{
    m_inputFile = filename;

    if (m_tileType == OpenStreetMap) {
        QUrl url = QUrl(filename);
        if (url.scheme().isEmpty()) {
            // local file
            m_boundingBox = boundingBox(m_inputFile);
        } else {
            // remote file: check if already downloaded
            QFileInfo cacheFile = QFileInfo(QString("%1/%2").arg(m_cacheDir).arg(url.fileName()));
            if (!cacheFile.exists()) {
                download(filename, cacheFile.absoluteFilePath());
            }
            m_inputFile = cacheFile.absoluteFilePath();

            QString polyFile = QUrl(filename).fileName();
            polyFile.replace("-latest.osm.pbf", ".poly");
            polyFile.replace(".osm.pbf", ".poly");
            polyFile.replace(".pbf", ".poly");
            QString poly = QString("%1/%2").arg(url.adjusted(QUrl::RemoveFilename).toString()).arg(polyFile);
            QString const polyTarget = QString("%1/%2").arg(m_cacheDir).arg(polyFile);
            if (!QFileInfo::exists(polyTarget)) {
                download(poly, polyTarget);
            }
            setBoundingPolygon(polyTarget);
        }
    }
}

GeoDataDocument *TileDirectory::clip(int zoomLevel, int tileX, int tileY)
{
    QSharedPointer<GeoDataDocument> oldMap = m_landmass;
    load(zoomLevel, tileX, tileY);
    if (!m_clipper || oldMap != m_landmass || m_tagZoomLevel != zoomLevel) {
        setTagZoomLevel(zoomLevel);
        GeoDataDocument *input = m_tagsFilter ? m_tagsFilter->accepted() : m_landmass.data();
        if (input) {
            m_clipper = QSharedPointer<VectorClipper>(new VectorClipper(input, m_maxZoomLevel));
        }
    }
    return m_clipper ? m_clipper->clipTo(zoomLevel, tileX, tileY) : nullptr;
}

QString TileDirectory::name() const
{
    return QString("%1/%2/%3").arg(m_zoomLevel).arg(m_tileX).arg(m_tileY);
}

QSharedPointer<GeoDataDocument> TileDirectory::open(const QString &filename, ParsingRunnerManager &manager)
{
    // Timeout is set to 10 min. If the file is reaaally huge, set it to something bigger.
    GeoDataDocument *map = manager.openFile(filename, DocumentRole::MapDocument, 600000);
    if (map == nullptr) {
        qWarning() << "File" << filename << "couldn't be loaded.";
    }
    QSharedPointer<GeoDataDocument> result = QSharedPointer<GeoDataDocument>(map);
    return result;
}

TagsFilter::Tags TileDirectory::tagsFilteredIn(int zoomLevel) const
{
    if (m_tags.isEmpty()) {
        QSet<GeoDataPlacemark::GeoDataVisualCategory> categories;
        for (int i = GeoDataPlacemark::PlaceCity; i < GeoDataPlacemark::LastIndex; ++i) {
            categories << GeoDataPlacemark::GeoDataVisualCategory(i);
        }

        auto const tagMap = StyleBuilder::osmTagMapping();
        for (auto category : std::as_const(categories)) {
            for (auto iter = tagMap.begin(), end = tagMap.end(); iter != end; ++iter) {
                if (iter.value() == category) {
                    int zoomLevel = StyleBuilder::minimumZoomLevel(category);
                    if (zoomLevel < 17) {
                        m_tags[zoomLevel] << iter.key();
                    }
                }
            }
        }
    }

    TagsFilter::Tags result;
    for (auto iter = m_tags.begin(), end = m_tags.end(); iter != end && iter.key() <= zoomLevel + 1; ++iter) {
        result << iter.value();
    }
    return result;
}

void TileDirectory::setTagZoomLevel(int zoomLevel)
{
    m_tagZoomLevel = zoomLevel;
    if (m_tileType == OpenStreetMap) {
        if (m_tagZoomLevel < 17) {
            auto const tags = tagsFilteredIn(m_tagZoomLevel);
            m_tagsFilter = QSharedPointer<TagsFilter>(new TagsFilter(m_landmass.data(), tags, TagsFilter::FilterRailwayService));
        } else {
            m_tagsFilter.clear();
        }
    }
}

void TileDirectory::download(const QString &url, const QString &target)
{
    m_download = QSharedPointer<Download>(new Download);
    m_download->target = target;
    m_download->reply = m_downloadManager.get(QNetworkRequest(QUrl(url)));
    connect(m_download->reply, SIGNAL(downloadProgress(qint64, qint64)), m_download.data(), SLOT(updateProgress(qint64, qint64)));
    connect(m_download->reply, SIGNAL(downloadProgress(qint64, qint64)), this, SLOT(updateProgress()));
    QEventLoop loop;
    connect(m_download->reply, SIGNAL(finished()), &loop, SLOT(quit()));
    loop.exec();
    cout << endl;
}

QString TileDirectory::osmFileFor(const TileId &tileId) const
{
    QString const outputDir = QString("%1/osm/%2/%3").arg(m_cacheDir).arg(tileId.zoomLevel()).arg(tileId.x());
    return QString("%1/%2.o5m").arg(outputDir).arg(tileId.y());
}

void TileDirectory::printProgress(double progress, int barWidth)
{
    int const position = barWidth * progress;
    cout << " [" << string(position, '=') << ">";
    cout << string(barWidth - position, ' ') << "]  " << std::right << setw(3) << int(progress * 100.0) << "%";
}

GeoDataLatLonBox TileDirectory::boundingBox() const
{
    return m_boundingBox;
}

void TileDirectory::setBoundingBox(const GeoDataLatLonBox &boundingBox)
{
    m_boundingBox = boundingBox;
}

void TileDirectory::setBoundingPolygon(const QString &file)
{
    m_boundingPolygon.clear();
    QFile input(file);
    QString country = "Unknown";
    if (input.open(QFile::ReadOnly)) {
        QTextStream stream(&input);
        country = stream.readLine();
        double lat(0.0), lon(0.0);
        GeoDataLinearRing box;
        while (!stream.atEnd()) {
            bool inside = true;
            QString line = stream.readLine().trimmed();
            QStringList entries = line.split(QLatin1Char(' '), Qt::SkipEmptyParts);
            if (entries.size() == 1) {
                if (entries.first() == QLatin1StringView("END") && inside) {
                    inside = false;
                    if (!box.isEmpty()) {
                        m_boundingPolygon << box;
                        box = GeoDataLinearRing();
                    }
                } else if (entries.first() == QLatin1StringView("END") && !inside) {
                    qDebug() << "END not expected here";
                } else if (entries.first().startsWith(QLatin1StringView("!"))) {
                    qDebug() << "Warning: Negative polygons not supported, skipping";
                } else {
                    // int number = entries.first().toInt();
                    inside = true;
                }
            } else if (entries.size() == 2) {
                lon = entries.first().toDouble();
                lat = entries.last().toDouble();
                GeoDataCoordinates point(lon, lat, 0.0, GeoDataCoordinates::Degree);
                box << point;
            } else {
                qDebug() << "Warning: Ignoring line in" << file << "with" << entries.size() << "fields:" << line;
            }
        }
    }

    if (!m_boundingPolygon.isEmpty()) {
        m_boundingBox = GeoDataLatLonBox::fromLineString(m_boundingPolygon.first());
        for (int i = 1, n = m_boundingPolygon.size(); i < n; ++i) {
            m_boundingBox |= GeoDataLatLonBox::fromLineString(m_boundingPolygon[i]);
        }
    } else {
        m_boundingBox = boundingBox(m_inputFile);
    }
}

void TileDirectory::createTiles()
{
    if (m_tileType == OpenStreetMap) {
        createOsmTiles();
        return;
    }

    QString const landmassDir = QString("%1/land-polygons-split-4326").arg(m_cacheDir);
    m_inputFile = QString("%1/land_polygons.shp").arg(landmassDir);
    auto const landmassZip = QString("%1/%2").arg(m_cacheDir).arg(m_landmassFile);
    if (!QFileInfo::exists(landmassZip)) {
        QString const url = QString("https://osmdata.openstreetmap.de/download/%1").arg(m_landmassFile);
        download(url, landmassZip);
    }

    if (!QFileInfo::exists(landmassDir)) {
        MarbleZipReader unzip(landmassZip);
        if (!unzip.extractAll(m_cacheDir)) {
            qWarning() << "Failed to extract" << landmassZip << "to" << m_cacheDir;
        }
    }

    QSharedPointer<GeoDataDocument> map;
    QSharedPointer<VectorClipper> clipper;
    TileIterator iter(m_boundingBox, m_zoomLevel);
    qint64 count = 0;
    for (auto const &tileId : iter) {
        ++count;
        QString const outputDir = QString("%1/%2").arg(m_baseDir).arg(tileId.x());
        QString const outputFile = QString("%1/%2.o5m").arg(outputDir).arg(tileId.y());
        if (QFileInfo::exists(outputFile)) {
            continue;
        }

        printProgress(count / double(iter.total()));
        cout << " Creating landmass cache tile " << count << "/" << iter.total() << " (";
        cout << m_zoomLevel << "/" << tileId.x() << "/" << tileId.y() << ')' << string(20, ' ') << '\r';
        cout.flush();

        QDir().mkpath(outputDir);
        if (!clipper) {
            map = open(m_inputFile, m_manager);
            if (!map) {
                qCritical() << "Failed to open " << m_inputFile
                            << ". This can happen when Marble was compiled without shapelib (libshp), when the system has too little memory (RAM + swap need "
                               "to be at least 8G), or when the download of the landmass data file failed.";
            }
            clipper = QSharedPointer<VectorClipper>(new VectorClipper(map.data(), m_zoomLevel));
        }
        std::unique_ptr<GeoDataDocument> tile(clipper->clipTo(m_zoomLevel, tileId.x(), tileId.y()));
        if (!GeoDataDocumentWriter::write(outputFile, *tile)) {
            qWarning() << "Failed to write tile" << outputFile;
        }
    }
    printProgress(1.0);
    cout << "  landmass cache tiles complete." << string(20, ' ') << endl;
}

void TileDirectory::createOsmTiles() const
{
    const GeoSceneMercatorTileProjection tileProjection;
    const QRect rect = tileProjection.tileIndexes(m_boundingBox, m_zoomLevel);
    TileCoordsPyramid pyramid(0, m_zoomLevel);
    pyramid.setBottomLevelCoords(rect);

    qint64 const maxCount = pyramid.tilesCount();
    QMap<int, QList<TileId>> tileLevels;
    for (int zoomLevel = 0; zoomLevel <= m_zoomLevel; ++zoomLevel) {
        QRect const rect = pyramid.coords(zoomLevel);
        if (zoomLevel < m_zoomLevel && rect.width() * rect.height() < 2) {
            continue;
        }

        TileIterator iter(m_boundingBox, zoomLevel);
        for (auto const &tileId : iter) {
            tileLevels[zoomLevel] << TileId(0, zoomLevel, tileId.x(), tileId.y());
        }
    }

    bool hasAllTiles = true;
    for (auto const &tileId : tileLevels[m_zoomLevel]) {
        auto const outputFile = osmFileFor(tileId);
        if (!QFileInfo::exists(outputFile)) {
            hasAllTiles = false;
            break;
        }
    }

    bool first = true;
    if (!hasAllTiles) {
        qint64 count = 0;
        for (auto const &tiles : std::as_const(tileLevels)) {
            for (auto const &tileId : tiles) {
                ++count;
                QString const inputFile =
                    first ? m_inputFile : QString("%1/osm/%2/%3/%4.o5m").arg(m_cacheDir).arg(tileId.zoomLevel() - 1).arg(tileId.x() >> 1).arg(tileId.y() >> 1);
                QString const outputFile = osmFileFor(tileId);
                if (QFileInfo::exists(outputFile)) {
                    continue;
                }

                printProgress(count / double(maxCount));
                cout << " Creating osm cache tile " << count << "/" << maxCount << " (";
                cout << tileId.zoomLevel() << "/" << tileId.x() << "/" << tileId.y() << ')' << string(20, ' ') << '\r';
                cout.flush();

                QDir().mkpath(QFileInfo(outputFile).absolutePath());
                QString const output = QString("-o=%1").arg(outputFile);

                const GeoDataLatLonBox tileBoundary = m_tileProjection.geoCoordinates(tileId.zoomLevel(), tileId.x(), tileId.y());

                double const minLon = tileBoundary.west(GeoDataCoordinates::Degree);
                double const maxLon = tileBoundary.east(GeoDataCoordinates::Degree);
                double const maxLat = tileBoundary.north(GeoDataCoordinates::Degree);
                double const minLat = tileBoundary.south(GeoDataCoordinates::Degree);
                QString const bbox = QString("-b=%1,%2,%3,%4").arg(minLon).arg(minLat).arg(maxLon).arg(maxLat);
                QProcess osmconvert;
                osmconvert.start("osmconvert",
                                 QStringList() << "--drop-author" << "--drop-version"
                                               << "--complete-ways" << "--complex-ways" << bbox << output << inputFile);
                osmconvert.waitForFinished(10 * 60 * 1000);
                if (osmconvert.exitCode() != 0) {
                    qWarning() << osmconvert.readAllStandardError();
                    qWarning() << "osmconvert failed: " << osmconvert.errorString();
                }
            }
            first = false;
        }
    }

    tileLevels.remove(m_zoomLevel);
    for (auto const &tiles : std::as_const(tileLevels)) {
        for (auto const &tileId : tiles) {
            QFile::remove(osmFileFor(tileId));
        }
    }

    printProgress(1.0);
    cout << "  osm cache tiles complete." << string(20, ' ') << endl;
}

int TileDirectory::innerNodes(const TileId &tile) const
{
    const GeoDataLatLonBox tileBoundary = m_tileProjection.geoCoordinates(tile.zoomLevel(), tile.x(), tile.y());

    double const west = tileBoundary.west();
    double const east = tileBoundary.east();
    double const north = tileBoundary.north();
    double const south = tileBoundary.south();
    QList<GeoDataCoordinates> bounds;
    bounds << GeoDataCoordinates(west, north);
    bounds << GeoDataCoordinates(east, north);
    bounds << GeoDataCoordinates(east, south);
    bounds << GeoDataCoordinates(west, south);

    int innerNodes = 0;
    if (m_boundingPolygon.isEmpty()) {
        for (auto const &coordinate : std::as_const(bounds)) {
            if (m_boundingBox.contains(coordinate)) {
                ++innerNodes;
            }
        }
        return innerNodes;
    }

    for (auto const &coordinate : std::as_const(bounds)) {
        for (auto const &ring : m_boundingPolygon) {
            if (ring.contains(coordinate)) {
                ++innerNodes;
            }
        }
    }
    return innerNodes;
}

void TileDirectory::updateProgress()
{
    double const progress = m_download->total > 0 ? m_download->received / double(m_download->total) : 0.0;
    printProgress(progress);

    cout << "  ";
    cout << std::fixed << std::setprecision(1) << m_download->received / 1000000.0 << '/';
    cout << std::fixed << std::setprecision(1) << m_download->total / 1000000.0 << " MB";

    cout << "  Downloading " << m_download->reply->url().fileName().toStdString();

    cout << string(20, ' ') << '\r';
    cout.flush();
}

void TileDirectory::handleFinishedDownload(const QString &filename, const QString &id)
{
    qDebug() << "File " << filename << "(" << id << ") has been downloaded.";
}

GeoDataLatLonBox TileDirectory::boundingBox(const QString &filename) const
{
    QProcess osmconvert;
    osmconvert.start("osmconvert", QStringList() << "--out-statistics" << filename);
    osmconvert.waitForFinished(10 * 60 * 1000);
    QStringList const output = QString(osmconvert.readAllStandardOutput()).split('\n');
    GeoDataLatLonBox boundingBox;
    for (QString const &line : output) {
        if (line.startsWith("lon min:")) {
            boundingBox.setWest(QStringView{line}.mid(8).toDouble(), GeoDataCoordinates::Degree);
        } else if (line.startsWith("lon max")) {
            boundingBox.setEast(QStringView{line}.mid(8).toDouble(), GeoDataCoordinates::Degree);
        } else if (line.startsWith("lat min:")) {
            boundingBox.setSouth(QStringView{line}.mid(8).toDouble(), GeoDataCoordinates::Degree);
        } else if (line.startsWith("lat max:")) {
            boundingBox.setNorth(QStringView{line}.mid(8).toDouble(), GeoDataCoordinates::Degree);
        }
    }
    return boundingBox;
}

void Download::updateProgress(qint64 received_, qint64 total_)
{
    received = received_;
    total = total_;

    QString const tempFile = QString("%1.download").arg(target);
    if (!m_file.isOpen()) {
        m_file.setFileName(tempFile);
        m_file.open(QFile::WriteOnly);
    }
    m_file.write(reply->readAll());

    if (reply->isFinished()) {
        m_file.flush();
        m_file.close();
        QFile::rename(tempFile, target);
    }
}

}

#include "moc_TileDirectory.cpp"
