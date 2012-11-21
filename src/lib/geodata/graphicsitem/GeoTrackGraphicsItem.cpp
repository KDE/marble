//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "GeoTrackGraphicsItem.h"

#include "GeoDataLineString.h"
#include "GeoDataTrack.h"
#include "MarbleDebug.h"

using namespace Marble;

GeoTrackGraphicsItem::GeoTrackGraphicsItem( const GeoDataTrack *track )
    : GeoLineStringGraphicsItem( new GeoDataLineString() )
{
    setTrack( track );
}

void GeoTrackGraphicsItem::setTrack( const GeoDataTrack* track )
{
    m_track = track;
    update();
}

void GeoTrackGraphicsItem::paint( GeoPainter *painter, const ViewportParams *viewport )
{
    update();

    GeoLineStringGraphicsItem::paint( painter, viewport );
}

void GeoTrackGraphicsItem::update()
{
    setLineString( m_track->lineString() );
    setCoordinate( GeoDataCoordinates(0, 0) );
}
