//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
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

#include <QApplication>
#include <QCommandLineParser>
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QString>
#include <QElapsedTimer>
#include <QSharedPointer>
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
#include "SpellChecker.h"

#include <iostream>

using namespace Marble;

GeoDataDocument* mergeDocuments(GeoDataDocument* map1, GeoDataDocument* map2)
{
    GeoDataDocument* mergedMap = new GeoDataDocument(*map1);

    OsmPlacemarkData marbleLand;
    marbleLand.addTag("marble_land","landmass");
    for (auto placemark: map2->placemarkList()) {
        GeoDataPlacemark* land = new GeoDataPlacemark(*placemark);
        if (geodata_cast<GeoDataPolygon>(land->geometry())) {
            land->setOsmData(marbleLand);
        }
        mergedMap->append(land);
    }

    return mergedMap;
}

QString tileFileName(const QCommandLineParser &parser, int x, int y, int zoomLevel)
{
    QString const extension = parser.value("extension");
    QString const output = parser.isSet("development") ? QString("%1/maps/earth/vectorosm-dev").arg(MarbleDirs::localPath()) : parser.value("output");
    QString const outputDir = QString("%1/%2/%3").arg(output).arg(zoomLevel).arg(x);
    QString const outputFile = QString("%1/%2.%3").arg(outputDir).arg(y).arg(extension);
    return outputFile;
}

void writeBoundaryTile(GeoDataDocument* tile, const QString &region, const QCommandLineParser &parser, int x, int y, int zoomLevel)
{
    QString const extension = parser.value("extension");
    QString const outputDir = QString("%1/boundaries/%2/%3/%4").arg(parser.value("cache-directory")).arg(region).arg(zoomLevel).arg(x);
    QString const outputFile = QString("%1/%2.%3").arg(outputDir).arg(y).arg(extension);
    QDir().mkpath(outputDir);
    GeoDataDocumentWriter::write(outputFile, *tile);
}

