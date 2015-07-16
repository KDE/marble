/*
    Copyright (C) 2007 Murad Tagirov <tmurad@gmail.com>
    Copyright (C) 2007 Nikolas Zimmermann <zimmermann@kde.org>

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
#include "GeoSceneDocument.h"

// Marble
#include "GeoSceneHead.h"
#include "GeoSceneMap.h"
#include "GeoSceneLegend.h"
#include "GeoSceneSettings.h"
#include "GeoSceneTypes.h"

namespace Marble
{

class GeoSceneDocumentPrivate
{
  public:
    GeoSceneDocumentPrivate()
        : m_head(new GeoSceneHead),
          m_map(new GeoSceneMap),
          m_settings(new GeoSceneSettings),
          m_legend(new GeoSceneLegend)
    {
    }

    ~GeoSceneDocumentPrivate()
    {
        delete m_head;
        delete m_map;
        delete m_settings;
        delete m_legend;
    }
    
    const char* nodeType() const
    {
        return GeoSceneTypes::GeoSceneDocumentType;
    }
    
    GeoSceneHead*     m_head;
    GeoSceneMap*      m_map;
    GeoSceneSettings* m_settings;
    GeoSceneLegend*   m_legend;
};


GeoSceneDocument::GeoSceneDocument()
    : GeoDocument(),
      d( new GeoSceneDocumentPrivate )
{
    // Establish connection of property changes to the outside, e.g. the LegendBrowser
    connect ( d->m_settings, SIGNAL(valueChanged(QString,bool)), 
                          SIGNAL(valueChanged(QString,bool)) );
}

GeoSceneDocument::~GeoSceneDocument()
{
    delete d;
}

const char* GeoSceneDocument::nodeType() const
{
    return d->nodeType();
}

const GeoSceneHead* GeoSceneDocument::head() const
{
    return d->m_head;
}

GeoSceneHead* GeoSceneDocument::head()
{
    return d->m_head;
}

const GeoSceneMap* GeoSceneDocument::map() const
{
    return d->m_map;
}

GeoSceneMap* GeoSceneDocument::map()
{
    return d->m_map;
}

const GeoSceneSettings* GeoSceneDocument::settings() const
{
    return d->m_settings;
}

GeoSceneSettings* GeoSceneDocument::settings()
{
    return d->m_settings;
}

const GeoSceneLegend* GeoSceneDocument::legend() const
{
    return d->m_legend;
}

GeoSceneLegend* GeoSceneDocument::legend()
{
    return d->m_legend;
}

}

#include "moc_GeoSceneDocument.cpp"
