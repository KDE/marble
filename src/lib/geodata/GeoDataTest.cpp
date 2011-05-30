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

#include "MarbleDebug.h"
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtGui/QApplication>

#include "GeoDataParser.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"

#include "GeoSceneDocument.h"
#include "GeoSceneHead.h"
#include "GeoSceneIcon.h"
#include "GeoSceneLayer.h"
#include "GeoSceneMap.h"
#include "GeoSceneParser.h"
#include "GeoSceneProperty.h"
#include "GeoSceneSettings.h"
#include "GeoSceneZoom.h"

#ifdef Q_OS_WIN
#include <stdio.h>
#include <cstdlib>

void myMessageOutput(QtMsgType type, const char *msg)
{
    switch (type) {
        case QtDebugMsg:
            fprintf(stdout, "Debug: %s\n", msg);
            break;
        case QtWarningMsg:
            fprintf(stdout, "Warning: %s\n", msg);
            break;
        case QtCriticalMsg:
            fprintf(stdout, "Critical: %s\n", msg);
            break;
        case QtFatalMsg:
            fprintf(stdout, "Fatal: %s\n", msg);
            abort();
    }
}
#endif

using namespace Marble;

void dumpGeoDataDocument(GeoDataDocument*);
void dumpGeoSceneDocument(GeoSceneDocument*);

int main(int argc, char** argv)
{
#ifdef Q_OS_WIN
    qInstallMsgHandler(myMessageOutput);
#endif
    QApplication app(argc, argv);

    // Expect document as first command line argument
    if (app.arguments().size() <= 1) {
        qWarning("Pass file name as first argument!");
        return -1;
    }

    // Check whether file exists
    QFile file(app.arguments().at(1));
    if (!file.exists()) {
        qWarning("File does not exist!");
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
        qWarning("Could not determine file format!");
        return -1;
    }

    if (!parser->read(&file)) {
        qWarning("Could not parse file!");
        return -1;
    }

    // Get result document
    GeoDocument* document = parser->releaseDocument();
    Q_ASSERT(document);

    if (document->isGeoDataDocument()) {
        GeoDataDocument *dataDocument = static_cast<GeoDataDocument*>(document);
        QVector<GeoDataFeature*>::const_iterator it = dataDocument->featureList().constBegin();
        QVector<GeoDataFeature*>::const_iterator end = dataDocument->featureList().constEnd();
        qDebug() << "---------------------------------------------------------";
        for (; it != end; ++it) {
            qDebug() << "Name: " << (*it)->name();
        }
        dumpGeoDataDocument(static_cast<GeoDataDocument*>(document));
    } else if (document->isGeoSceneDocument()) {
        GeoSceneDocument *sceneDocument = static_cast<GeoSceneDocument*>(document);
        qDebug() << "Name: " << sceneDocument->head()->name(); 
        qDebug() << "Target: " << sceneDocument->head()->target(); 
        qDebug() << "Theme: " << sceneDocument->head()->theme(); 
        qDebug() << "Icon (pixmap): " << sceneDocument->head()->icon()->pixmap(); 
        qDebug() << "Icon (color) : " << sceneDocument->head()->icon()->color(); 
        qDebug() << "Description: " << sceneDocument->head()->description(); 
        qDebug() << "Visible: " << sceneDocument->head()->visible(); 
        qDebug() << "Discrete: " << sceneDocument->head()->zoom()->discrete(); 
        qDebug() << "Minimum: " << sceneDocument->head()->zoom()->minimum(); 
        qDebug() << "Maximum: " << sceneDocument->head()->zoom()->maximum(); 
        if(sceneDocument->settings() && 
           sceneDocument->settings()->property( "cities" ) && 
           sceneDocument->settings()->property( "relief" )) {
            qDebug() << "CitiesName: " << sceneDocument->settings()->property( "cities" )->name(); 
            qDebug() << "Available: " << sceneDocument->settings()->property( "cities" )->available(); 
            qDebug() << "Value: " << sceneDocument->settings()->property( "cities" )->value(); 
            qDebug() << "ReliefName: " << sceneDocument->settings()->property( "relief" )->name(); 
            qDebug() << "Available: " << sceneDocument->settings()->property( "relief" )->available(); 
            qDebug() << "Value: " << sceneDocument->settings()->property( "relief" )->value(); 
            qDebug() << "Test query a nonexistent property: " << sceneDocument->settings()->property( "nonexistent" )->name();
        }
        if(sceneDocument->map()) {
            qDebug() << "layers:" << sceneDocument->map()->layers().size();
            QVector<GeoSceneLayer*>::const_iterator it = sceneDocument->map()->layers().constBegin();
            QVector<GeoSceneLayer*>::const_iterator end = sceneDocument->map()->layers().constEnd();
            for (; it != end; it++) {
                GeoSceneLayer* layer = *it;
                qDebug() << "\t---------------------------------------------------------";
                qDebug() << "\tLayerName: " << layer->name();
                qDebug() << "\tBackend: " << layer->backend();
                QVector<GeoSceneAbstractDataset*>::const_iterator it = layer->datasets().constBegin();
                QVector<GeoSceneAbstractDataset*>::const_iterator end = layer->datasets().constEnd();
                for (; it != end; ++it) {
                    GeoSceneAbstractDataset* dataset = *it;
                    qDebug() << "\t\tDatasetName: " << dataset->name();
                    qDebug() << "\t\tFileformat: " << dataset->fileFormat();
                    qDebug() << "\t\tType: " << dataset->type();
                }
            }
        }
        dumpGeoSceneDocument(static_cast<GeoSceneDocument*>(document));
    }
    else {
        // A parsed document should either be a GeoDataDocument or a GeoSceneDocument!
        Q_ASSERT(false);
    }

    qDebug() << "\nSuccessfully parsed file!";
    delete document;
    qDebug() << "\nSuccessfully deleted file!";
    return 0;
}

