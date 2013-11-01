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

#include "GeoScenePalette.h"

namespace Marble
{

GeoScenePalette::GeoScenePalette( const QString& type,
                                  const QString& file )
    : m_type( type ),
      m_file( file )
{
}

QString GeoScenePalette::type() const
{
    return m_type;
}

void GeoScenePalette::setType( const QString& type )
{
    m_type = type;
}

QString GeoScenePalette::file() const
{
    return m_file;
}

void GeoScenePalette::setFile( const QString& file )
{
    m_file = file;
}

bool GeoScenePalette::operator==( const GeoScenePalette& rhs ) const
{
    return m_file == rhs.m_file && m_type == rhs.m_type;
}

}
