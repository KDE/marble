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

#include "GeoSceneLayer.h"

GeoSceneLayer::GeoSceneLayer( const QString& name )
    : m_name( name ),
      m_plugin( "" ),
      m_type( "" )
{
    /* NOOP */
}

GeoSceneLayer::~GeoSceneLayer()
{
    /* NOOP */
}

QString GeoSceneLayer::name() const
{
    return m_name;
}

QString GeoSceneLayer::plugin() const
{
    return m_plugin;
}

void GeoSceneLayer::setPlugin( const QString& plugin )
{
    m_plugin = plugin;
}

QString GeoSceneLayer::type() const
{
    return m_type;
}

void GeoSceneLayer::setType( const QString& type )
{
    m_type = type;
}
