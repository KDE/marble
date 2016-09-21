//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007        Inge Wallin   <ingwa@kde.org>
// Copyright 2007-2012   Torsten Rahn  <rahn@kde.org>
// Copyright 2012        Cezar Mocan   <mocancezar@gmail.com>
//

// Local
#include "AbstractProjection.h"

#include "AbstractProjection_p.h"

#include "MarbleDebug.h"
#include <QRegion>
#include <QPainterPath>

// Marble
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataLatLonAltBox.h"
#include "ViewportParams.h"

using namespace Marble;

AbstractProjection::AbstractProjection()
    : d_ptr( new AbstractProjectionPrivate( this ) )
{
}

AbstractProjection::AbstractProjection( AbstractProjectionPrivate* dd )
    : d_ptr( dd )
{
}

AbstractProjection::~AbstractProjection()
{
}

AbstractProjectionPrivate::AbstractProjectionPrivate( AbstractProjection * parent )
    : m_maxLat(0),
      m_minLat(0),
      m_previousResolution(-1),
      m_level(-1),
      q_ptr( parent)
{
}

int AbstractProjectionPrivate::levelForResolution(qreal resolution) const {
    if (m_previousResolution == resolution) return m_level;

    m_previousResolution = resolution;

    if (resolution < 0.0000005) m_level = 17;
    else if (resolution < 0.0000010) m_level = 16;
    else if (resolution < 0.0000020) m_level = 15;
    else if (resolution < 0.0000040) m_level = 14;
    else if (resolution < 0.0000080) m_level = 13;
    else if (resolution < 0.0000160) m_level = 12;
    else if (resolution < 0.0000320) m_level = 11;
    else if (resolution < 0.0000640) m_level = 10;
    else if (resolution < 0.0001280) m_level = 9;
    else if (resolution < 0.0002560) m_level = 8;
    else if (resolution < 0.0005120) m_level = 7;
    else if (resolution < 0.0010240) m_level = 6;
    else if (resolution < 0.0020480) m_level = 5;
    else if (resolution < 0.0040960) m_level = 4;
    else if (resolution < 0.0081920) m_level = 3;
    else if (resolution < 0.0163840) m_level = 2;
    else m_level =  1;

    return m_level;
}

qreal AbstractProjection::maxValidLat() const
{
    return +90.0 * DEG2RAD;
}

qreal AbstractProjection::maxLat() const
{
    Q_D(const AbstractProjection );
    return d->m_maxLat;
}

void AbstractProjection::setMaxLat( qreal maxLat )
{
    if ( maxLat < maxValidLat() ) {
        mDebug() << Q_FUNC_INFO << "Trying to set maxLat to a value that is out of the valid range.";
        return;
    }

    Q_D( AbstractProjection );
    d->m_maxLat = maxLat;
}

qreal AbstractProjection::minValidLat() const
{
    return -90.0 * DEG2RAD;
}

qreal AbstractProjection::minLat() const
{
    Q_D( const AbstractProjection );
    return d->m_minLat;
}

void AbstractProjection::setMinLat( qreal minLat )
{
    if ( minLat < minValidLat() ) {
        mDebug() << Q_FUNC_INFO << "Trying to set minLat to a value that is out of the valid range.";
        return;
    }

    Q_D( AbstractProjection );
    d->m_minLat = minLat;
}

bool AbstractProjection::repeatableX() const
{
    return false;
}

bool AbstractProjection::traversablePoles() const
{
    return false;
}

bool AbstractProjection::traversableDateLine() const
{
    return false;
}

AbstractProjection::PreservationType AbstractProjection::preservationType() const
{
    return NoPreservation;
}

bool AbstractProjection::isOrientedNormal() const
{
    return true;
}

bool AbstractProjection::isClippedToSphere() const
{
    return false;
}

qreal AbstractProjection::clippingRadius() const
{
    return 0;
}


bool AbstractProjection::screenCoordinates( const qreal lon, const qreal lat,
                                            const ViewportParams *viewport,
                                            qreal &x, qreal &y ) const
{
    bool globeHidesPoint;
    GeoDataCoordinates geopoint(lon, lat);
    return screenCoordinates( geopoint, viewport, x, y, globeHidesPoint );
}

