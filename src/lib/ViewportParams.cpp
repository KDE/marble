//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#include "ViewportParams.h"

#include <QtCore/QRect>

ViewportParams::ViewportParams( )
{
    // Default projection
    setProjection( Spherical );

    // Default view
    setPlanetAxis( Quaternion( 1.0, 0.0, 0.0, 0.0 ) );

    m_radius     = 2000;
    m_size       = QSize( 100, 100 );
    m_mapQuality = Normal;
}

ViewportParams::~ViewportParams()
{
}


// ================================================================
//                    Getters and setters


Projection ViewportParams::projection() const
{
    return m_projection;
}

AbstractProjection *ViewportParams::currentProjection() const
{
    return m_currentProjection;
}

void ViewportParams::setProjection(Projection newProjection)
{
    m_projection = newProjection;

    // Set the pointer to the current projection class.
    switch ( newProjection ) {
    case Spherical:
        m_currentProjection = &s_sphericalProjection;
        break;
    case Equirectangular:
        m_currentProjection = &s_equirectProjection;
        break;
    case Mercator:
        m_currentProjection = &s_mercatorProjection;
        break;
    }
}

int ViewportParams::polarity() const
{
    GeoDataPoint northPole( 0.0, +M_PI * 0.5 );
    GeoDataPoint southPole( 0.0, -M_PI * 0.5 );

    bool globeHidesN, globeHidesS;
    int x;
    int yN, yS;

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
    return m_radius;
}

void ViewportParams::setRadius(int newRadius)
{
    m_radius = newRadius;
}

bool ViewportParams::globeCoversViewport() const
{
    // This first test is a quick one that will catch all really big
    // radii and prevent overflow in the real test.
    if ( m_radius > m_size.width() + m_size.height() )
        return true;

    // This is the real test.  The 4 is because we are really
    // comparing to width/2 and height/2.
    if ( 4 * m_radius * m_radius >= m_size.width() * m_size.width() + m_size.height() * m_size.height() )
        return true;

    return false;
}

Quaternion ViewportParams::planetAxis() const
{
    return m_planetAxis;
}

void ViewportParams::setPlanetAxis(const Quaternion &newAxis)
{
    m_planetAxis = newAxis;
    planetAxis().inverse().toMatrix( m_planetAxisMatrix );
}

matrix * ViewportParams::planetAxisMatrix() const
{
    return &m_planetAxisMatrix;
}

int ViewportParams::width()  const
{
    return m_size.width();
}

int ViewportParams::height() const
{
    return m_size.height();
}

QSize ViewportParams::size() const
{
    return m_size;
}


void ViewportParams::setWidth(int newWidth)
{
    m_size.setWidth( newWidth );
}

void ViewportParams::setHeight(int newHeight)
{
    m_size.setHeight( newHeight );
}

void ViewportParams::setSize(QSize newSize)
{
    m_size = newSize;
}

MapQuality ViewportParams::mapQuality()
{
    return m_mapQuality; 
}

void ViewportParams::setMapQuality( MapQuality mapQuality )
{
    m_mapQuality = mapQuality; 
}

BoundingBox ViewportParams::boundingBox() const
{
    return m_boundingBox;
}

void ViewportParams::setBoundingBox( const BoundingBox & boundingBox )
{
    m_boundingBox = boundingBox;
}

// ================================================================
//                        Other functions


void ViewportParams::centerCoordinates( double &centerLon, double &centerLat ) const
{
    // Calculate translation of center point
    centerLat = - m_planetAxis.pitch();
    if ( centerLat > M_PI )
        centerLat -= 2 * M_PI;

    centerLon = + m_planetAxis.yaw();
    if ( centerLon > M_PI )
        centerLon -= 2 * M_PI;

//     qDebug() << "centerLon" << centerLon * RAD2DEG;
//     qDebug() << "centerLat" << centerLat * RAD2DEG;
}

GeoDataLatLonAltBox ViewportParams::viewLatLonAltBox() const
{
    return m_currentProjection->latLonAltBox( QRect( QPoint( 0, 0 ), m_size ), this );
}


SphericalProjection  ViewportParams::s_sphericalProjection;
EquirectProjection   ViewportParams::s_equirectProjection;
MercatorProjection   ViewportParams::s_mercatorProjection;
