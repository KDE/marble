//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Torsten Rahn <tackat@kde.org>
//


#include <QtGui/QApplication>
#include <QtCore/QDataStream>
#include <QtCore/QFile>
#include <QtCore/QStringList>
#include <QtCore/QTextStream>
#include <QtCore/QDebug>

int main(int argc, char *argv[])
{
    const qreal INT2SVG = 216.0 / 10800.0;
    const qreal INT2DEG = 180.0 / 10800.0;

    QApplication  app(argc, argv);

    qDebug( " Syntax: pnt2svg [-i pnt-sourcefile -o svg-targetfile -cn clipNorth -cs clipSouth -cw clipWest -ce clipEast" );

    QString inputFilename("PBORDERS.PNT");
    int inputIndex = app.arguments().indexOf("-i");
    if (inputIndex > 0 && inputIndex + 1 < argc )
        inputFilename = app.arguments().at( inputIndex + 1 );

    QString outputFilename("output.svg");
    int outputIndex = app.arguments().indexOf("-o");
    if (outputIndex > 0 && outputIndex + 1 < argc )
        outputFilename = app.arguments().at( outputIndex + 1 );

    qreal clipNorth = 90.0;
    int clipNorthIndex = app.arguments().indexOf("-cn");
    if (clipNorthIndex > 0 && clipNorthIndex + 1 < argc )
        clipNorth = app.arguments().at( clipNorthIndex + 1 ).toDouble();

    qreal clipSouth = -90.0;
    int clipSouthIndex = app.arguments().indexOf("-cs");
    if (clipSouthIndex > 0 && clipSouthIndex + 1 < argc )
        clipSouth = app.arguments().at( clipSouthIndex + 1 ).toDouble();

    qreal clipEast = 180.0;
    int clipEastIndex = app.arguments().indexOf("-ce");
    if (clipEastIndex > 0 && clipEastIndex + 1 < argc )
        clipEast = app.arguments().at( clipEastIndex + 1 ).toDouble();

    qreal clipWest = -180.0;
    int clipWestIndex = app.arguments().indexOf("-cw");
    if (clipWestIndex > 0 && clipWestIndex + 1 < argc )
        clipWest = app.arguments().at( clipWestIndex + 1 ).toDouble();

    qDebug() << "input filename:" << inputFilename;
    qDebug() << "output filename:" << outputFilename;
    qDebug() << "clipNorth:" << clipNorth;
    qDebug() << "clipSouth:" << clipSouth;
    qDebug() << "clipWest:" << clipWest;
    qDebug() << "clipEast:" << clipEast;

    // INPUT
    QStringList pathList;
    QString pathString;
    QString closePolygon;
    QString styleString;
    int    count = 0;
    int    origHeader = 0;
    int    pathIndex = 0;

    QFile  file( inputFilename );

    if ( file.open( QIODevice::ReadOnly ) ) {
        QDataStream stream( &file );  // read the data serialized from the file
        stream.setByteOrder( QDataStream::LittleEndian );

        short  header;
        short  iLat;
        short  iLon;

        bool clipped = false;

        while( !stream.atEnd() ){
            stream >> header >> iLat >> iLon;
            // Transforming Range of Coordinates to iLat [0,ARCMINUTE] , iLon [0,2 * ARCMINUTE]

            if ( header > 5 ) {
                // Find out whether the Polyline is a river or a closed polygon
                if ( ( header >= 7000 && header < 8000 )
                     || ( header >= 9000 && header < 20000 ) ) {
                    closePolygon=" \"";
                    styleString = QString(" fill=\"none\" stroke=\"black\" stroke-width=\"0.02\"");
                }
                else {
                    closePolygon=" z \"";
                    styleString = QString(" fill=\"lightgrey\" stroke=\"black\" stroke-width=\"0.02\"");
                }
                // Finish old path
                if (!pathString.isEmpty() && !clipped) {
                    pathString += closePolygon;
                    pathString += styleString;
                    pathString += QString(" id=\"path%1_%2\" />").arg(pathIndex).arg(origHeader);
                    pathList.append(pathString);
                }
                // Start new path
                origHeader = header;
                clipped = false;
                pathString = QString("<path d=\"M %1, %2").arg((qreal)(iLon) * INT2SVG + 216 ).arg(-(qreal)(iLat) * INT2SVG + 108);
                ++pathIndex;
            }
            else {
                pathString += QString(" L %1, %2").arg((qreal)(iLon) * INT2SVG + 216 ).arg(-(qreal)(iLat) * INT2SVG + 108);
            }
            ++count;

            if ((qreal)(iLat) * INT2DEG > clipNorth || (qreal)(iLat) * INT2DEG < clipSouth)
                clipped = true;
            if ((qreal)(iLon) * INT2DEG > clipEast || (qreal)(iLon) * INT2DEG < clipWest)
                clipped = true;
        }

        // Finish old path
        if (!pathString.isEmpty() && !clipped) {
            pathString += closePolygon;
            pathString += QString(" id=\"path%1_%2\" />").arg(count).arg(origHeader);
                    if (!pathString.isEmpty()) pathList.append(pathString);
        }

        file.close();
    }
    else {
        qDebug() << "ERROR: Source file not found!";
    }


    // OUTPUT
    QFile data(outputFilename);
    if (data.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&data);

        out << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>" << endl;
        out << "<svg width=\"432.00000px\" height=\"216.00000px\">" << endl;
        foreach ( const QString & path, pathList)
            out << path << endl;
        out << "</svg>" << endl;
        qDebug() << "Done!";
    }
    else {
        qDebug() << "ERROR: Couldn't write output file to disc!";
    }

    app.exit();
}
