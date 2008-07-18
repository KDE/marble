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
    QRegion  m_activeRegion;
    QRegion  m_projectedRegion;
    int      m_navigationStripe;
};



AbstractProjectionHelper::AbstractProjectionHelper()
    : d( new AbstractProjectionHelperPrivate )
{
    d->m_navigationStripe = 25;
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
    the shape of the "projected region" (which contains  
    the set of possible projected values) from the very same place 
    to ease implementation.

    Both the paintBase and the activeRegion can easily get defined 
    by providing a virtual method which has a QPainterPath as a
    return value. The implementation of that virtual method would
    just contain drawing routines for the QPainterPath.

    The method QRegion activeRegion() would get moved into 
    ViewportParams and would return a cached QRegion object that 
    gets created via QRegion( painterPath.toFillPolygon() ).
    As the activeRegion has the same shape but is smaller 
    than the projection border the polygon created in between 
    could get scaled down by the size of the 
    navigationStripe using QMatrix::map().  

    The paintBase method would just use painterPath.toFillPolygon()
    to create the needed polygon (as drawing polygons is AFAIK faster.
    than drawing a painterpath).

*/

void AbstractProjectionHelper::setActiveRegion( const QRegion& region )
{
    d->m_activeRegion = region;
}

const QRegion AbstractProjectionHelper::activeRegion() const
{
    return d->m_activeRegion;
}

void AbstractProjectionHelper::setProjectedRegion( const QRegion& region )
{
    d->m_projectedRegion = region;
}

const QRegion AbstractProjectionHelper::projectedRegion() const
{
    return d->m_projectedRegion;
}

int AbstractProjectionHelper::navigationStripe() const
{
    return d->m_navigationStripe;
}
