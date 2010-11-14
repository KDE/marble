//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//

#include "GeoDataColorStyle.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataColorStylePrivate
{
  public:
    GeoDataColorStylePrivate()
      : m_color( Qt::white ),
        m_colorMode( GeoDataColorStyle::Normal )
    {
    }

    QString nodeType() const
    {
        return GeoDataTypes::GeoDataColorStyleType;
    }

    /// stores the current color
    QColor     m_color;
    /// stores the current color mode
    GeoDataColorStyle::ColorMode  m_colorMode;
};

GeoDataColorStyle::GeoDataColorStyle()
      : d( new GeoDataColorStylePrivate )
{
}

GeoDataColorStyle::GeoDataColorStyle( const GeoDataColorStyle& other )
      : GeoDataObject( other ),
        d( new GeoDataColorStylePrivate( *other.d ) )
{
}

GeoDataColorStyle::~GeoDataColorStyle()
{
    delete d;
}

GeoDataColorStyle& GeoDataColorStyle::operator=( const GeoDataColorStyle& other )
{
    GeoDataObject::operator=( other );
    *d = *other.d;
    return *this;
}

QString GeoDataColorStyle::nodeType() const
{
    return d->nodeType();
}

void GeoDataColorStyle::setColor( const QColor &value )
{
    d->m_color = value;
}

QColor GeoDataColorStyle::color() const
{
    return d->m_color;
}

void GeoDataColorStyle::setColorMode( const ColorMode &colorMode )
{
    d->m_colorMode = colorMode;
}

GeoDataColorStyle::ColorMode GeoDataColorStyle::colorMode() const
{
    return d->m_colorMode;
}

void GeoDataColorStyle::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << d->m_color;
    // FIXME: Why is not colorMode saved?
//    stream << m_colorMode;
}

void GeoDataColorStyle::unpack( QDataStream& stream )
{
    GeoDataObject::unpack( stream );

    stream >> d->m_color;
    // FIXME: Why is not colorMode saved?
//    stream >> m_colorMode;
}

}
