
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//


#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtGui/QApplication>

#include <cmath>

#define ENABLEGUI

int main(int argc, char *argv[])
{
    QCoreApplication  app(argc, argv);

    QFile file("stars.dat");
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);

    // Write a header with a "magic number" and a version
    out << (quint32)0x73746172;
    out << (qint32)002;

    out.setVersion(QDataStream::Qt_4_3);

    QFile data("catalog.dat");
    if ( data.open( QFile::ReadOnly ) ) {
        QTextStream stream(&data);
        QString line;
        do {
            line = stream.readLine();

            QString idString = line.mid(0,4);
            int idValue = idString.toInt();

            QString recString = line.mid( 75, 6 );

            double raHH = recString.mid( 0, 2 ).toDouble();
            double raMM = recString.mid( 2, 2 ).toDouble();
            double raSS = recString.mid( 4, 2 ).toDouble();

            double raValue = 15 * ( raHH + raMM / 60.0 + raSS / 3600.0 ) / 180.0 * M_PI;

            QString decString = line.mid( 83, 7 );

            double deSign = ( decString.mid( 0, 1 ) == "-" ) ? -1.0 : 1.0; 
            double deHH = decString.mid( 1, 2 ).toDouble();
            double deMM = decString.mid( 3, 2 ).toDouble();
            double deSS = decString.mid( 5, 2 ).toDouble();

            double deValue = deSign * ( deHH + deMM / 60.0 + deSS / 3600.0 ) / 180.0 * M_PI;

            QString magString = line.mid( 102, 5 );
            double magValue = magString.toDouble();

//            qDebug() << "Rec:" << recString << "Dec.:" << decString << "Mag.:" << magString;
            if ( !line.isNull() && magValue < 6.0 ) {
                qDebug() << "ID:" << idValue << "RA:" << raValue << "DE:" << deValue << "mag:" << magValue;
                out << idValue;
                out << raValue;
                out << deValue;
                out << magValue;
            }
        } while ( !line.isNull() );
    }
    file.flush();

    QFile starFile("stars.dat");
    starFile.open(QIODevice::ReadOnly);
    QDataStream in(&starFile);    // read the data serialized from the file
 // Read and check the header
    quint32 magic;
    in >> magic;
    if (magic != 0x73746172)
        return -1;

    // Read the version
    qint32 version;
    in >> version;
    if (version > 002) {
        qDebug() << "stars.dat: file too new.";
     return -1;
    }
    int id;
    double ra;
    double de;
    double mag;

    while ( !in.atEnd() ) {
        in >> id;
        in >> ra;
        in >> de;
        in >> mag;
        qDebug() << "ID:" << id << "RA:" << ra << "DE:" << de << "MAG:" << mag;
    }

    app.exit();
}