bool AbstractProjection::screenCoordinates( const GeoDataCoordinates &geopoint,
                                            const ViewportParams *viewport,
                                            qreal &x, qreal &y ) const
{
    bool globeHidesPoint;

    return screenCoordinates( geopoint, viewport, x, y, globeHidesPoint );
}

GeoDataLatLonAltBox AbstractProjection::latLonAltBox( const QRect& screenRect,
                                                      const ViewportParams *viewport ) const
{
    // For the case where the whole viewport gets covered there is a 
    // pretty dirty and generic detection algorithm:

    // Move along the screenborder and save the highest and lowest lon-lat values.
    QRect projectedRect = mapRegion( viewport ).boundingRect();
    QRect mapRect = screenRect.intersected( projectedRect );

    GeoDataLineString boundingLineString;

    qreal lon, lat;

    for ( int x = mapRect.left(); x < mapRect.right(); x += latLonAltBoxSamplingRate ) {
        if ( geoCoordinates( x, mapRect.bottom(), viewport, lon, lat,
                             GeoDataCoordinates::Radian ) ) {
            boundingLineString << GeoDataCoordinates( lon, lat );
        }

        if ( geoCoordinates( x, mapRect.top(),
                             viewport, lon, lat, GeoDataCoordinates::Radian ) ) {
            boundingLineString << GeoDataCoordinates( lon, lat );
        }
    }

    if ( geoCoordinates( mapRect.right(), mapRect.top(), viewport, lon, lat,
                         GeoDataCoordinates::Radian ) ) {
        boundingLineString << GeoDataCoordinates( lon, lat );
    }

    if ( geoCoordinates( mapRect.right(), mapRect.bottom(),
                         viewport, lon, lat, GeoDataCoordinates::Radian ) ) {
        boundingLineString << GeoDataCoordinates( lon, lat );
    }

    for ( int y = mapRect.bottom(); y < mapRect.top(); y += latLonAltBoxSamplingRate ) {
        if ( geoCoordinates( mapRect.left(), y, viewport, lon, lat,
                             GeoDataCoordinates::Radian ) ) {
            boundingLineString << GeoDataCoordinates( lon, lat );
        }

        if ( geoCoordinates( mapRect.right(), y,
                             viewport, lon, lat, GeoDataCoordinates::Radian ) ) {
            boundingLineString << GeoDataCoordinates( lon, lat );
        }
    }

    GeoDataLatLonAltBox latLonAltBox = boundingLineString.latLonAltBox();
    
    // Now we need to check whether maxLat (e.g. the north pole) gets displayed
    // inside the viewport.

    // We need a point on the screen at maxLat that definitely gets displayed:

    // FIXME: Some of the following code can be safely removed as soon as we properly handle
    //        GeoDataLinearRing::latLonAltBox().
    qreal averageLongitude = ( latLonAltBox.west() + latLonAltBox.east() ) / 2.0;

    GeoDataCoordinates maxLatPoint( averageLongitude, maxLat(), 0.0, GeoDataCoordinates::Radian );
    GeoDataCoordinates minLatPoint( averageLongitude, minLat(), 0.0, GeoDataCoordinates::Radian );

    qreal dummyX, dummyY; // not needed

    if ( latLonAltBox.north() > maxLat() ||
         screenCoordinates( maxLatPoint, viewport, dummyX, dummyY ) ) {
        latLonAltBox.setNorth( maxLat() );
    }
    if ( latLonAltBox.north() < minLat() ||
         screenCoordinates( minLatPoint, viewport, dummyX, dummyY ) ) {
        latLonAltBox.setSouth( minLat() );
    }

    latLonAltBox.setMinAltitude(      -100000000.0 );
    latLonAltBox.setMaxAltitude( 100000000000000.0 );

    return latLonAltBox;
}


QRegion AbstractProjection::mapRegion( const ViewportParams *viewport ) const
{
    return QRegion( mapShape( viewport ).toFillPolygon().toPolygon() );
}
