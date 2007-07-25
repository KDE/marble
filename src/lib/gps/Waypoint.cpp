//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//

#include "Waypoint.h"
#include "katlasdirs.h"
#include "ClipPainter.h"

#include <QtGui/QPixmap>
#include <QtGui/QPainter>
#include <QtCore/QPoint>
#include <QtCore/QDebug>

Waypoint::Waypoint( double lat, double lon ):
                    AbstractLayerData( lat, lon ),
                    GpsElement(), m_elevation(0), m_time(0),
                    m_magVariation(0), m_geoIdHeight(0),
                    m_gpsSymbol(0), m_type(0), m_fix(0),
                    m_satalites(0), m_hdop(0), m_vdop(0),
                    m_pdop(0), m_ageOfGpsData(0), m_dgpsid(0){}

Waypoint::Waypoint( GeoPoint position ):
                    AbstractLayerData( position ), GpsElement(),
                    m_elevation(0), m_time(0), m_magVariation(0),
                    m_geoIdHeight(0), m_gpsSymbol(0), m_type(0),
                    m_fix(0), m_satalites(0), m_hdop(0), m_vdop(0),
                    m_pdop(0), m_ageOfGpsData(0), m_dgpsid(0){}

Waypoint::~Waypoint()
{
//     delete m_lat;
//     delete m_lon;
    delete m_elevation;
    delete m_time;
    delete m_magVariation;
    delete m_geoIdHeight;
    delete m_gpsSymbol;
    delete m_type;
    delete m_fix;
    delete m_satalites;
    delete m_hdop;
    delete m_vdop;
    delete m_pdop;
    delete m_ageOfGpsData;
    delete m_dgpsid;
}

void Waypoint::draw( ClipPainter * painter,
                              const QPoint &position)
{
    painter->drawEllipse( position.x(), position.y(), 3, 3 );
}

void Waypoint::draw( ClipPainter *painter, const QSize &canvasSize,
                     double radius, Quaternion invRotAxis )
{
    QPoint position;
    bool draw = false;
    
    draw = getPixelPos( canvasSize, invRotAxis, (int)radius, 
                        &position );
   
    if ( draw) {
        this->draw( painter, position );
    }
    
}

/*
QPixmap Waypoint::symbolPixmap()
{
    QPixmap temp( KAtlasDirs::path( "bitmaps/waypoint.png" ));
    return temp;
}
*/
/*
void Waypoint::setLat( const double &lat )
{
    m_lat = new double;
    *m_lat = lat;
}

void Waypoint::setLon( const double &lon )
{
    m_lon = new double;
    *m_lon = lon;
}*/

double Waypoint::elevation()
{
    return *m_elevation;
}

void Waypoint::setElevation( const double &elevation )
{
    m_elevation = new double;
    *m_elevation = elevation;
}

QTime Waypoint::time()
{
    return QTime( *m_time );
}

void Waypoint::setTime( const QTime &time )
{
    m_time = new QTime();
    *m_time = time;
}

double Waypoint::magVariation()
{
    return *m_magVariation;
}

void Waypoint::setMagVariation( const double &magVar )
{
    m_magVariation = new double;
    *m_magVariation = magVar;
}

double Waypoint::geoIdHeight()
{
    return *m_geoIdHeight;
}

void Waypoint::setGeoIdHeight( const double &geoHeightId )
{
    m_geoIdHeight = new double;
    *m_geoIdHeight = geoHeightId;
}

QString Waypoint::gpsSymbol()
{
    return QString( *m_gpsSymbol );
}

void Waypoint::setGpsSymbol( const QString &gpsSymbol )
{
    m_gpsSymbol = new QString( gpsSymbol );
}

QString Waypoint::type()
{
    return QString( *m_type );
}

void Waypoint::setType( const QString &type )
{
    m_type = new QString( type );
}

QString Waypoint::fix()
{
    return QString( *m_fix );
}

void Waypoint::setFix( const QString &fix )
{
    m_fix = new QString( fix );
}

int Waypoint::satalites()
{
    return *m_satalites;
}

void Waypoint::setSatalites( const int &satalites )
{
    m_satalites = new int;
    *m_satalites = satalites;
}

double Waypoint::hdop()
{
    return *m_hdop;
}

void Waypoint::setHdop( const double &hdop )
{
    m_hdop = new double;
    *m_hdop = hdop;
}

double Waypoint::vdop()
{
    return *m_vdop;
}

void Waypoint::setVdop( const double &vdop )
{
    m_vdop = new double;
    *m_vdop = vdop;
}

double Waypoint::pdop()
{
    return *m_pdop;
}

void Waypoint::setPdop( const double &pdop )
{
    m_pdop = new double;
    *m_pdop = pdop;
}

double Waypoint::ageOfGpsData()
{
    return *m_ageOfGpsData;
}

void Waypoint::setAgeOfGpsData( const double &age )
{
    m_ageOfGpsData = new double;
    *m_ageOfGpsData = age;
}

int Waypoint::dgpsid()
{
    return *m_dgpsid;
}

void Waypoint::setDgpsid( const int &gpsdid )
{
    m_dgpsid = new int;
    *m_dgpsid = gpsdid;
}
