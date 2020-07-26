/*
    SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
    SPDX-FileCopyrightText: 2020 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TirexBackend.h"

#include "GeoDataDocumentWriter.h"
#include "GeoDataPolygon.h"
#include "MarbleModel.h"
#include "NodeReducer.h"
#include "ParsingRunnerManager.h"
#include "TileDirectory.h"
#include "TileId.h"
#include "VectorClipper.h"
#include "WayConcatenator.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QSaveFile>

using namespace Marble;

GeoDataDocument* mergeDocuments(GeoDataDocument* map1, GeoDataDocument* map2)
{
    GeoDataDocument* mergedMap = new GeoDataDocument(*map1);
    if (!map2) {
        return mergedMap;
    }

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

int main(int argc, char **argv)
{
    setenv("QT_LOGGING_TO_CONSOLE", "0", true); // redirects qDebug to syslog

    QCoreApplication app(argc, argv);

    // for stand-alone testing only, in normal operation this is entirely controlled via the Tirex command socket
    QCommandLineParser parser;
    parser.addOptions({
                      {{"c", "cache-directory"}, "Directory for temporary data.", "cache"},
                      {"x", "x coordinate of the requested tile", "x"},
                      {"y", "y coordinate of the requested tile", "y"},
                      {"z", "zoom level of the requested tile", "z"},
                      });
    parser.process(app);

    TirexBackend backend;

    MarbleModel model;
    ParsingRunnerManager manager(model.pluginManager());
    auto cacheDirectory = backend.configValue(QStringLiteral("cache-directory")).toString();
    if (cacheDirectory.isEmpty()) {
        cacheDirectory = parser.value("cache-directory");
    }

    QObject::connect(&backend, &TirexBackend::tileRequested, &app, [&](const TirexMetatileRequest &req) {
        // assuming the requested meta tile is a square power of two, we break that down into square power-of-two blocks
        // for high zoom levels using few (or even just one block is most efficient), for lower zoom levels we need to use
        // more blocks to reduce memory use
        // to avoid TileDirectory reloading the same block multiple times, we need to do the below processing in the proper order
        int loadZ = req.tile.z;
        if (backend.metatileColumns() == backend.metatileRows() && backend.metatileRows() == 8) {
            loadZ = req.tile.z - 3;
            loadZ = std::max(11, loadZ);
            loadZ = std::min(req.tile.z, loadZ);
        }
        const int blockSize = 1 << (req.tile.z - loadZ);
        const int blockColumns = backend.metatileColumns() / blockSize;
        const int blockRows = backend.metatileRows() / blockSize;

        TileDirectory mapTiles(cacheDirectory, QStringLiteral("planet.osmx"), manager, req.tile.z, loadZ);
        TileDirectory landTiles(TileDirectory::Landmass, cacheDirectory, manager, req.tile.z);

        QSaveFile f(backend.metatileFileName(req));
        if (!f.open(QFile::WriteOnly)) {
            backend.tileError(req, f.errorString());
            return;
        }

        backend.writeMetatileHeader(&f, req.tile);
        for (int blockX = 0; blockX < blockColumns; ++blockX) {
            for (int blockY = 0; blockY < blockRows; ++blockY) {
                for (int tileX = 0; tileX < blockSize; ++tileX) {
                    for (int tileY = 0; tileY < blockSize; ++tileY) {
                        const auto x = blockX * blockSize + tileX;
                        const auto y = blockY * blockSize + tileY;

                        auto const tileId = TileId (0, req.tile.z, x + req.tile.x, y + req.tile.y);
                        using GeoDocPtr = QSharedPointer<GeoDataDocument>;
                        GeoDocPtr tile1 = GeoDocPtr(mapTiles.clip(tileId.zoomLevel(), tileId.x(), tileId.y()));
                        TagsFilter::removeAnnotationTags(tile1.data());
                        if (tileId.zoomLevel() < 17) {
                            WayConcatenator concatenator(tile1.data());
                        }
                        NodeReducer nodeReducer(tile1.data(), tileId);
                        GeoDocPtr tile2 = GeoDocPtr(landTiles.clip(tileId.zoomLevel(), tileId.x(), tileId.y()));
                        GeoDocPtr combined = GeoDocPtr(mergeDocuments(tile1.data(), tile2.data()));

                        const auto offset = f.pos();
                        if (GeoDataDocumentWriter::write(&f, *combined, QStringLiteral("o5m"))) {
                            backend.writeMetatileEntry(&f, x * backend.metatileColumns() + y, offset, f.pos() - offset);
                        } else {
                            qWarning() << "Could not write the tile " << combined->name();
                        }
                    }
                }
            }
        }

        f.commit();
        backend.tileDone(req);
    });

    if (parser.isSet("x") && parser.isSet("y") && parser.isSet("z")) {
        TirexMetatileRequest req;
        req.tile.x = parser.value("x").toInt();
        req.tile.y = parser.value("y").toInt();
        req.tile.z = parser.value("z").toInt();
        emit backend.tileRequested(req);
        return 0;
    }

    return app.exec();
}
