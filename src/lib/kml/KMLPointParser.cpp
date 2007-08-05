//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "KMLPointParser.h"

#include "GeoPoint.h"
#include "KMLPlaceMark.h"

namespace
{
    const QString POINT_TAG = "point";
    const QString COORDINATES_TAG = "coordinates";
}

KMLPointParser::KMLPointParser( KMLPlaceMark& placemark )
  : KMLObjectParser( placemark ),
    m_parsed( false ),
    m_waitCoordinates( false ),
    m_lon( 0.0 ),
    m_lat( 0.0 )
{
}

bool KMLPointParser::startElement( const QString& namespaceURI,
                            const QString& localName,
                            const QString& name,
                            const QXmlAttributes& atts )
{
    Q_UNUSED( namespaceURI );
    Q_UNUSED( localName );
    Q_UNUSED( atts );

    if ( m_parsed ) {
        return false;
    }

    m_level++;

    bool result = false;
    QString lowerName = name.toLower();

    if ( lowerName == POINT_TAG ) {
        result = true;
    }
    else if ( lowerName == COORDINATES_TAG ) {
        if ( ! m_waitCoordinates ) {
            m_waitCoordinates = true;
            result = true;
        }
    }

    return result;
}

bool KMLPointParser::endElement( const QString& namespaceURI,
                 const QString& localName,
                 const QString& qName )
{
    Q_UNUSED( namespaceURI );
    Q_UNUSED( localName );

    if ( m_parsed ) {
        return false;
    }

    bool result = false;
    QString lowerName = qName.toLower();

    if ( lowerName == POINT_TAG ) {
        KMLPlaceMark& placemark = (KMLPlaceMark&) m_object;
        placemark.setCoordinate( m_lon, m_lat );
        m_parsed = true;

        result = true;
    }
    else if ( lowerName == COORDINATES_TAG ) {
        if ( m_waitCoordinates ) {
            m_waitCoordinates = false;
            result = true;
        }
    }

    m_level--;

    return result;
}

bool KMLPointParser::characters( const QString& str )
{
    if ( m_parsed ) {
        /*
         * We already parsed start/stop
         * tags for this element
         */
        return false;
    }

    bool result = false;

    if ( m_waitCoordinates ) {
        QStringList splitline = str.split( "," );

        m_lon = deg2rad * splitline[0].toFloat();
        m_lat = -deg2rad * splitline[1].toFloat();

        result = true;
    }

    return result;
}
