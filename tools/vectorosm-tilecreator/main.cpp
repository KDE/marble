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

enum DebugLevel {
    Debug,
    Info,
    Mute
};

DebugLevel debugLevel = Info;

void debugOutput( QtMsgType type, const QMessageLogContext &context, const QString &msg )
{
    switch ( type ) {
    case QtDebugMsg:
        if ( debugLevel == Debug ) {
            qDebug() << "Debug: " << context.file << ":" << context.line << " " << msg;
        }
        break;
    case QtInfoMsg:
        if ( debugLevel < Mute ) {
            qInfo() << "Info: " << context.file << ":" << context.line << " " << msg;
        }
        break;
    case QtWarningMsg:
        if ( debugLevel < Mute ) {
            qDebug() << "Warning: " << context.file << ":" << context.line << " " << msg;
        }
        break;
    case QtCriticalMsg:
        if ( debugLevel < Mute ) {
            qDebug() << "Critical: " << context.file << ":" << context.line << " " << msg;
        }
        break;
    case QtFatalMsg:
        if ( debugLevel < Mute ) {
            qDebug() << "Fatal: " << context.file << ":" << context.line << " " << msg;
            abort();
        }
    }
}

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

bool writeTile(const QCommandLineParser &parser, const QString &outputName, GeoDataDocument* tile, int x, int y, int zoomLevel)
{
    if (tile->size() == 0) {
        return true;
    }
    QString const extension = parser.value("extension");
    QString const baseDir = parser.isSet("output") ? (outputName + QLatin1Char('/')) : QString();
    QString const outputDir = QString("%1%2/%3").arg(baseDir).arg(zoomLevel).arg(x);
    QDir().mkpath(outputDir);
    QString const outputFile = QString("%1/%2.%3").arg(outputDir).arg(y).arg(extension);
    if (!GeoDataDocumentWriter::write(outputFile, *tile)) {
        qWarning() << "Could not write the file " << outputName;
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
                          {{"d", "debug"}, "Debug output in the terminal."},
                          {{"t", "osmconvert"}, "Tile data using osmconvert."},
                          {{"s", "silent"}, "Don't output to terminal."},
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
    bool debug = parser.isSet("debug");
    bool silent = parser.isSet("silent");
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

    if(debug) {
        debugLevel = Debug;
        qInstallMessageHandler( debugOutput );
    }
    if(silent) {
        debugLevel = Mute;
        qInstallMessageHandler( debugOutput );
    }


    QFileInfo file( inputFileName );
    if ( !file.exists() ) {
        qWarning() << "File " << file.absoluteFilePath() << " does not exist. Exiting.";
        return 2;
    }

    MarbleModel model;
    ParsingRunnerManager manager(model.pluginManager());

    bool const keepAllNodes = parser.isSet("keep-all-nodes");
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
                QDir().mkpath(directory);
                QString const output = QString("-o=%1/%2.%3").arg(directory).arg(tileId.y()).arg(extension);
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
                GeoDataDocument* tile = processor.clipTo(zoomLevel, tileId.x(), tileId.y());
                QSharedPointer<NodeReducer> reducer = QSharedPointer<NodeReducer>(keepAllNodes ? nullptr : new NodeReducer(tile, zoomLevel+1));
                if (!writeTile(parser, outputName, tile, tileId.x(), tileId.y(), zoomLevel)) {
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

        foreach(auto zoomLevel, zoomLevels) {
            // @todo FIXME Assumes placemark ownership
            //WayConcatenator concatenator(tagsFilter.accepted(), QStringList() << "highway=*", false);

            TileIterator iter(boundingBox(inputFileName), zoomLevel);
            qint64 count = 0;
            qint64 const total = iter.total();
            foreach(auto const &tileId, iter) {
                ++count;
                GeoDataDocument* tile1 = mapTiles.clip(zoomLevel, tileId.x(), tileId.y());
                GeoDataDocument* tile2 = loader.clip(zoomLevel, tileId.x(), tileId.y());
                GeoDataDocument* combined = mergeDocuments(tile1, tile2);
                QSharedPointer<NodeReducer> reducer = QSharedPointer<NodeReducer>(keepAllNodes ? nullptr : new NodeReducer(combined, zoomLevel));
                if (!writeTile(parser, outputName, combined, tileId.x(), tileId.y(), zoomLevel)) {
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

