//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//


// #include <zlib.h>

#include <QCoreApplication>
#include <QDebug>
#include <QFile>
#include <QStringList>


QString escapeXml( const QString &str )
{
    QString xml = str;
    xml.replace(QLatin1Char('&'), QStringLiteral("&amp;"));
    xml.replace(QLatin1Char('<'), QStringLiteral("&lt;"));
    xml.replace(QLatin1Char('>'), QStringLiteral("&gt;"));
    xml.replace(QLatin1Char('\''), QStringLiteral("&apos;"));
    xml.replace(QLatin1Char('"'), QStringLiteral("&quot;"));

    return xml;
}


int main(int argc, char *argv[])
{
    QString  sourcefilename;
    QString  targetfilename;

    QCoreApplication  app( argc, argv );

    for ( int i = 1; i < argc; ++i ) {
        if ( strcmp( argv[ i ], "-o" ) != 0 )
            continue;

        targetfilename = QString(argv[i+1]);
        sourcefilename = QString(argv[i+2]);

        qDebug() << "Source: " << sourcefilename;
        qDebug() << "Target: " << targetfilename;

        QFile  sourcefile( sourcefilename );
        sourcefile.open( QIODevice::ReadOnly );

        // Read the data serialized from the file.
        QTextStream  sourcestream( &sourcefile );
        sourcestream.setCodec("UTF-8");

        QFile  targetfile( targetfilename );
        targetfile.open( QIODevice::ReadWrite );

        QTextStream  targetstream( &targetfile );
        targetstream.setCodec("UTF-8");

        // gzFile gzDoc = gzopen( targetfilename.toLatin1(), "w");
        // QTextStream targetstream( new QString() );

        targetstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n"
                     << "<kml xmlns=\"http://earth.google.com/kml/2.1\"> \n"
                     << "<Document> \n";

        targetstream << "    <Schema name=\"MarblePlacemark\" parent=\"Placemark\"> \n";
        targetstream << "       <SimpleField name=\"pop\" type=\"int\"></SimpleField> \n";
        targetstream << "       <SimpleField name=\"role\" type=\"string\"></SimpleField> \n";
        targetstream << "    </Schema> \n";

        targetstream << "\n";

        QString  rawline;
        QString  nameString;
        QString  latString;
        QString  lonString;
        QString  popString;
        QString  roleString;
        QString  description;
        float    lat;
        float    lon;
        int          population;
        QStringList  splitline;

        while ( !sourcestream.atEnd() ) {

            rawline = sourcestream.readLine();
//            if ( !rawline.startsWith("\"E\"|\"m\"|\"" ) ) {
            if ( !rawline.startsWith(QLatin1String( "\"V\"|\"V\"|\"" ) ) ) {
                continue;
            }
            rawline.replace(QStringLiteral("\"|"), QStringLiteral("|"));
            rawline.replace(QStringLiteral("|\""), QStringLiteral("|"));
            if (rawline.startsWith(QLatin1Char('\"')) && rawline.endsWith(QLatin1Char('\"'))) {
                rawline = rawline.left( rawline.length() - 1 );
                rawline = rawline.right( rawline.length() - 2 );
            }

            splitline = rawline.split(QLatin1Char('|'));

            nameString  = splitline[2];
            latString   = splitline[3];
            lonString   = splitline[4];
            popString   = splitline[10];
            roleString  = splitline[18];
            description = splitline[19];

            // if (roleString == QLatin1String("SF")) continue;

            QString marbleRoleString = QLatin1String( "o" );

            if (roleString == QLatin1String("AA") || roleString == QLatin1String("SF")) marbleRoleString = "c";
            if (   roleString == QLatin1String("ME") || roleString == QLatin1String("OC")
                || roleString == QLatin1String("LC") || roleString == QLatin1String("SI")) marbleRoleString = "a";
            if (roleString == QLatin1String("MO")) marbleRoleString = "m";
            if (roleString == QLatin1String("VA")) marbleRoleString = "v";

            population = (int) ( 1000.0 * popString.toFloat() );

            lon = lonString.toFloat();

            if ( lon > 180.0 ) lon = lon - 360.0;

            if ( rawline.startsWith(QLatin1String( "\"M\"|\"M\"|\"" ) ) || rawline.startsWith("\"V\"|\"V\"|\"" ) ) {
                lon = -lon;
            }

            lat = latString.toFloat();

            description.remove(QLatin1Char('"'));

            targetstream << "    <MarblePlacemark> \n";
            targetstream << "        <name>" << escapeXml( nameString ) << "</name> \n";
            targetstream << "        <role>" << escapeXml( marbleRoleString ) << "</role> \n";
            targetstream << "        <pop>"
                         << escapeXml( QString::number( population ) ) << "</pop> \n";
            targetstream << "        <description>" << escapeXml( description ) << "</description> \n";
            targetstream << "        <Point>\n"
                         << "            <coordinates>"
                         << escapeXml( QString::number( lon ) )
                         << ","
                         << escapeXml( QString::number( lat ) )
                         << "</coordinates> \n"
                         << "        </Point> \n";
            targetstream << "    </MarblePlacemark> \n";
        }

        targetstream << "</Document> \n"
                     << "</kml> \n";
        qDebug("...");

        sourcefile.close();
        targetfile.close();

        qDebug("Finished!");
        return 0;
    }

    qDebug(" iau2kml -o targetfile sourcefile");
    app.exit();
}
