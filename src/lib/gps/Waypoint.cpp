//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "Waypoint.h"

//#include "MarbleDirs.h"
#include "ClipPainter.h"

#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtCore/QPoint>
#include "MarbleDebug.h"

using namespace Marble;

Waypoint::Waypoint( qreal lat, qreal lon )
    : AbstractLayerData( lat, lon ),
      GpsElement(),
      m_elevation(0.0),
      m_magVariation(0.0),
      m_geoIdHeight(0.0),
      m_satalites(0),
      m_hdop(0.0),
      m_vdop(0.0),
      m_pdop(0.0),
      m_ageOfGpsData(0.0),
      m_dgpsid(0)
{
}

Waypoint::Waypoint( const GeoDataCoordinates& position )
    : AbstractLayerData( position ),
      GpsElement(),
      m_elevation(0.0),
      m_magVariation(0.0),
      m_geoIdHeight(0.0),
      m_satalites(0),
      m_hdop(0.0),
      m_vdop(0.0),
      m_pdop(0.0),
      m_ageOfGpsData(0.0),
      m_dgpsid(0)
{
}

void Waypoint::draw( ClipPainter * painter,
                              const QPoint &position)
{
    painter->drawEllipse( position.x(), position.y(), 3, 3 );
}

void Waypoint::draw( ClipPainter *painter, const QSize &canvasSize,
                     ViewParams *viewParams )
{
    QPoint position;
    const bool draw = getPixelPos( canvasSize, viewParams, &position );
    if ( draw) {
        this->draw( painter, position );
    }
}

void Waypoint::printToStream( QTextStream &out ) const 
{
    out     << "<wpt lat=\"" << lat() 
            << "\" lon=\"" << lon() << "\">\n";
    printBodyToStream( out );
    out     << "</wpt>\n";
}

void Waypoint::printBodyToStream( QTextStream &out ) const
{
    if ( m_elevation ) {
        out << "<ele>" << m_elevation << "</ele>\n";
    }
    //TODO:: add rest of body GPX elements here
}

/*
QPixmap Waypoint::symbolPixmap()
{
    QPixmap temp( MarbleDirs::path( "bitmaps/waypoint.png" ));
    return temp;
}
*/
/*
void Waypoint::setLat( const qreal &lat )
{
    m_lat = new qreal;
    *m_lat = lat;
}

void Waypoint::setLon( const qreal &lon )
{
    m_lon = new qreal;
    *m_lon = lon;
}*/

qreal Waypoint::elevation() const
{
    return m_elevation;
}

void Waypoint::setElevation( const qreal &elevation )
{
    m_elevation = elevation;
}

QTime Waypoint::time() const
{
    return m_time;
}

void Waypoint::setTime( const QTime &time )
{
    m_time = time;
}

qreal Waypoint::magVariation() const
{
    return m_magVariation;
}

void Waypoint::setMagVariation( const qreal &magVar )
{
    m_magVariation = magVar;
}

qreal Waypoint::geoIdHeight() const
{
    return m_geoIdHeight;
}

void Waypoint::setGeoIdHeight( const qreal &geoHeightId )
{
    m_geoIdHeight = geoHeightId;
}

QString Waypoint::gpsSymbol() const
{
    return m_gpsSymbol;
}

void Waypoint::setGpsSymbol( const QString &gpsSymbol )
{
    m_gpsSymbol = gpsSymbol;
}

QString Waypoint::type() const
{
    return m_type;
}

void Waypoint::setType( const QString &type )
{
    m_type = type;
}

QString Waypoint::fix() const
{
    return m_fix;
}

void Waypoint::setFix( const QString &fix )
{
    m_fix = fix;
}

int Waypoint::satalites() const
{
    return m_satalites;
}

void Waypoint::setSatalites( const int &satalites )
{
    m_satalites = satalites;
}

qreal Waypoint::hdop() const
{
    return m_hdop;
}

void Waypoint::setHdop( const qreal &hdop )
{
    m_hdop = hdop;
}

qreal Waypoint::vdop() const
{
    return m_vdop;
}

void Waypoint::setVdop( const qreal &vdop )
{
    m_vdop = vdop;
}

qreal Waypoint::pdop() const
{
    return m_pdop;
}

void Waypoint::setPdop( const qreal &pdop )
{
    m_pdop = pdop;
}

qreal Waypoint::ageOfGpsData() const
{
    return m_ageOfGpsData;
}

void Waypoint::setAgeOfGpsData( const qreal &age )
{
    m_ageOfGpsData = age;
}

int Waypoint::dgpsid() const
{
    return m_dgpsid;
}

void Waypoint::setDgpsid( const int &gpsdid )
{
    m_dgpsid = gpsdid;
}
