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

#include "GeoSceneDocument.h"

GeoSceneDocument::GeoSceneDocument()
    : GeoDocument()
    , m_head(new GeoSceneHead)
    , m_map(new GeoSceneMap)
    , m_settings(new GeoSceneSettings)
    , m_legend(new GeoSceneLegend)
{
}

GeoSceneDocument::~GeoSceneDocument()
{
    delete m_head;
    delete m_map;
    delete m_settings;
    delete m_legend;

}

GeoSceneHead* GeoSceneDocument::head() const
{
    return m_head;
}

GeoSceneMap* GeoSceneDocument::map() const
{
    return m_map;
}

GeoSceneSettings* GeoSceneDocument::settings() const
{
    return m_settings;
}

GeoSceneLegend* GeoSceneDocument::legend() const
{
    return m_legend;
}
