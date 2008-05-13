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
#include "MercatorProjectionHelper.h"

// Marble
#include "GeoPainter.h"
#include "ViewportParams.h"


MercatorProjectionHelper::MercatorProjectionHelper()
    : AbstractProjectionHelper()
{
}

MercatorProjectionHelper::~MercatorProjectionHelper()
{
}


void MercatorProjectionHelper::paintBase( GeoPainter     *painter, 
					   ViewportParams *viewport,
					   QPen           &pen,
					   QBrush         &brush,
					   bool            antialiasing )
{
    int  radius = viewport->radius();

    painter->setRenderHint( QPainter::Antialiasing, antialiasing );

    painter->setPen( pen );
    painter->setBrush( brush );

    int yTop;
    //int yBottom;
    int dummy;
    AbstractProjection *proj = viewport->currentProjection();

    // Get the top pixel of the projected map.
    proj->screenCoordinates( 0.0, proj->maxLat(), viewport, 
			     dummy, yTop );
    if ( yTop < 0 )
      yTop = 0;
#if 0
    proj->screenCoordinates( 0.0, -proj->maxLat(), viewport, 
			     dummy, yBottom );
    if ( yBottom > viewport->height() )
      yBottom = viewport->height();
#endif

    painter->drawRect( 0, yTop, viewport->width(), 2 * radius);
}
