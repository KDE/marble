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

#include "GeoSceneSettings.h"
#include "MapThemeManager.h"

#include <QtGui/QImage>


ViewParams::ViewParams( )
{
    m_mapTheme = 0;

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

void ViewParams::setPropertyValue( const QString &name, bool value )
{
    if ( m_mapTheme ) {
        m_mapTheme->settings()->setPropertyValue( name, value );
    }
    else {
        qDebug() << "WARNING: Failed to access a map theme! Property: " << name;
    }
}

void ViewParams::propertyValue( const QString &name, bool &value )
{
    if ( m_mapTheme ) {
        m_mapTheme->settings()->propertyValue( name, value );
    }
    else {
        value = false;
        qDebug() << "WARNING: Failed to access a map theme! Property: " << name;
    }
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

QImage * ViewParams::canvasImage() const
{
    return m_canvasImage;
}

void ViewParams::setCanvasImage( QImage * const image )
{
    delete m_canvasImage;
    m_canvasImage = image;
}

QImage * ViewParams::coastImage() const
{
    return m_coastImage;
}

void ViewParams::setCoastImage( QImage * const image )
{
    delete m_coastImage;
    m_coastImage = image;
}

int ViewParams::radiusUpdated() const
{
    return m_radiusUpdated;
}

void ViewParams::setRadiusUpdated( const int radiusUpdated )
{
    m_radiusUpdated = radiusUpdated;
}

bool ViewParams::showGps() const
{
    return m_showGps;
}

void ViewParams::setShowGps( bool const showGps )
{
    m_showGps = showGps;
}

bool ViewParams::showElevationModel() const
{
    return m_showElevationModel;
}

void ViewParams::setShowElevationModel( const bool showElevationModel )
{
    m_showElevationModel = showElevationModel;
}

bool ViewParams::showAtmosphere() const
{
    return m_showAtmosphere;
}

void ViewParams::setShowAtmosphere( const bool showAtmosphere )
{
    m_showAtmosphere = showAtmosphere;
}

Quaternion ViewParams::planetAxisUpdated() const
{
    return m_planetAxisUpdated;
}

void ViewParams::setPlanetAxisUpdated( const Quaternion & planetAxisUpdated )
{
    m_planetAxisUpdated = planetAxisUpdated;
}
