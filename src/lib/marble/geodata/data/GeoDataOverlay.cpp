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
#include "GeoDataOverlay_p.h"

namespace Marble {

GeoDataOverlay::GeoDataOverlay()
    : GeoDataFeature(new GeoDataOverlayPrivate)
{
    // nothing to do
}

GeoDataOverlay::GeoDataOverlay(const GeoDataOverlay &other)
    : GeoDataFeature(other, new GeoDataOverlayPrivate(*other.d_func()))
{
    // nothing to do
}

GeoDataOverlay::GeoDataOverlay(GeoDataOverlayPrivate *priv)
    : GeoDataFeature(priv)
{
}

GeoDataOverlay::GeoDataOverlay(const GeoDataOverlay& other, GeoDataOverlayPrivate *priv)
    : GeoDataFeature(other, priv)
{
}

GeoDataOverlay::~GeoDataOverlay()
{
}

GeoDataOverlay &GeoDataOverlay::operator=( const GeoDataOverlay &other )
{
    if (this != &other) {
        Q_D(GeoDataOverlay);
        *d = *other.d_func();
    }

    return *this;
}

GeoDataFeature * GeoDataOverlay::clone() const
{
    return new GeoDataOverlay(*this);
}

QColor GeoDataOverlay::color() const
{
    Q_D(const GeoDataOverlay);
    return d->m_color;
}

void GeoDataOverlay::setColor( const QColor &color )
{
    Q_D(GeoDataOverlay);
    d->m_color = color;
}

int GeoDataOverlay::drawOrder() const
{
    Q_D(const GeoDataOverlay);
    return d->m_drawOrder;
}

void GeoDataOverlay::setDrawOrder( int order )
{
    Q_D(GeoDataOverlay);
    d->m_drawOrder = order;
}

QImage GeoDataOverlay::icon() const
{
    Q_D(const GeoDataOverlay);
    if ( d->m_image.isNull() && !d->m_iconPath.isEmpty() ) {
        d->m_image = QImage( absoluteIconFile() );
    }
    return d->m_image;
}

void GeoDataOverlay::setIcon( const QImage &icon )
{
    Q_D(GeoDataOverlay);
    d->m_image = icon;
}

void GeoDataOverlay::setIconFile( const QString &path )
{
    Q_D(GeoDataOverlay);
    d->m_iconPath = path;
    d->m_image = QImage( path );
}

QString GeoDataOverlay::iconFile() const
{
    Q_D(const GeoDataOverlay);
    return d->m_iconPath;
}

QString GeoDataOverlay::absoluteIconFile() const
{
    Q_D(const GeoDataOverlay);
    return resolvePath( d->m_iconPath );
}

bool GeoDataOverlay::equals(const GeoDataOverlay& other) const
{
    Q_D(const GeoDataOverlay);
    const GeoDataOverlayPrivate* const other_d = other.d_func();

    return GeoDataFeature::equals(other) &&
           d->m_drawOrder == other_d->m_drawOrder &&
           d->m_color == other_d->m_color &&
           d->m_iconPath == other_d->m_iconPath &&
           d->m_image == other_d->m_image;
}

}
