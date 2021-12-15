// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Wes Hardaker <hardaker@users.sourceforge.net>
//

#include "GeoAprsCoordinates.h"

using namespace Marble;

GeoAprsCoordinates::GeoAprsCoordinates( qreal lon, qreal lat, int where )
    : GeoDataCoordinates( lon, lat, 0, GeoDataCoordinates::Degree ),
      m_seenFrom( where )
{
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
GeoAprsCoordinates::resetTimestamp()
{
    m_timestamp.start();
}

const QElapsedTimer &
GeoAprsCoordinates::timestamp() const
{
    return m_timestamp;
}
