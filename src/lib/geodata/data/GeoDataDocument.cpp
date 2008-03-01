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

GeoDataDocument::GeoDataDocument()
    : GeoDocument()
    , GeoDataContainer()
{
}

GeoDataDocument::~GeoDataDocument()
{
}

void GeoDataDocument::addStyle(GeoDataStyle* style)
{
    Q_ASSERT(style);
    qDebug("GeoDataDocument: Add new style");
    m_styleHash.insert(style->styleId(), style);
}

const GeoDataStyle* GeoDataDocument::style(const QString& styleId) const
{
    /*
     * FIXME: m_styleHash always should contain at least default
     *        GeoDataStyle element
     */
    return m_styleHash.value(styleId);
}
