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


#include "svgxmlhandler.h"
#include <QtCore/QDebug>

SVGXmlHandler::SVGXmlHandler(const QString& targetfile)
{
    m_header      = 8000;
    m_pointnum    = 0;
    m_initialized = false;
    m_targetfile  = targetfile;
}

bool SVGXmlHandler::startElement(const QString& nspace,
                                 const QString& localName,
                                 const QString& qName,
                                 const QXmlAttributes &atts)
{
    qDebug();
    if ( qName == "g" ) {
        qDebug( "Parsing Data ..." );
        m_initialized = true;
    }

    if ( qName == "path" && m_initialized ) {
        QString  coordinates = atts.value( "d" );

        QStringList  stringlist;
        coordinates.chop(2);
        stringlist << coordinates.mid( 1 ).split( 'L' );
        // The last element is the first element
        //		stringlist.removeLast();
        bool     firstheader = true;
        QFile    file( m_targetfile );

        // Read the data serialized from the file.
        file.open( QIODevice::Append );
        QDataStream stream( &file );
        stream.setByteOrder( QDataStream::LittleEndian );

        int  count = 0;
        qDebug() << "Starting to write path" << atts.value( "id" );
        foreach ( const QString& str, stringlist ) {
            // qDebug()<<str;
            float  x;
            float  y;
            x = str.section( ',', 0, 0 ).toFloat();
            y = str.section( ',', 1, 1 ).toFloat();
			
            // qDebug() << "x:" << x << "y:" << y;
			
            short  header;
            short  lat;
            short  lng;	

            if ( firstheader ) {
                header      = m_header;
                firstheader = false;
            }
            else {
                if ( stringlist.size() > 14 ) {
                    if ( count % 9 == 0 ) 
                        header = 5;
                    else if ( count % 5 == 0 )
                        header = 3;
                    else if ( count % 2 == 0 )
                        header = 2;
                    else
                        header = 1;
                }
                else if ( stringlist.size() > 6 ) {
                    if ( count % 2 == 0 )
                        header = 3;
                    else
                        header = 1;
                }
                else {
                    header = 2;
                }
            }
            if ( count == stringlist.size() - 1 )
                header = 5;

            lng =  (int)( x * 50 - 10800 );
            lat = -(int)( y * 50 - 5400 );

            qDebug() << "lng:" << lng << "lat:" << lat << "header:"
                     << header << "node#:" << m_pointnum;

            stream << header << lat << lng;	
            m_pointnum++;	
            count++;		
        }
		
        m_header++;
    }

    return true;
}
