//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Torsten Rahn <tackat@kde.org>
//


#include <QApplication>
#include <QDataStream>
#include <QFile>
#include <QDebug>
#include <QXmlInputSource>
#include <QXmlSimpleReader>
#include "svgxmlhandler.h"

void parseSvg( const QString & svgFilename, QDataStream * out, const QString & path, int header ) {

    SVGXmlHandler     handler( out, path, header );
    QFile             xmlFile( svgFilename );
    QXmlInputSource   inputSource(&xmlFile);
    QXmlSimpleReader  reader;

    reader.setContentHandler(&handler);
    reader.parse( inputSource );
}

int main(int argc, char *argv[])
{
    QApplication  app(argc, argv);

    qDebug( " Syntax: pntreplace [-i pnt-sourcefile -o pnt-targetfile -s svg-replacementfile -id pntidNumber -path svgpathid] " );

    QString inputFilename("PDIFFBORDERS.PNT");
    int inputIndex = app.arguments().indexOf("-i");
    if (inputIndex > 0 && inputIndex + 1 < argc )
        inputFilename = app.arguments().at( inputIndex + 1 );

    QString outputFilename("NEW.PNT");
    int outputIndex = app.arguments().indexOf("-o");
    if (outputIndex > 0 && outputIndex + 1 < argc )
        outputFilename = app.arguments().at( outputIndex + 1 );

    QString svgFilename("output.svg");
    int svgIndex = app.arguments().indexOf("-s");
    if (svgIndex > 0 && svgIndex + 1 < argc )
        svgFilename = app.arguments().at( svgIndex + 1 );

    QString path("id_path");
    int pathIndex = app.arguments().indexOf("-path");
    if (pathIndex > 0 && pathIndex + 1 < argc )
        path = app.arguments().at( pathIndex + 1 );

    int delIndex = -1;
    int idIndex = app.arguments().indexOf("-id");
    if (idIndex > 0 && idIndex + 1 < argc )
        delIndex = app.arguments().at( idIndex + 1 ).toInt();


    qDebug() << "input filename:" << inputFilename;
    qDebug() << "output filename:" << outputFilename;
    qDebug() << "svg replacement filename:" << svgFilename;
    qDebug() << "replace index:" << delIndex;
    qDebug() << "replacement:" << path;

    // INPUT
    QFile  file( inputFilename );

    if ( file.open( QIODevice::ReadOnly ) ) {
        QDataStream stream( &file );  // read the data serialized from the file
        stream.setByteOrder( QDataStream::LittleEndian );

        // OUTPUT
        QFile data(outputFilename);

        if (data.open(QFile::WriteOnly | QFile::Truncate)) {
            QDataStream out(&data);
            out.setByteOrder( QDataStream::LittleEndian );

            short  header;
            short  iLat;
            short  iLon;

            bool skip = false;

            while( !stream.atEnd() ){
                stream >> header >> iLat >> iLon;
                if ( header == delIndex ) {
                    parseSvg( svgFilename, &out, path, delIndex );
                    skip = true;
                }
                else if ( header > 5 )
                    skip = false;

                if ( !skip )
                    out << header << iLat << iLon;
            }
            data.close();
        }
        else {
            qDebug() << "ERROR: Couldn't write output file to disc!";
        }
        file.close();
    }
    else {
        qDebug() << "ERROR: Source file not found!";
    }

    app.exit();
}
