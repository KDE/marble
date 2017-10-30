/*
    Copyright (C) 2008 Torsten Rahn <rahn@kde.org>

    This file is part of the KDE project

    This library is free software you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    aint with this library see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

// Own
#include "GeoSceneHead.h"

// Marble
#include "GeoSceneTypes.h"
#include "GeoSceneIcon.h"
#include "GeoSceneZoom.h"
#include "GeoSceneLicense.h"

namespace Marble
{

class GeoSceneHeadPrivate
{
  public:
    GeoSceneHeadPrivate()
        : m_zoom(new GeoSceneZoom),
          m_icon(new GeoSceneIcon),
          m_license(new GeoSceneLicense),
          m_name(),
          m_target(),
          m_theme(),
          m_description(),
          m_radius( 0.0 ),
          m_visible( true )
    {
    }

    ~GeoSceneHeadPrivate()
    {
        delete m_icon;
        delete m_zoom;
        delete m_license;
    }

    GeoSceneZoom* m_zoom;
    GeoSceneIcon* m_icon;
    GeoSceneLicense* m_license;

    QString m_name;
    QString m_target;
    QString m_theme;
    QString m_description;
    qreal m_radius;

    bool    m_visible;
};


GeoSceneHead::GeoSceneHead()
    : d( new GeoSceneHeadPrivate )
{
}

GeoSceneHead::~GeoSceneHead()
{
    delete d;
}

const char* GeoSceneHead::nodeType() const
{
    return GeoSceneTypes::GeoSceneHeadType;
}

QString GeoSceneHead::name() const
{
    return d->m_name;
}

void GeoSceneHead::setName( const QString& name )
{
    d->m_name = name;
}

QString GeoSceneHead::target() const
{
    return d->m_target;
}

void GeoSceneHead::setTarget( const QString& target )
{
    d->m_target = target;
}

QString GeoSceneHead::theme() const
{
    return d->m_theme;
}

QString GeoSceneHead::mapThemeId() const
{
    return d->m_target + QLatin1Char('/') + d->m_theme + QLatin1Char('/') + d->m_theme + QLatin1String(".dgml");
}

void GeoSceneHead::setTheme( const QString& theme )
{
    d->m_theme = theme;
}

QString GeoSceneHead::description() const
{
    return d->m_description;
}

void GeoSceneHead::setDescription( const QString& description )
{
    d->m_description = description;
}

qreal GeoSceneHead::radius() const
{
    return d->m_radius;
}

void GeoSceneHead::setRadius( qreal radius )
{
    d->m_radius = radius;
}

bool GeoSceneHead::visible() const
{
    return d->m_visible;
}

void GeoSceneHead::setVisible( bool visible )
{
    d->m_visible = visible;
}

const GeoSceneIcon* GeoSceneHead::icon() const
{
    return d->m_icon;
}

GeoSceneIcon* GeoSceneHead::icon()
{
    return d->m_icon;
}

const GeoSceneZoom* GeoSceneHead::zoom() const
{
    return d->m_zoom;
}

GeoSceneZoom* GeoSceneHead::zoom()
{
    return d->m_zoom;
}

const GeoSceneLicense* GeoSceneHead::license() const {
    return d->m_license;
}

GeoSceneLicense* GeoSceneHead::license()
{
    return d->m_license;
}

}
