//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#ifndef MARBLE_ABSTRACTGEOPOLYGONGRAPHICSITEM_H
#define MARBLE_ABSTRACTGEOPOLYGONGRAPHICSITEM_H

#include "GeoGraphicsItem.h"
#include "marble_export.h"

#include <QImage>
#include <QColor>
#include <QPixmapCache>

namespace Marble
{

class GeoDataLinearRing;
class GeoDataPlacemark;
class GeoDataPolygon;

class MARBLE_EXPORT AbstractGeoPolygonGraphicsItem : public GeoGraphicsItem
{
protected:
    explicit AbstractGeoPolygonGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataPolygon *polygon);
    explicit AbstractGeoPolygonGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataLinearRing *ring);

public:
    virtual const GeoDataLatLonAltBox& latLonAltBox() const;
    virtual void paint(GeoPainter* painter, const ViewportParams *viewport, const QString &layer);

protected:
    QPen configurePainter(GeoPainter* painter, const ViewportParams *viewport);
    inline
    const GeoDataPolygon *polygon() const { return m_polygon; }
    inline
    const GeoDataLinearRing *ring() const { return m_ring; }

    static int extractElevation(const GeoDataPlacemark &placemark);

private:
    QPixmap texture(const QString &path, const QImage &textureImage, const QColor &color);

    const GeoDataPolygon *const m_polygon;
    const GeoDataLinearRing *const m_ring;
    static QPixmapCache m_textureCache;
};

}

#endif
