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
#include "AbstractProjectionHelper_p.h"

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
    int      imgrx      = viewport->width() / 2;
    int      imgry      = viewport->height() / 2;
    int      radius     = viewport->radius();
    quint64  imgradius2 = ( (quint64)imgrx * (quint64)imgrx
			    + (quint64)imgry * (quint64)imgry );

    painter->setRenderHint( QPainter::Antialiasing, antialiasing );

    painter->setPen( pen );
    painter->setBrush( brush );

    if ( imgradius2 < (quint64)radius * (quint64)radius ) {
        painter->drawRect( 0, 0, 
			   viewport->width() - 1, viewport->height() - 1 );
    }
    else {
        painter->drawEllipse( imgrx - radius, imgry - radius, 
                              2 * radius, 2 * radius );
    }
}


void SphericalProjectionHelper::setActiveRegion( ViewportParams *viewport )
{
    int  radius    = viewport->radius();
    int  imgWidth  = viewport->width();
    int  imgHeight = viewport->height();

    // If the globe covers the whole image, then the active region is
    // all of the image except a strip 25 pixels wide in all
    // directions.  Otherwise the active region is the globe.
    if ( viewport->mapCoversViewport() ) {
    	d->activeRegion = QRegion( 25, 25, imgWidth - 50, imgHeight - 50,
				   QRegion::Rectangle );
    }
    else {
    	d->activeRegion = QRegion( imgWidth  / 2 - radius,
				   imgHeight / 2 - radius,
				   2 * radius, 2 * radius,
				   QRegion::Ellipse );
    }
}
