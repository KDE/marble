//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//


#include "ViewportParams.h"

#include <QtCore/qmath.h>
#include <QtCore/QRect>

#include <QtGui/QPainterPath>
#include <QtGui/QPainterPathStroker>
#include <QtGui/QRegion>

#include "MarbleDebug.h"
#include "SphericalProjection.h"
#include "EquirectProjection.h"
#include "MercatorProjection.h"


namespace Marble
{

class ViewportParamsPrivate
{
public:
    ViewportParamsPrivate();

    void setPlanetAxis( const Quaternion &newAxis );

    // These two go together.  m_currentProjection points to one of
    // the static Projection classes at the bottom.
    Projection           m_projection;
    const AbstractProjection *m_currentProjection;

    // Parameters that determine the painting
    qreal                m_centerLongitude;
    qreal                m_centerLatitude;
    Quaternion           m_planetAxis;   // Position, coded in a quaternion
    mutable matrix       m_planetAxisMatrix;
    int                  m_radius;       // Zoom level (pixels / globe radius)
    qreal                m_angularResolution;
    int                  m_detailLevel;

    static const qreal   m_constantDetailFactor = 162.974665751;
    static const qreal   m_constantLnOneOverTwo = -0.693147181;

    QSize                m_size;         // width, height


    bool                 m_dirtyBox;
    bool                 m_dirtyDetailLevel;
    GeoDataLatLonAltBox  m_viewLatLonAltBox;

    static const SphericalProjection  s_sphericalProjection;
    static const EquirectProjection   s_equirectProjection;
    static const MercatorProjection   s_mercatorProjection;

