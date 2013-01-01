//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Utku Aydın <utkuaydin34@gmail.com>
//

#include "GeoSceneGeodata.h"
#include "GeoSceneTypes.h"

#include <QtCore/QDebug>

namespace Marble
{
GeoSceneGeodata::GeoSceneGeodata( QString name )
    : GeoSceneAbstractDataset( name ),
      m_sourceFile( QString() ),
      m_pen( QPen( Qt::NoPen ) ),
      m_brush( QBrush( Qt::transparent ) )
{
}

GeoSceneGeodata::~GeoSceneGeodata()
{
}

const char* GeoSceneGeodata::nodeType() const
{
    return GeoSceneTypes::GeoSceneGeodataType;
}

QString GeoSceneGeodata::property() const
{
    return m_property;
}

void GeoSceneGeodata::setProperty( QString property )
{
    m_property = property;
}

QString GeoSceneGeodata::sourceFile() const
{
    return m_sourceFile;
}

void GeoSceneGeodata::setSourceFile(QString sourceFile)
{
    m_sourceFile = sourceFile;
}

QPen GeoSceneGeodata::pen() const
{
    return m_pen;
}

void GeoSceneGeodata::setPen( const QPen& pen )
{
    m_pen = pen;
}

QBrush GeoSceneGeodata::brush() const
{
    return m_brush;
}

void GeoSceneGeodata::setBrush( const QBrush& brush )
{
    m_brush = brush;
}
QString GeoSceneGeodata::type()
{
    return "geodata";
}

}
