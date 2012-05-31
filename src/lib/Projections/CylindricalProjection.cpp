//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007       Inge Wallin  <ingwa@kde.org>
// Copyright 2007-2009  Torsten Rahn  <rahn@kde.org>
// Copyright 2012		Cezar Mocan	<mocancezar@gmail.com>
//

// Local
#include "CylindricalProjection.h"

// Marble
#include "ViewportParams.h"

using namespace Marble;

CylindricalProjection::CylindricalProjection()
    : AbstractProjection(),
	d( 0 )
{
}


CylindricalProjection::~CylindricalProjection()
{
}


QPainterPath CylindricalProjection::mapShape( const ViewportParams *viewport ) const
{
    // Convenience variables
    int  width  = viewport->width();
    int  height = viewport->height();

    qreal  yTop;
    qreal  yBottom;
    qreal  xDummy;

    // Get the top and bottom coordinates of the projected map.
    screenCoordinates( 0.0, maxLat(), viewport, xDummy, yTop );
    screenCoordinates( 0.0, minLat(), viewport, xDummy, yBottom );

    // Don't let the map area be outside the image
	if ( yTop < 0 )
        yTop = 0;
    if ( yBottom > height )
        yBottom =  height;

    QPainterPath mapShape;
    mapShape.addRect(
                    0,
                    yTop,
                    width,
                    yBottom - yTop );

    return mapShape;
}
