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

#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoPainter.h"
#include "StyleBuilder.h"

namespace Marble
{

GeoPointGraphicsItem::GeoPointGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataPoint *point) :
    GeoGraphicsItem(placemark),
    m_point(point)
{
    if (placemark) {
        QString const paintLayer = QLatin1String("Point/") + StyleBuilder::visualCategoryName(placemark->visualCategory());
        setPaintLayers(QStringList() << paintLayer);
    }
}

void GeoPointGraphicsItem::paint(GeoPainter* painter, const ViewportParams* viewport , const QString &layer)
{
    Q_UNUSED(viewport);
    Q_UNUSED(layer);
    painter->drawPoint(*m_point);
}

const GeoDataLatLonAltBox& GeoPointGraphicsItem::latLonAltBox() const
{
    return m_point->latLonAltBox();
}

}
