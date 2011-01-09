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

#include "GeoSceneItem.h"

#include "GeoSceneIcon.h"
#include "GeoSceneTypes.h"

namespace Marble
{

GeoSceneItem::GeoSceneItem( const QString& name )
    : m_icon(new GeoSceneIcon),
      m_name( name ),
      m_text(),
      m_connectTo(),
      m_checkable( false ),
      m_spacing( 12 )
{
}

GeoSceneItem::~GeoSceneItem()
{
    delete m_icon;
}

const char* GeoSceneItem::nodeType() const
{
    return GeoSceneTypes::GeoSceneItemType;
}

const GeoSceneIcon* GeoSceneItem::icon() const
{
    return m_icon;
}

GeoSceneIcon* GeoSceneItem::icon()
{
    return m_icon;
}

QString GeoSceneItem::name() const
{
    return m_name;
}

QString GeoSceneItem::text() const
{
    return m_text;
}

void GeoSceneItem::setText( const QString& text )
{
    m_text = text;
}

bool GeoSceneItem::checkable() const
{
    return m_checkable;
}

void GeoSceneItem::setCheckable( bool checkable )
{
    m_checkable = checkable;
}

QString GeoSceneItem::connectTo() const
{
    return m_connectTo;
}

void GeoSceneItem::setConnectTo( const QString& connectTo )
{
    m_connectTo = connectTo;
}

int  GeoSceneItem::spacing() const
{
    return m_spacing;
}

void GeoSceneItem::setSpacing( int spacing )
{
    m_spacing = spacing;
}

}
