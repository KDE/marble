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


#include "ViewParams.h"

#include <QtGui/QImage>

#include "MarbleDebug.h"
#include "AbstractProjection.h"
#include "GeoSceneDocument.h"
#include "MapThemeManager.h"
#include "ViewportParams.h"

namespace Marble
{

class ViewParamsPrivate
{
public:
    ViewParamsPrivate();
    ~ViewParamsPrivate();

    GeoSceneDocument *m_mapTheme;

    ViewportParams  m_viewport;

    MapQuality      m_stillQuality;
    MapQuality      m_animationQuality;

    // The context that is active now.
    ViewContext     m_viewContext;

    // The quality that we are painting right now.
    MapQuality           m_mapQuality;


    // Parameters that determine the painting
    // Show/don't show options

    bool        m_showAtmosphere;

    bool        m_showClouds;

    // Cached data that will make painting faster.
    QSharedPointer<QImage>  m_canvasImage;     // Base image with space and atmosphere
    QSharedPointer<QImage>  m_coastImage;      // A slightly higher level image.

    void optimizeCanvasImageFormat();
};

ViewParamsPrivate::ViewParamsPrivate()
    : m_mapTheme( 0 ),
      m_viewport(),
      m_stillQuality( HighQuality ),
      m_animationQuality( LowQuality ),
      m_viewContext( Still ),
      m_mapQuality( m_stillQuality ),
      // Show / don't show parameters
      m_showAtmosphere( true ),
      m_showClouds( false ),
      // Just to have something.  These will be resized anyway.
      m_canvasImage( new QImage( 10, 10, QImage::Format_RGB32 )),
      m_coastImage( new QImage( 10, 10, QImage::Format_RGB32 ))
{
}

ViewParamsPrivate::~ViewParamsPrivate()
{
}

void ViewParamsPrivate::optimizeCanvasImageFormat()
{
    // If the globe covers fully the screen then we can use the faster
    // RGB32 as there are no translucent areas involved.
    QImage::Format imageFormat = ( m_viewport.mapCoversViewport() )
                                 ? QImage::Format_RGB32
                                 : QImage::Format_ARGB32_Premultiplied;

    // Recreate the canvas image with the new size.
    m_canvasImage = QSharedPointer<QImage>( new QImage( m_viewport.size(), imageFormat ) );

    // Repaint the background if necessary
    if ( !m_viewport.currentProjection()->mapCoversViewport( &m_viewport ) ) {
        m_canvasImage->fill(0); // Using Qt::transparent is wrong here (equals "18")!
    }

    // Recreate the coastline detection offscreen image
    m_coastImage = QSharedPointer<QImage>( new QImage( m_viewport.size(), QImage::Format_RGB32 ) );
}


ViewParams::ViewParams()
    : d( new ViewParamsPrivate )
{
}

ViewParams::~ViewParams()
{
    delete d;
}

ViewportParams *ViewParams::viewport()
{
    return &d->m_viewport;
}

Projection ViewParams::projection() const
{
    return d->m_viewport.projection();
}

MapQuality ViewParams::mapQuality( ViewContext viewContext ) const
{
    if ( viewContext == Still )
        return d->m_stillQuality;

    Q_ASSERT( viewContext == Animation );
    return d->m_animationQuality; 
}

MapQuality ViewParams::mapQuality() const
{
    return d->m_mapQuality;
}

void ViewParams::setMapQualityForViewContext( MapQuality quality, ViewContext viewContext )
{
    if ( viewContext == Still ) {
        d->m_stillQuality = quality;
    }
    else if ( viewContext == Animation ) {
        d->m_animationQuality = quality;
    }

    if ( d->m_viewContext == viewContext ) {
        d->m_mapQuality = quality;
    }
}

ViewContext ViewParams::viewContext() const
{
    return d->m_viewContext;
}

void ViewParams::setViewContext( ViewContext viewContext )
{
    d->m_viewContext = viewContext;

    if ( viewContext == Still )
        d->m_mapQuality = d->m_stillQuality;
    if ( viewContext == Animation )
        d->m_mapQuality = d->m_animationQuality;
}

const AbstractProjection *ViewParams::currentProjection() const
{
    return d->m_viewport.currentProjection();
}

void ViewParams::setProjection(Projection newProjection)
{
    d->m_viewport.setProjection( newProjection );

    // Repaint the background if necessary
    if ( !currentProjection()->mapCoversViewport( viewport() ) ) {
        d->m_canvasImage->fill(0); // Using Qt::transparent is wrong here (equals "18")!
    }
}

void ViewParams::setMapThemeId( const QString& mapThemeId )
{
    GeoSceneDocument* mapTheme = MapThemeManager::loadMapTheme( mapThemeId );

    // Check whether the selected theme got parsed well
    if ( !mapTheme ) {

        // Check whether the previous theme works 
        if ( !d->m_mapTheme ){ 
            // Fall back to default theme
            QString defaultTheme = "earth/srtm/srtm.dgml";
            qWarning() << "Falling back to default theme " << defaultTheme;
            mapTheme = MapThemeManager::loadMapTheme(defaultTheme);

            // If this last resort doesn't work either shed a tear and exit
            if ( !mapTheme ) {
                qWarning() << "Couldn't find a valid DGML map.";
                return;
            }
        }
        else {
            qWarning() << "Selected theme doesn't work, so we stick to the previous one";
            return;
        }
    }

    d->m_mapTheme = mapTheme;
}

GeoSceneDocument *ViewParams::mapTheme()
{
    return d->m_mapTheme; 
}

void ViewParams::setPropertyValue( const QString &name, bool value )
{
    if ( d->m_mapTheme ) {
        d->m_mapTheme->settings()->setPropertyValue( name, value );
    }
    else {
        mDebug() << "WARNING: Failed to access a map theme! Property: " << name;
    }
}

void ViewParams::propertyValue( const QString &name, bool &value )
{
    if ( d->m_mapTheme ) {
        d->m_mapTheme->settings()->propertyValue( name, value );
    }
    else {
        value = false;
        mDebug() << "WARNING: Failed to access a map theme! Property: " << name;
    }
}

void ViewParams::propertyAvailable( const QString &name, bool &value )
{
    if ( d->m_mapTheme ) {
        d->m_mapTheme->settings()->propertyAvailable( name, value );
    }
    else {
        value = false;
        mDebug() << "WARNING: Failed to access a map theme! Property: " << name;
    }
}

int ViewParams::radius() const
{
    return d->m_viewport.radius();
}

void ViewParams::setRadius(int newRadius)
{
    // Avoid expensive clearing of the canvas image if there is no change:
    if ( d->m_viewport.radius() == newRadius ) return;

    bool mapDidCoverViewport = d->m_viewport.mapCoversViewport();

    d->m_viewport.setRadius( newRadius );

    if ( mapDidCoverViewport != d->m_viewport.mapCoversViewport() ) {
        d->optimizeCanvasImageFormat();
        return;
    }

    if ( !currentProjection()->mapCoversViewport( viewport() ) ) {
        d->m_canvasImage->fill(0); // Using Qt::transparent is wrong here (equals "18")!
    }
}

Quaternion ViewParams::planetAxis() const
{
    return d->m_viewport.planetAxis();
}

void ViewParams::setPlanetAxis(const Quaternion &newAxis)
{
    d->m_viewport.setPlanetAxis( newAxis );
}

void ViewParams::centerCoordinates( qreal &centerLon, qreal &centerLat )
{
    d->m_viewport.centerCoordinates( centerLon, centerLat );
}

int ViewParams::width() const
{
    return d->m_canvasImage->width();
}

int ViewParams::height() const
{
    return d->m_canvasImage->height();
}

void ViewParams::setSize( int width, int height )
{    
    d->m_viewport.setSize( QSize( width, height ) );
    d->optimizeCanvasImageFormat();
}

QSharedPointer<QImage> ViewParams::canvasImagePtr() const
{
    return d->m_canvasImage;
}

QImage * ViewParams::canvasImage() const
{
    return d->m_canvasImage.data();
}

QSharedPointer<QImage> ViewParams::coastImagePtr() const
{
    return d->m_coastImage;
}

QImage * ViewParams::coastImage() const
{
    return d->m_coastImage.data();
}

bool ViewParams::showAtmosphere() const
{
    return d->m_showAtmosphere;
}

void ViewParams::setShowAtmosphere( bool showAtmosphere )
{
    d->m_showAtmosphere = showAtmosphere;
}

bool ViewParams::showClouds() const
{
    return d->m_showClouds;
}

void ViewParams::setShowClouds( bool const showClouds )
{
    d->m_showClouds = showClouds;
}

}
