//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Levente Kurusa <levex@linux.com>
//


#include "GeoUriParser.h"
#include <QString>

#include "MarbleDebug.h"

namespace Marble {

GeoUriParser::GeoUriParser( const QString& geoUri )
    : m_geoUri( geoUri ),
      m_coordinates()
{
}

void GeoUriParser::setGeoUri( const QString &geoUri )
{
    m_geoUri = geoUri;
    m_coordinates = GeoDataCoordinates();
}

QString GeoUriParser::geoUri() const
{
    return m_geoUri;
}

GeoDataCoordinates GeoUriParser::coordinates() const
{
    return m_coordinates;
}

bool GeoUriParser::parse()
{
    if ( m_geoUri.isEmpty() ) {
        return false;
    }

    QString const floatRegexp = "[-+]?[0-9]*\\.?[0-9]+(?:[eE][-+]?[0-9]+)?";

    QRegExp geoUriRegexp( "(?:geo:)(" + floatRegexp + "),(" + floatRegexp + "),?(" + floatRegexp + ")?(?:\;crs\=)?(.*)?(?:[\,\;]u\=)?(\d+)?" , Qt::CaseSensitive, QRegExp::RegExp2 );

    if ( geoUriRegexp.indexIn( m_geoUri ) > -1 && geoUriRegexp.captureCount() > 1 ) {
        double const lat = geoUriRegexp.capturedTexts()[1].toDouble();
        double const lon = geoUriRegexp.capturedTexts()[2].toDouble();
        double const alt = geoUriRegexp.captureCount() > 2 ? geoUriRegexp.capturedTexts()[3].toDouble() : 0.0;

        if ( geoUriRegexp.captureCount() > 3 ) {
            mDebug() << "Captured crs or u parameter, but those are not supported by Marble (yet).";
        }
        GeoDataCoordinates const coordinates( lon, lat, alt, GeoDataCoordinates::Degree );
        if ( coordinates.isValid() ) {
            m_coordinates = coordinates;
            return true;
        }
    }
    return false;
}

}
