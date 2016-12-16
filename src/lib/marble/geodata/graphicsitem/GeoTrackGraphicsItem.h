//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Guillaume Martres <smarter@ubuntu.com>
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
