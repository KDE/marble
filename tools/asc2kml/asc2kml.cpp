//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2010      Harshit Jain <hjain.itbhu@gmail.com>
//


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
    QCoreApplication  app( argc, argv );

    for ( int i = 1; i < argc; ++i ) {
        if ( strcmp( argv[ i ], "-o" ) != 0 )
            continue;

        const QString targetfilename   = QString( argv[i+1] );
        const QString sourcefilename   = QString( argv[i+2] );
        const QString supportfilename  = QString( argv[i+3] );
        const QString timezonefilename = QString( argv[i+4] );

        qDebug() << "Source: " << sourcefilename;
        qDebug() << "Support: " << supportfilename;
        qDebug() << "Target: " << targetfilename;
        qDebug() << "Timezone: " << timezonefilename;

        QFile  sourcefile( sourcefilename );
        sourcefile.open( QIODevice::ReadOnly );

        // Read the data serialized from the file.
        QTextStream  sourcestream( &sourcefile );
        sourcestream.setCodec("UTF-8");

        QFile  targetfile( targetfilename );
        targetfile.open( QIODevice::WriteOnly );

        QTextStream  targetstream( &targetfile );
        targetstream.setCodec("UTF-8");

        QFile  supportfile( supportfilename );
        supportfile.open( QIODevice::ReadOnly );

        QTextStream  supportstream( &supportfile );
        supportstream.setCodec("UTF-8");

        QFile  timezonefile( timezonefilename );
        timezonefile.open( QIODevice::ReadOnly );

        QTextStream  timezonestream( &timezonefile );
        timezonestream.setCodec("UTF-8");

        // gzFile gzDoc = gzopen( targetfilename.toLatin1(), "w");
        // QTextStream targetstream( new QString() );

        targetstream << "<?xml version=\"1.0\" encoding=\"UTF-8\"?> \n"
                     << "<kml xmlns=\"http://www.opengis.net/kml/2.2\"> \n"
                     << "<Document> \n";
        QString  state;
        QString  gmt;
        QString	 dst;

        while ( !sourcestream.atEnd() ) {

            const QString rawline = sourcestream.readLine();
            const QStringList splitline = rawline.split(QLatin1Char('\t'));

            const QString name       = splitline[1];
            const QString latstring  = splitline[4];
            const QString lngstring  = splitline[5];
            const QString role       = splitline[7];
            const QString country    = splitline[8];
            const QString statecode  = splitline[10];
            const QString popstring  = splitline[14];
            const QString elestring  = splitline[16];
            const QString timezone   = splitline[17];
	
            supportstream.seek(0);
            while ( !supportstream.atEnd() ) {
                const QString supportrawline = supportstream.readLine();
                const QStringList supportsplitline = supportrawline.split(QLatin1Char('\t'));
                if (supportsplitline[0] == (country + QLatin1Char('.') +statecode)) {
                    state = supportsplitline[1];
                    break;
                }
            }   

            timezonestream.seek(0);
            timezonestream.readLine();
            while ( !timezonestream.atEnd() ) {
                    const QString timezonerawline = timezonestream.readLine();
                    const QStringList timezonesplitline = timezonerawline.split(QLatin1Char('\t'));

                    if( timezonesplitline[1] == timezone )
                    {
                        gmt = timezonesplitline[2];
                        dst = timezonesplitline[3];
                        break;
                    }
            }

            const int gmtoffset = ( int ) ( gmt.toFloat() * 100 );
            const int dstoffset = ( int ) ( dst.toFloat() * 100 ) - gmtoffset;
	
            if (role != QLatin1String("PPLX")) {
	            targetstream << "    <Placemark> \n";
         	    targetstream << "        <name>" << escapeXml( name ) << "</name> \n";
                targetstream << "        <state>" << escapeXml( state ) << "</state> \n";
      	        targetstream << "        <CountryNameCode>" << escapeXml( country.toUpper() ) << "</CountryNameCode>\n";
        	    targetstream << "        <role>" << escapeXml( role ) << "</role> \n";
        	    targetstream << "        <pop>"
                             << escapeXml( popstring ) << "</pop> \n";
           	    targetstream << "        <Point>\n"
                        	 << "            <coordinates>"
                      		 << escapeXml( lngstring )
                        	 << ","
                        	 << escapeXml( latstring )
				<< ","
                             << escapeXml( elestring )
	                         << "</coordinates> \n"
	                         << "        </Point> \n";
		        targetstream << "        <ExtendedData>\n"
                                 << "            <Data name=\"gmt\">\n"
                                 << "                <value>" << escapeXml( QString::number( gmtoffset ) ) << "</value>\n"
                                 << "            </Data>\n";
                    if( dstoffset )
                    {
                        targetstream << "            <Data name=\"dst\">\n"
                                 << "                <value>" << escapeXml( QString::number( dstoffset) ) << "</value>\n"
                                 << "            </Data>\n";
                    }
                    targetstream << "        </ExtendedData>\n";  
	            targetstream << "    </Placemark> \n";
            }
        }

        targetstream << "</Document> \n"
                     << "</kml> \n";
        qDebug("Putting");

        // gzputs( gzDoc, targetstream.readAll().toUtf8() );
        // gzclose( gzDoc );

        sourcefile.close();
        targetfile.close();
        supportfile.close();
        timezonefile.close();
        qDebug("Finished!");
        return 0;
    }

    qDebug(" asc2kml -o targetfile sourcefile supporfile timezonefile");
    app.exit();
}
