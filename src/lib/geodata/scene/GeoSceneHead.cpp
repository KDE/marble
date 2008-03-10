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

#include "GeoSceneHead.h"

GeoSceneHead::GeoSceneHead()
    : m_zoom(new GeoSceneZoom),
      m_icon(new GeoSceneIcon),
      m_name( "" ),
      m_target( "" ),
      m_theme( "" ),
      m_description( "" ),
      m_visible( true )
{
    /* NOOP */
}

GeoSceneHead::~GeoSceneHead()
{
    delete m_icon;
    delete m_zoom;
}

GeoSceneIcon* GeoSceneHead::icon() const
{
    return m_icon;
}

GeoSceneZoom* GeoSceneHead::zoom() const
{
    return m_zoom;
}

const QString GeoSceneHead::name() const
{
    return m_name;
}

void GeoSceneHead::setName( const QString& name )
{
    m_name = name;
}

const QString GeoSceneHead::target() const
{
    return m_target;
}

void GeoSceneHead::setTarget( const QString& target )
{
    m_target = target;
}

const QString GeoSceneHead::theme() const
{
    return m_theme;
}

void GeoSceneHead::setTheme( const QString& theme )
{
    m_theme = theme;
}

const QString GeoSceneHead::description() const
{
    return m_description;
}

void GeoSceneHead::setDescription( const QString& description )
{
    m_description = description;
}

bool GeoSceneHead::visible() const
{
    return m_visible;
}

void GeoSceneHead::setVisible( bool visible )
{
    m_visible = visible;
}
