//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#include "GeoDataTypes.h"
#include "GeoDataDocumentWriter.h"
#include "MarbleModel.h"
#include "ParsingRunnerManager.h"
#include "GeoDataGeometry.h"
#include "GeoDataPlacemark.h"
#include "GeoDataLatLonAltBox.h"
#include "TileId.h"
#include "MarbleDirs.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QElapsedTimer>
#include <QSharedPointer>
#include <QFileInfo>
#include <QUrl>
#include <QBuffer>

#include <QMessageLogContext>
#include <QProcess>

#include "VectorClipper.h"
#include "NodeReducer.h"
#include "WayConcatenator.h"
#include "TileIterator.h"
#include "TileDirectory.h"
#include "MbTileWriter.h"

#include <iostream>

using namespace Marble;

GeoDataDocument* mergeDocuments(GeoDataDocument* map1, GeoDataDocument* map2)
{
    GeoDataDocument* mergedMap = new GeoDataDocument(*map1);

    OsmPlacemarkData marbleLand;
    marbleLand.addTag("marble_land","landmass");
    foreach (auto placemark, map2->placemarkList()) {
        GeoDataPlacemark* land = new GeoDataPlacemark(*placemark);
        if(land->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType) {
            land->setOsmData(marbleLand);
        }
        mergedMap->append(land);
    }

    return mergedMap;
}

QString tileFileName(const QCommandLineParser &parser, int x, int y, int zoomLevel)
{
    QString const extension = parser.value("extension");
    QString const outputDir = QString("%1/%2/%3").arg(parser.value("output")).arg(zoomLevel).arg(x);
    QString const outputFile = QString("%1/%2.%3").arg(outputDir).arg(y).arg(extension);
    return outputFile;
}

