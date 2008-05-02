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

#include "MapThemeManager.h"

#include <QtGui/QImage>


ViewParams::ViewParams( )
{
    m_mapTheme = 0;
    m_mapQuality = Normal;

    // Show / don't show parameters
    m_showAtmosphere     = true;

    m_showElevationModel = false;

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


ViewportParams *ViewParams::viewport()
{
    return &m_viewport;
}

Projection ViewParams::projection() const
{
    return m_viewport.projection();
}

AbstractProjection *ViewParams::currentProjection() const
{
    return m_viewport.currentProjection();
}

void ViewParams::setProjection(Projection newProjection)
{
    m_viewport.setProjection( newProjection );
}

void ViewParams::setMapThemeId( const QString& mapThemeId )
{
    GeoSceneDocument* mapTheme = MapThemeManager::loadMapTheme( mapThemeId );

    // Check whether the selected theme got parsed well
    if ( !mapTheme ) {

        // Check whether the previous theme works 
        if ( !m_mapTheme ){ 
            // Fall back to default theme
            qDebug() << "Falling back to default theme";
            mapTheme = MapThemeManager::loadMapTheme("earth/srtm/srtm.dgml");

            // If this last resort doesn't work either shed a tear and exit
            if ( !mapTheme ) {
                qDebug() << "Couldn't find a valid DGML map.";
                exit(-1);
            }
        }
        else {
            qDebug() << "Selected theme doesn't work, so we stick to the previous one";
            return;
        }
    }

    m_mapTheme = mapTheme;
}

GeoSceneDocument *ViewParams::mapTheme()
{
    return m_mapTheme; 
}

MapQuality ViewParams::mapQuality()
{
    return m_mapQuality; 
}

void ViewParams::setMapQuality( MapQuality mapQuality )
{
    m_mapQuality = mapQuality; 
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
    m_viewport.centerCoordinates( centerLon, centerLat );
}
