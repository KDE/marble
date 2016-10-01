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

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QElapsedTimer>
#include <QSharedPointer>
#include <QFileInfo>

#include <QMessageLogContext>
#include <QProcess>

#include "VectorClipper.h"
#include "NodeReducer.h"
#include "WayConcatenator.h"
#include "TileIterator.h"
#include "TileDirectory.h"

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

QString tileFileName(const QCommandLineParser &parser, const QString &outputName, int x, int y, int zoomLevel)
{
    QString const extension = parser.value("extension");
    QString const baseDir = parser.isSet("output") ? (outputName + QLatin1Char('/')) : QString();
    QString const outputDir = QString("%1%2/%3").arg(baseDir).arg(zoomLevel).arg(x);
    QDir().mkpath(outputDir);
    QString const outputFile = QString("%1/%2.%3").arg(outputDir).arg(y).arg(extension);
    return outputFile;
}

bool writeTile(GeoDataDocument* tile, const QString &outputFile)
{
    if (tile->size() == 0) {
        return true;
    }
    if (!GeoDataDocumentWriter::write(outputFile, *tile)) {
        qWarning() << "Could not write the file " << outputFile;
        return false;
    }
    return true;
}

GeoDataLatLonBox boundingBox(const QString &inputFileName)
{
    QProcess osmconvert;
    osmconvert.start("osmconvert", QStringList() << "--out-statistics" << inputFileName);
    osmconvert.waitForFinished();
    QStringList const output = QString(osmconvert.readAllStandardOutput()).split('\n');
    GeoDataLatLonBox boundingBox;
    foreach(QString const &line, output) {
        if (line.startsWith("lon min:")) {
            boundingBox.setWest(line.mid(8).toDouble(), GeoDataCoordinates::Degree);
        } else if (line.startsWith("lon max")) {
            boundingBox.setEast(line.mid(8).toDouble(), GeoDataCoordinates::Degree);
        } else if (line.startsWith("lat min:")) {
            boundingBox.setSouth(line.mid(8).toDouble(), GeoDataCoordinates::Degree);
        } else if (line.startsWith("lat max:")) {
            boundingBox.setNorth(line.mid(8).toDouble(), GeoDataCoordinates::Degree);
        }
    }
    return boundingBox;
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
                          {{"k", "keep-all-nodes"}, "Do not reduce nodes in line strings and rings."},
                          {{"m", "merge"}, "Merge the main document with the file <file_to_merge_with>.", "file_to_merge_with"},
                          {{"z", "zoom-level"}, "Zoom level according to which OSM information has to be processed.", "number"},
                          {{"o", "output"}, "Output file or directory", "output"},
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

    QString inputFileName = args.at(0);
    auto const levels = parser.value("zoom-level").split(',');
    QVector<unsigned int> zoomLevels;
    foreach(auto const &level, levels) {
        zoomLevels << level.toInt();
    }

    if (zoomLevels.isEmpty()) {
        parser.showHelp(1);
        return 1;
    }

    QString outputName;
    if(parser.isSet("output")) {
        outputName = parser.value("output");
    } else {
        outputName = "s_" + inputFileName;
    }

    QFileInfo file( inputFileName );
    if ( !file.exists() ) {
        qWarning() << "File " << file.absoluteFilePath() << " does not exist. Exiting.";
        return 2;
    }

    MarbleModel model;
    ParsingRunnerManager manager(model.pluginManager());

    bool const keepAllNodes = parser.isSet("keep-all-nodes");
    bool const overwriteTiles = parser.value("conflict-resolution") == "overwrite";
    if (parser.isSet("osmconvert")) {
        QString const extension = parser.value("extension");
        auto mapArea = boundingBox(inputFileName);
        foreach(auto zoomLevel, zoomLevels) {
            int const N = pow(2, zoomLevel);
            TileIterator iter(mapArea, zoomLevel);
            qint64 count = 0;
            qint64 const total = iter.total();
            foreach(auto const &tileId, iter) {
                ++count;
                QString directory = QString("%1/%2/%3").arg(outputName).arg(zoomLevel).arg(tileId.x());
                QString const output = QString("-o=%1/%2.%3").arg(directory).arg(tileId.y()).arg(extension);
                if (!overwriteTiles && QFileInfo(output).exists()) {
                    continue;
                }
                QDir().mkpath(directory);
                double const minLon = TileId::tileX2lon(tileId.x(), N) * RAD2DEG;
                double const maxLon = TileId::tileX2lon(tileId.x()+1, N) * RAD2DEG;
                double const maxLat = TileId::tileY2lat(tileId.y(), N) * RAD2DEG;
                double const minLat = TileId::tileY2lat(tileId.y()+1, N) * RAD2DEG;
                QString const bbox = QString("-b=%1,%2,%3,%4").arg(minLon).arg(minLat).arg(maxLon).arg(maxLat);
                QProcess osmconvert;
                osmconvert.start("osmconvert", QStringList() << "--drop-author" << "--drop-version"
                                 << "--complete-ways" << "--complex-ways" << bbox << output << inputFileName);
                osmconvert.waitForFinished();
                std::cout << "Tile " << count << "/" << total << " done.      \r";
                std::cout.flush();
            }
        }
    } else if (*zoomLevels.cbegin() <= 9) {
        auto map = TileDirectory::open(inputFileName, manager);
        VectorClipper processor(map.data());
        GeoDataLatLonBox world(85.0, -85.0, 180.0, -180.0, GeoDataCoordinates::Degree);
        foreach(auto zoomLevel, zoomLevels) {
            TileIterator iter(world, zoomLevel);
            qint64 count = 0;
            qint64 const total = iter.total();
            foreach(auto const &tileId, iter) {
                ++count;
                QString const filename = tileFileName(parser, outputName, tileId.x(), tileId.y(), zoomLevel);
                if (!overwriteTiles && QFileInfo(filename).exists()) {
                    continue;
                }
                GeoDataDocument* tile = processor.clipTo(zoomLevel, tileId.x(), tileId.y());
                QSharedPointer<NodeReducer> reducer = QSharedPointer<NodeReducer>(keepAllNodes ? nullptr : new NodeReducer(tile, zoomLevel+1));
                if (!writeTile(tile, filename)) {
                    return 4;
                }
                std::cout << "Tile " << count << "/" << total << " (" << tile->name().toStdString() << ") done.";
                if (reducer) {
                    double const reduction = reducer->removedNodes() / qMax(1.0, double(reducer->remainingNodes() + reducer->removedNodes()));
                    std::cout << " Node reduction: " << qRound(reduction * 100.0) << "%";
                }
                std::cout << "      \r";
                std::cout.flush();
                delete tile;
            }
        }
    } else {
        TileDirectory loader(parser.value("merge"), manager, parser.value("extension"));
        TileDirectory mapTiles("tiles/10", manager, parser.value("extension"));
        mapTiles.setFilterTags(true);

        typedef QMap<QString, QVector<TileId> > Tiles;
        Tiles tiles;

        qint64 total = 0;
        foreach(auto zoomLevel, zoomLevels) {
            // @todo FIXME Assumes placemark ownership
            //WayConcatenator concatenator(tagsFilter.accepted(), QStringList() << "highway=*", false);

            TileIterator iter(boundingBox(inputFileName), zoomLevel);
            total += iter.total();
            foreach(auto const &tileId, iter) {
                auto const tile = TileId(QString(), zoomLevel, tileId.x(), tileId.y());
                auto const mapTile = mapTiles.tileFor(zoomLevel, tileId.x(), tileId.y());
                auto const name = QString("%1/%2/%3").arg(mapTile.zoomLevel()).arg(mapTile.x()).arg(mapTile.y());
                tiles[name] << tile;
            }
        }

        qint64 count = 0;
        foreach(auto const &tileList, tiles) {
            foreach(auto const &tileId, tileList) {
                ++count;
                int const zoomLevel = tileId.zoomLevel();
                QString const filename = tileFileName(parser, outputName, tileId.x(), tileId.y(), zoomLevel);
                if (!overwriteTiles && QFileInfo(filename).exists()) {
                    continue;
                }
                GeoDataDocument* tile1 = mapTiles.clip(zoomLevel, tileId.x(), tileId.y());
                GeoDataDocument* tile2 = loader.clip(zoomLevel, tileId.x(), tileId.y());
                GeoDataDocument* combined = mergeDocuments(tile1, tile2);
                QSharedPointer<NodeReducer> reducer = QSharedPointer<NodeReducer>(keepAllNodes ? nullptr : new NodeReducer(combined, zoomLevel));
                if (!writeTile(combined, filename)) {
                    return 4;
                }

                std::cout << "Tile " << count << "/" << total << " (landmass ";
                std::cout << loader.name().toStdString() << " + map ";
                std::cout << mapTiles.name().toStdString() << " ~> ";
                std::cout << combined->name().toStdString() << ") done.";
                if (reducer) {
                    double const reduction = reducer->removedNodes() / qMax(1.0, double(reducer->remainingNodes() + reducer->removedNodes()));
                    std::cout << " Node reduction: " << qRound(reduction * 100.0) << "%";
                }
                std::cout << "      \r";
                std::cout.flush();
                delete combined;
                delete tile1;
                delete tile2;
            }
        }
    }

    return 0;
}

