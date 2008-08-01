//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson  <g.real.ate@gmail.com>
// Copyright 2008      Torsten Rahn   <rahn@kde.org>
//


#include "GeoDataLatLonAltBox.h"

#include <QtCore/QDebug>

class GeoDataLatLonBoxPrivate
{
 public:
    GeoDataLatLonBoxPrivate()
        : m_north( +M_PI / 2.0),
          m_south( -M_PI / 2.0),
          m_east(  +M_PI ),
          m_west(  -M_PI ),
          m_rotation( 0.0 )
    {
    }

    double m_north;
    double m_south;
    double m_east;
    double m_west;
    double m_rotation; // NOT implemented yet!
};

bool operator==( GeoDataLatLonBox const& lhs, GeoDataLatLonBox const& rhs )
{
    return lhs.d->m_west == rhs.d->m_west
        && lhs.d->m_east == rhs.d->m_east
        && lhs.d->m_north == rhs.d->m_north
        && lhs.d->m_south == rhs.d->m_south
        && lhs.d->m_rotation == rhs.d->m_rotation;
}

GeoDataLatLonBox::GeoDataLatLonBox()
    : GeoDataObject(),
      d( new GeoDataLatLonBoxPrivate )
{
}

GeoDataLatLonBox::GeoDataLatLonBox( double north, double south, double east, double west, GeoDataCoordinates::Unit unit )
    : GeoDataObject(),
      d( new GeoDataLatLonBoxPrivate )
{
    setBoundaries( north, south, east, west, unit );
}

GeoDataLatLonBox::GeoDataLatLonBox( const GeoDataLatLonBox & other )
    : GeoDataObject( other ),
      d( new GeoDataLatLonBoxPrivate( *other.d ))
{
}

GeoDataLatLonBox::~GeoDataLatLonBox()
{
    delete d;
}

double GeoDataLatLonBox::north( GeoDataCoordinates::Unit unit ) const
{
    if ( unit == GeoDataCoordinates::Degree ) {
        return d->m_north * RAD2DEG;
    }
    return d->m_north;
}

void GeoDataLatLonBox::setNorth( const double north, GeoDataCoordinates::Unit unit )
{
    switch( unit ){
    case GeoDataCoordinates::Radian:
        d->m_north = GeoDataPoint::normalizeLat( north );
        break;
    case GeoDataCoordinates::Degree:
        d->m_north = GeoDataPoint::normalizeLat( north * DEG2RAD );
        break;
    }
}

double GeoDataLatLonBox::south( GeoDataCoordinates::Unit unit ) const
{
    if ( unit == GeoDataCoordinates::Degree ) {
        return d->m_south * RAD2DEG;
    }
    return d->m_south;
}

void GeoDataLatLonBox::setSouth( const double south, GeoDataCoordinates::Unit unit )
{
    switch( unit ){
    case GeoDataCoordinates::Radian:
        d->m_south = GeoDataPoint::normalizeLat( south );
        break;
    case GeoDataCoordinates::Degree:
        d->m_south = GeoDataPoint::normalizeLat( south * DEG2RAD );
        break;
    }
}


double GeoDataLatLonBox::east( GeoDataCoordinates::Unit unit ) const
{
    if ( unit == GeoDataCoordinates::Degree ) {
        return d->m_east * RAD2DEG;
    }
    return d->m_east;
}

void GeoDataLatLonBox::setEast( const double east, GeoDataCoordinates::Unit unit )
{
    switch( unit ){
    case GeoDataCoordinates::Radian:
        d->m_east = GeoDataPoint::normalizeLon( east );
        break;
    case GeoDataCoordinates::Degree:
        d->m_east = GeoDataPoint::normalizeLon( east * DEG2RAD );
        break;
    }
}

double GeoDataLatLonBox::west( GeoDataCoordinates::Unit unit ) const
{
    if ( unit == GeoDataCoordinates::Degree ) {
        return d->m_west * RAD2DEG;
    }
    return d->m_west;
}

void GeoDataLatLonBox::setWest( const double west, GeoDataCoordinates::Unit unit )
{
    switch( unit ){
    case GeoDataCoordinates::Radian:
        d->m_west = GeoDataPoint::normalizeLon( west );
        break;
    case GeoDataCoordinates::Degree:
        d->m_west = GeoDataPoint::normalizeLon( west * DEG2RAD );
        break;
    }
}

void GeoDataLatLonBox::setRotation( const double rotation, GeoDataCoordinates::Unit unit )
{
    switch( unit ){
    case GeoDataCoordinates::Radian:
        d->m_rotation = rotation;
        break;
    case GeoDataCoordinates::Degree:
        d->m_rotation = rotation * DEG2RAD;
        break;
    }
}

double GeoDataLatLonBox::rotation( GeoDataCoordinates::Unit unit ) const
{
    if ( unit == GeoDataCoordinates::Degree ) {
        return d->m_rotation * RAD2DEG;
    }
    return d->m_rotation;
}

