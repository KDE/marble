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

#include "GeoSceneAbstractDataset.h"

#include <limits>

namespace Marble
{

// FIXME: Filters are a Dataset.

GeoSceneAbstractDataset::GeoSceneAbstractDataset( const QString& name )
    : m_name( name ),
      m_fileFormat(),
      m_expire( std::numeric_limits<int>::max() )
{
}

QString GeoSceneAbstractDataset::name() const
{
    return m_name;
}

QString GeoSceneAbstractDataset::fileFormat() const
{
    return m_fileFormat;
}

void GeoSceneAbstractDataset::setFileFormat( const QString& fileFormat )
{
    m_fileFormat = fileFormat;
}

int GeoSceneAbstractDataset::expire() const
{
    return m_expire;
}

void GeoSceneAbstractDataset::setExpire( int expire )
{
    m_expire = expire;
}

}
