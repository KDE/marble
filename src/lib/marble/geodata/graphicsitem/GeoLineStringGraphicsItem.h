//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#ifndef MARBLE_GEOLINESTRINGGRAPHICSITEM_H
#define MARBLE_GEOLINESTRINGGRAPHICSITEM_H

#include "GeoGraphicsItem.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "MarbleGlobal.h"
#include "marble_export.h"

#include <QRegion>

namespace Marble
{

class GeoDataPlacemark;

class MARBLE_EXPORT GeoLineStringGraphicsItem : public GeoGraphicsItem
{
    Q_DECLARE_TR_FUNCTIONS(GeoLineStringGraphicsItem)

public:
    explicit GeoLineStringGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataLineString *lineString);
    ~GeoLineStringGraphicsItem() override;

    void setLineString( const GeoDataLineString* lineString );
    const GeoDataLineString* lineString() const;
    static GeoDataLineString merge(const QVector<const GeoDataLineString*> &lineStrings);
    void setMergedLineString(const GeoDataLineString &sharedLineString);

    const GeoDataLatLonAltBox& latLonAltBox() const override;

    void paint(GeoPainter* painter, const ViewportParams *viewport, const QString &layer, int tileZoomLevel) override;
    bool contains(const QPoint &screenPosition, const ViewportParams *viewport) const override;

    static const GeoDataStyle *s_previousStyle;
    static bool s_paintInline;
    static bool s_paintOutline;

protected:
    void handleRelationUpdate(const QVector<const GeoDataRelation *> &relations) override;

private:
    void paintOutline(GeoPainter *painter, const ViewportParams *viewport) const;
    void paintInline(GeoPainter *painter, const ViewportParams *viewport);
    void paintLabel(GeoPainter *painter, const ViewportParams *viewport) const;

    bool configurePainterForLine(GeoPainter* painter, const ViewportParams *viewport, const bool isOutline = false) const;
    bool configurePainterForLabel(GeoPainter* painter,  const ViewportParams *viewport, LabelPositionFlags &labelPositionFlags) const;

    static bool canMerge(const GeoDataCoordinates &a, const GeoDataCoordinates &b);

    const GeoDataLineString *m_lineString;
    const GeoDataLineString *m_renderLineString;
    GeoDataLineString m_mergedLineString;
    QVector<QPolygonF*> m_cachedPolygons;
    bool m_renderLabel;
    qreal m_penWidth;
    mutable QRegion m_cachedRegion;
    QString m_name;
};

}

#endif
