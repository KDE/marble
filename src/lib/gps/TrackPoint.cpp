//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson  <g.real.ate@gmail.com>
//

#include "TrackPoint.h"
#include "ClipPainter.h"

#include <QtCore/QPoint>

TrackPoint::TrackPoint( double lat, double lon )
    :Waypoint ( lat, lon )
{
}

void TrackPoint::draw( ClipPainter * painter,
                     const QPoint &position)
{
    painter->drawEllipse( position.x()-2, position.y()-2, 3, 3 );
}
