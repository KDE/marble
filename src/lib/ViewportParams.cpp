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


int ViewportParams::radius() const
{
    return m_radius;
}

void ViewportParams::setRadius(int newRadius)
{
    m_radius = newRadius;
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
