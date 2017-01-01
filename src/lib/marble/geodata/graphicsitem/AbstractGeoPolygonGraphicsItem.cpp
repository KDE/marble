//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "AbstractGeoPolygonGraphicsItem.h"

#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"
#include "GeoPainter.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataStyle.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLineStyle.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataTypes.h"
#include "OsmPlacemarkData.h"
#include "MarbleDebug.h"
#include "ViewportParams.h"

#include <QtMath>
#include <QImageReader>
#include <QPixmapCache>

namespace Marble
{

const void *AbstractGeoPolygonGraphicsItem::s_previousStyle = 0;

AbstractGeoPolygonGraphicsItem::AbstractGeoPolygonGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataPolygon *polygon) :
    GeoGraphicsItem(placemark),
    m_polygon(polygon),
    m_ring(0)
{
}

AbstractGeoPolygonGraphicsItem::AbstractGeoPolygonGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataLinearRing *ring) :
    GeoGraphicsItem(placemark),
    m_polygon(0),
    m_ring(ring)
{
}

AbstractGeoPolygonGraphicsItem::~AbstractGeoPolygonGraphicsItem()
{
}

const GeoDataLatLonAltBox& AbstractGeoPolygonGraphicsItem::latLonAltBox() const
{
    if( m_polygon ) {
        return m_polygon->latLonAltBox();
    }

    return m_ring->latLonAltBox();
}

void AbstractGeoPolygonGraphicsItem::paint( GeoPainter* painter, const ViewportParams* viewport, const QString &layer, int tileZoomLevel)
{
    Q_UNUSED(layer);
    Q_UNUSED(tileZoomLevel);

    bool isValid = true;
    if (s_previousStyle != style().data()) {
        isValid = configurePainter(painter, viewport);
    }
    s_previousStyle = style().data();

    if (!isValid) return;

    if ( m_polygon ) {
        bool innerResolved = false;

        for(auto const & ring : m_polygon->innerBoundaries()) {
            if (viewport->resolves(ring.latLonAltBox(), 4)) {
               innerResolved = true;
               break;
            }
        }

        if (innerResolved) {
            painter->drawPolygon(*m_polygon);
        }
        else {
            painter->drawPolygon(m_polygon->outerBoundary());
        }
    } else if ( m_ring ) {
        painter->drawPolygon( *m_ring );
    }
}

bool AbstractGeoPolygonGraphicsItem::contains(const QPoint &screenPosition, const ViewportParams *viewport) const
{
    auto const visualCategory = static_cast<const GeoDataPlacemark*>(feature())->visualCategory();
    if (visualCategory == GeoDataPlacemark::Landmass ||
            visualCategory == GeoDataPlacemark::UrbanArea ||
            (visualCategory >= GeoDataPlacemark::LanduseAllotments && visualCategory <= GeoDataPlacemark::LanduseVineyard)) {
        return false;
    }

    double lon, lat;
    viewport->geoCoordinates(screenPosition.x(), screenPosition.y(), lon, lat, GeoDataCoordinates::Radian);
    auto const coordinates = GeoDataCoordinates(lon, lat);
    if (m_polygon) {
        return m_polygon->contains(coordinates);
    } else if (m_ring) {
        return m_ring->contains(coordinates);
    }
    return false;
}

bool AbstractGeoPolygonGraphicsItem::configurePainter(GeoPainter *painter, const ViewportParams *viewport)
{
    QPen currentPen = painter->pen();
    GeoDataStyle::ConstPtr style = this->style();
    if (!style) {
        painter->setPen( QPen() ); // "style-less" polygons: a 1px black solid line
    }
    else {
        const GeoDataPolyStyle& polyStyle = style->polyStyle();

        if (polyStyle.outline()) {
            const GeoDataLineStyle& lineStyle = style->lineStyle();

            // To save performance we avoid making changes to the painter's pen.
            // So we first take a copy of the actual painter pen, make changes to it
            // and only if the resulting pen is different from the actual pen
            // we replace the painter's pen with our new pen.

            // We want to avoid the mandatory detach in QPen::setColor(),
            // so we carefully check whether applying the setter is needed
            currentPen.setColor(lineStyle.paintedColor());
            currentPen.setWidthF(lineStyle.width());
            currentPen.setCapStyle(lineStyle.capStyle());
            currentPen.setStyle(lineStyle.penStyle());

            if (painter->pen().color() != currentPen.color()) {
                painter->setPen(currentPen);
            }
        }
        else {
            // polygons without outline: Qt::NoPen (not drawn)
            if (currentPen.style() != Qt::NoPen) {
                painter->setPen(Qt::NoPen);
            }
        }

        if (!polyStyle.fill()) {            
            painter->setBrush(Qt::transparent);
        }
        else {
            const QColor paintedColor = polyStyle.paintedColor();
            if (painter->brush().color() != paintedColor ||
                painter->brush().style() != polyStyle.brushStyle()) {
                if (!polyStyle.texturePath().isEmpty() || !polyStyle.textureImage().isNull()) {
                    GeoDataCoordinates coords = latLonAltBox().center();
                    qreal x, y;
                    viewport->screenCoordinates(coords, x, y);
                    QBrush brush(texture(polyStyle.texturePath(), paintedColor));
                    painter->setBrush(brush);
                    painter->setBrushOrigin(QPoint(x,y));
                }
                else {
                    painter->setBrush(QBrush(paintedColor, polyStyle.brushStyle()));
                }
            }
        }
    }

    return true;
}

int AbstractGeoPolygonGraphicsItem::extractElevation(const GeoDataPlacemark &placemark)
{
    int elevation = 0;

    const OsmPlacemarkData &osmData = placemark.osmData();

    const auto tagIter = osmData.findTag(QStringLiteral("ele"));
    if (tagIter != osmData.tagsEnd()) {
        elevation = tagIter.value().toInt();
    }

    return elevation;
}

QPixmap AbstractGeoPolygonGraphicsItem::texture(const QString &texturePath, const QColor &color)
{
    QString const key = QString::number(color.rgba()) + '/' + texturePath;
    QPixmap texture;
    if (!QPixmapCache::find(key, texture)) {
        QImageReader imageReader(style()->polyStyle().resolvePath(texturePath));
        texture = QPixmap::fromImageReader(&imageReader);

        if (texture.hasAlphaChannel()) {
            QPixmap pixmap (texture.size());
            pixmap.fill(color);
            QPainter imagePainter(&pixmap);
            imagePainter.drawPixmap(0, 0, texture);
            imagePainter.end();
            texture = pixmap;
        }
        QPixmapCache::insert(key, texture);
    }
    return texture;
}

}