    GeoDataCoordinates   m_focusPoint;
    bool                 m_hasFocusPoint;

};

ViewportParamsPrivate::ViewportParamsPrivate()
    : m_projection( Spherical ),
      m_currentProjection( &s_sphericalProjection ),
      m_centerLongitude( 0 ),
      m_centerLatitude( 0 ),
      m_planetAxis( 1.0, 0.0, 0.0, 0.0 ), // Default view
      m_planetAxisMatrix(),
      m_radius( 2000 ),
      m_angularResolution( 0.25 * M_PI / fabs( (qreal)( m_radius ) ) ),
      m_detailLevel( 0 ),
      m_size( 100, 100 ),
      m_dirtyBox( true ),
      m_dirtyDetailLevel( true ),
      m_viewLatLonAltBox(),
      m_hasFocusPoint(false)
{
    m_planetAxis.inverse().toMatrix( m_planetAxisMatrix );
}


const SphericalProjection  ViewportParamsPrivate::s_sphericalProjection;
const EquirectProjection   ViewportParamsPrivate::s_equirectProjection;
const MercatorProjection   ViewportParamsPrivate::s_mercatorProjection;


ViewportParams::ViewportParams()
    : d( new ViewportParamsPrivate )
{
}

ViewportParams::~ViewportParams()
{
    delete d;
}


// ================================================================
//                    Getters and setters


Projection ViewportParams::projection() const
{
    return d->m_projection;
}

const AbstractProjection *ViewportParams::currentProjection() const
{
    return d->m_currentProjection;
}

void ViewportParams::setProjection(Projection newProjection)
{
    d->m_dirtyBox = true;

    d->m_projection = newProjection;

    // Set the pointer to the current projection class.
    switch ( newProjection ) {
    case Spherical:
        d->m_currentProjection = &d->s_sphericalProjection;
        break;
    case Equirectangular:
        d->m_currentProjection = &d->s_equirectProjection;
        break;
    case Mercator:
        d->m_currentProjection = &d->s_mercatorProjection;
        break;
    }

    // We now need to reset the planetAxis to make sure
    // that it's a valid axis orientation!
    // So this line is important (although it might look odd) ! :
    centerOn( d->m_centerLongitude, d->m_centerLatitude );
}

int ViewportParams::polarity() const
{
    // For mercator this just gives the extreme latitudes
    // instead of the actual poles but it works fine as well:
    GeoDataCoordinates northPole( 0.0, +currentProjection()->maxLat() );
    GeoDataCoordinates southPole( 0.0, -currentProjection()->maxLat() );

    bool globeHidesN, globeHidesS;
    qreal x;
    qreal yN, yS;

    currentProjection()->screenCoordinates( northPole, this,
                                          x, yN, globeHidesN );
    currentProjection()->screenCoordinates( southPole, this,
                                          x, yS, globeHidesS );

    int polarity = 0;

    // case of the flat map:
    if ( !globeHidesN && !globeHidesS ) {
        if ( yN < yS ) {
            polarity = +1;
        }
        if ( yS < yN ) {
            polarity = -1;
        }
    }
    else {
        if ( !globeHidesN && yN < height() / 2 ) {
            polarity = +1;
        }
        if ( !globeHidesN && yN > height() / 2 ) {
            polarity = -1;
        }
        if ( !globeHidesS && yS > height() / 2 ) {
            polarity = +1;
        }
        if ( !globeHidesS && yS < height() / 2 ) {
            polarity = -1;
        }
    }

    return polarity;
}

int ViewportParams::radius() const
{
    return d->m_radius;
}

void ViewportParams::setRadius(int newRadius)
{
    if ( newRadius > 0 ) {
        d->m_dirtyBox = true;
        d->m_dirtyDetailLevel = true;

        d->m_radius = newRadius;
        d->m_angularResolution = 0.25 * M_PI / fabs( (qreal)(d->m_radius) );
    }
}

bool ViewportParams::globeCoversViewport() const
{
    // This first test is a quick one that will catch all really big
    // radii and prevent overflow in the real test.
    if ( d->m_radius > d->m_size.width() + d->m_size.height() )
        return true;

    // This is the real test.  The 4 is because we are really
    // comparing to width/2 and height/2.
    if ( 4 * d->m_radius * d->m_radius >= d->m_size.width() * d->m_size.width()
                                          + d->m_size.height() * d->m_size.height() )
        return true;

    return false;
}

void ViewportParams::centerOn( qreal lon, qreal lat )
{
    if ( !d->m_currentProjection->traversablePoles() ) {
        if ( lat > d->m_currentProjection->maxLat() )
            lat = d->m_currentProjection->maxLat();

        if ( lat < d->m_currentProjection->minLat() )
            lat = d->m_currentProjection->minLat();
    } else {
        while ( lat > M_PI )
            lat -= 2 * M_PI;
        while ( lat < -M_PI )
            lat += 2 * M_PI;
    }

    while ( lon > M_PI )
        lon -= 2 * M_PI;
    while ( lon < -M_PI )
        lon += 2 * M_PI;

    d->m_centerLongitude = lon;
    d->m_centerLatitude = lat;

    Quaternion axis = Quaternion::fromEuler( -lat, lon, 0.0 );
    axis.normalize();

    d->setPlanetAxis( axis );
}

Quaternion ViewportParams::planetAxis() const
{
    return d->m_planetAxis;
}

void ViewportParamsPrivate::setPlanetAxis(const Quaternion &newAxis)
{
    m_dirtyBox = true;
    m_planetAxis = newAxis;
    m_planetAxis.inverse().toMatrix( m_planetAxisMatrix );
}

const matrix * ViewportParams::planetAxisMatrix() const
{
    return &d->m_planetAxisMatrix;
}

int ViewportParams::width()  const
{
    return d->m_size.width();
}

int ViewportParams::height() const
{
    return d->m_size.height();
}

QSize ViewportParams::size() const
{
    return d->m_size;
}


void ViewportParams::setWidth(int newWidth)
{
    d->m_dirtyBox = true;

    d->m_size.setWidth( newWidth );
}

void ViewportParams::setHeight(int newHeight)
{
    d->m_dirtyBox = true;

    d->m_size.setHeight( newHeight );
}

void ViewportParams::setSize(QSize newSize)
{
    if ( newSize == d->m_size )
        return;

    d->m_dirtyBox = true;

    d->m_size = newSize;
}

// ================================================================
//                        Other functions

qreal ViewportParams::centerLongitude() const
{
    return d->m_centerLongitude;
}

qreal ViewportParams::centerLatitude() const
{
    return d->m_centerLatitude;
}

void ViewportParams::centerCoordinates( qreal &centerLon, qreal &centerLat ) const
{
    centerLon = d->m_centerLongitude;
    centerLat = d->m_centerLatitude;
}

GeoDataLatLonAltBox ViewportParams::viewLatLonAltBox() const
{
    if (d->m_dirtyBox) {
        d->m_viewLatLonAltBox = d->m_currentProjection->latLonAltBox( QRect( QPoint( 0, 0 ), 
                        d->m_size ),
                        this );
        d->m_dirtyBox = false;
    }

    return d->m_viewLatLonAltBox;
}

GeoDataLatLonAltBox ViewportParams::latLonAltBox( const QRect &screenRect ) const
{
    return d->m_currentProjection->latLonAltBox( screenRect, this );
}

qreal ViewportParams::angularResolution() const
{
    // We essentially divide the diameter by 180 deg and
    // take half of the result as a guess for the angle per pixel resolution. 
    // d->m_angularResolution = 0.25 * M_PI / fabs( (qreal)(d->m_radius);
    return d->m_angularResolution;
}

int ViewportParams::detailLevel() const
{
    if ( d->m_dirtyDetailLevel ) {
        d->m_dirtyDetailLevel = false;
        qreal firstOp, detailLevelF;
        int detailLevel;


        firstOp = d->m_constantDetailFactor * angularResolution();

        detailLevelF = qLn( firstOp ) / d->m_constantLnOneOverTwo;
        detailLevel = (int) ( detailLevelF ) + 1;
        d->m_detailLevel = detailLevel;
    }

    return d->m_detailLevel;

}

bool ViewportParams::resolves ( const GeoDataLatLonBox &latLonBox ) const
{
    return latLonBox.width() + latLonBox.height() > 2.0 * angularResolution();
}


bool ViewportParams::resolves ( const GeoDataLatLonAltBox &latLonAltBox ) const
{
    return    latLonAltBox.width() + latLonAltBox.height() > 2.0 * angularResolution()
           || latLonAltBox.maxAltitude() - latLonAltBox.minAltitude() > 10000;
           
}

bool ViewportParams::resolves ( const GeoDataCoordinates &coord1, 
                                const GeoDataCoordinates &coord2 ) const
{
    qreal lon1, lat1;
    coord1.geoCoordinates( lon1, lat1 );

    qreal lon2, lat2;
    coord2.geoCoordinates( lon2, lat2 );

    // We take the manhattan length as an approximation for the distance
    return ( fabs( lon2 - lon1 ) + fabs( lat2 - lat1 ) < angularResolution() );
}


bool ViewportParams::screenCoordinates( const qreal lon, const qreal lat,
                        qreal &x, qreal &y ) const
{
    return d->m_currentProjection->screenCoordinates( lon, lat, this, x, y );
}

bool ViewportParams::screenCoordinates( const GeoDataCoordinates &geopoint,
                        qreal &x, qreal &y,
                        bool &globeHidesPoint ) const
{
    return d->m_currentProjection->screenCoordinates( geopoint, this, x, y, globeHidesPoint );
}

bool ViewportParams::screenCoordinates( const GeoDataCoordinates &geopoint,
                        qreal &x, qreal &y ) const
{
    return d->m_currentProjection->screenCoordinates( geopoint, this, x, y );
}

bool ViewportParams::screenCoordinates( const GeoDataCoordinates &geopoint,
                        QPointF &screenpoint ) const
{
    return d->m_currentProjection->screenCoordinates( geopoint, this, screenpoint );
}

bool ViewportParams::screenCoordinates( const GeoDataCoordinates &coordinates,
                        qreal *x, qreal &y, int &pointRepeatNum,
                        bool &globeHidesPoint ) const
{
    return d->m_currentProjection->screenCoordinates( coordinates, this, x, y, pointRepeatNum, globeHidesPoint );
}

bool ViewportParams::screenCoordinates( const GeoDataCoordinates &coordinates,
                        qreal *x, qreal &y, int &pointRepeatNum,
                        const QSizeF& size,
                        bool &globeHidesPoint ) const
{
    return d->m_currentProjection->screenCoordinates( coordinates, this, x, y, pointRepeatNum, size, globeHidesPoint );
}


bool ViewportParams::screenCoordinates( const GeoDataLineString &lineString,
                        QVector<QPolygonF*> &polygons ) const
{
    return d->m_currentProjection->screenCoordinates( lineString, this, polygons );
}

bool ViewportParams::geoCoordinates( const int x, const int y,
                     qreal &lon, qreal &lat,
                     GeoDataCoordinates::Unit unit ) const
{
    return d->m_currentProjection->geoCoordinates( x, y, this, lon, lat, unit );
}

bool  ViewportParams::mapCoversViewport() const
{
    return d->m_currentProjection->mapCoversViewport( this );
}

QPainterPath ViewportParams::mapShape() const
{
    return d->m_currentProjection->mapShape( this );
}

QRegion ViewportParams::mapRegion() const
{
    return d->m_currentProjection->mapRegion( this );
}

GeoDataCoordinates ViewportParams::focusPoint() const
{
    if (d->m_hasFocusPoint) {
        return d->m_focusPoint;
    }
    else {
       const qreal lon = d->m_centerLongitude;
       const qreal lat = d->m_centerLatitude;

       return GeoDataCoordinates(lon, lat, 0.0, GeoDataCoordinates::Radian);
    }

}

void ViewportParams::setFocusPoint(const GeoDataCoordinates &focusPoint)
{
    d->m_focusPoint = focusPoint;
    d->m_hasFocusPoint = true;
}

void ViewportParams::resetFocusPoint()
{
    d->m_hasFocusPoint = false;
}

}
