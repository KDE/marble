// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Guillaume Martres <smarter@ubuntu.com>
//

#include "GeoTrackGraphicsItem.h"

#include "GeoDataPlacemark.h"
#include "GeoDataTrack.h"
#include "StyleBuilder.h"

using namespace Marble;

GeoTrackGraphicsItem::GeoTrackGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataTrack *track)
    : GeoLineStringGraphicsItem(placemark, track->lineString())
{
    setTrack(track);
    if (placemark) {
        QString const paintLayer = QLatin1StringView("Track/") + StyleBuilder::visualCategoryName(placemark->visualCategory());
        setPaintLayers(QStringList() << paintLayer);
    }
}

void GeoTrackGraphicsItem::setTrack(const GeoDataTrack *track)
{
    m_track = track;
    update();
}

void GeoTrackGraphicsItem::paint(GeoPainter *painter, const ViewportParams *viewport, const QString &layer, int tileZoomLevel)
{
    Q_UNUSED(layer);
    Q_UNUSED(tileZoomLevel);
    update();
    GeoLineStringGraphicsItem::paint(painter, viewport, layer, tileZoomLevel);
}

void GeoTrackGraphicsItem::update()
{
    setLineString(m_track->lineString());
}
