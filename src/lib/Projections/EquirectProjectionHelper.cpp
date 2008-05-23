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

    int yCenterOffset = (int)( centerLat * (double)( 2 * radius ) / M_PI );
    int yTop          = viewport->height() / 2 - radius + yCenterOffset;

    painter->drawRect( 0, yTop, viewport->width(), 2 * radius);
}


void EquirectProjectionHelper::setActiveRegion( ViewportParams *viewport )
{
    int  radius = viewport->radius();

    // Calculate translation of center point
    double  centerLon;
    double  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    int yCenterOffset = (int)((double)( 2 * radius ) / M_PI * centerLat);
    int yTop          = viewport->height() / 2 - radius + yCenterOffset;
    d->activeRegion = QRegion( 0, yTop, viewport->width(), 2 * radius,
			       QRegion::Rectangle );
}