QSharedPointer<GeoDataDocument> mergeBoundaryTiles(const QSharedPointer<GeoDataDocument> &background, ParsingRunnerManager &manager, const QCommandLineParser &parser, int x, int y, int zoomLevel)
{
    GeoDataDocument* mergedMap = new GeoDataDocument;
    OsmPlacemarkData marbleLand;
    marbleLand.addTag("marble_land","landmass");
    for (auto placemark: background->placemarkList()) {
        GeoDataPlacemark* land = new GeoDataPlacemark(*placemark);
        if (geodata_cast<GeoDataPolygon>(land->geometry())) {
            land->setOsmData(marbleLand);
        }
        mergedMap->append(land);
    }

    QString const extension = parser.value("extension");
    QString const boundaryDir = QString("%1/boundaries").arg(parser.value("cache-directory"));
    for(auto const &dir: QDir(boundaryDir).entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QString const file = QString("%1/%2/%3/%4/%5.%6").arg(boundaryDir).arg(dir).arg(zoomLevel).arg(x).arg(y).arg(extension);
        if (QFileInfo(file).exists()) {
            auto tile = TileDirectory::open(file, manager);
            if (tile) {
                for (auto placemark: tile->placemarkList()) {
                    mergedMap->append(placemark->clone());
                }
            }
        }
    }

    return QSharedPointer<GeoDataDocument>(mergedMap);
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
                          {{"s", "spellcheck"}, "Use this geonames.org cities file for spell-checking city names", "spellcheck"},
                          {"verbose", "Increase amount of shell output information"},
                          {"boundaries", "Write boundary tiles (implied by conflict-resolution=merge)"},
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

    bool const overwriteTiles = parser.value("conflict-resolution") == "overwrite";
    bool const mergeTiles = parser.value("conflict-resolution") == "merge";
    bool const writeBoundaries = mergeTiles || parser.isSet("boundaries");
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
                if (tile->size() > 0) {
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
        QString const region = QFileInfo(inputFileName).fileName();
        QString const regionDir = QString("%1/%2").arg(cacheDirectory).arg(QFileInfo(inputFileName).baseName());
        TileDirectory mapTiles(TileDirectory::OpenStreetMap, regionDir, manager, extension, maxZoomLevel);
        mapTiles.setInputFile(inputFileName);
        mapTiles.createTiles();
        auto const boundingBox = mapTiles.boundingBox();

        TileDirectory loader(TileDirectory::Landmass, cacheDirectory, manager, extension, maxZoomLevel);
        loader.setBoundingBox(boundingBox);
        loader.createTiles();

        typedef QMap<QString, QVector<TileId> > Tiles;
        Tiles tiles;

        qint64 total = 0;
        QSet<QString> boundaryTiles;
        for(auto zoomLevel: zoomLevels) {
            TileIterator iter(mapTiles.boundingBox(), zoomLevel);
            total += iter.total();
            for(auto const &tileId: iter) {
                auto const tile = TileId(0, zoomLevel, tileId.x(), tileId.y());
                int const innerNodes = mapTiles.innerNodes(tile);
                if (innerNodes > 0) {
                    auto const mapTile = mapTiles.tileFor(zoomLevel, tileId.x(), tileId.y());
                    auto const name = QString("%1/%2/%3").arg(mapTile.zoomLevel()).arg(mapTile.x()).arg(mapTile.y());
                    tiles[name] << tile;
                    if (innerNodes < 4) {
                        boundaryTiles << name;
                    }
                } else {
                    --total;
                }
            }
        }

        qint64 count = 0;
        for (auto iter = tiles.begin(), end = tiles.end(); iter != end; ++iter) {
            for(auto const &tileId: iter.value()) {
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

                typedef QSharedPointer<GeoDataDocument> GeoDocPtr;
                GeoDocPtr tile2 = GeoDocPtr(loader.clip(zoomLevel, tileId.x(), tileId.y()));
                if (tile2->size() > 0) {
                    GeoDocPtr tile1 = GeoDocPtr(mapTiles.clip(zoomLevel, tileId.x(), tileId.y()));
                    TagsFilter::removeAnnotationTags(tile1.data());
                    int originalWays = 0;
                    int mergedWays = 0;
                    if (zoomLevel < 17) {
                        WayConcatenator concatenator(tile1.data());
                        originalWays = concatenator.originalWays();
                        mergedWays = concatenator.mergedWays();
                    }
                    NodeReducer nodeReducer(tile1.data(), tileId);
                    if (tile1->size() > 0 && tile2->size() > 0) {
                        GeoDocPtr combined = GeoDocPtr(mergeDocuments(tile1.data(), tile2.data()));

                        if (writeBoundaries && boundaryTiles.contains(iter.key())) {
                            writeBoundaryTile(tile1.data(), region, parser, tileId.x(), tileId.y(), zoomLevel);
                            if (mergeTiles) {
                                combined = mergeBoundaryTiles(tile2, manager, parser, tileId.x(), tileId.y(), zoomLevel);
                            }
                        }

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
                            if (!writeTile(combined.data(), filename)) {
                                return 4;
                            }
                        }

                        TileDirectory::printProgress(count / double(total));
                        std::cout << "  Tile " << count << "/" << total << " (";
                        std::cout << combined->name().toStdString() << ").";
                        double const reduction = nodeReducer.removedNodes() / qMax(1.0, double(nodeReducer.remainingNodes() + nodeReducer.removedNodes()));
                        std::cout << " Node reduction: " << qRound(reduction * 100.0) << "%";
                        if (originalWays > 0) {
                            std::cout << " , " << originalWays << " ways merged to " << mergedWays;
                        }
                    } else {
                        TileDirectory::printProgress(count / double(total));
                        std::cout << "  Skipping empty tile " << count << "/" << total << " (" << tile1->name().toStdString() << ").";
                    }
                } else {
                    TileDirectory::printProgress(count / double(total));
                    std::cout << "  Skipping sea tile " << count << "/" << total << " (" << tile2->name().toStdString() << ").";
                }

                std::cout << std::string(20, ' ') << '\r';
                std::cout.flush();
            }
        }
        TileDirectory::printProgress(1.0);
        std::cout << "  Vector OSM tiles complete." << std::string(30, ' ') << std::endl;
    }

    return 0;
}
