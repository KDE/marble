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

    // These two go together.  m_currentProjection points to one of
    // the static Projection classes at the bottom.
    Projection           m_projection;
    const AbstractProjection *m_currentProjection;

    // Parameters that determine the painting
    Quaternion           m_planetAxis;   // Position, coded in a quaternion
    mutable matrix       m_planetAxisMatrix;
    int                  m_radius;       // Zoom level (pixels / globe radius)
    qreal                m_angularResolution;

    QSize                m_size;         // width, height


    bool                 m_dirtyBox;
    GeoDataLatLonAltBox  m_viewLatLonAltBox;
    bool                 m_dirtyRegion;
    QRegion              m_activeRegion;

    static const SphericalProjection  s_sphericalProjection;
    static const EquirectProjection   s_equirectProjection;
    static const MercatorProjection   s_mercatorProjection;

    GeoDataCoordinates   m_focusPoint;
    bool                 m_hasFocusPoint;

};

ViewportParamsPrivate::ViewportParamsPrivate()
    : m_projection( Spherical ),
      m_currentProjection( &s_sphericalProjection ),
      m_planetAxis(),
      m_planetAxisMatrix(),
      m_radius( 2000 ),
      m_angularResolution( 0.25 * M_PI / fabs( (qreal)( m_radius ) ) ),
      m_size( 100, 100 ),
      m_dirtyBox( true ),
      m_viewLatLonAltBox(),
      m_dirtyRegion( true ),
      m_activeRegion(),
      m_hasFocusPoint(false)
{
} 


const SphericalProjection  ViewportParamsPrivate::s_sphericalProjection;
const EquirectProjection   ViewportParamsPrivate::s_equirectProjection;
const MercatorProjection   ViewportParamsPrivate::s_mercatorProjection;


ViewportParams::ViewportParams()
    : d( new ViewportParamsPrivate )
{
    // Default view
    setPlanetAxis( Quaternion( 1.0, 0.0, 0.0, 0.0 ) );
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
    d->m_dirtyRegion = true;

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
    setPlanetAxis( planetAxis() );
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
    d->m_dirtyBox = true;
    d->m_dirtyRegion = true;

    d->m_radius = newRadius;
    d->m_angularResolution = 0.25 * M_PI / fabs( (qreal)(d->m_radius) );
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

Quaternion ViewportParams::planetAxis() const
{
    return d->m_planetAxis;
}

bool ViewportParams::setPlanetAxis(const Quaternion &newAxis)
{
    d->m_dirtyBox = true;
    d->m_dirtyRegion = true;

    bool valid = true;

    qreal maxLat = currentProjection()->maxLat();

    // Make sure that the planetAxis doesn't get invalid
    // The planetAxis is invalid if lat exceeds
    // the maximum latitude or longitude as specified by the 
    // projection
    // This is should not be done for projections where the
    // maximum latitude is traversable (e.g. for a sphere).

    if ( !currentProjection()->traversablePoles() && fabs( newAxis.pitch() ) > maxLat ) {

        qreal centerLon, centerLat;
        centerCoordinates( centerLon, centerLat );

        // Normalize latitude and longitude
        GeoDataPoint::normalizeLat( centerLat );

        // Checking whether the latitude is valid:
        if ( fabs( centerLat ) > maxLat )
        {
            valid = false;
            centerLat = maxLat * centerLat / fabs( centerLat );
        }
        
        d->m_planetAxis.createFromEuler( -centerLat, centerLon, newAxis.roll() );
    }
    else {
        d->m_planetAxis = newAxis;
    }

    // creating matching planetAxis matrix
    planetAxis().inverse().toMatrix( d->m_planetAxisMatrix );

    return valid;
}

matrix * ViewportParams::planetAxisMatrix() const
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
    d->m_dirtyRegion = true;

    d->m_size.setWidth( newWidth );
}

void ViewportParams::setHeight(int newHeight)
{
    d->m_dirtyBox = true;
    d->m_dirtyRegion = true;

    d->m_size.setHeight( newHeight );
}

void ViewportParams::setSize(QSize newSize)
{
    if ( newSize == d->m_size )
        return;

    d->m_dirtyBox = true;
    d->m_dirtyRegion = true;

    d->m_size = newSize;
}

// ================================================================
//                        Other functions


void ViewportParams::centerCoordinates( qreal &centerLon, qreal &centerLat ) const
{
    // Calculate translation of center point
    centerLat = - d->m_planetAxis.pitch();
    if ( centerLat > M_PI )
        centerLat -= 2 * M_PI;

    centerLon = + d->m_planetAxis.yaw();
    if ( centerLon > M_PI )
        centerLon -= 2 * M_PI;
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

qreal ViewportParams::angularResolution() const
{
    // We essentially divide the diameter by 180 deg and
    // take half of the result as a guess for the angle per pixel resolution. 
    // d->m_angularResolution = 0.25 * M_PI / fabs( (qreal)(d->m_radius);
    return d->m_angularResolution;
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

bool  ViewportParams::mapCoversViewport() const
{
    return d->m_currentProjection->mapCoversViewport( this );
}

QRegion ViewportParams::activeRegion() const
{
    if (d->m_dirtyRegion) {
        // Take the mapShape and subtract a stroke that is twice as wide as the
        // navigation strip:
        qreal navigationStrip = 25;

        QPainterPath mapShape = d->m_currentProjection->mapShape( this );

        QPainterPathStroker mapShapeStroke;
        mapShapeStroke.setWidth( 2.0 * navigationStrip );
        QPainterPath mapShapeStrokePath = mapShapeStroke.createStroke( mapShape );

        QPainterPath activeShape = mapShape.subtracted( mapShapeStrokePath );
        d->m_activeRegion = QRegion( activeShape.boundingRect().toRect() );
        d->m_dirtyRegion = false;
    }

    return d->m_activeRegion;
}

GeoDataCoordinates ViewportParams::focusPoint() const
{
    if (d->m_hasFocusPoint) {
        return d->m_focusPoint;
    }
    else {
       qreal lon, lat;
       centerCoordinates(lon, lat);
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
