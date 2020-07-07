/*
    SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
    SPDX-FileCopyrightText: 2020 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "tirexbackend.h"

#include "GeoDataDocumentWriter.h"
#include "GeoDataPolygon.h"
#include "MarbleModel.h"
#include "NodeReducer.h"
#include "ParsingRunnerManager.h"
#include "TileDirectory.h"
#include "TileId.h"
#include "VectorClipper.h"
#include "WayConcatenator.h"

#include <QCoreApplication>
#include <QDebug>
#include <QFile>

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

int main(int argc, char **argv)
{
    setenv("QT_LOGGING_TO_CONSOLE", "0", true); // redirects qDebug to syslog

    QCoreApplication app(argc, argv);
    TirexBackend backend;

    MarbleModel model;
    ParsingRunnerManager manager(model.pluginManager());
    const auto cacheDirectory = backend.configValue(QStringLiteral("cache-directory")).toString();

    QObject::connect(&backend, &TirexBackend::tileRequested, &app, [&](const TirexMetatileRequest &req) {
        // load bigger tiles at high-z levels, it's more efficient there
        // that however assumes we are rendering square power of two meta-tiles with proper alignment
        int loadZ = req.tile.z;
        if (backend.metatileColumns() == backend.metatileRows() && backend.metatileRows() == 8 && req.tile.z <= 15) {
            loadZ = req.tile.z - 3;
        }
        TileDirectory mapTiles(cacheDirectory, QStringLiteral("planet.osmx"), manager, req.tile.z, loadZ);
        TileDirectory landTiles(TileDirectory::Landmass, cacheDirectory, manager, req.tile.z);

        QFile f(backend.metatileFileName(req));
        if (!f.open(QFile::WriteOnly)) {
            backend.tileError(req, f.errorString());
            return;
        }

        backend.writeMetatileHeader(&f, req.tile);
        for (int x = 0; x < backend.metatileColumns(); ++x) {
            for (int y = 0; y < backend.metatileRows(); ++y) {
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
        backend.tileDone(req);
    });

    return app.exec();
}