void GeoDataLatLonBox::boundaries( double &west, double &east, double &north, double &south, GeoDataCoordinates::Unit unit )
{
    switch( unit ){
    case GeoDataCoordinates::Radian:
        north = d->m_north;
        south = d->m_south;
        east = d->m_east;
        west = d->m_west;
        break;
    case GeoDataCoordinates::Degree:
        north = d->m_north * RAD2DEG;
        south = d->m_south * RAD2DEG;
        east = d->m_east * RAD2DEG;
        west = d->m_west * RAD2DEG;
        break;
    }
}

void GeoDataLatLonBox::setBoundaries( double west, double east, double north, double south, GeoDataCoordinates::Unit unit )
{
    switch( unit ){
    case GeoDataCoordinates::Radian:
        d->m_north = GeoDataPoint::normalizeLat( north );
        d->m_south = GeoDataPoint::normalizeLat( south );
        d->m_east =  GeoDataPoint::normalizeLon( east );
        d->m_west =  GeoDataPoint::normalizeLon( west );
        break;
    case GeoDataCoordinates::Degree:
        d->m_north = GeoDataPoint::normalizeLat( north * DEG2RAD );
        d->m_south = GeoDataPoint::normalizeLat( south * DEG2RAD );
        d->m_east =  GeoDataPoint::normalizeLon( east * DEG2RAD );
        d->m_west =  GeoDataPoint::normalizeLon( west * DEG2RAD );
        break;
    }
}

bool GeoDataLatLonBox::crossesDateLine() const
{
    if ( d->m_east < d->m_west ) {
        return true;
    }

    return false;
}

bool GeoDataLatLonBox::contains( const GeoDataCoordinates &point )
{
    double lon, lat;

    point.geoCoordinates( lon, lat );

    // We need to take care of the normal case ...
    if ( ( ( lon < d->m_west || lon > d->m_east ) && ( d->m_west < d->m_east ) ) ||
    // ... and the case where the bounding box crosses the date line:
       ( ( lon < d->m_west || lon > d->m_east ) && ( d->m_west < d->m_east ) ) )
        return false;
    
    if ( lat < d->m_south || lat > d->m_north )
        return false;

    return true;
}

bool GeoDataLatLonBox::contains( const GeoDataPoint &point )
{
    double lon, lat;

    point.geoCoordinates( lon, lat );

    // We need to take care of the normal case ...
    if ( ( ( lon < d->m_west || lon > d->m_east ) && ( d->m_west < d->m_east ) ) ||
    // ... and the case where the bounding box crosses the date line:
       ( ( lon < d->m_west || lon > d->m_east ) && ( d->m_west < d->m_east ) ) )
        return false;
    
    if ( lat < d->m_south || lat > d->m_north )
        return false;

    return true;
}

bool GeoDataLatLonBox::intersects( const GeoDataLatLonBox & box )
{
    // Case 1: east border of box intersects:
    if ( d->m_east < box.east() && box.west() < d->m_east )
        return true;

    // Case 2: west border of box intersects:
    if ( d->m_west > box.west() && box.east() > d->m_west )
        return true;

    // Case 3: north border of box intersects:
    if ( d->m_north < box.north() && box.south() < d->m_north )
        return true;

    // Case 4: south border of box intersects:
    if ( d->m_south > box.south() && box.north() > d->m_south )
        return true;

    return false;
}

QString GeoDataLatLonBox::text( GeoDataCoordinates::Unit unit ) const
{
    switch( unit ){
    case GeoDataCoordinates::Radian:
        return QString( "North: %1; West: %2 \n South: %3; East: %4 " )
            .arg( d->m_north * RAD2DEG ).arg( d->m_west * RAD2DEG ).arg( d->m_south * RAD2DEG ).arg( d->m_east * RAD2DEG ); 
        break;
    case GeoDataCoordinates::Degree:
        return QString( "North: %1; West: %2 \n South: %3; East: %4 " )
            .arg( d->m_north * RAD2DEG ).arg( d->m_west * RAD2DEG ).arg( d->m_south * RAD2DEG ).arg( d->m_east * RAD2DEG ); 
        break;
    }

    return QString( "GeoDataLatLonBox::text(): Error in unit: %1\n" )
	.arg( unit );
}

GeoDataLatLonBox& GeoDataLatLonBox::operator=( const GeoDataLatLonBox &other )
{
    // FIXME: this check is not needed, remove or keep it?
    if ( this == &other )
        return *this;
    *d = *other.d;
    return *this;
}

void GeoDataLatLonBox::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << d->m_north << d->m_south << d->m_east << d->m_west << d->m_rotation;
}

void GeoDataLatLonBox::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );

    stream >> d->m_north >> d->m_south >> d->m_east >> d->m_west >> d->m_rotation;
}

class GeoDataLatLonAltBoxPrivate
{
 public:
    GeoDataLatLonAltBoxPrivate()
        : m_minAltitude( 0 ),
          m_maxAltitude( 0 ),
          m_altitudeMode( ClampToGround )
    {
    }

