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


// #include <zlib.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QStringList>


QString escapeXml( const QString &str )
{
    QString xml = str;
    xml.replace('&', "&amp;");
    xml.replace('<', "&lt;");
    xml.replace('>', "&gt;");
    xml.replace('\'', "&apos;");
    xml.replace('"', "&quot;");

    return xml;
}


int main(int argc, char *argv[])
{
    QString  sourcefilename;
    QString  targetfilename;
    QString  supportfilename;
    QString  timezonefilename;

    QCoreApplication  app( argc, argv );

    for ( int i = 1; i < argc; ++i ) {
        if ( strcmp( argv[ i ], "-o" ) != 0 )
            continue;

        targetfilename   = QString( argv[i+1] );
        sourcefilename   = QString( argv[i+2] );
        supportfilename  = QString( argv[i+3] );
        timezonefilename = QString( argv[i+4] );

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
                     << "<kml xmlns=\"http://earth.google.com/kml/2.1\"> \n"
                     << "<Document> \n";
        QString  rawline;
        QString  name;
    	QString  elestring;
        QString  state;
        QString  statecode;
        QString  country;
        QString  role;
        QString  popstring;
        QString  latstring;
        QString  lngstring;
        QString  timezone;
        QString  gmt;
        QString	 dst;
        int      dstoffset;
        int      gmtoffset;
        QStringList  splitline;

        while ( !sourcestream.atEnd() ) {

            rawline=sourcestream.readLine();
            splitline = rawline.split('\t');

            name       = splitline[1];
            latstring  = splitline[4];
            lngstring  = splitline[5];
            role       = splitline[7];
            country    = splitline[8];
            statecode  = splitline[10];
            popstring  = splitline[14];
            elestring  = splitline[16];
            timezone   = splitline[17];
	
            supportstream.seek(0);
            while ( !supportstream.atEnd() ) {
                QString supportrawline;
                QStringList supportsplitline;
                supportrawline = supportstream.readLine();
                supportsplitline = supportrawline.split('\t');
                if(supportsplitline[0] == (country + '.' +statecode))
                {
                    state = supportsplitline[1];
                    break;
                }
            }   

            timezonestream.seek(0);
            timezonestream.readLine();
            while ( !timezonestream.atEnd() ) {
                    QString timezonerawline;
                    QStringList timezonesplitline;
                    timezonerawline = timezonestream.readLine();
                    timezonesplitline = timezonerawline.split('\t');

                    if( timezonesplitline[0] == timezone )
                    {
                        gmt = timezonesplitline[1];
                        dst = timezonesplitline[2];
                        break;
                    }
            }

            gmtoffset = ( int ) ( gmt.toFloat() * 100 );
            dstoffset = ( int ) ( dst.toFloat() * 100 ) - gmtoffset;
	
            if(role != "PPLX")
            {          
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
