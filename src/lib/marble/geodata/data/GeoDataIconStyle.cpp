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

#include <QImageReader>

namespace Marble
{

class GeoDataIconStylePrivate
{
  public:
    GeoDataIconStylePrivate()
        : m_scale( 1.0 ),
        m_size(0, 0),
        m_iconPath(),
        m_heading( 0 )
    {
    }

    GeoDataIconStylePrivate( const QString& iconPath, const QPointF &hotSpot )
        : m_scale( 1.0 ),
          m_size(0, 0),
          m_iconPath( iconPath ),
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

    QSize scaledSize(const QSize &size) const
    {
        QSize iconSize = size.isNull() ? m_icon.size() : size;
        // Scaling the placemark's icon based on its size, scale, and maximum icon size.
        if ( iconSize.width()*m_scale > s_maximumIconSize.width()
             || iconSize.height()*m_scale > s_maximumIconSize.height() ) {
            iconSize.scale( s_maximumIconSize, Qt::KeepAspectRatio );
        }
        else if ( iconSize.width()*m_scale < s_minimumIconSize.width()
                  || iconSize.height()*m_scale < s_minimumIconSize.width() ) {
            iconSize.scale( s_minimumIconSize, Qt::KeepAspectRatio );
        }
        else {
            iconSize *= m_scale;
        }

        return iconSize;
    }

    QImage loadIcon(const QString &path, const QSize &size) const
    {
        if (!path.isEmpty()) {
            // Icons from the local file system
            if (!size.isNull()) {
                QImageReader imageReader;
                imageReader.setScaledSize(size);
                imageReader.setFileName(path);
                QImage icon = imageReader.read();
                if (icon.isNull()) {
                    mDebug() << "GeoDataIconStyle: Failed to read image " << path << ": " << imageReader.errorString();
                }
                return icon;
            }
            QImage icon = QImage(path);
            if (!icon.isNull()) {
                return icon;
            }
        }

        if(QUrl(m_iconPath).isValid() ) {
            // if image is not found on disk, check whether the icon is
            // at remote location. If yes then go for remote icon loading
            return remoteIconLoader()->load(QUrl(m_iconPath));
        }

        mDebug() << "Unable to open style icon at: " << path;
        return QImage();
    }

    float            m_scale;

    QImage           m_icon;
    QSize            m_size;
    QImage           m_scaledIcon;
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

GeoDataIconStyle::GeoDataIconStyle( const QString& iconPath, const QPointF &hotSpot ) :
    d( new GeoDataIconStylePrivate( iconPath, hotSpot ) )
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
           d->m_size == other.d->m_size &&
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
    d->m_scaledIcon = QImage();
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
    d->m_scaledIcon = QImage();
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
        d->m_icon = d->loadIcon(resolvePath(d->m_iconPath), d->m_size);
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

void GeoDataIconStyle::setSize(const QSize &size)
{
    if (size == d->m_size) {
        return;
    }

    d->m_size = size;
    if (!size.isNull() && !d->m_icon.isNull()) {
        // Resize existing icon that cannot be restored from an image path
        d->m_icon = d->m_icon.scaled(size);
    } else if (!d->m_iconPath.isEmpty()) {
        // Lazily reload the icons
        d->m_icon = QImage();
        d->m_scaledIcon = QImage();
    }
}

QSize GeoDataIconStyle::size() const
{
    return d->m_size;
}

void GeoDataIconStyle::setScale(float scale)
{
    d->m_scale = scale;
}

float GeoDataIconStyle::scale() const
{
    return d->m_scale;
}

QImage GeoDataIconStyle::scaledIcon() const
{
    if (!d->m_scaledIcon.isNull()) {
        return d->m_scaledIcon;
    }

    if (d->m_scale == 1.0) {
        return icon();
    }

    // Scale shouldn't be 0, but if it is, returning regular icon.
    if(d->m_scale <= 0 || icon().isNull() ) {
        return icon();
    }

    // Try to load it
    d->m_scaledIcon = d->loadIcon(resolvePath(d->m_iconPath), d->scaledSize(d->m_size));

    if (d->m_scaledIcon.isNull()) {
        // Direct loading failed, try to scale the icon as a last resort
        QImage const image = icon();
        if (!image.isNull()) {
            QSize iconSize = d->scaledSize(image.size());
            d->m_scaledIcon = image.scaled( iconSize, Qt::KeepAspectRatio, Qt::SmoothTransformation ) ;
        }
    }
    return d->m_scaledIcon;
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
