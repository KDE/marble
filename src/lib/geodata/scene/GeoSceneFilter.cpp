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

#include "GeoSceneFilter.h"

#include "GeoScenePalette.h"

namespace Marble
{

GeoSceneFilter::GeoSceneFilter( const QString& name )
    : m_name( name ),
      m_type( "none" )
{
}

GeoSceneFilter::~GeoSceneFilter()
{
   qDeleteAll( m_palette );
}

QString GeoSceneFilter::name() const
{
    return m_name;
}

void GeoSceneFilter::setName( const QString& name )
{
    m_name = name;
}

QString GeoSceneFilter::sourceFile() const
{
    return m_sourceFile;
}

void GeoSceneFilter::setSourceFile( const QString& sourceFile )
{
    m_sourceFile = sourceFile;
}

QString GeoSceneFilter::coastlines() const
{
    return m_coastlines;
}

void GeoSceneFilter::setCoastlines( const QString& coastlines )
{
    m_coastlines = coastlines;
}

QString GeoSceneFilter::lakes() const
{
    return m_lakes;
}

void GeoSceneFilter::setLakes( const QString& lakes )
{
    m_lakes = lakes;
}

QString GeoSceneFilter::glaciers() const
{
    return m_glaciers;
}

void GeoSceneFilter::setGlaciers( const QString& glaciers )
{
    m_glaciers = glaciers;
}

QString GeoSceneFilter::fileFormat() const
{
    return m_fileFormat;
}

void GeoSceneFilter::setFileFormat( const QString& fileFormat )
{
    m_fileFormat = fileFormat;
}

QString GeoSceneFilter::type() const
{
    return m_type;
}

void GeoSceneFilter::setType( const QString& type )
{
    m_type = type;
}

QList<GeoScenePalette*> GeoSceneFilter::palette() const
{
    return m_palette;
}

void GeoSceneFilter::addPalette( GeoScenePalette *palette )
{
    m_palette.append( palette );
}

int GeoSceneFilter::removePalette( GeoScenePalette *palette )
{
    return m_palette.removeAll( palette );
}

}
