//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataLabelStyle.h"

#include <QFont>
#include <QColor>
#include <QDataStream>

#include "GeoDataTypes.h"

namespace Marble
{
#ifdef Q_OS_MACX
static const int defaultSize = 10;
#else
static const int defaultSize = 8;
#endif


class GeoDataLabelStylePrivate
{
  public:
    GeoDataLabelStylePrivate() 
        : m_scale( 1.0 ),
          m_alignment( GeoDataLabelStyle::Corner ),
          m_font( QFont(QStringLiteral("Sans Serif")).family(), defaultSize, 50, false ),
          m_glow( true )
    {
    }

    explicit GeoDataLabelStylePrivate( const QFont &font )
        : m_scale( 1.0 ),
          m_alignment( GeoDataLabelStyle::Corner ),
          m_font( font ),
          m_glow( true )
    {
    }

    /// The current scale of the label
    float  m_scale;
    /// The current alignment of the label
    GeoDataLabelStyle::Alignment m_alignment;
    /// The current font of the label
    QFont  m_font;   // Not a KML property
    /// Whether or not the text should glow
    bool m_glow; // Not a KML property
};

GeoDataLabelStyle::GeoDataLabelStyle()
    : d (new GeoDataLabelStylePrivate )
{
    setColor( QColor( Qt::black ) );
}

GeoDataLabelStyle::GeoDataLabelStyle( const GeoDataLabelStyle& other )
    : GeoDataColorStyle( other ), d (new GeoDataLabelStylePrivate( *other.d ) )
{
}

GeoDataLabelStyle::GeoDataLabelStyle( const QFont &font, const QColor &color )
    : d (new GeoDataLabelStylePrivate( font ) )
{
    setColor( color );
}

GeoDataLabelStyle::~GeoDataLabelStyle()
{
    delete d;
}

GeoDataLabelStyle& GeoDataLabelStyle::operator=( const GeoDataLabelStyle& other )
{
    GeoDataColorStyle::operator=( other );
    *d = *other.d;
    return *this;
}

bool GeoDataLabelStyle::operator==( const GeoDataLabelStyle &other ) const
{
    if ( GeoDataColorStyle::operator!=( other ) ) {
        return false;
    }

    return d->m_scale == other.d->m_scale &&
           d->m_alignment == other.d->m_alignment &&
           d->m_font == other.d->m_font &&
           d->m_glow == other.d->m_glow;
}

bool GeoDataLabelStyle::operator!=( const GeoDataLabelStyle &other ) const
{
    return !this->operator==( other );
}

const char* GeoDataLabelStyle::nodeType() const
{
    return GeoDataTypes::GeoDataLabelStyleType;
}

void GeoDataLabelStyle::setAlignment( GeoDataLabelStyle::Alignment alignment )
{
    d->m_alignment = alignment;
}

GeoDataLabelStyle::Alignment GeoDataLabelStyle::alignment() const
{
    return d->m_alignment;
}

void GeoDataLabelStyle::setScale(float scale)
{
    d->m_scale = scale;
}

float GeoDataLabelStyle::scale() const
{
    return d->m_scale;
}

void GeoDataLabelStyle::setFont( const QFont &font )
{
    d->m_font = font;
}

QFont GeoDataLabelStyle::font() const
{
    return d->m_font;
}

QFont GeoDataLabelStyle::scaledFont() const
{
   // Font shouldn't be smaller (or equal to) than 0, but if it is, regular font is returned
   // setPointSize() takes an integer as parameter, so rounded value should be checked
   if( qRound( font().pointSize() * scale() ) <= 0 ) {
       return font();
   }

   QFont scaledFont = font();
   scaledFont.setPointSize( qRound( scaledFont.pointSize() * scale() ));
   return scaledFont;
}

bool GeoDataLabelStyle::glow() const
{
    return d->m_glow;
}

void GeoDataLabelStyle::setGlow(bool on)
{
    d->m_glow = on;
}

void GeoDataLabelStyle::pack( QDataStream& stream ) const
{
    GeoDataColorStyle::pack( stream );

    stream << d->m_scale;
    stream << d->m_alignment;
    stream << d->m_font;
}

void GeoDataLabelStyle::unpack( QDataStream& stream )
{
    int a;
    GeoDataColorStyle::unpack( stream );

    stream >> d->m_scale;
    stream >> a;
    stream >> d->m_font;

    d->m_alignment = static_cast<GeoDataLabelStyle::Alignment>( a );
}

}
