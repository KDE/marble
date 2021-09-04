/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "GeoSceneZoom.h"

#include "GeoSceneTypes.h"

namespace Marble
{

GeoSceneZoom::GeoSceneZoom()
    : GeoNode(),
      // Arbitrary defaults, as used in Marble
      m_minimum( 900 ),
      m_maximum( 2500 ),
      m_discrete( false )
{
}

int GeoSceneZoom::minimum() const
{
    return m_minimum;
}

void GeoSceneZoom::setMinimum( int minimum )
{
    m_minimum = minimum;
}

int GeoSceneZoom::maximum() const
{
    return m_maximum;
}

void GeoSceneZoom::setMaximum( int maximum )
{
    m_maximum = maximum;
}

bool GeoSceneZoom::discrete() const
{
    return m_discrete;
}

void GeoSceneZoom::setDiscrete( bool discrete )
{
    m_discrete = discrete;
}

const char *GeoSceneZoom::nodeType() const
{
    return GeoSceneTypes::GeoSceneZoomType;
}

}
