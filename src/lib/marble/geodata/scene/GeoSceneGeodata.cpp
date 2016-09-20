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

#include <QDebug>

namespace Marble
{
GeoSceneGeodata::GeoSceneGeodata( const QString& name )
    : GeoSceneAbstractDataset( name ),
      m_sourceFile( QString() ),
      m_alpha( 1.0 ),
      m_pen( QPen( Qt::NoPen ) ),
      m_brush( QBrush( Qt::transparent ) ),
      m_renderOrder( 0 )
{
}

GeoSceneGeodata::~GeoSceneGeodata()
{
}

const char* GeoSceneGeodata::nodeType() const
{
    return GeoSceneTypes::GeoSceneGeodataType;
}

bool GeoSceneGeodata::operator==( const GeoSceneGeodata &other ) const
{
    return m_sourceFile == other.sourceFile()
            && m_pen == other.pen()
            && m_brush == other.brush();
}

QString GeoSceneGeodata::property() const
{
    return m_property;
}

void GeoSceneGeodata::setProperty( const QString& property )
{
    m_property = property;
}

QString GeoSceneGeodata::sourceFile() const
{
    return m_sourceFile;
}

void GeoSceneGeodata::setSourceFile(const QString& sourceFile)
{
    m_sourceFile = sourceFile;
}

QString GeoSceneGeodata::colorize() const
{
    return m_colorize;
}

void GeoSceneGeodata::setColorize( const QString& colorize )
{
    m_colorize = colorize;
}

QPen GeoSceneGeodata::pen() const
{
    return m_pen;
}

void GeoSceneGeodata::setAlpha(qreal alpha)
{
    m_alpha = alpha;
}

qreal GeoSceneGeodata::alpha() const
{
    return m_alpha;
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

int GeoSceneGeodata::renderOrder() const
{
    return m_renderOrder;
}

void GeoSceneGeodata::setRenderOrder( int renderOrder )
{
    m_renderOrder = renderOrder;
}

QVector<QColor> GeoSceneGeodata::colors() const
{
    return m_colors;
}

void GeoSceneGeodata::setColors(const QVector<QColor> &colors)
{
    m_colors = colors;
}

}
