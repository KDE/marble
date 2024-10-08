/*
    SPDX-FileCopyrightText: 2007 Murad Tagirov <tmurad@gmail.com>
    SPDX-FileCopyrightText: 2007 Nikolas Zimmermann <zimmermann@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

// Own
#include "GeoSceneDocument.h"

// Marble
#include "GeoSceneHead.h"
#include "GeoSceneLegend.h"
#include "GeoSceneMap.h"
#include "GeoSceneSettings.h"
#include "GeoSceneTypes.h"

namespace Marble
{

class GeoSceneDocumentPrivate
{
public:
    GeoSceneDocumentPrivate()
        : m_head(new GeoSceneHead)
        , m_map(new GeoSceneMap)
        , m_settings(new GeoSceneSettings)
        , m_legend(new GeoSceneLegend)
    {
    }

    ~GeoSceneDocumentPrivate()
    {
        delete m_head;
        delete m_map;
        delete m_settings;
        delete m_legend;
    }

    GeoSceneHead *const m_head;
    GeoSceneMap *const m_map;
    GeoSceneSettings *const m_settings;
    GeoSceneLegend *const m_legend;
};

GeoSceneDocument::GeoSceneDocument()
    : GeoDocument()
    , d(new GeoSceneDocumentPrivate)
{
    // Establish connection of property changes to the outside, e.g. the LegendBrowser
    connect(d->m_settings, SIGNAL(valueChanged(QString, bool)), SIGNAL(valueChanged(QString, bool)));
}

GeoSceneDocument::~GeoSceneDocument()
{
    delete d;
}

const char *GeoSceneDocument::nodeType() const
{
    return GeoSceneTypes::GeoSceneDocumentType;
}

const GeoSceneHead *GeoSceneDocument::head() const
{
    return d->m_head;
}

GeoSceneHead *GeoSceneDocument::head()
{
    return d->m_head;
}

const GeoSceneMap *GeoSceneDocument::map() const
{
    return d->m_map;
}

GeoSceneMap *GeoSceneDocument::map()
{
    return d->m_map;
}

const GeoSceneSettings *GeoSceneDocument::settings() const
{
    return d->m_settings;
}

GeoSceneSettings *GeoSceneDocument::settings()
{
    return d->m_settings;
}

const GeoSceneLegend *GeoSceneDocument::legend() const
{
    return d->m_legend;
}

GeoSceneLegend *GeoSceneDocument::legend()
{
    return d->m_legend;
}

}

#include "moc_GeoSceneDocument.cpp"
