/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "GeoScenePalette.h"

#include "GeoSceneTypes.h"

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

const char *GeoScenePalette::nodeType() const
{
    return GeoSceneTypes::GeoScenePaletteType;
}

}
