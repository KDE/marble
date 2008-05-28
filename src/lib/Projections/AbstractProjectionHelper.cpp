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
#include "AbstractProjectionHelper.h"

// Qt
#include <QtGui/QRegion>

// Marble
//#include "ViewportParams.h"


class AbstractProjectionHelperPrivate
{
 public:
    QRegion  activeRegion;
};



AbstractProjectionHelper::AbstractProjectionHelper()
    : d( new AbstractProjectionHelperPrivate )
{
}

AbstractProjectionHelper::~AbstractProjectionHelper()
{
    delete d;
}

#if 0 // Made pure virtual (= 0)
void AbstractProjectionHelper::paintBase( GeoPainter     *painter, 
					  ViewportParams *viewport,
					  QPen            &pen,
					  QBrush          &brush,
					  bool            antialiasing)
{
}
#endif

/* 
    FIXME: Actually the paintBase and activeRegion should both draw 
    the shape of the "projection border" (which is the "border" of 
    the set of possible values) from the very same place to ease
    implementation.

    Both the paintBase and the activeRegion can easily get defined 
    by providing a virtual method which has a QPainterPath as a
    return value. The implementation of that virtual method would
    just contain drawing routines for the QPainterPath.

    The method QRegion activeRegion() would get moved into 
    ViewportParams and would return a cached QRegion object that 
    gets created via QRegion( painterPath.toFillPolygon() ).
    As the activeRegion has the same shape but is smaller 
    than the projection border the polygon created in between 
    could get scaled down by 50 pixels using QMatrix::map().

    The paintBase method would just use painterPath.toFillPolygon()
    to create the needed polygon (as drawing polygons is AFAIK faster.
    than drawing a painterpath).

*/

const QRegion AbstractProjectionHelper::activeRegion() const
{
    return d->activeRegion;
}
