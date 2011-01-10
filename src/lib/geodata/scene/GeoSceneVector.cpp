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

#include "GeoSceneVector.h"
#include "GeoSceneTypes.h"

namespace Marble
{

GeoSceneVector::GeoSceneVector( const QString& name )
    : GeoSceneAbstractDataset( name ),
      m_sourceFile(),
      m_feature(),
      m_pen(),
      m_brush()
{
}

const char* GeoSceneVector::nodeType() const
{
    return GeoSceneTypes::GeoSceneVectorType;
}

QString GeoSceneVector::sourceFile() const
{
    return m_sourceFile;
}

void GeoSceneVector::setSourceFile( const QString& sourceFile )
{
    m_sourceFile = sourceFile;
}

QString GeoSceneVector::feature() const
{
    return m_feature;
}

void GeoSceneVector::setFeature( const QString& feature )
{
    m_feature = feature;
}

QPen GeoSceneVector::pen() const
{
    return m_pen;
}

void GeoSceneVector::setPen( const QPen& pen )
{
    m_pen = pen;
}

QBrush GeoSceneVector::brush() const
{
    return m_brush;
}

void GeoSceneVector::setBrush( const QBrush& brush )
{
    m_brush = brush;
}

QString GeoSceneVector::type()
{
    return "vector";
}

}
