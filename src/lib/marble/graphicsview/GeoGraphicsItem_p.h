// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_GEOGRAPHICSITEMPRIVATE_H
#define MARBLE_GEOGRAPHICSITEMPRIVATE_H

#include "GeoGraphicsItem.h"

// Marble
#include "GeoDataStyle.h"
#include "StyleBuilder.h"

#include <QSet>

namespace Marble
{

class GeoGraphicsItemPrivate
{
public:
    explicit GeoGraphicsItemPrivate(const GeoDataFeature *feature)
        : m_zValue(0)
        , m_minZoomLevel(0)
        , m_feature(feature)
        , m_styleBuilder(nullptr)
        , m_highlighted(false)
    {
    }

    virtual ~GeoGraphicsItemPrivate() = default;

    qreal m_zValue;
    GeoGraphicsItem::GeoGraphicsItemFlags m_flags;

    int m_minZoomLevel;
    const GeoDataFeature *m_feature;
    RenderContext m_renderContext;
    GeoDataStyle::ConstPtr m_style;
    const StyleBuilder *m_styleBuilder;
    QList<const GeoDataRelation *> m_relations;

    QStringList m_paintLayers;

    // To highlight a placemark
    bool m_highlighted;
    GeoDataStyle::ConstPtr m_highlightStyle;
};

}

#endif
