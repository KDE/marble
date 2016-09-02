//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2014      Levente Kurusa <levex@linux.com>
//

#include <QString>
#include <QUrl>
#include <QUrlQuery>

#include "Planet.h"
#include "PlanetFactory.h"
#include "GeoUriParser.h"
#include "MarbleDebug.h"

namespace Marble {

GeoUriParser::GeoUriParser( const QString& geoUri )
    : m_geoUri( geoUri ),
      m_coordinates(),
      m_planet(PlanetFactory::construct(QStringLiteral("earth")))
{
}

void GeoUriParser::setGeoUri( const QString &geoUri )
{
    m_geoUri = geoUri;
    m_coordinates = GeoDataCoordinates();
    m_planet = PlanetFactory::construct(QStringLiteral("earth"));
}

QString GeoUriParser::geoUri() const
{
    return m_geoUri;
}

GeoDataCoordinates GeoUriParser::coordinates() const
{
    return m_coordinates;
}

Planet GeoUriParser::planet() const
{
    return m_planet;
}

QString GeoUriParser::queryValue(const QUrl& url, const QString& one, const QString& two)
{
    QUrlQuery query( url );
    if ( query.hasQueryItem( one ) ) {
        return query.queryItemValue( one );
    } else if ( query.hasQueryItem( two )  ) {
        return query.queryItemValue( two );
    }

    return QString();
}

bool GeoUriParser::parse()
{
    if ( m_geoUri.isEmpty() ) {
        return false;
    }

    QString const floatRegexp = "[-+]?[0-9]*\\.?[0-9]+(?:[eE][-+]?[0-9]+)?";

    QRegExp geoUriRegexp( "geo:(" + floatRegexp + "),(" + floatRegexp + "),?(" + floatRegexp + ")?(?:;(crs|u)=([\\w\\d-]+))?(?:;(crs|u)=([\\w\\d-]+))?" , Qt::CaseInsensitive, QRegExp::RegExp2 );


    if ( geoUriRegexp.indexIn( m_geoUri ) > -1 && geoUriRegexp.captureCount() > 1 ) {
        double const lat = geoUriRegexp.capturedTexts()[1].toDouble();
        double const lon = geoUriRegexp.capturedTexts()[2].toDouble();
        double const alt = geoUriRegexp.captureCount() > 2 ? geoUriRegexp.capturedTexts()[3].toDouble() : 0.0;

        if ( geoUriRegexp.captureCount() > 3 ) {
            // this is not a bug! The '<=' was intended, otherwise we would skip that last Cgroups's data!
            for ( int i = 4; i <= geoUriRegexp.captureCount(); ++i )
            {
                if (geoUriRegexp.capturedTexts()[i] == QLatin1String("crs")) {
                    foreach ( const QString& str, PlanetFactory::planetList()) {
                        if ( geoUriRegexp.captureCount() < i+1 ) {
                            i = geoUriRegexp.captureCount() + 1;
                            break;
                        }
                        if ( geoUriRegexp.capturedTexts()[i+1].contains(str, Qt::CaseInsensitive) ) {
                            m_planet = PlanetFactory::construct( str );
                            break;
                        }
                    }
                    ++i;
                } else if (geoUriRegexp.capturedTexts()[i] == QLatin1String("u")) {
                    mDebug() << "Captured uncertainity parameter, but this is not supported by Marble (yet).";
                    ++i;
                }
            }
        }
        GeoDataCoordinates const coordinates( lon, lat, alt, GeoDataCoordinates::Degree );
        if ( coordinates.isValid() ) {
            m_coordinates = coordinates;
            return true;
        }
    }
    if ( m_geoUri.startsWith(QLatin1String("worldwind://goto/")) ) {
        m_geoUri.replace(QStringLiteral("goto/"), QStringLiteral("goto/?"));
        QUrl worldwindUrl( m_geoUri );

        double lat = queryValue(worldwindUrl, "lat", "latitude").toDouble();
        double lon = queryValue(worldwindUrl, "lon", "longitude").toDouble();
        double alt = queryValue(worldwindUrl, "alt", "altitude").toDouble();
        //double bank = getDoubleFromParameter(worldwindUrl, "bank", "");
        //double dir = getDoubleFromParameter(worldwindUrl, "dir", "direction");
        //double tilt = getDoubleFromParameter(worldwindUrl, "tilt", "");
        //QString layer = worldwindUrl.queryItemValue("layer");
        QString world = queryValue(worldwindUrl, "world");

        foreach ( const QString& str, PlanetFactory::planetList()) {
            if ( world.contains(str, Qt::CaseInsensitive) ) {
                m_planet = PlanetFactory::construct( str );
                break;
            }
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
