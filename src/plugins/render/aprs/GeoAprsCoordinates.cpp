//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#include "GeoAprsCoordinates.h"

using namespace Marble;

GeoAprsCoordinates::GeoAprsCoordinates( qreal lon, qreal lat, int where )
    : GeoDataCoordinates( lon, lat, 0, GeoDataCoordinates::Degree ),
      m_seenFrom( where )
{
    m_timestamp = QTime::currentTime();
    m_timestamp.start();
}

GeoAprsCoordinates::~GeoAprsCoordinates()
{
}

void
GeoAprsCoordinates::addSeenFrom( int where )
{
    m_seenFrom = ( m_seenFrom | where );
}

int
GeoAprsCoordinates::seenFrom() const
{
    return m_seenFrom;
}

void
GeoAprsCoordinates::setTimestamp( const QTime &t )
{
    m_timestamp = t;
    m_timestamp.start();
}

const QTime &
GeoAprsCoordinates::timestamp() const
{
    return m_timestamp;
}
