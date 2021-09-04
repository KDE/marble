/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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
