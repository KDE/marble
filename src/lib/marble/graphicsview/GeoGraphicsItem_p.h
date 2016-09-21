//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_GEOGRAPHICSITEMPRIVATE_H
#define MARBLE_GEOGRAPHICSITEMPRIVATE_H

#include "GeoGraphicsItem.h"

// Marble
#include "GeoDataStyle.h"
#include "StyleBuilder.h"

namespace Marble
{

class GeoGraphicsItemPrivate
{
 public:
    explicit GeoGraphicsItemPrivate( const GeoDataFeature *feature)
        : m_zValue( 0 ),
          m_minZoomLevel( 0 ),
          m_feature( feature ),
          m_highlighted( false )
    {
    }

    virtual ~GeoGraphicsItemPrivate()
    {
    }

    qreal m_zValue;
    GeoGraphicsItem::GeoGraphicsItemFlags m_flags;

    int m_minZoomLevel;
    const GeoDataFeature *m_feature;
    RenderContext m_renderContext;
    GeoDataStyle::ConstPtr m_style;
    const StyleBuilder *m_styleBuilder;

    QStringList m_paintLayers;

    // To highlight a placemark
    bool m_highlighted;
    GeoDataStyle::ConstPtr m_highlightStyle;
};

}

#endif
