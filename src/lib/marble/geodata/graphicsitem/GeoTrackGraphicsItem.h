// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Guillaume Martres <smarter@ubuntu.com>
//

#ifndef MARBLE_GEOTRACKGRAPHICSITEM_H
#define MARBLE_GEOTRACKGRAPHICSITEM_H

#include "GeoLineStringGraphicsItem.h"

namespace Marble
{

class GeoDataPlacemark;
class GeoDataTrack;

class MARBLE_EXPORT GeoTrackGraphicsItem : public GeoLineStringGraphicsItem
{
public:
    explicit GeoTrackGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataTrack *track);

    void paint(GeoPainter *painter, const ViewportParams *viewport, const QString &layer, int tileZoomLevel) override;

private:
    void setTrack(const GeoDataTrack *track);

    const GeoDataTrack *m_track;
    void update();
};

}

#endif // MARBLE_GEOTRACKGRAPHICSITEM_H
