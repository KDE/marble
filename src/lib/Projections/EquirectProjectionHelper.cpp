//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Inge Wallin  <ingwa@kde.org>
//


// Local
#include "EquirectProjectionHelper.h"
#include "AbstractProjectionHelper_p.h"

// Qt
#include "QtCore/QDebug"

// Marble
#include "GeoPainter.h"
#include "ViewportParams.h"

using namespace Marble;

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
    // Convenience variables
    int  radius = viewport->radius();
    int  width  = viewport->width();
    int  height = viewport->height();

    // Igor, prepare the painter!
    painter->setRenderHint( QPainter::Antialiasing, antialiasing );
    painter->setPen( pen );
    painter->setBrush( brush );

    // Calculate translation of center point
    qreal  centerLon;
    qreal  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    int yCenterOffset = (int)(centerLat * (qreal)( 2 * radius ) / M_PI );
    int yTop          = height / 2 - radius + yCenterOffset;
    int yBottom       = yTop + 2 * radius;

    // Bound the values to the viewport.
    if ( yTop < 0 )
	yTop = 0;
    if ( yBottom > height )
	yBottom =  height;

    painter->drawRect( 0, yTop, width - 1, yBottom - yTop );
}


void EquirectProjectionHelper::createActiveRegion( ViewportParams *viewport )
{
    // Convenience variables
    int  radius = viewport->radius();
    int  width  = viewport->width();
    int  height = viewport->height();

    // Calculate translation of center point
    qreal  centerLon;
    qreal  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    int yCenterOffset = (int)( centerLat * (qreal)( 2 * radius ) / M_PI );
    int yTop          = height / 2 - radius + yCenterOffset;
    int yBottom       = yTop + 2 * radius;

    // Don't let the active area be outside the image
    if ( yTop < 0 )
        yTop = 0;
    if ( yBottom > height )
        yBottom =  height;

    setActiveRegion( QRegion(
                    navigationStripe(), 
                    navigationStripe() + yTop, 
                    width - 2 * navigationStripe(), 
                    yBottom - yTop - 2 * navigationStripe(),
			        QRegion::Rectangle ) );
}

void EquirectProjectionHelper::createProjectedRegion( ViewportParams *viewport )
{
    // Convenience variables
    int  radius = viewport->radius();
    int  width  = viewport->width();
    int  height = viewport->height();

    // Calculate translation of center point
    qreal  centerLon;
    qreal  centerLat;
    viewport->centerCoordinates( centerLon, centerLat );

    int yCenterOffset = (int)( centerLat * (qreal)( 2 * radius ) / M_PI );
    int yTop          = height / 2 - radius + yCenterOffset;
    int yBottom       = yTop + 2 * radius;

    // Don't let the active area be outside the image
    if ( yTop < 0 )
        yTop = 0;
    if ( yBottom > height )
        yBottom =  height;

    setProjectedRegion( QRegion(
                    0, 
                    yTop, 
                    width, 
                    yBottom - yTop,
                    QRegion::Rectangle ) );
}