    double m_minAltitude;
    double m_maxAltitude;
    AltitudeMode m_altitudeMode;
};

bool operator==( GeoDataLatLonAltBox const& lhs, GeoDataLatLonAltBox const& rhs )
{
    return lhs.west() == rhs.west()
        && lhs.east() == rhs.east()
        && lhs.north() == rhs.north()
        && lhs.south() == rhs.south()
        && lhs.rotation() == rhs.rotation() 
        && lhs.d->m_minAltitude == rhs.d->m_minAltitude
        && lhs.d->m_maxAltitude == rhs.d->m_maxAltitude
        && lhs.d->m_altitudeMode == rhs.d->m_altitudeMode;
}

GeoDataLatLonAltBox& GeoDataLatLonAltBox::operator=( const GeoDataLatLonAltBox &other )
{
    // FIXME: this check is not needed, remove or keep it?
    if ( this == &other )
        return *this;

    setWest(  other.west() );
    setEast(  other.east() );
    setNorth( other.north() );
    setSouth( other.south() );
    setRotation( other.rotation() );

    *d = *other.d;
    return *this;
}

GeoDataLatLonAltBox::GeoDataLatLonAltBox()
    : GeoDataLatLonBox(),
      d( new GeoDataLatLonAltBoxPrivate )
{
}

GeoDataLatLonAltBox::GeoDataLatLonAltBox( const GeoDataLatLonAltBox & other )
    : GeoDataLatLonBox( other ),
      d( new GeoDataLatLonAltBoxPrivate( *other.d ))
{
}

GeoDataLatLonAltBox::~GeoDataLatLonAltBox()
{
    delete d;
}

double GeoDataLatLonAltBox::minAltitude() const
{
    return d->m_minAltitude;
}

void GeoDataLatLonAltBox::setMinAltitude( const double minAltitude )
{
    d->m_minAltitude = minAltitude;
}

double GeoDataLatLonAltBox::maxAltitude() const
{
    return d->m_maxAltitude;
}

void GeoDataLatLonAltBox::setMaxAltitude( const double maxAltitude )
{
    d->m_maxAltitude = maxAltitude;
}

AltitudeMode GeoDataLatLonAltBox::altitudeMode() const
{
    return d->m_altitudeMode;
}

void GeoDataLatLonAltBox::setAltitudeMode( const AltitudeMode altitudeMode )
{
    d->m_altitudeMode = altitudeMode;
}

bool GeoDataLatLonAltBox::contains( const GeoDataCoordinates &point )
{
    if ( !GeoDataLatLonBox::contains( point ) )
        return false;

    if ( point.altitude() < d->m_minAltitude || point.altitude() > d->m_maxAltitude ) {
        return false;
    }

    return true;
}

bool GeoDataLatLonAltBox::contains( const GeoDataPoint &point )
{
    if ( !GeoDataLatLonBox::contains( point ) )
        return false;

    if ( point.altitude() < d->m_minAltitude || point.altitude() > d->m_maxAltitude ) {
        return false;
    }

    return true;
}

bool GeoDataLatLonAltBox::intersects( const GeoDataLatLonAltBox & box )
{
    if ( GeoDataLatLonBox::intersects( box ) == true )
        return true;

    // Case 1: minimum altitude of box intersects:
    if ( d->m_minAltitude < box.maxAltitude() && box.minAltitude() < d->m_minAltitude )
        return true;

    // Case 2: maximum altitude of box intersects:
    if ( d->m_maxAltitude > box.minAltitude() && box.maxAltitude() > d->m_maxAltitude )
        return true;

    return false;
}

QString GeoDataLatLonAltBox::text( GeoDataCoordinates::Unit unit ) const
{
    switch( unit ){
    case GeoDataCoordinates::Radian:
        return QString( "North: %1; West: %2 MaxAlt: %3\n South: %4; East: %5 MinAlt: %6" )
	    .arg( north() ).arg( west() )
	    .arg( d->m_maxAltitude ).arg( south() )
	    .arg( east() ).arg( d->m_minAltitude ); 
        break;
    case GeoDataCoordinates::Degree:
        return QString( "North: %1; West: %2 MaxAlt: %3\n South: %4; East: %5 MinAlt: %6" )
            .arg( north() * RAD2DEG ).arg( west() * RAD2DEG )
	    .arg( d->m_maxAltitude ).arg( south() * RAD2DEG )
	    .arg( east() * RAD2DEG ).arg( d->m_minAltitude ); 
        break;
    }

    return QString( "GeoDataLatLonAltBox::text(): Error in unit: %1\n" )
	.arg( unit );
}

void GeoDataLatLonAltBox::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << d->m_minAltitude << d->m_maxAltitude;
    stream << d->m_altitudeMode;
}

void GeoDataLatLonAltBox::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );

    stream >> d->m_minAltitude >> d->m_maxAltitude;
    int a;
    stream >> a;
    d->m_altitudeMode = static_cast<AltitudeMode>( a );
}
