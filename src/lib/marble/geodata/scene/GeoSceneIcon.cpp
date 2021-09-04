/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "GeoSceneIcon.h"

#include "GeoSceneTypes.h"

namespace Marble
{

GeoSceneIcon::GeoSceneIcon()
    : m_color()
{
}

GeoSceneIcon::~GeoSceneIcon()
{
}

QString GeoSceneIcon::pixmap() const
{
    return m_pixmap;
}

void GeoSceneIcon::setPixmap( const QString& pixmap )
{
    m_pixmap = pixmap;
}

QColor GeoSceneIcon::color() const
{
    return m_color;
}

void GeoSceneIcon::setColor( const QColor& color )
{
    m_color = color;
}

const char *GeoSceneIcon::nodeType() const
{
    return GeoSceneTypes::GeoSceneIconType;
}

}
