/*
    Copyright (C) 2007 Murad Tagirov <tmurad@gmail.com>
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>

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

#include "GeoDataDocument.h"

#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"

#include <QtCore/QDebug>

class GeoDataDocumentPrivate
{
  public:
    GeoDataDocumentPrivate()
    {
    }

    ~GeoDataDocumentPrivate()
    {
    }

    QHash<QString, GeoDataStyle*> m_styleHash;
};

GeoDataDocument::GeoDataDocument()
    : GeoDocument()
    , GeoDataContainer()
    , d( new GeoDataDocumentPrivate() )
{
}

GeoDataDocument::~GeoDataDocument()
{
#if DEBUG_GEODATA
    qDebug() << "delete Document";
#endif
    delete d;
}

void GeoDataDocument::addStyle(GeoDataStyle* style)
{
    Q_ASSERT(style);
    qDebug("GeoDataDocument: Add new style");
    d->m_styleHash.insert(style->styleId(), style);
}

const GeoDataStyle* GeoDataDocument::style(const QString& styleId) const
{
    /*
     * FIXME: m_styleHash always should contain at least default
     *        GeoDataStyle element
     */
    return d->m_styleHash.value(styleId);
}

void GeoDataDocument::pack( QDataStream& stream ) const
{
    GeoDataContainer::pack( stream );

    stream << d->m_styleHash.size();
    
    
    for( QHash<QString, GeoDataStyle*>::const_iterator iterator 
          = d->m_styleHash.constBegin(); 
        iterator != d->m_styleHash.constEnd(); 
        ++iterator ) {
        iterator.value()->pack( stream );
    }
}


void GeoDataDocument::unpack( QDataStream& stream )
{
    GeoDataContainer::unpack( stream );

    int size = 0;

    stream >> size;
    for( int i = 0; i < size; i++ ) {
        GeoDataStyle* style = new GeoDataStyle();
        style->unpack( stream );
        d->m_styleHash.insert( style->styleId(), style );
    }
}
