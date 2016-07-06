//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012 Torsten Rahn <tackat@kde.org>
//


#include <QCoreApplication>
#include <QDataStream>
#include <QFile>
#include <QDebug>

int main(int argc, char *argv[])
{
    QCoreApplication  app(argc, argv);

    qDebug( " Syntax: pntdel [-i pnt-sourcefile -o pnt-targetfile -id idNumber] " );

    QString inputFilename("PDIFFBORDERS.PNT");
    int inputIndex = app.arguments().indexOf("-i");
    if (inputIndex > 0 && inputIndex + 1 < argc )
        inputFilename = app.arguments().at( inputIndex + 1 );

    QString outputFilename("NEW.PNT");
    int outputIndex = app.arguments().indexOf("-o");
    if (outputIndex > 0 && outputIndex + 1 < argc )
        outputFilename = app.arguments().at( outputIndex + 1 );

    int delIndex = -1;
    int idIndex = app.arguments().indexOf("-id");
    if (idIndex > 0 && idIndex + 1 < argc )
        delIndex = app.arguments().at( idIndex + 1 ).toInt();


    qDebug() << "input filename:" << inputFilename;
    qDebug() << "output filename:" << outputFilename;
    qDebug() << "remove index:" << delIndex;

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
