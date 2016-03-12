//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "GeoPointGraphicsItem.h"

#include "GeoPainter.h"
#include "GeoDataFeature.h"

namespace Marble
{

GeoPointGraphicsItem::GeoPointGraphicsItem( const GeoDataFeature *feature )
        : GeoGraphicsItem( feature )
{
    if (feature) {
        QString const paintLayer = QString("Point/%1").arg(GeoDataFeature::visualCategoryName(feature->visualCategory()));
        setPaintLayers(QStringList() << paintLayer);
    }
}

void GeoPointGraphicsItem::setPoint( const GeoDataPoint& point )
{
    m_point = point;
}

GeoDataPoint GeoPointGraphicsItem::point() const
{
    return m_point;
}

void GeoPointGraphicsItem::paint(GeoPainter* painter, const ViewportParams* viewport , const QString &layer)
{
    Q_UNUSED(viewport);
    Q_UNUSED(layer);
    painter->drawPoint( m_point );
}

const GeoDataLatLonAltBox& GeoPointGraphicsItem::latLonAltBox() const
{
    return m_point.latLonAltBox();
}

}
