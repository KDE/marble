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
#include "ViewportParams.h"

namespace Marble
{

class ViewParamsPrivate
{
public:
    ViewParamsPrivate();
    ~ViewParamsPrivate();

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
};

ViewParamsPrivate::ViewParamsPrivate()
    : m_viewport(),
      m_stillQuality( HighQuality ),
      m_animationQuality( LowQuality ),
      m_viewContext( Still ),
      m_mapQuality( m_stillQuality ),
      // Show / don't show parameters
      m_showAtmosphere( true ),
      m_showClouds( false )
{
}

ViewParamsPrivate::~ViewParamsPrivate()
{
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
}

int ViewParams::radius() const
{
    return d->m_viewport.radius();
}

void ViewParams::setRadius(int newRadius)
{
    // Avoid expensive clearing of the canvas image if there is no change:
    if ( d->m_viewport.radius() == newRadius ) return;

    d->m_viewport.setRadius( newRadius );
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
    return d->m_viewport.width();
}

int ViewParams::height() const
{
    return d->m_viewport.height();
}

void ViewParams::setSize( int width, int height )
{    
    d->m_viewport.setSize( QSize( width, height ) );
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
