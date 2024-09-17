// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 David Kolozsvari <freedawson@gmail.com>
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
//

#include "GeoDataDocumentWriter.h"
#include "MarbleModel.h"
#include "ParsingRunnerManager.h"
#include "GeoDataGeometry.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"
#include "GeoDataLatLonAltBox.h"
#include "TileId.h"
#include "MarbleDirs.h"
#ifdef STATIC_BUILD
#include "src/plugins/runner/osm/translators/O5mWriter.h"
#endif

#include <QGuiApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QUrl>

#include "VectorClipper.h"
#include "NodeReducer.h"
#include "TileIterator.h"
#include "TileDirectory.h"
#include "SpellChecker.h"

#ifdef STATIC_BUILD
#include <QtPlugin>
Q_IMPORT_PLUGIN(OsmPlugin)
Q_IMPORT_PLUGIN(ShpPlugin)
#endif

#include <iostream>

using namespace Marble;

static QString tileFileName(const QCommandLineParser &parser, int x, int y, int zoomLevel)
{
    QString const extension = parser.value("extension");
    QString const output = parser.isSet("development") ? QString("%1/maps/earth/vectorosm-dev").arg(MarbleDirs::localPath()) : parser.value("output");
    QString const outputDir = QString("%1/%2/%3").arg(output).arg(zoomLevel).arg(x);
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
    QGuiApplication app(argc, argv);

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
                          {{"s", "spellcheck"}, "Use this geonames.org cities file for spell-checking city names", "spellcheck"},
                          {"verbose", "Increase amount of shell output information"},
                          {{"d", "development"}, "Use local development vector osm map theme as output storage"},
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
    for(auto const &level: levels) {
        int const zoomLevel = level.toInt();
        maxZoomLevel = qMax(zoomLevel, maxZoomLevel);
        zoomLevels << zoomLevel;
    }

    if (zoomLevels.isEmpty()) {
        parser.showHelp(1);
        return 1;
    }

    // work around MARBLE_ADD_WRITER not working for static builds
#ifdef STATIC_BUILD
    GeoDataDocumentWriter::registerWriter(new O5mWriter, QStringLiteral("o5m"));
#endif

    bool const overwriteTiles = parser.value("conflict-resolution") == "overwrite";

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
        if (parser.isSet("spellcheck")) {
            SpellChecker spellChecker(parser.value("spellcheck"));
            spellChecker.setVerbose(parser.isSet("verbose"));
            spellChecker.correctPlaceLabels(map.data()->placemarkList());
        }
        for(auto zoomLevel: zoomLevels) {
            TileIterator iter(world, zoomLevel);
            qint64 count = 0;
            qint64 const total = iter.total();
            for(auto const &tileId: iter) {
                ++count;
                QString const filename = tileFileName(parser, tileId.x(), tileId.y(), zoomLevel);
                if (!overwriteTiles && QFileInfo(filename).exists()) {
                    continue;
                }
                GeoDataDocument* tile = processor.clipTo(zoomLevel, tileId.x(), tileId.y());
                if (!tile->isEmpty()) {
                    NodeReducer nodeReducer(tile, TileId(0, zoomLevel, tileId.x(), tileId.y()));
                    if (!writeTile(tile, filename)) {
                        return 4;
                    }
                    TileDirectory::printProgress(count / double(total));
                    std::cout << " Tile " << count << "/" << total << " (" << tile->name().toStdString() << ") done.";
                    double const reduction = nodeReducer.removedNodes() / qMax(1.0, double(nodeReducer.remainingNodes() + nodeReducer.removedNodes()));
                    std::cout << " Node reduction: " << qRound(reduction * 100.0) << "%";
                } else {
                    TileDirectory::printProgress(count / double(total));
                    std::cout << " Skipping empty tile " << count << "/" << total << " (" << tile->name().toStdString() << ").";
                }
                std::cout << std::string(20, ' ') << '\r';
                std::cout.flush();
                delete tile;
            }
        }
    } else {
        qWarning() << "high zoom level tiles cannot be created with this tool!";
        return 1;
    }

    return 0;
}
