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

#include "GeoSceneSection.h"

#include "MarbleDebug.h"

#include "GeoSceneTypes.h"
#include "GeoSceneItem.h"

namespace Marble
{

GeoSceneSection::GeoSceneSection( const QString& name )
    : m_name( name ),
      m_heading( "" ),
      m_connectTo( "" ),
      m_checkable( false ),
      m_spacing( 12 )
{
}

GeoSceneSection::~GeoSceneSection()
{
    qDeleteAll( m_items );
}

const char* GeoSceneSection::nodeType() const
{
    return GeoSceneTypes::GeoSceneSectionType;
}

void GeoSceneSection::addItem( GeoSceneItem* item )
{
    // Remove any item that has the same name
    QVector<GeoSceneItem*>::iterator it = m_items.begin();
    while (it != m_items.end()) {
        GeoSceneItem* currentItem = *it;
        if ( currentItem->name() == item->name() ) {
            delete currentItem;
            it = m_items.erase(it);
            break;
        }
        else {
            ++it;
        }
     }

    if ( item ) {
        m_items.append( item );
    }
}

GeoSceneItem* GeoSceneSection::item( const QString& name )
{
    GeoSceneItem* item = 0;

    QVector<GeoSceneItem*>::const_iterator it = m_items.constBegin();
    QVector<GeoSceneItem*>::const_iterator end = m_items.constEnd();
    for (; it != end; ++it) {
        if ( (*it)->name() == name ) {
            item = *it;
            break;
        }
    }

    if ( !item ) {
        item = new GeoSceneItem( name );
        addItem( item );
    }

    return item;
}

QVector<GeoSceneItem*> GeoSceneSection::items() const
{
    return m_items;
}

QString GeoSceneSection::name() const
{
    return m_name;
}

QString GeoSceneSection::heading() const
{
    return m_heading;
}

void GeoSceneSection::setHeading( const QString& heading )
{
    m_heading = heading;
}

bool GeoSceneSection::checkable() const
{
    return m_checkable;
}

void GeoSceneSection::setCheckable( bool checkable )
{
    m_checkable = checkable;
}

QString GeoSceneSection::connectTo() const
{
    return m_connectTo;
}

void GeoSceneSection::setConnectTo( const QString& connectTo )
{
    m_connectTo = connectTo;
}

int  GeoSceneSection::spacing() const
{
    return m_spacing;
}

void GeoSceneSection::setSpacing( int spacing )
{
    m_spacing = spacing;
}

}
