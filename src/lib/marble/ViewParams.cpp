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

namespace Marble
{

class ViewParamsPrivate
{
public:
    ViewParamsPrivate();
    ~ViewParamsPrivate();

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
    : m_stillQuality( HighQuality ),
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
