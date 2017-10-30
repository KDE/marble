//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataLineStyle.h"

#include "GeoDataTypes.h"

#include <QDataStream>

namespace Marble
{

class GeoDataLineStylePrivate
{
  public:
    GeoDataLineStylePrivate() 
        : m_width( 1.0 ), m_physicalWidth( 0.0 ),
          m_capStyle( Qt::FlatCap ), m_penStyle( Qt::SolidLine ),
          m_cosmeticOutline( false ), m_background( false )
    {
    }

    /// The current width of the line
    float  m_width;
    /// The current real width of the line
    float  m_physicalWidth;
    Qt::PenCapStyle m_capStyle;
    Qt::PenStyle m_penStyle;
    bool m_cosmeticOutline;
    bool m_background;
    QVector< qreal > m_pattern;
};

GeoDataLineStyle::GeoDataLineStyle()
    : d (new GeoDataLineStylePrivate )
{
}

GeoDataLineStyle::GeoDataLineStyle( const GeoDataLineStyle& other )
    : GeoDataColorStyle( other ), d (new GeoDataLineStylePrivate( *other.d ) )
{
}

GeoDataLineStyle::GeoDataLineStyle( const QColor &color )
    : d ( new GeoDataLineStylePrivate )
{
    setColor( color );
}

GeoDataLineStyle::~GeoDataLineStyle()
{
    delete d;
}

GeoDataLineStyle& GeoDataLineStyle::operator=( const GeoDataLineStyle& other )
{
    GeoDataColorStyle::operator=( other );
    *d = *other.d;
    return *this;
}

bool GeoDataLineStyle::operator==( const GeoDataLineStyle &other ) const
{
    if ( GeoDataColorStyle::operator!=( other ) ) {
        return false;
    }

    return d->m_width == other.d->m_width &&
           d->m_physicalWidth == other.d->m_physicalWidth &&
           d->m_capStyle == other.d->m_capStyle &&
           d->m_penStyle == other.d->m_penStyle &&
           d->m_background == other.d->m_background &&
           d->m_pattern == other.d->m_pattern;
}

bool GeoDataLineStyle::operator!=( const GeoDataLineStyle &other ) const
{
    return !this->operator==( other );
}

const char* GeoDataLineStyle::nodeType() const
{
    return GeoDataTypes::GeoDataLineStyleType;
}

void GeoDataLineStyle::setWidth(float width)
{
    d->m_width = width;
}

float GeoDataLineStyle::width() const
{
    return d->m_width;
}

float GeoDataLineStyle::physicalWidth() const
{
    return d->m_physicalWidth;
}

void GeoDataLineStyle::setPhysicalWidth(float realWidth)
{
    d->m_physicalWidth = realWidth;
}

bool GeoDataLineStyle::cosmeticOutline() const
{
    return d->m_cosmeticOutline;
}

void GeoDataLineStyle::setCosmeticOutline(bool enabled)
{
    d->m_cosmeticOutline = enabled;
}

Qt::PenCapStyle GeoDataLineStyle::capStyle() const
{
    return d->m_capStyle;
}

void GeoDataLineStyle::setCapStyle( Qt::PenCapStyle style )
{
    d->m_capStyle = style;
}

Qt::PenStyle GeoDataLineStyle::penStyle() const
{
    return d->m_penStyle;
}

void GeoDataLineStyle::setPenStyle( Qt::PenStyle style )
{
   d->m_penStyle = style;
}

bool GeoDataLineStyle::background() const
{
    return d->m_background;
}

void GeoDataLineStyle::setBackground( bool background )
{
    d->m_background = background;
}

QVector< qreal > GeoDataLineStyle::dashPattern() const
{
    return d->m_pattern;
}

void GeoDataLineStyle::setDashPattern( const QVector< qreal >& pattern )
{
    d->m_pattern = pattern;
}

void GeoDataLineStyle::pack( QDataStream& stream ) const
{
    GeoDataColorStyle::pack( stream );
    
    stream << d->m_width;
    stream << d->m_physicalWidth;
    stream << (int)d->m_penStyle;
    stream << (int)d->m_capStyle;
    stream << d->m_background;
}

void GeoDataLineStyle::unpack( QDataStream& stream )
{
    GeoDataColorStyle::unpack( stream );
    
    stream >> d->m_width;
    stream >> d->m_physicalWidth;
    int style;
    stream >> style;
    d->m_penStyle = ( Qt::PenStyle ) style;
    stream >> style;
    d->m_capStyle = ( Qt::PenCapStyle ) style;
    stream >> d->m_background;
}

}
