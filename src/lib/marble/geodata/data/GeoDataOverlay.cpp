//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Dennis Nienhüser <nienhueser@kde.org>
//

#include "GeoDataOverlay.h"
#include "GeoDataDocument.h"

#include <QFileInfo>
#include <QDir>

namespace Marble {

class GeoDataOverlayPrivate
{
public:
    QColor m_color;

    int m_drawOrder;

    QImage m_image;

    QString m_iconPath;

    GeoDataOverlayPrivate();
};

GeoDataOverlayPrivate::GeoDataOverlayPrivate() : m_color( Qt::white ), m_drawOrder( 0 )
{
    // nothing to do
}

GeoDataOverlay::GeoDataOverlay() : d( new GeoDataOverlayPrivate )
{
    // nothing to do
}

GeoDataOverlay::~GeoDataOverlay()
{
    delete d;
}

GeoDataOverlay::GeoDataOverlay( const GeoDataOverlay &other ) :
    GeoDataFeature( other ), d( new GeoDataOverlayPrivate( *other.d ) )
{
    // nothing to do
}

GeoDataOverlay &GeoDataOverlay::operator=( const GeoDataOverlay &other )
{
    GeoDataFeature::operator=( other );
    *d = *other.d;
    return *this;
}

QColor GeoDataOverlay::color() const
{
    return d->m_color;
}

void GeoDataOverlay::setColor( const QColor &color )
{
    d->m_color = color;
}

int GeoDataOverlay::drawOrder() const
{
    return d->m_drawOrder;
}

void GeoDataOverlay::setDrawOrder( int order )
{
    d->m_drawOrder = order;
}

QImage GeoDataOverlay::icon() const
{
    if ( d->m_image.isNull() && !d->m_iconPath.isEmpty() ) {
        d->m_image = QImage( absoluteIconFile() );
    }
    return d->m_image;
}

void GeoDataOverlay::setIcon( const QImage &icon )
{
    d->m_image = icon;
}

void GeoDataOverlay::setIconFile( const QString &path )
{
    d->m_iconPath = path;
    d->m_image = QImage( path );
}

QString GeoDataOverlay::iconFile() const
{
    return d->m_iconPath;
}

QString GeoDataOverlay::absoluteIconFile() const
{
    return resolvePath( d->m_iconPath );
}

bool GeoDataOverlay::equals(const GeoDataOverlay& other) const
{
    return GeoDataFeature::equals(other) &&
           d->m_drawOrder == other.d->m_drawOrder &&
           d->m_color == other.d->m_color &&
           d->m_iconPath == other.d->m_iconPath &&
           d->m_image == other.d->m_image;
}

}