QString formatOutput(int depth)
{
    QString result;
    for (int i = 0; i < depth; ++i)
        result += "  ";

    return result;
}

void dumpGeoDataPlacemark(const GeoDataPlacemark *placemark)
{
    Q_UNUSED(placemark);
//	qDebug() << placemark.name() << placemark.population() << placemark.coordinate().toString();
}

void dumpFoldersRecursively(const GeoDataContainer *container, int depth)
{
    qDebug() << "dumping container with" << container->size() << "children...";

    QVector<GeoDataFolder*> folders = container->folderList();
    QVector<GeoDataPlacemark*> placemarks = container->placemarkList();
    QString format = formatOutput(depth);

    qDebug() << qPrintable(QString(format + QString::fromLatin1("Dumping container with %1 child folders!\n").arg(folders.size())));

    QVector<GeoDataFolder*>::const_iterator it = folders.constBegin();
    const QVector<GeoDataFolder*>::const_iterator end = folders.constEnd();

    for (; it != end; ++it) {
        qDebug() << qPrintable(QString(format + QString::fromLatin1("Dumping child %1\n").arg(it - folders.constBegin() + 1)));
        dumpFoldersRecursively(*it, ++depth);
    }

    qDebug() << qPrintable(QString(format + QString::fromLatin1("Dumping container with %1 child placemarks!\n").arg(placemarks.size())));

    QVector<GeoDataPlacemark*>::const_iterator pit = placemarks.constBegin();
    const QVector<GeoDataPlacemark*>::const_iterator pend = placemarks.constEnd();

    for (; pit != pend; ++pit) {
        qDebug() << qPrintable(QString(format + QString::fromLatin1("Dumping child %1\n").arg(pit - placemarks.constBegin() + 1)));
        dumpGeoDataPlacemark(*pit);
    }
}

void dumpGeoDataDocument(GeoDataDocument* document)
{
    dumpFoldersRecursively(document, 0);
    // TODO: Dump all features!
} 

void dumpGeoSceneDocument(GeoSceneDocument* document)
{
    Q_UNUSED( document )

    // TODO: Maybe dump parsed datastructures here!
}
