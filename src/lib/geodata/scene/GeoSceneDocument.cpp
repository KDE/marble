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
    , m_head(0)
    , m_map(0)
    , m_settings(0)
    , m_legend(0)
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
    if (!m_head)
        m_head = new GeoSceneHead;

    return m_head;
}

GeoSceneMap* GeoSceneDocument::map() const
{
    if (!m_map)
        m_map = new GeoSceneMap;

    return m_map;
}

GeoSceneSettings* GeoSceneDocument::settings() const
{
    if (!m_settings)
        m_settings = new GeoSceneSettings;

    return m_settings;
}

GeoSceneLegend* GeoSceneDocument::legend() const
{
    if (!m_legend)
        m_legend = new GeoSceneLegend;

    return m_legend;
}
