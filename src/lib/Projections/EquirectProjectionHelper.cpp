//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Inge Wallin  <ingwa@kde.org>"
//


// Local
#include "EquirectProjectionHelper.h"
#include "AbstractProjectionHelper_p.h"

// Marble
#include "GeoPainter.h"
#include "ViewportParams.h"


EquirectProjectionHelper::EquirectProjectionHelper()
    : AbstractProjectionHelper()
{
}

EquirectProjectionHelper::~EquirectProjectionHelper()
{
}


void EquirectProjectionHelper::paintBase( GeoPainter     *painter, 
					  ViewportParams *viewport,
					  QPen           &pen,
					  QBrush         &brush,
					  bool            antialiasing )
{
    int  radius = viewport->radius();

    painter->setRenderHint( QPainter::Antialiasing, antialiasing );

    painter->setPen( pen );
    painter->setBrush( brush );

    // Calculate translation of center point
    double  centerLon;
    double  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    // yCenterOffset is the number of pixels that the 
    int yCenterOffset = (int)( centerLat * (double)( 2 * radius ) / M_PI );
    int yTop          = viewport->height() / 2 - radius + yCenterOffset;

    painter->drawRect( 0, yTop, viewport->width(), 2 * radius);
}


void EquirectProjectionHelper::setActiveRegion( ViewportParams *viewport )
{
    // Convenience variables
    int  radius = viewport->radius();
    int  width  = viewport->width();
    int  height = viewport->height();

    // Calculate translation of center point
    double  centerLon;
    double  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    int yCenterOffset = (int)((double)( 2 * radius ) / M_PI * centerLat);
    int yTop          = height / 2 - radius + yCenterOffset;
    int yBottom       = yTop + 2 * radius;

    // Don't let the active area be outside the image, and also let a
    // thin strip 25 pixels wide be outside it.
    if ( yTop < 25 )
	yTop = 25;
    if ( yBottom > height - 25 )
	yBottom =  height - 25;
    d->activeRegion = QRegion( 25, yTop, width - 50, yBottom - yTop,
			       QRegion::Rectangle );
}
