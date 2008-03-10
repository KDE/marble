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

#include "GeoSceneIcon.h"

GeoSceneIcon::GeoSceneIcon()
    // Arbitary defaults, as used in Marble
    : m_pixmap( "" ),
      m_color( "" )
{
    /* NOOP */
}

GeoSceneIcon::~GeoSceneIcon()
{
    /* NOOP */
}

const QString GeoSceneIcon::pixmap() const
{
    return m_pixmap;
}

void GeoSceneIcon::setPixmap( const QString& pixmap )
{
    m_pixmap = pixmap;
}

const QString GeoSceneIcon::color() const
{
    return m_color;
}

void GeoSceneIcon::setColor( const QString& color )
{
    m_color = color;
}
