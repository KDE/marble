/*
    SPDX-FileCopyrightText: 2008 Torsten Rahn <rahn@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "GeoSceneLegend.h"

#include "GeoSceneSection.h"
#include "GeoSceneTypes.h"

namespace Marble
{

class GeoSceneLegendPrivate
{
public:
    ~GeoSceneLegendPrivate()
    {
        qDeleteAll(m_sections);
    }

    /// The vector holding all the sections in the legend.
    /// (We want to preserve the order and don't care
    /// much about speed here), so we don't use a hash
    QList<const GeoSceneSection *> m_sections;
};

GeoSceneLegend::GeoSceneLegend()
    : d(new GeoSceneLegendPrivate)
{
}

GeoSceneLegend::~GeoSceneLegend()
{
    delete d;
}

const char *GeoSceneLegend::nodeType() const
{
    return GeoSceneTypes::GeoSceneLegendType;
}

void GeoSceneLegend::addSection(const GeoSceneSection *section)
{
    // Remove any section that has the same name
    QList<const GeoSceneSection *>::iterator it = d->m_sections.begin();
    while (it != d->m_sections.end()) {
        const GeoSceneSection *currentSection = *it;
        if (currentSection->name() == section->name()) {
            delete currentSection;
            d->m_sections.erase(it);
            break;
        } else {
            ++it;
        }
    }

    if (section) {
        d->m_sections.append(section);
    }
}

QList<const GeoSceneSection *> GeoSceneLegend::sections() const
{
    return d->m_sections;
}

}
