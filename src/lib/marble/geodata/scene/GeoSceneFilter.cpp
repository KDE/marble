/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "GeoSceneFilter.h"

#include "GeoScenePalette.h"
#include "GeoSceneTypes.h"

namespace Marble
{

GeoSceneFilter::GeoSceneFilter(const QString &name)
    : m_name(name)
    , m_type(QStringLiteral("none"))
{
}

GeoSceneFilter::~GeoSceneFilter()
{
    qDeleteAll(m_palette);
}

QString GeoSceneFilter::name() const
{
    return m_name;
}

void GeoSceneFilter::setName(const QString &name)
{
    m_name = name;
}

QString GeoSceneFilter::type() const
{
    return m_type;
}

void GeoSceneFilter::setType(const QString &type)
{
    m_type = type;
}

QList<const GeoScenePalette *> GeoSceneFilter::palette() const
{
    return m_palette;
}

void GeoSceneFilter::addPalette(const GeoScenePalette *palette)
{
    m_palette.append(palette);
}

int GeoSceneFilter::removePalette(const GeoScenePalette *palette)
{
    return m_palette.removeAll(palette);
}

const char *GeoSceneFilter::nodeType() const
{
    return GeoSceneTypes::GeoSceneFilterType;
}

}
