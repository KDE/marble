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

#include "GeoSceneLegend.h"

GeoSceneLegend::GeoSceneLegend()
{
    /* NOOP */
}

GeoSceneLegend::~GeoSceneLegend()
{
    qDeleteAll(m_sections);
}

void GeoSceneLegend::addSection( GeoSceneSection* section )
{
    if ( section ) {
        m_sections.insert( section->name(), section );
    }
}

GeoSceneSection* GeoSceneLegend::section( const QString& name )
{
    GeoSceneSection* section = m_sections.value(name);
    if ( section ) {
        Q_ASSERT(section->name() == name);
        return section;
    }

    section = new GeoSceneSection( name );
    addSection( section );

    return section;
}
