//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include "GeoDataIconStyle.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"

#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataIconStylePrivate
{
  public:
    GeoDataIconStylePrivate()
        : m_scale( 1.0 ),
        m_iconPath( MarbleDirs::path( "bitmaps/default_location.png" ) ),
        m_heading( 0 )
    {
    }

    GeoDataIconStylePrivate( const QImage& icon, const QPointF &hotSpot )
        : m_scale( 1.0 ),
          m_icon( icon ),
          m_hotSpot( hotSpot ),
          m_heading( 0 )
    {
    }

    const char* nodeType() const
    {
        return GeoDataTypes::GeoDataIconStyleType;
    }

    float            m_scale;

    QImage           m_icon;
    QString          m_iconPath;
    GeoDataHotSpot   m_hotSpot;
    mutable QPointF  m_pixelHotSpot;
    int              m_heading;
};

GeoDataIconStyle::GeoDataIconStyle() :
    d( new GeoDataIconStylePrivate() )
{
}

GeoDataIconStyle::GeoDataIconStyle( const GeoDataIconStyle& other ) :
    GeoDataColorStyle( other ), d( new GeoDataIconStylePrivate( *other.d ) )
{
}

GeoDataIconStyle::GeoDataIconStyle( const QImage& icon, const QPointF &hotSpot ) :
    d( new GeoDataIconStylePrivate( icon, hotSpot ) )
{
}

GeoDataIconStyle::~GeoDataIconStyle()
{
    delete d;
}

GeoDataIconStyle& GeoDataIconStyle::operator=( const GeoDataIconStyle& other )
{
    GeoDataColorStyle::operator=( other );
    *d = *other.d;
    return *this;
}

const char* GeoDataIconStyle::nodeType() const
{
    return d->nodeType();
}

void GeoDataIconStyle::setIcon(const QImage &icon)
{
    d->m_icon = icon;
}

void GeoDataIconStyle::setIconPath( const QString& filename )
{
    d->m_iconPath = filename;
}

QString GeoDataIconStyle::iconPath() const
{
    return d->m_iconPath;
}

QImage GeoDataIconStyle::icon() const
{
    if(!d->m_icon.isNull())
        return d->m_icon;
    else if(!d->m_iconPath.isEmpty()) {
        d->m_icon = QImage( resolvePath( d->m_iconPath ) );
        return d->m_icon;
    }
    else
        return QImage();
}

void GeoDataIconStyle::setHotSpot( const QPointF& hotSpot,
                                   GeoDataHotSpot::Units xunits,
                                   GeoDataHotSpot::Units yunits )
{
    d->m_hotSpot.setHotSpot( hotSpot, xunits, yunits );
}

QPointF GeoDataIconStyle::hotSpot( GeoDataHotSpot::Units &xunits, GeoDataHotSpot::Units &yunits ) const
{
    return d->m_hotSpot.hotSpot( xunits, yunits );
}

const QPointF& GeoDataIconStyle::hotSpot() const // always in pixels, Origin upper left
{
    GeoDataHotSpot::Units xunits;
    GeoDataHotSpot::Units yunits;

    d->m_pixelHotSpot = d->m_hotSpot.hotSpot( xunits, yunits );
    switch ( xunits ) {
    case GeoDataHotSpot::Fraction:
        d->m_pixelHotSpot.setX( d->m_icon.width() * d->m_pixelHotSpot.x() );
        break;
    case GeoDataHotSpot::Pixels:
        /* nothing to do */
        break;
    case GeoDataHotSpot::InsetPixels:
        d->m_pixelHotSpot.setX( d->m_icon.width() - d->m_pixelHotSpot.x() );
        break;
    }

    switch ( yunits ) {
    case GeoDataHotSpot::Fraction:
        d->m_pixelHotSpot.setY( d->m_icon.height() * ( 1.0 - d->m_pixelHotSpot.y() ) );
        break;
    case GeoDataHotSpot::Pixels:
        /* nothing to do */
        break;
    case GeoDataHotSpot::InsetPixels:
        d->m_pixelHotSpot.setY( d->m_icon.height() - d->m_pixelHotSpot.y() );
        break;
    }

    return d->m_pixelHotSpot;
}

void GeoDataIconStyle::setScale( const float &scale )
{
    d->m_scale = scale;
}

float GeoDataIconStyle::scale() const
{
    return d->m_scale;
}

int GeoDataIconStyle::heading() const
{
    return d->m_heading;
}

void GeoDataIconStyle::setHeading( int heading )
{
    d->m_heading = heading;
}

void GeoDataIconStyle::pack( QDataStream& stream ) const
{
    GeoDataColorStyle::pack( stream );

    stream << d->m_scale;
    stream << d->m_icon;
    d->m_hotSpot.pack( stream );
}

void GeoDataIconStyle::unpack( QDataStream& stream )
{
    GeoDataColorStyle::unpack( stream );

    stream >> d->m_scale;
    stream >> d->m_icon;
    d->m_hotSpot.unpack( stream );
}

}
