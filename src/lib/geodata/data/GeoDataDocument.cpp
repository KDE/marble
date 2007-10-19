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
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.
*/

#include "GeoDataDocument.h"

#include "PlaceMark.h"
#include "GeoDataStyle.h"

GeoDataDocument::GeoDataDocument()
{
}

GeoDataDocument::~GeoDataDocument()
{
}

const QVector<GeoDataFolder>& GeoDataDocument::folders() const
{
    return m_folders;
}

void GeoDataDocument::addFolder(const GeoDataFolder& folder)
{
    m_folders.append(folder);
}

void GeoDataDocument::addStyle( GeoDataStyle* style )
{
    qDebug("GeoDataDocument: Add new style");
    m_styleHash.insert( style->styleId(), style );
}

const GeoDataStyle& GeoDataDocument::getStyle( QString styleId ) const
{
    /*
     * TODO: m_styleHash always should contain at least default GeoDataStyle element
     */
    return *m_styleHash.value( styleId );
}