bool writeTile(GeoDataDocument* tile, const QString &outputFile)
{
    QDir().mkpath(QFileInfo(outputFile).path());
    if (!GeoDataDocumentWriter::write(outputFile, *tile)) {
        qWarning() << "Could not write the file " << outputFile;
        return false;
    }
    return true;
}

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QCoreApplication::setApplicationName("marble-vectorosm-tilecreator");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("A tool for Marble, which is used to reduce the details of osm maps.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("input", "The input .osm or .shp file.");

    parser.addOptions({
                          {{"t", "osmconvert"}, "Tile data using osmconvert."},
                          {"conflict-resolution", "How to deal with existing tiles: overwrite, skip or merge", "mode", "overwrite"},
                          {{"c", "cache-directory"}, "Directory for temporary data.", "cache", "cache"},
                          {{"m", "mbtile"}, "Store tiles at level 15 onwards in a mbtile database.", "mbtile"},
                          {{"z", "zoom-level"}, "Zoom level according to which OSM information has to be processed.", "levels", "11,13,15,17"},
                          {{"o", "output"}, "Output file or directory", "output", QString("%1/maps/earth/vectorosm").arg(MarbleDirs::localPath())},
                          {{"e", "extension"}, "Output file type: o5m (default), osm or kml", "file extension", "o5m"}
                      });

    // Process the actual command line arguments given by the user
    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.isEmpty()) {
        parser.showHelp();
        return 0;
    }
    // input is args.at(0), output is args.at(1)

    QString const extension = parser.value("extension");
    QString inputFileName = args.at(0);
    auto const levels = parser.value("zoom-level").split(',');
    QVector<unsigned int> zoomLevels;
    int maxZoomLevel = 0;
    foreach(auto const &level, levels) {
        int const zoomLevel = level.toInt();
        maxZoomLevel = qMax(zoomLevel, maxZoomLevel);
        zoomLevels << zoomLevel;
    }

    if (zoomLevels.isEmpty()) {
        parser.showHelp(1);
        return 1;
    }

    bool const overwriteTiles = parser.value("conflict-resolution") == "overwrite";
    QSharedPointer<MbTileWriter> mbtileWriter;
    if (parser.isSet("mbtile")) {
        QString const mbtile = parser.value("mbtile");
        mbtileWriter = QSharedPointer<MbTileWriter>(new MbTileWriter(mbtile, extension));
        mbtileWriter->setReportProgress(false);
        mbtileWriter->setCommitInterval(500);
    }

    MarbleModel model;
    ParsingRunnerManager manager(model.pluginManager());
    QString const cacheDirectory = parser.value("cache-directory");
    QDir().mkpath(cacheDirectory);
    if (!QFileInfo(cacheDirectory).isWritable()) {
        qWarning() << "Cannot write to cache directory" << cacheDirectory;
        parser.showHelp(1);
    }

    if (*zoomLevels.cbegin() <= 9) {
        auto map = TileDirectory::open(inputFileName, manager);
        VectorClipper processor(map.data(), maxZoomLevel);
        GeoDataLatLonBox world(85.0, -85.0, 180.0, -180.0, GeoDataCoordinates::Degree);
        foreach(auto zoomLevel, zoomLevels) {
            TileIterator iter(world, zoomLevel);
            qint64 count = 0;
            qint64 const total = iter.total();
            foreach(auto const &tileId, iter) {
                ++count;
                QString const filename = tileFileName(parser, tileId.x(), tileId.y(), zoomLevel);
                if (!overwriteTiles && QFileInfo(filename).exists()) {
                    continue;
                }
                GeoDataDocument* tile = processor.clipTo(zoomLevel, tileId.x(), tileId.y());
                NodeReducer nodeReducer(tile, zoomLevel+1);
                if (!writeTile(tile, filename)) {
                    return 4;
                }
                std::cout << "Tile " << count << "/" << total << " (" << tile->name().toStdString() << ") done.";
                double const reduction = nodeReducer.removedNodes() / qMax(1.0, double(nodeReducer.remainingNodes() + nodeReducer.removedNodes()));
                std::cout << " Node reduction: " << qRound(reduction * 100.0) << "%";
                std::cout << "      \r";
                std::cout.flush();
                delete tile;
            }
        }
    } else {
        TileDirectory mapTiles(TileDirectory::OpenStreetMap, cacheDirectory, manager, extension, maxZoomLevel);
        mapTiles.setInputFile(inputFileName);
        mapTiles.createTiles();
        auto const boundingBox = mapTiles.boundingBox();

        TileDirectory loader(TileDirectory::Landmass, cacheDirectory, manager, extension, maxZoomLevel);
        loader.setBoundingBox(boundingBox);
        loader.createTiles();

        typedef QMap<QString, QVector<TileId> > Tiles;
        Tiles tiles;

        qint64 total = 0;
        foreach(auto zoomLevel, zoomLevels) {
            // @todo FIXME Assumes placemark ownership
            //WayConcatenator concatenator(tagsFilter.accepted(), QStringList() << "highway=*", false);

            TileIterator iter(mapTiles.boundingBox(), zoomLevel);
            total += iter.total();
            foreach(auto const &tileId, iter) {
                auto const tile = TileId(QString(), zoomLevel, tileId.x(), tileId.y());
                if (mapTiles.contains(tile)) {
                    auto const mapTile = mapTiles.tileFor(zoomLevel, tileId.x(), tileId.y());
                    auto const name = QString("%1/%2/%3").arg(mapTile.zoomLevel()).arg(mapTile.x()).arg(mapTile.y());
                    tiles[name] << tile;
                }
            }
        }

        qint64 count = 0;
        foreach(auto const &tileList, tiles) {
            foreach(auto const &tileId, tileList) {
                ++count;
                int const zoomLevel = tileId.zoomLevel();
                QString const filename = tileFileName(parser, tileId.x(), tileId.y(), zoomLevel);
                if (!overwriteTiles) {
                    if (zoomLevel > 13 && mbtileWriter && mbtileWriter->hasTile(tileId.x(), tileId.y(), zoomLevel)) {
                        continue;
                    } else if (QFileInfo(filename).exists()) {
                        continue;
                    }
                }
                GeoDataDocument* tile1 = mapTiles.clip(zoomLevel, tileId.x(), tileId.y());
                TagsFilter::removeAnnotationTags(tile1);
                GeoDataDocument* tile2 = loader.clip(zoomLevel, tileId.x(), tileId.y());
                GeoDataDocument* combined = mergeDocuments(tile1, tile2);
                NodeReducer nodeReducer(combined, zoomLevel);
                if (zoomLevel > 13 && mbtileWriter) {
                    QBuffer buffer;
                    buffer.open(QBuffer::ReadWrite);
                    if (GeoDataDocumentWriter::write(&buffer, *combined, extension)) {
                        buffer.seek(0);
                        mbtileWriter->addTile(&buffer, tileId.x(), tileId.y(), zoomLevel);
                    } else {
                        qWarning() << "Could not write the tile " << combined->name();
                    }
                } else {
                    if (!writeTile(combined, filename)) {
                        return 4;
                    }
                }

                TileDirectory::printProgress(count / double(total));
                std::cout << "  Tile " << count << "/" << total << " (";
                std::cout << combined->name().toStdString() << ").";
                double const reduction = nodeReducer.removedNodes() / qMax(1.0, double(nodeReducer.remainingNodes() + nodeReducer.removedNodes()));
                std::cout << " Node reduction: " << qRound(reduction * 100.0) << "%";
                std::cout << "      \r";
                std::cout.flush();
                delete combined;
                delete tile1;
                delete tile2;
            }
        }
        TileDirectory::printProgress(1.0);
        std::cout << "  Vector OSM tiles complete." << std::string(30, ' ') << std::endl;
    }

    return 0;
}
