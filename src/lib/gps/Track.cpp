//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson  <g.real.ate@gmail.com>
//


#include "Track.h"


Track::Track() : AbstractLayerData(0,0)
{
    m_trackSegs = new QVector<WaypointContainer>();
}

Track::~Track()
{
}


void Track::addTrackSeg( WaypointContainer trackSeg )
{
    m_trackSegs->append( trackSeg );
}
