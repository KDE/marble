//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016 Torsten Rahn <tackat@kde.org>
//


#include <QApplication>
#include <QFileSystemModel>
#include <QDebug>

#include <tileprocessor.h>


int main(int argc, char *argv[])
{
    QString  sourcefile;
    QString  targetfile;

    QApplication  app(argc, argv);

    qDebug( " sentineltile -o targetfile sourcefile" );
    qDebug( " Syntax: pntreplace -d tileleveldirectory -m maskfile -b bathymetryfile " );
    qDebug( "         e.g. sentineltile -d /home/tackat/tilefab/sentinel2/14 " );
    qDebug( "                           -m /home/tackat/bathymetry/coastline_mask.png " );
    qDebug( "                           -b /home/tackat/bathymetry/bathymetry_equirect.jpg" );
    // /home/tackat/tilefab/sentinel2/14
    QString tileDirPath;
    int tileDirIndex = app.arguments().indexOf("-d");
    if (tileDirIndex > 0 && tileDirIndex + 1 < argc )
        tileDirPath = app.arguments().at( tileDirIndex + 1 );

    // /home/tackat/bathymetry/coastline_mask.png
    QString coastLineMaskPath;
    int maskIndex = app.arguments().indexOf("-m");
    if (maskIndex > 0 && maskIndex + 1 < argc )
        coastLineMaskPath = app.arguments().at( maskIndex + 1 );

    // /home/tackat/bathymetry/bathymetry_equirect.jpg
    QString bathymetryPath;
    int bathymetryIndex = app.arguments().indexOf("-b");
    if (bathymetryIndex > 0 && bathymetryIndex + 1 < argc )
        bathymetryPath = app.arguments().at( bathymetryIndex + 1 );

    TileProcessor * tileProc = new TileProcessor();

    tileProc->parseFileList(tileDirPath);
    tileProc->loadReferenceImages(coastLineMaskPath, bathymetryPath);
    tileProc->process();

    app.exit();
}
