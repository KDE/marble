//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>"
//


#include "GeoDataCoordinates_p.h"
#include "GeoDataCoordinates.h"

#include <cmath>

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>
#include "global.h"

GeoDataCoordinates::Notation GeoDataCoordinates::s_notation = GeoDataCoordinates::DMS;

GeoDataCoordinates::GeoDataCoordinates( double _lon, double _lat, double _alt, GeoDataCoordinates::Unit unit, int _detail )
  : d_ptr( new GeoDataCoordinatesPrivate() )
{
    d_ptr->m_altitude = _alt;
    d_ptr->m_detail = _detail;
    switch( unit ){
    case Radian:
        d_ptr->m_q = Quaternion( _lon, _lat );
        d_ptr->m_lon = _lon;
        d_ptr->m_lat = _lat;
        break;
    case Degree:
        d_ptr->m_q = Quaternion( _lon * DEG2RAD , _lat * DEG2RAD  );
        d_ptr->m_lon = _lon * DEG2RAD;
        d_ptr->m_lat = _lat * DEG2RAD;
        break;
    }
}

GeoDataCoordinates::GeoDataCoordinates( const GeoDataCoordinates& other )
  : d_ptr( new GeoDataCoordinatesPrivate( *other.d_ptr ) )
{
}

GeoDataCoordinates::GeoDataCoordinates()
  : d_ptr( new GeoDataCoordinatesPrivate() )
{
}

GeoDataCoordinates::~GeoDataCoordinates()
{
    delete d_ptr;
#ifdef DEBUG_GEODATA
//    qDebug() << "delete coordinates";
#endif
}

void GeoDataCoordinates::set( double _lon, double _lat, double _alt, GeoDataCoordinates::Unit unit )
{
    d_ptr->m_altitude = _alt;
    switch( unit ){
    case Radian:
        d_ptr->m_q = Quaternion( _lon, _lat );
        d_ptr->m_lon = _lon;
        d_ptr->m_lat = _lat;
        break;
    case Degree:
        d_ptr->m_q = Quaternion( _lon * DEG2RAD , _lat * DEG2RAD  );
        d_ptr->m_lon = _lon * DEG2RAD;
        d_ptr->m_lat = _lat * DEG2RAD;
        break;
    }
}

void GeoDataCoordinates::geoCoordinates( double& lon, double& lat, 
                               GeoDataCoordinates::Unit unit ) const
{
    switch ( unit ) 
    {
    case Radian:
            lon = d_ptr->m_lon;
            lat = d_ptr->m_lat;
        break;
    case Degree:
            lon = d_ptr->m_lon * RAD2DEG;
            lat = d_ptr->m_lat * RAD2DEG;
        break;
    }
}

GeoDataCoordinates::Notation GeoDataCoordinates::defaultNotation()
{
    return s_notation;
}

void GeoDataCoordinates::setDefaultNotation( GeoDataCoordinates::Notation notation )
{
    s_notation = notation;
}

QString GeoDataCoordinates::toString()
{
    return GeoDataCoordinates::toString( s_notation );
}

QString GeoDataCoordinates::toString( GeoDataCoordinates::Notation notation )
{
    QString nsstring = ( d_ptr->m_lat > 0 ) ? QCoreApplication::tr("N") : QCoreApplication::tr("S");  
    QString westring = ( d_ptr->m_lon < 0 ) ? QCoreApplication::tr("W") : QCoreApplication::tr("E");  

    double lat, lon;
    lon = fabs( d_ptr->m_lon * RAD2DEG );
    lat = fabs( d_ptr->m_lat * RAD2DEG );

    if ( notation == GeoDataCoordinates::DMS )
    {
        int londeg = (int) lon;
        int lonmin = (int) ( 60 * (lon - londeg) );
        int lonsec = (int) ( 3600 * (lon - londeg - ((double)(lonmin) / 60) ) );


        int latdeg = (int) lat;
        int latmin = (int) ( 60 * (lat - latdeg) );
        int latsec = (int) ( 3600 * (lat - latdeg - ((double)(latmin) / 60) ) );

        return QString("%1\xb0 %2\' %3\"%4, %5\xb0 %6\' %7\"%8")
        .arg(londeg, 3, 10, QChar(' ') ).arg(lonmin, 2, 10, QChar('0') )
        .arg(lonsec, 2, 10, QChar('0') ).arg(westring)
    	.arg(latdeg, 3, 10, QChar(' ') ).arg(latmin, 2, 10, QChar('0') )
        .arg(latsec, 2, 10, QChar('0') ).arg(nsstring);
    }
    else // notation = GeoDataCoordinates::Decimal
    {
        return QString("%L1\xb0%2, %L3\xb0%4")
        .arg(lon, 6, 'f', 3, QChar(' ') ).arg(westring)
        .arg(lat, 6, 'f', 3, QChar(' ') ).arg(nsstring);
    }
}

bool GeoDataCoordinates::operator==( const GeoDataCoordinates &test ) const
{
    // Comparing 2 ints is faster than comparing 4 ints
    // Therefore we compare the Lon-Lat coordinates instead 
    // of the Position-Quaternion.

    double lonTest;
    double latTest;
    double lonThis;
    double latThis;
    
    geoCoordinates( lonThis, latThis );
    test.geoCoordinates( lonTest, latTest );
    
    return ( lonThis == lonTest 
             && latTest == latThis );
}

void GeoDataCoordinates::setAltitude( const double altitude )
{
    d_ptr->m_altitude = altitude;
}

double GeoDataCoordinates::altitude() const
{
    return d_ptr->m_altitude;
}

int GeoDataCoordinates::detail() const
{
    return d_ptr->m_detail;
}

void GeoDataCoordinates::setDetail( const int det )
{
    d_ptr->m_detail = det;
}

const Quaternion& GeoDataCoordinates::quaternion() const
{
    return d_ptr->m_q;
}

GeoDataCoordinates& GeoDataCoordinates::operator=( const GeoDataCoordinates &other )
{
    *d_ptr = *other.d_ptr;
    return *this;
}

void GeoDataCoordinates::pack( QDataStream& stream ) const
{
    stream << d_ptr->m_lon;
    stream << d_ptr->m_lat;
    stream << d_ptr->m_altitude;
}

void GeoDataCoordinates::unpack( QDataStream& stream )
{
    stream >> d_ptr->m_lon;
    stream >> d_ptr->m_lat;
    stream >> d_ptr->m_altitude;

    d_ptr->m_q.set( d_ptr->m_lon, d_ptr->m_lat );
}
