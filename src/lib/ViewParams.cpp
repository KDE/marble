//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//


#include "ViewParams.h"

#include <QImage>


ViewParams::ViewParams( )
{
    // Default projection
#if 0
    m_projection    = Spherical;
#endif
    m_oldProjection = Spherical;

    // Default view
#if 0
    m_planetAxis = Quaternion( 1.0, 0.0, 0.0, 0.0 );
    m_radius     = 2000;
#endif    // FIXME: planetAxisUpdated, radiusUpdated
    // FIXME: boundingBox

    // Show / don't show parameters
    m_showGrid           = true;
    m_showPlaceMarks     = true;
    m_showElevationModel = false;

    m_showRelief         = true;

    // Terrain features
    m_showIceLayer       = true;
    m_showBorders        = true;
    m_showRivers         = true;
    m_showLakes          = true;

    // Placemarks
    m_showCities         = true;
    m_showTerrain        = true;
    m_showOtherPlaces    = true;

    // Other layers
    m_showGps            = false;

    // Just to have something.  These will be resized anyway.
    m_canvasImage = new QImage( 10, 10, QImage::Format_ARGB32_Premultiplied );
    m_coastImage  = new QImage( 10, 10, QImage::Format_ARGB32_Premultiplied );
}

ViewParams::~ViewParams()
{
    delete m_canvasImage;
    m_canvasImage = 0;

    delete m_coastImage;
    m_coastImage  = 0;
}


const ViewportParams *ViewParams::viewport() const
{
    return &m_viewport;
}

Projection ViewParams::projection() const
{
    return m_viewport.projection();
}

void ViewParams::setProjection(Projection newProjection)
{
    m_viewport.setProjection( newProjection );
}


int ViewParams::radius() const
{
    return m_viewport.radius();
}

void ViewParams::setRadius(int newRadius)
{
    m_viewport.setRadius( newRadius );
}


Quaternion ViewParams::planetAxis() const
{
    return m_viewport.planetAxis();
}

void ViewParams::setPlanetAxis(const Quaternion &newAxis)
{
    m_viewport.setPlanetAxis( newAxis );
}


void ViewParams::centerCoordinates( double &centerLon, double &centerLat )
{
#if 1
    m_viewport.centerCoordinates( centerLon, centerLat );
#else

    // Calculate translation of center point
    centerLat = - m_planetAxis.pitch();
    if ( centerLat > M_PI )
        centerLat -= 2 * M_PI;

    centerLon = + m_planetAxis.yaw();
    if ( centerLon > M_PI )
        centerLon -= 2 * M_PI;

    // qDebug() << "centerLon" << centerLon * RAD2DEG;
    // qDebug() << "centerLat" << centerLat * RAD2DEG;
#endif
}
