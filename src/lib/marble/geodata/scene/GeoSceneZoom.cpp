/*
    Copyright (C) 2008 Torsten Rahn <rahn@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "GeoSceneZoom.h"

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

}
