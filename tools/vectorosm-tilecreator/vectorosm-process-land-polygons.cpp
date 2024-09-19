/*
    SPDX-FileCopyrightText: 2020 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "TileDirectory.h"

#include "GeoDataDocumentWriter.h"
#include "GeoDataLatLonAltBox.h"
#include "MarbleModel.h"
#include "ParsingRunnerManager.h"
#ifdef STATIC_BUILD
#include "src/plugins/runner/osm/translators/O5mWriter.h"
#endif

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>

#ifdef STATIC_BUILD
#include <QtPlugin>
Q_IMPORT_PLUGIN(OsmPlugin)
Q_IMPORT_PLUGIN(ShpPlugin)
#endif

using namespace Marble;

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName("marble-vectorosm-process-land-polygons");
    QCoreApplication::setApplicationVersion("0.1");

    QCommandLineParser parser;
    parser.setApplicationDescription("Preprocess land polygons for use by the on-demand tile generator.");
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addOptions({
        {{"c", "cache-directory"}, "Directory for temporary data.", "cache", "cache"},
    });
    parser.process(app);

    // work around MARBLE_ADD_WRITER not working for static builds
#ifdef STATIC_BUILD
    GeoDataDocumentWriter::registerWriter(new O5mWriter, QStringLiteral("o5m"));
#endif

    MarbleModel model;
    ParsingRunnerManager manager(model.pluginManager());
    QString const cacheDirectory = parser.value("cache-directory");
    QDir().mkpath(cacheDirectory);
    if (!QFileInfo(cacheDirectory).isWritable()) {
        qWarning() << "Cannot write to cache directory" << cacheDirectory;
        parser.showHelp(1);
    }

    GeoDataLatLonBox world(85.0, -85.0, 180.0, -180.0, GeoDataCoordinates::Degree);
    TileDirectory loader(TileDirectory::Landmass, cacheDirectory, manager, 17);
    loader.setBoundingBox(world);
    loader.createTiles();
    return 0;
}
