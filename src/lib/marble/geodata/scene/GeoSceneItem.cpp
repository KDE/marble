/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
