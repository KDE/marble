//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
// Copyright 2010-2013 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//


#include "ViewportParams.h"

#include <QRect>

#include <QPainterPath>
#include <QRegion>

#include "MarbleDebug.h"
#include "GeoDataLatLonAltBox.h"
#include "SphericalProjection.h"
#include "EquirectProjection.h"
#include "MercatorProjection.h"
#include "GnomonicProjection.h"
#include "LambertAzimuthalProjection.h"
#include "AzimuthalEquidistantProjection.h"
#include "StereographicProjection.h"
#include "VerticalPerspectiveProjection.h"


namespace Marble
{

class ViewportParamsPrivate
{
public:
    ViewportParamsPrivate( Projection projection,
                           qreal centerLongitude, qreal centerLatitude,
                           int radius,
                           const QSize &size );

    static const AbstractProjection *abstractProjection( Projection projection );

    // These two go together.  m_currentProjection points to one of
    // the static Projection classes at the bottom.
    Projection           m_projection;
    const AbstractProjection *m_currentProjection;

    // Parameters that determine the painting
    qreal                m_centerLongitude;
    qreal                m_centerLatitude;
    Quaternion           m_planetAxis;   // Position, coded in a quaternion
    matrix               m_planetAxisMatrix;
    int                  m_radius;       // Zoom level (pixels / globe radius)
    qreal                m_angularResolution;

    QSize                m_size;         // width, height


    bool                 m_dirtyBox;
    GeoDataLatLonAltBox  m_viewLatLonAltBox;

    static const SphericalProjection  s_sphericalProjection;
    static const EquirectProjection   s_equirectProjection;
    static const MercatorProjection   s_mercatorProjection;
    static const GnomonicProjection   s_gnomonicProjection;
    static const StereographicProjection   s_stereographicProjection;
    static const LambertAzimuthalProjection   s_lambertAzimuthalProjection;
    static const AzimuthalEquidistantProjection   s_azimuthalEquidistantProjection;
    static const VerticalPerspectiveProjection   s_verticalPerspectiveProjection;

    GeoDataCoordinates   m_focusPoint;
};

const SphericalProjection  ViewportParamsPrivate::s_sphericalProjection;
const EquirectProjection   ViewportParamsPrivate::s_equirectProjection;
const MercatorProjection   ViewportParamsPrivate::s_mercatorProjection;
const GnomonicProjection   ViewportParamsPrivate::s_gnomonicProjection;
const StereographicProjection   ViewportParamsPrivate::s_stereographicProjection;
const LambertAzimuthalProjection   ViewportParamsPrivate::s_lambertAzimuthalProjection;
const AzimuthalEquidistantProjection   ViewportParamsPrivate::s_azimuthalEquidistantProjection;
const VerticalPerspectiveProjection   ViewportParamsPrivate::s_verticalPerspectiveProjection;

ViewportParamsPrivate::ViewportParamsPrivate( Projection projection,
                                              qreal centerLongitude, qreal centerLatitude,
                                              int radius,
                                              const QSize &size )
    : m_projection( projection ),
      m_currentProjection( abstractProjection( projection ) ),
      m_centerLongitude( centerLongitude ),
      m_centerLatitude( centerLatitude ),
      m_planetAxis(),
      m_planetAxisMatrix(),
      m_radius( radius ),
      m_angularResolution(4.0 / fabs(m_radius)),
      m_size( size ),
      m_dirtyBox( true ),
      m_viewLatLonAltBox()
{
}

const AbstractProjection *ViewportParamsPrivate::abstractProjection(Projection projection)
{
    switch ( projection ) {
    case Spherical:
        return &s_sphericalProjection;
    case Equirectangular:
        return &s_equirectProjection;
    case Mercator:
        return &s_mercatorProjection;
    case Gnomonic:
        return &s_gnomonicProjection;
    case Stereographic:
        return &s_stereographicProjection;
    case LambertAzimuthal:
        return &s_lambertAzimuthalProjection;
    case AzimuthalEquidistant:
        return &s_azimuthalEquidistantProjection;
    case VerticalPerspective:
        return &s_verticalPerspectiveProjection;
    }

    return 0;
}


ViewportParams::ViewportParams()
    : d( new ViewportParamsPrivate( Spherical, 0, 0, 2000, QSize( 100, 100 ) ) )
{
    centerOn( d->m_centerLongitude, d->m_centerLatitude );
}

ViewportParams::ViewportParams( Projection projection,
                                qreal centerLongitude, qreal centerLatitude,
                                int radius,
                                const QSize &size )
    : d( new ViewportParamsPrivate( projection, centerLongitude, centerLatitude, radius, size ) )
{
    centerOn( d->m_centerLongitude, d->m_centerLatitude );
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
    d->m_projection = newProjection;
    d->m_currentProjection = ViewportParamsPrivate::abstractProjection( newProjection );

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

        d->m_radius = newRadius;
        d->m_angularResolution = 4.0 / d->m_radius;
    }
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

    d->m_planetAxis = Quaternion::fromEuler( -lat, lon, 0.0 );
    d->m_planetAxis.normalize();

    d->m_dirtyBox = true;
    d->m_planetAxis.inverse().toMatrix( d->m_planetAxisMatrix );
}

Quaternion ViewportParams::planetAxis() const
{
    return d->m_planetAxis;
}

const matrix &ViewportParams::planetAxisMatrix() const
{
    return d->m_planetAxisMatrix;
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
    setSize( QSize( newWidth, height() ) );
}

void ViewportParams::setHeight(int newHeight)
{
    setSize( QSize( width(), newHeight ) );
}

void ViewportParams::setSize(const QSize& newSize)
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

const GeoDataLatLonAltBox& ViewportParams::viewLatLonAltBox() const
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

bool ViewportParams::resolves ( const GeoDataLatLonBox &latLonBox, qreal pixel ) const
{
    return latLonBox.width() + latLonBox.height() > pixel * d->m_angularResolution;
}


bool ViewportParams::resolves ( const GeoDataLatLonAltBox &latLonAltBox, qreal pixel, qreal altitude ) const
{
    return    latLonAltBox.width() + latLonAltBox.height() > pixel * d->m_angularResolution
           || latLonAltBox.maxAltitude() - latLonAltBox.minAltitude() > altitude;
}

bool ViewportParams::resolves ( const GeoDataCoordinates &coord1, 
                                const GeoDataCoordinates &coord2 ) const
{
    qreal lon1, lat1;
    coord1.geoCoordinates( lon1, lat1 );

    qreal lon2, lat2;
    coord2.geoCoordinates( lon2, lat2 );

    // We take the manhattan length as an approximation for the distance
    return ( fabs( lon2 - lon1 ) + fabs( lat2 - lat1 ) > d->m_angularResolution );
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
    if (d->m_focusPoint.isValid()) {
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
}

void ViewportParams::resetFocusPoint()
{
    d->m_focusPoint = GeoDataCoordinates();
}

}
