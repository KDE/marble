/*
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>
    Copyright (C) 2008 Torsten Rahn <rahn@kde.org>

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

#include "GeoSceneParser.h"

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

    GeoSceneDataSource source = GeoSceneData_DGML; // KML default for now.

    if (file.fileName().endsWith(".dgml"))
        source = GeoSceneData_DGML;

    // Let our baby do the work!
    GeoSceneParser parser(source);
    if (!parser.read(&file)) {
        qFatal("Could not parse file!");
        return -1;
    }

    // Get result document
    const GeoSceneDocument& document = parser.document();
    qDebug() << "\nSuccesfully parsed file!";
    return 0;
}
