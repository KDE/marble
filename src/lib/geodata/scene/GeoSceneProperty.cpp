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

#include "GeoSceneProperty.h"

GeoSceneProperty::GeoSceneProperty()
    : m_name(""),
      m_available( false ),
      m_enabled( false )
{
    /* NOOP */
}

GeoSceneProperty::~GeoSceneProperty()
{
    /* NOOP */
}

QString GeoSceneProperty::name() const
{
    return m_name;
}

void GeoSceneProperty::setName( const QString &name )
{
    m_name = name;
}

bool GeoSceneProperty::available() const
{
    return m_available;
}

void GeoSceneProperty::setAvailable( bool available )
{
    m_available = available;
}

bool GeoSceneProperty::enabled() const
{
    return m_enabled;
}

void GeoSceneProperty::setEnabled( bool enabled )
{
    m_enabled = enabled;
}
