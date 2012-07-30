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
      m_name( name ),
      m_sourceFile( QString() ),
      m_sourceFileFormat( QString() ),
      m_pen( QPen( QColor( 255, 255, 255 ) ) ),
      m_brush( QBrush( QColor( 255, 255, 255 ) ) )
{
}

GeoSceneGeodata::~GeoSceneGeodata()
{
}

const char* GeoSceneGeodata::nodeType() const
{
    return GeoSceneTypes::GeoSceneGeodataType;
}

QString GeoSceneGeodata::name() const
{
    return m_name;
}

QString GeoSceneGeodata::sourceFile() const
{
    return m_sourceFile;
}

void GeoSceneGeodata::setSourceFile(QString sourceFile)
{
    m_sourceFile = sourceFile;
}

QString GeoSceneGeodata::sourceFileFormat() const
{
    return m_sourceFileFormat;
}

void GeoSceneGeodata::setSourceFileFormat(QString format)
{
    m_sourceFileFormat = format;
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
