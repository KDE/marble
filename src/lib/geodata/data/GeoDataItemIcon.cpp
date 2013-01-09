//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2012      Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "GeoDataItemIcon.h"
#include "GeoDataTypes.h"

namespace Marble
{

class GeoDataItemIconPrivate
{
public:
    GeoDataItemIconPrivate();

    GeoDataItemIcon::ItemIconStates m_state;
    QString m_iconPath;
    QImage m_icon;
};

GeoDataItemIconPrivate::GeoDataItemIconPrivate() :
    m_state(),
    m_iconPath(),
    m_icon()
{
}

GeoDataItemIcon::GeoDataItemIcon() :
    d( new GeoDataItemIconPrivate )
{
}

GeoDataItemIcon::GeoDataItemIcon( const Marble::GeoDataItemIcon &other ) :
    GeoDataFeature(), d( new GeoDataItemIconPrivate( *other.d ) )
{
}

GeoDataItemIcon &GeoDataItemIcon::operator=( const GeoDataItemIcon &other )
{
    GeoDataFeature::operator=( other );
    *d = *other.d;
    return *this;
}

GeoDataItemIcon::~GeoDataItemIcon()
{
    delete d;
}

const char *GeoDataItemIcon::nodeType() const
{
    return GeoDataTypes::GeoDataItemIconType;
}

GeoDataItemIcon::ItemIconStates GeoDataItemIcon::state() const
{
    return d->m_state;
}

void GeoDataItemIcon::setState( const ItemIconStates &state )
{
    d->m_state = state;
}

void GeoDataItemIcon::setIcon( const QImage &icon )
{
    d->m_icon = icon;
}

QString GeoDataItemIcon::iconPath() const
{
    return d->m_iconPath;
}

void GeoDataItemIcon::setIconPath( const QString &path )
{
    d->m_iconPath = path;
}

QImage GeoDataItemIcon::icon() const
{
    if(!d->m_icon.isNull())
    {
        return d->m_icon;
    }
    else if(!d->m_iconPath.isEmpty())
    {
        d->m_icon = QImage(d->m_iconPath);
        return d->m_icon;
    }
    else
    {
        return QImage();
    }
}

}
