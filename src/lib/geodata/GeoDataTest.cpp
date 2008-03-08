/*
    Copyright (C) 2007, 2008 Nikolas Zimmermann <zimmermann@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include <QApplication>
#include <QDebug>
#include <QFile>
#include <QStringList>

#include "GeoDataFolder.h"

#include "GeoDataParser.h"
#include "GeoDataDocument.h"

#include "GeoSceneParser.h"
#include "GeoSceneDocument.h"

void dumpGeoDataDocument(GeoDataDocument*);
void dumpGeoSceneDocument(GeoSceneDocument*);

int main(int argc, char** argv)
{
    QApplication app(argc, argv);

    // Expect document as first command line argument
    if (app.arguments().size() <= 1) {
        qFatal("Pass file name as first argument!");
        return -1;
    }

    // Check whether file exists
    QFile file(app.arguments().at(1));
    if (!file.exists()) {
        qFatal("File does not exist!");
        return -1;
    }

    // Open file in right mode
    file.open(QIODevice::ReadOnly);

    GeoParser* parser = 0;
    const QString& fileName = file.fileName().toLower();

    // A real application, would use other heuristics to determine the source type!
    if (fileName.endsWith(".dgml"))
        parser = new GeoSceneParser(GeoScene_DGML);
    else if (fileName.endsWith(".kml"))
        parser = new GeoDataParser(GeoData_KML);
    else if (fileName.endsWith(".gpx"))
        parser = new GeoDataParser(GeoData_GPX);

    if (!parser) {
        qFatal("Could not determine file format!");
        return -1;
    }

    if (!parser->read(&file)) {
        qFatal("Could not parse file!");
        return -1;
    }

    // Get result document
    GeoDocument* document = parser->releaseDocument();
    Q_ASSERT(document);

    if (document->isGeoDataDocument())
        dumpGeoDataDocument(static_cast<GeoDataDocument*>(document));
    else if (document->isGeoSceneDocument())
    {
        qDebug() << "Name:" << static_cast<GeoSceneDocument*>(document)->head()->name(); 
        qDebug() << "Description:" << static_cast<GeoSceneDocument*>(document)->head()->description(); 
        dumpGeoSceneDocument(static_cast<GeoSceneDocument*>(document));
    }
    else {
        // A parsed document should either be a GeoDataDocument or a GeoSceneDocument!
        Q_ASSERT(false);
    }

    qDebug() << "\nSuccesfully parsed file!";
    return 0;
}

QString formatOutput(int depth)
{
    QString result;
    for (int i = 0; i < depth; ++i)
        result += "  ";

    return result;
}

void dumpFoldersRecursively(const GeoDataContainer* container, int depth)
{
    QVector<GeoDataFolder*> folders = container->folders();
    QString format = formatOutput(depth);

    fprintf(stderr, "%s", qPrintable(format + QString("Dumping container with %1 child folders!\n").arg(folders.size())));

    QVector<GeoDataFolder*>::const_iterator it = folders.constBegin();
    const QVector<GeoDataFolder*>::const_iterator end = folders.constEnd();

    for (; it != end; ++it) {
        fprintf(stderr, "%s", qPrintable(format + QString("Dumping child %1\n").arg(it - folders.constBegin() + 1)));
        dumpFoldersRecursively(*it, ++depth);
    }
}

void dumpGeoDataDocument(GeoDataDocument* document)
{
    dumpFoldersRecursively(document, 0);
    // TODO: Dump all features!
} 

void dumpGeoSceneDocument(GeoSceneDocument* document)
{
    // TODO: Maybe dump parsed datastructures here!
}
