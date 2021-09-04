/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
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

}
