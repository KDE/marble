//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataPolyStyle.h"
#include "GeoDataTypes.h"
#include "MarbleDirs.h"

#include <QDataStream>
#include <QImage>

namespace Marble
{

class GeoDataPolyStylePrivate
{
  public:
    GeoDataPolyStylePrivate()
        : m_fill(true)
        , m_outline(true)
        , m_brushStyle(Qt::SolidPattern)
        , m_colorIndex(0)
    {
    }

    const char* nodeType() const
    {
        return GeoDataTypes::GeoDataPolyStyleType;
    }

    /// whether to fill the polygon
    bool  m_fill;
    /// whether to draw the outline
    bool  m_outline;
    Qt::BrushStyle m_brushStyle;
    /// The value of colorIndex will be maped to a color for brush
    quint8 m_colorIndex;
    QString m_texturePath;
    QImage m_textureImage;
};

GeoDataPolyStyle::GeoDataPolyStyle()
    : d( new GeoDataPolyStylePrivate )
{
}

GeoDataPolyStyle::GeoDataPolyStyle( const GeoDataPolyStyle& other )
    : GeoDataColorStyle( other ), d( new GeoDataPolyStylePrivate( *other.d ) )
{
}

GeoDataPolyStyle::GeoDataPolyStyle( const QColor &color )
    : d( new GeoDataPolyStylePrivate )
{
    setColor( color );
}

GeoDataPolyStyle::~GeoDataPolyStyle()
{
    delete d;
}

GeoDataPolyStyle& GeoDataPolyStyle::operator=( const GeoDataPolyStyle& other )
{
    GeoDataColorStyle::operator=( other );
    *d = *other.d;
    return *this;
}

bool GeoDataPolyStyle::operator==( const GeoDataPolyStyle &other ) const
{
    if ( GeoDataColorStyle::operator!=( other ) ) {
        return false;
    }

    return d->m_fill == other.d->m_fill &&
           d->m_outline == other.d->m_outline &&
           d->m_brushStyle == other.d->m_brushStyle;
}

bool GeoDataPolyStyle::operator!=( const GeoDataPolyStyle &other ) const
{
    return !this->operator==( other );
}

const char* GeoDataPolyStyle::nodeType() const
{
    return d->nodeType();
}

void GeoDataPolyStyle::setFill(bool fill)
{
    d->m_fill = fill;
}

bool GeoDataPolyStyle::fill() const
{
    return d->m_fill;
}

void GeoDataPolyStyle::setOutline(bool outline)
{
    d->m_outline = outline;
}

bool GeoDataPolyStyle::outline() const
{
    return d->m_outline;
}

void GeoDataPolyStyle::setBrushStyle( const Qt::BrushStyle style )
{
    d->m_brushStyle = style;
}

Qt::BrushStyle GeoDataPolyStyle::brushStyle() const
{
    return d->m_brushStyle;
}

void GeoDataPolyStyle::setColorIndex( quint8 colorIndex )
{
    d->m_colorIndex = colorIndex;
}

quint8 GeoDataPolyStyle::colorIndex() const
{
    return d->m_colorIndex;
}

void GeoDataPolyStyle::setTexturePath( const QString& texturePath )
{
    d->m_texturePath = texturePath;
    d->m_textureImage = QImage();
}

QString GeoDataPolyStyle::texturePath() const
{
    return d->m_texturePath;
}

QImage GeoDataPolyStyle::textureImage() const
{
    if ( !d->m_textureImage.isNull() ) {
        return d->m_textureImage;
    } else if ( !d->m_texturePath.isEmpty() ) {
        d->m_textureImage = QImage( resolvePath( d->m_texturePath ) );
    }

    return d->m_textureImage;
}

void GeoDataPolyStyle::pack( QDataStream& stream ) const
{
    GeoDataColorStyle::pack( stream );

    stream << d->m_fill;
    stream << d->m_outline;
    stream << d->m_colorIndex;
}

void GeoDataPolyStyle::unpack( QDataStream& stream )
{
    GeoDataColorStyle::unpack( stream );

    stream >> d->m_fill;
    stream >> d->m_outline;
    stream >> d->m_colorIndex;
}

}
