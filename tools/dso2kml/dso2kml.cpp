//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013      Mohammed Nafees <nafees.technocool@gmail.com>
//


#include <QFile>
#include <QDebug>
#include <QCoreApplication>
#include <QStringList>

#include <cmath>
#include <iostream>

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QFile dsoData( "dso.dat" );
    if ( !dsoData.open( QFile::ReadOnly ) ) {
        std::cout << "File dso.dat not found in current path. Exiting." << std::endl;
        return 1;
    }

    QFile file( "dso.kml" );
    file.open( QIODevice::WriteOnly );
    QTextStream out( &file );

    out << "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n"
        << "<kml xmlns=\"http://www.opengis.net/kml/2.2\" hint=\"target=sky\"> \n"
        << "<Document> \n"
        << "    <Style id=\"iconStyle\"> \n"
        << "        <IconStyle> \n"
        << "            <Icon> \n"
        << "                <href>deepsky.png</href> \n"
        << "            </Icon> \n"
        << "        </IconStyle> \n"
        << "    </Style> \n";

    QTextStream in( &dsoData );

    QString line;
    qreal longitude;
    qreal latitude;

    while ( !in.atEnd() ) {
        line = in.readLine();

        // Check for null line at end of file
        if ( line.isNull() ) {
            continue;
        }

        // Ignore Comment lines in header and
        // between dso entries
        if (line.startsWith(QLatin1Char('#'))) {
            continue;
        }

        QStringList entries = line.split( QLatin1Char( ',' ) );

        QString id = entries.at( 0 );
        QString longName = entries.at(7);

        double raH = entries.at( 1 ).toDouble();
        double raM = entries.at( 2 ).toDouble();
        double raS = entries.at( 3 ).toDouble();
        double decH = entries.at( 4 ).toDouble();
        double decM = entries.at( 5 ).toDouble();
        double decS = entries.at( 6 ).toDouble();

        double ra = ( raH + raM / 60.0 + raS / 3600.0 ) * 15.0;
        double dec;

        if ( decH >= 0.0 ) {
            dec = decH + decM /60.0 + decS / 3600.0;
        }
        else {
            dec = decH - decM / 60.0 - decS / 3600.0;
        }

        longitude = ra - 180.0;
        latitude = dec;

        out << "    <Placemark> \n"
            << "        <name>" << id << "</name> \n"
            << "        <description>" << longName << "</description> \n"
            << "        <styleUrl>#iconStyle</styleUrl> \n"
            << "        <Point> \n"
            << "            <coordinates>" << longitude << "," << latitude << "</coordinates> \n"
            << "        </Point> \n"
            << "    </Placemark> \n";
    }

    out << "</Document> \n"
        << "</kml> \n";

    dsoData.close();
    file.close();

    app.exit();
}


