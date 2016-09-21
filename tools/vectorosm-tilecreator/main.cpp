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

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QElapsedTimer>
#include <QSharedPointer>

#include <QMessageLogContext>

#include "VectorClipper.h"
#include "NodeReducer.h"
#include "WayConcatenator.h"
#include "TileIterator.h"

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

QStringList tagsFilteredIn(int zoomLevel)
{
    QStringList tags;
    tags << "highway=motorway" << "highway=motorway_link";
    tags << "highway=trunk" << "highway=trunk_link";
    tags << "highway=primary" << "highway=primary_link";
    tags << "highway=secondary" << "highway=secondary_link";

    if (zoomLevel >= 13) {
        tags << "highway=tertiary" << "highway=tertiary_link";
        tags << "highway=track";

        tags << "public_transport=station";
        tags << "railway=light_rail";
        tags << "railway=monorail";
        tags << "railway=narrow_gauge";
        tags << "railway=preserved";
        tags << "railway=rail";
        tags << "railway=subway";
        tags << "railway=tram";

        tags << "natural=scrub";
        tags << "natural=heath";
        tags << "natural=grassland";
        tags << "natural=glacier";
        tags << "natural=beach";
        tags << "natural=coastline";
        tags << "natural=water";
        tags << "natural=wood";
        tags << "leisure=stadium";
        tags << "tourism=alpine_hut";

        tags << "waterway=river";
        tags << "waterway=stream";
        tags << "waterway=canal";

        tags << "place=suburb";
        tags << "place=village";

        tags << "natural=peak";
    }

    if (zoomLevel == 13) {
        tags << "landuse=forest";
        tags << "landuse=meadow";
        tags << "landuse=farmland";
    }

    if (zoomLevel >= 15) {
        tags << "highway=unclassified";
        tags << "highway=residential";

        tags << "landuse=*";

        tags << "leisure=pitch";
        tags << "leisure=swimming_area";

        tags << "place=hamlet";
        tags << "place=isolated_dwelling";

        tags << "man_made=beacon";
        tags << "man_made=bridge";
        tags << "man_made=campanile";
        tags << "man_made=chimney";
        tags << "man_made=communications_tower";
        tags << "man_made=cross";
        tags << "man_made=gasometer";
        tags << "man_made=lighthouse";
        tags << "man_made=tower";
        tags << "man_made=water_tower";
        tags << "man_made=windmill";
    }

    tags << "leisure=nature_reserve";
    tags << "leisure=park";

    tags << "place=city";
    tags << "place=town";
    tags << "place=locality";

    tags << "boundary=administrative";
    tags << "boundary=political";
    tags << "boundary=national_park";
    tags << "boundary=protected_area";
    return tags;
}

