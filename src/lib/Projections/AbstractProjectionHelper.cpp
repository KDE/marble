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

const QRegion AbstractProjectionHelper::activeRegion() const
{
    return d->activeRegion;
}
