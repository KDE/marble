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
			   viewport->width(), viewport->height() );
    }
    else {
        painter->drawEllipse( imgrx - radius, imgry - radius, 
                              2 * radius, 2 * radius );
    }
}

void SphericalProjectionHelper::createActiveRegion( ViewportParams *viewport )
{
    int  radius    = viewport->radius();
    int  imgWidth  = viewport->width();
    int  imgHeight = viewport->height();

    // If the globe covers the whole image, then the active region represents
    // all of the image except for the navigationStripe at the border
    // of the viewport ("rectActiveRegion").  
    // Otherwise the active region has got the shape of the visible globe but 
    // is diminuished by the amount of the width of the navigationStripe ).

    QRegion rectActiveRegion( navigationStripe() , navigationStripe() , 
    imgWidth - 2 * navigationStripe() , imgHeight - 2 * navigationStripe() ,
    QRegion::Rectangle );

    if ( viewport->mapCoversViewport() ) {
        setActiveRegion( rectActiveRegion );
    }
    else {
        setActiveRegion( QRegion(
            imgWidth  / 2 - radius + navigationStripe() ,
            imgHeight / 2 - radius + navigationStripe() ,
            2 * ( radius - navigationStripe()  ),
            2 * ( radius - navigationStripe()  ),
            QRegion::Ellipse ).intersect( rectActiveRegion ) );
    }
}

void SphericalProjectionHelper::createProjectedRegion( ViewportParams *viewport )
{
    int  radius    = viewport->radius();
    int  imgWidth  = viewport->width();
    int  imgHeight = viewport->height();

    // If the globe covers the whole image, then the projected region represents
    // all of the image.  
    // Otherwise the active region has got the shape of the visible globe.

    QRegion rectProjectedRegion( 0 , 0 , 
    imgWidth, imgHeight,
    QRegion::Rectangle );

    if ( viewport->mapCoversViewport() ) {
        setProjectedRegion( rectProjectedRegion );
    }
    else {
        setProjectedRegion( QRegion(
            imgWidth  / 2 - radius,
            imgHeight / 2 - radius,
            2 * radius,
            2 * radius,
            QRegion::Ellipse ).intersect( rectProjectedRegion ) );
    }
}