QSharedPointer<GeoDataDocument> open(const QString &filename, ParsingRunnerManager &manager)
{
    QElapsedTimer timer;
    timer.start();

    // Timeout is set to 10 min. If the file is reaaally huge, set it to something bigger.
    GeoDataDocument* map = manager.openFile(filename, DocumentRole::MapDocument, 600000);
    if(map == nullptr) {
        qWarning() << "File" << filename << "couldn't be loaded.";
    } else {
        qint64 parsingTime = timer.elapsed();
        qDebug() << "Opened" << filename << "after" << parsingTime << "ms";
    }
    QSharedPointer<GeoDataDocument> result = QSharedPointer<GeoDataDocument>(map);
    return result;
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
    QString const extension = parser.value("extension");
    QString const baseDir = parser.isSet("output") ? (outputName + QLatin1Char('/')) : QString();
    QString const outputDir = QString("%1%2/%3").arg(baseDir).arg(zoomLevel).arg(x);
    QDir().mkpath(outputDir);
    QString const outputFile = QString("%1/%2.%3").arg(outputDir).arg(y).arg(extension);
    if (!GeoDataDocumentWriter::write(outputFile, *tile)) {
        qDebug() << "Could not write the file " << outputName;
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
                          {{"d","debug"}, "Debug output in the terminal."},
                          {{"s","silent"}, "Don't output to terminal."},
                          {{"l","landmass"}, "Convert the given <landmass> file and reduce nodes"},
                          {{"m","merge"}, "Merge the main document with the file <file_to_merge_with>. This works together with the -c flag.", "file_to_merge_with"},
                          {{"z", "zoom-level"}, "Zoom level according to which OSM information has to be processed.", "number"},
                          {{"o", "output"}, "Output file or directory", "output"},
                          {{"e", "extension"}, "Output file type: osm (default), o5m or kml", "file extension", "osm"}
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
    unsigned int zoomLevel = parser.value("zoom-level").toInt();

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

    if (parser.isSet("landmass")) {
        auto map = open(parser.value("landmass"), manager);
        if(!map) {
            qWarning() << "File" << inputFileName << "couldn't be loaded.";
            return -2;
        }

        OsmPlacemarkData marbleLand;
        marbleLand.addTag("marble_land","landmass");
        foreach(auto land, map->placemarkList()) {
            if(land->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType) {
                land->setOsmData(marbleLand);
            }
        }

        NodeReducer reducer(map.data(), zoomLevel+1);

        QString const extension = parser.value("extension");
        QString const outputFile = QString("%1/landmass-level-%2.%3").arg(outputName).arg(zoomLevel).arg(extension);
        if (!GeoDataDocumentWriter::write(outputFile, *map)) {
            return 4;
        }
        qDebug() << "Landmass file " << outputFile << " done";

    } else if (zoomLevel <= 9) {
        auto map = open(inputFileName, manager);
        VectorClipper processor(map.data());
        GeoDataLatLonBox world(85.0, -85.0, 180.0, -180.0, GeoDataCoordinates::Degree);
        TileIterator iter(world, zoomLevel);
        qint64 count = 0;
        qint64 const total = iter.total();
        foreach(auto const &tileId, iter) {
            ++count;
            GeoDataDocument* tile = processor.clipTo(zoomLevel, tileId.x(), tileId.y());
            NodeReducer reducer(tile, zoomLevel+1);

            if (!writeTile(parser, outputName, tile, tileId.x(), tileId.y(), zoomLevel)) {
                return 4;
            }
            double const reduction = reducer.removedNodes() / qMax(1.0, double(reducer.remainingNodes() + reducer.removedNodes()));
            std::cout << "Tile " << count << "/" << total << " (" << tile->name().toStdString() << ") done.";
            std::cout << " Node reduction: " << qRound(reduction * 100.0) << "%      " << '\r';
            std::cout.flush();
            delete tile;
        }
    } else {
        auto map = open(inputFileName, manager);
        QStringList const tags = tagsFilteredIn(zoomLevel);
        GeoDataDocument* input = map.data();
        QSharedPointer<TagsFilter> tagsFilter;
        if (zoomLevel < 17) {
            tagsFilter = QSharedPointer<TagsFilter>(new TagsFilter(map.data(), tags));
            input = tagsFilter->accepted();
            map.clear();
        }
        VectorClipper processor(input);

        auto mergeMap = open(parser.value("merge"), manager);
        VectorClipper background(mergeMap.data());
        GeoDataDocument* landmass = background.clipTo(input->latLonAltBox());
        mergeMap.clear();
        VectorClipper landMassClipper(landmass);

        // @todo FIXME Assumes placemark ownership
        //WayConcatenator concatenator(tagsFilter.accepted(), QStringList() << "highway=*", false);

        TileIterator iter(input->latLonAltBox(), zoomLevel);
        qint64 count = 0;
        qint64 const total = iter.total();
        foreach(auto const &tileId, iter) {
            ++count;
            GeoDataDocument* tile1 = processor.clipTo(zoomLevel, tileId.x(), tileId.y());
            GeoDataDocument* tile2 = landMassClipper.clipTo(zoomLevel, tileId.x(), tileId.y());
            GeoDataDocument* combined = mergeDocuments(tile1, tile2);
            NodeReducer reducer(combined, zoomLevel+1);
            if (!writeTile(parser, outputName, combined, tileId.x(), tileId.y(), zoomLevel)) {
                return 4;
            }

            double const reduction = reducer.removedNodes() / qMax(1.0, double(reducer.remainingNodes() + reducer.removedNodes()));
            std::cout << "Tile " << count << "/" << total << " (" << combined->name().toStdString() << ") done.";
            std::cout << " Node reduction: " << qRound(reduction * 100.0) << "%      " << '\r';
            std::cout.flush();
            delete combined;
            delete tile1;
            delete tile2;
        }
    }

    return 0;
}
