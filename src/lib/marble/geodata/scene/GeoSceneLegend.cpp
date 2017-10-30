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

#include "GeoSceneLegend.h"

#include "GeoSceneTypes.h"
#include "GeoSceneSection.h"

namespace Marble
{

class GeoSceneLegendPrivate
{
  public:
    ~GeoSceneLegendPrivate()
    {
        qDeleteAll( m_sections );
    }

     /// The vector holding all the sections in the legend.
    /// (We want to preserve the order and don't care 
    /// much about speed here), so we don't use a hash
    QVector<const GeoSceneSection*> m_sections;
};


GeoSceneLegend::GeoSceneLegend()
    : d( new GeoSceneLegendPrivate )
{
}

GeoSceneLegend::~GeoSceneLegend()
{
    delete d;
}

const char* GeoSceneLegend::nodeType() const
{
    return GeoSceneTypes::GeoSceneLegendType;
}

void GeoSceneLegend::addSection( const GeoSceneSection* section )
{
    // Remove any section that has the same name
    QVector<const GeoSceneSection*>::iterator it = d->m_sections.begin();
    while (it != d->m_sections.end()) {
        const GeoSceneSection* currentSection = *it;
        if ( currentSection->name() == section->name() ) {
            delete currentSection;
            d->m_sections.erase(it);
            break;
        }
        else {
            ++it;
        }
     }

    if ( section ) {
        d->m_sections.append( section );
    }
}

QVector<const GeoSceneSection*> GeoSceneLegend::sections() const
{
    return d->m_sections;
}

}
