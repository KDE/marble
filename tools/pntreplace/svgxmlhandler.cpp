//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2012 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//


#include "svgxmlhandler.h"
#include <QtCore/QDebug>

SVGXmlHandler::SVGXmlHandler( QDataStream * out, const QString & path, int header )
    : m_stream(out),
      m_header(header),
      m_path(path)
{
}

bool SVGXmlHandler::startElement(const QString& nspace,
                                 const QString& localName,
                                 const QString& qName,
                                 const QXmlAttributes &atts)
{
    if ( qName == "path" && atts.value( "id" ) == m_path ) {
        QString  coordinates = atts.value( "d" );

        QStringList  stringlist;
        coordinates.chop(2);

        // This requires absolute paths and repeated L commands to
        // to be enforced in inkscape!
        stringlist << coordinates.mid( 1 ).split( 'L' );
        bool     firstheader = true;

        int  count = 0;
        qDebug() << "Starting to write path" << atts.value( "id" );
        foreach ( const QString& str, stringlist ) {
            float  x;
            float  y;
            x = str.section( ',', 0, 0 ).toFloat();
            y = str.section( ',', 1, 1 ).toFloat();
						
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

            *m_stream << header << lat << lng;
            count++;
        }		
    }

    return true;
}
