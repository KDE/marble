//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#include "GeoDataDocumentWriter.h"
#include "GeoDataGeometry.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"
#include "MarbleDirs.h"
#include "MarbleModel.h"
#include "MbTileWriter.h"
#include "NodeReducer.h"
#include "ParsingRunnerManager.h"
#include "TileDirectory.h"
#include "TileId.h"
#include "VectorClipper.h"
#include "WayConcatenator.h"
#include "TileQueue.h"

#include <QApplication>
#include <QBuffer>
#include <QCommandLineParser>
#include <QDebug>
#include <QFileInfo>
#include <QRect>

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

int main(int argc, char *argv[])
{
    QTime timer;
    timer.start();

    QCoreApplication app(argc, argv);

    QCoreApplication::setApplicationName("marble-vectorosm-cachetiles");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("Create a vectorosm tile and its neighborhood");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("tile", "The tile to create (in z/x/y.extension format)");

    parser.addOptions({
                      {{"c", "cache-directory"}, "Directory for temporary data.", "cache", "cache"},
                      {{"m", "mbtile"}, "Store tiles in a mbtile database.", "mbtile"},
                      {"verbose", "Write progress information to standard output."}
                      });
    parser.process(app);

    const QStringList args = parser.positionalArguments();
    if (args.size() != 1) {
        parser.showHelp();
        return 0;
    }

    auto const input = args.first().split('/');
    if (input.size() != 3) {
        qWarning() << "Tile" << input << "does not match the z/x/y.format convention";
        parser.showHelp();
        return 1;
    }

    bool canParse[] = {false, false, false};
    TileId centerTile(0, input[0].toInt(&canParse[0]), input[1].toInt(&canParse[1]), QFileInfo(input[2]).baseName().toInt(&canParse[2]));
    if (!canParse[0] || !canParse[1] || !canParse[2]) {
        qWarning() << "Tile" << input << "does not consist of digits in the format z/x/y.format";
        parser.showHelp();
        return 1;
    }

    TileQueue tileQueue;
    QSet<TileId> dynamicTiles;
    tileQueue.read(dynamicTiles);
    if (dynamicTiles.contains(centerTile)) {
        return 0;
    }

    QString const extension = QFileInfo(input[2]).completeSuffix();
    QString const mbtile = parser.value("mbtile");
    QSharedPointer<MbTileWriter> mbtileWriter = QSharedPointer<MbTileWriter>(new MbTileWriter(mbtile, extension));
    mbtileWriter->setReportProgress(false);
    mbtileWriter->setCommitInterval(500);

    MarbleModel model;
    ParsingRunnerManager manager(model.pluginManager());
    QString const cacheDirectory = parser.value("cache-directory");
    QDir().mkpath(cacheDirectory);
    if (!QFileInfo(cacheDirectory).isWritable()) {
        qWarning() << "Cannot write to cache directory" << cacheDirectory;
        parser.showHelp(1);
    }

    TileDirectory mapTiles(TileDirectory::OpenStreetMap, cacheDirectory, manager, extension, centerTile.zoomLevel());
    TileDirectory landTiles(TileDirectory::Landmass, cacheDirectory, manager, extension, centerTile.zoomLevel());

    int const offset = 3;
    int const N = pow(2,centerTile.zoomLevel());
    QRect boundaries = QRect(0, 0, N-1, N-1) & QRect(QPoint(centerTile.x()-offset, centerTile.y()-offset),
                                                     QPoint(centerTile.x()+offset, centerTile.y()+offset));
    int count = 0;
    int const total = boundaries.width() * boundaries.height();
    bool const printProgress = parser.isSet("verbose");
    for (int x=boundaries.left(); x<=boundaries.right(); ++x) {
        for (int y=boundaries.top(); y<=boundaries.bottom(); ++y) {
            auto const tileId = TileId (0, centerTile.zoomLevel(), x, y);
            ++count;
            if (mbtileWriter->hasTile(x, y, tileId.zoomLevel())) {
                continue;
            }

            typedef QSharedPointer<GeoDataDocument> GeoDocPtr;
            GeoDocPtr tile1 = GeoDocPtr(mapTiles.clip(tileId.zoomLevel(), tileId.x(), tileId.y()));
            TagsFilter::removeAnnotationTags(tile1.data());
            if (tileId.zoomLevel() < 17) {
                WayConcatenator concatenator(tile1.data());
            }
            NodeReducer nodeReducer(tile1.data(), tileId);
            GeoDocPtr tile2 = GeoDocPtr(landTiles.clip(tileId.zoomLevel(), tileId.x(), tileId.y()));
            GeoDocPtr combined = GeoDocPtr(mergeDocuments(tile1.data(), tile2.data()));

            QBuffer buffer;
            buffer.open(QBuffer::ReadWrite);
            if (GeoDataDocumentWriter::write(&buffer, *combined, extension)) {
                buffer.seek(0);
                mbtileWriter->addTile(&buffer, tileId.x(), tileId.y(), tileId.zoomLevel());
            } else {
                qWarning() << "Could not write the tile " << combined->name();
            }

            dynamicTiles << tileId;

            if (printProgress) {
                TileDirectory::printProgress(qreal(count) / total);
                std::cout << "  Tile " << count << "/" << total << " (";
                std::cout << combined->name().toStdString() << ").";
                std::cout << std::string(20, ' ') << '\r';
                std::cout.flush();
            }
        }
    }

    tileQueue.write(dynamicTiles);

    if (printProgress) {
        TileDirectory::printProgress(1.0);
        std::cout << "Vector OSM tiles complete after " << timer.elapsed() << " ms." << std::string(30, ' ') << std::endl;
    }

    return 0;
}
