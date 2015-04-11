//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
//


#include <QUrl>

#include "GeoDataIconStyle.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "RemoteIconLoader.h"

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

    RemoteIconLoader* remoteIconLoader() const
    {
        static RemoteIconLoader *remoteIconLoader = new RemoteIconLoader();
        return remoteIconLoader;
    }

    float            m_scale;

    QImage           m_icon;
    QString          m_iconPath;
    GeoDataHotSpot   m_hotSpot;
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

bool GeoDataIconStyle::operator==( const GeoDataIconStyle &other ) const
{
    if ( GeoDataColorStyle::operator!=( other ) ) {
        return false;
    }

    return d->m_scale == other.d->m_scale &&
           d->m_icon == other.d->m_icon &&
           d->m_iconPath == other.d->m_iconPath &&
           d->m_hotSpot == other.d->m_hotSpot &&
           d->m_heading == other.d->m_heading;
}

bool GeoDataIconStyle::operator!=( const GeoDataIconStyle &other ) const
{
    return !this->operator==( other );
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

    /**
     * Set the m_icon to be a default-constructed icon
     * so that m_icon is null and icon() doesn't return
     * prevously loaded icon.
     */
    d->m_icon = QImage();
}

QString GeoDataIconStyle::iconPath() const
{
    return d->m_iconPath;
}

QImage GeoDataIconStyle::icon() const
{
    if ( !d->m_icon.isNull() ) {
        return d->m_icon;
    }
    else if ( !d->m_iconPath.isEmpty() ) {
        d->m_icon = QImage( resolvePath( d->m_iconPath ) );
        if( d->m_icon.isNull() ) {
            // if image is not found on disk, check whether the icon is
            // at remote location. If yes then go for remote icon loading
            QUrl remoteLocation = QUrl( d->m_iconPath );
            if( remoteLocation.isValid() ) {
                d->m_icon = d->remoteIconLoader()->load( d->m_iconPath );
            }
            else {
                mDebug() << "Unable to open style icon at: " << d->m_iconPath;
            }
        }

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

void GeoDataIconStyle::setScale( const float &scale )
{
    d->m_scale = scale;
}

float GeoDataIconStyle::scale() const
{
    return d->m_scale;
}

QImage GeoDataIconStyle::scaledIcon() const
{
    // Scale shouldn't be 0, but if it is, returning regular icon.
    if( scale() <= 0 || icon().isNull() ) {
        return icon();
    }

    QSize iconSize = icon().size();

    // Scaling the placemark's icon based on its size, scale, and maximum icon size.

    if ( iconSize.width()*scale() > s_maximumIconSize.width()
         || iconSize.height()*scale() > s_maximumIconSize.height() ) {
       iconSize.scale( s_maximumIconSize, Qt::KeepAspectRatio );
    }
    else if ( iconSize.width()*scale() < s_minimumIconSize.width()
              || iconSize.height()*scale() < s_minimumIconSize.width() ) {
       iconSize.scale( s_maximumIconSize, Qt::KeepAspectRatio );
    }
    else {
       iconSize *= scale();
    }
    return icon().scaled( iconSize ) ;
}

int GeoDataIconStyle::heading() const
{
    return d->m_heading;
}

void GeoDataIconStyle::setHeading( int heading )
{
    d->m_heading = heading;
}

RemoteIconLoader *GeoDataIconStyle::remoteIconLoader() const
{
    return d->remoteIconLoader();
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
