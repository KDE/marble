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
#include "SphericalProjectionHelper.h"

// Marble
#include "GeoPainter.h"
#include "ViewportParams.h"


SphericalProjectionHelper::SphericalProjectionHelper()
    : AbstractProjectionHelper()
{
}

SphericalProjectionHelper::~SphericalProjectionHelper()
{
}


void SphericalProjectionHelper::paintBase( GeoPainter     *painter, 
					   ViewportParams *viewport,
					   QPen           &pen,
					   QBrush         &brush,
					   bool            antialiasing )
{
    int      imgrx = viewport->width() / 2;
    int      imgry = viewport->height() / 2;
    int      radius     = viewport->radius();
    quint64  imgradius2 = ( (quint64)imgrx * (quint64)imgrx
			    + (quint64)imgry * (quint64)imgry );

    painter->setRenderHint( QPainter::Antialiasing, antialiasing );

    painter->setPen( pen );
    painter->setBrush( brush );

    if ( imgradius2 < (double)radius * (double)radius ) {
        painter->drawRect( 0, 0, 
			   viewport->width() - 1, viewport->height() - 1 );
    }
    else {
        painter->drawEllipse( imgrx - radius, imgry - radius, 
                              2 * radius, 2 * radius );
    }
}
