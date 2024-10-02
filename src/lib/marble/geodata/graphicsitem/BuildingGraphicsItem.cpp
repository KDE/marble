// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "BuildingGraphicsItem.h"

#include "GeoDataBuilding.h"
#include "GeoDataLinearRing.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataPolygon.h"
#include "GeoPainter.h"
#include "MarbleDebug.h"
#include "OsmPlacemarkData.h"
#include "ViewportParams.h"

#include <QApplication>
#include <QScreen>

namespace Marble
{

BuildingGraphicsItem::BuildingGraphicsItem(const GeoDataPlacemark *placemark, const GeoDataBuilding *building)
    : AbstractGeoPolygonGraphicsItem(placemark, building)
{
    if (const auto ring = geodata_cast<GeoDataLinearRing>(&building->multiGeometry()->at(0))) {
        setLinearRing(ring);
    } else if (const auto poly = geodata_cast<GeoDataPolygon>(&building->multiGeometry()->at(0))) {
        setPolygon(poly);
    }

    setZValue(building->height());
    Q_ASSERT(building->height() > 0.0);

    QStringList paintLayers;
    paintLayers << QStringLiteral("Polygon/Building/frame") << QStringLiteral("Polygon/Building/roof");
    setPaintLayers(paintLayers);
}

BuildingGraphicsItem::~BuildingGraphicsItem()
{
    qDeleteAll(m_cachedOuterPolygons);
    qDeleteAll(m_cachedInnerPolygons);
    qDeleteAll(m_cachedOuterRoofPolygons);
    qDeleteAll(m_cachedInnerRoofPolygons);
}

void BuildingGraphicsItem::initializeBuildingPainting(const GeoPainter *painter,
                                                      const ViewportParams *viewport,
                                                      bool &drawAccurate3D,
                                                      bool &isCameraAboveBuilding) const
{
    drawAccurate3D = false;
    isCameraAboveBuilding = false;

    auto const screen = QApplication::screens().first();
    double const physicalSize = 1.0; // mm
    int const pixelSize = qRound(physicalSize * screen->physicalDotsPerInch() / (IN2M * M2MM));

    QPointF offsetAtCorner = buildingOffset(QPointF(0, 0), viewport, &isCameraAboveBuilding);
    qreal maxOffset = qMax(qAbs(offsetAtCorner.x()), qAbs(offsetAtCorner.y()));
    drawAccurate3D = painter->mapQuality() == HighQuality ? maxOffset > pixelSize : maxOffset > 1.5 * pixelSize;
}

// backface culling for the perspective drawing below assumes all polygons in clockwise order
static void normalizeWindingOrder(QPolygonF *polygon)
{
    double c = 0;
    for (int i = 0; i < polygon->size(); ++i) {
        c += (polygon->at((i + 1) % polygon->size()).x() - polygon->at(i).x()) * (polygon->at(i).y() + polygon->at((i + 1) % polygon->size()).y());
    }
    if (c < 0) {
        std::reverse(polygon->begin(), polygon->end());
    }
}

void BuildingGraphicsItem::updatePolygons(const ViewportParams &viewport,
                                          QList<QPolygonF *> &outerPolygons,
                                          QList<QPolygonF *> &innerPolygons,
                                          bool &hasInnerBoundaries) const
{
    // Since subtracting one fully contained polygon from another results in a single
    // polygon with a "connecting line" between the inner and outer part we need
    // to first paint the inner area with no pen and then the outlines with the correct pen.
    hasInnerBoundaries = polygon() ? !polygon()->innerBoundaries().isEmpty() : false;
    if (polygon()) {
        if (hasInnerBoundaries) {
            screenPolygons(viewport, polygon(), innerPolygons, outerPolygons);
        } else {
            viewport.screenCoordinates(polygon()->outerBoundary(), outerPolygons);
        }
    } else if (ring()) {
        viewport.screenCoordinates(*ring(), outerPolygons);
    }
    for (auto polygon : std::as_const(outerPolygons)) {
        normalizeWindingOrder(polygon);
    }
    for (auto polygon : std::as_const(innerPolygons)) {
        normalizeWindingOrder(polygon);
    }
}

QPointF BuildingGraphicsItem::centroid(const QPolygonF &polygon, double &area)
{
    auto centroid = QPointF(0.0, 0.0);
    area = 0.0;
    for (qsizetype i = 0, n = polygon.size(); i < n; ++i) {
        auto const x0 = polygon[i].x();
        auto const y0 = polygon[i].y();
        auto const j = i == n - 1 ? 0 : i + 1;
        auto const x1 = polygon[j].x();
        auto const y1 = polygon[j].y();
        auto const a = x0 * y1 - x1 * y0;
        area += a;
        centroid.rx() += (x0 + x1) * a;
        centroid.ry() += (y0 + y1) * a;
    }

    area *= 0.5;
    return area != 0 ? centroid / (6.0 * area) : polygon.boundingRect().center();
}

QPointF BuildingGraphicsItem::buildingOffset(const QPointF &point, const ViewportParams *viewport, bool *isCameraAboveBuilding) const
{
    qreal const cameraFactor = 0.5 * tan(0.5 * 110 * DEG2RAD);
    Q_ASSERT(building()->height() > 0.0);
    qreal const buildingFactor = building()->height() / EARTH_RADIUS;

    qreal const cameraHeightPixel = viewport->width() * cameraFactor;
    qreal buildingHeightPixel = viewport->radius() * buildingFactor;
    qreal const cameraDistance = cameraHeightPixel - buildingHeightPixel;

    if (isCameraAboveBuilding) {
        *isCameraAboveBuilding = cameraDistance > 0;
    }

    qreal const cc = cameraDistance * cameraHeightPixel;
    qreal const cb = cameraDistance * buildingHeightPixel;

    // The following lines calculate the same result, but are potentially slower due
    // to using more trigonometric method calls
    // qreal const alpha1 = atan2(offsetX, cameraHeightPixel);
    // qreal const alpha2 = atan2(offsetX, cameraHeightPixel-buildingHeightPixel);
    // qreal const shiftX = 2 * (cameraHeightPixel-buildingHeightPixel) * sin(0.5*(alpha2-alpha1));

    qreal const offsetX = point.x() - viewport->width() / 2.0;
    qreal const offsetY = point.y() - viewport->height() / 2.0;

    qreal const shiftX = offsetX * cb / (cc + offsetX);
    qreal const shiftY = offsetY * cb / (cc + offsetY);

    return {shiftX, shiftY};
}

void BuildingGraphicsItem::paint(GeoPainter *painter, const ViewportParams *viewport, const QString &layer, int tileZoomLevel)
{
    // Just display flat buildings for tile level 17
    if (tileZoomLevel == 17) {
        setZValue(0.0);
        if (layer.endsWith(QLatin1StringView("/roof"))) {
            AbstractGeoPolygonGraphicsItem::paint(painter, viewport, layer, tileZoomLevel);
        }
        return;
    }
    setZValue(building()->height());

    // For level 18, 19 .. render 3D buildings in perspective
    if (layer.endsWith(QLatin1StringView("/frame"))) {
        qDeleteAll(m_cachedOuterPolygons);
        qDeleteAll(m_cachedInnerPolygons);
        qDeleteAll(m_cachedOuterRoofPolygons);
        qDeleteAll(m_cachedInnerRoofPolygons);
        m_cachedOuterPolygons.clear();
        m_cachedInnerPolygons.clear();
        m_cachedOuterRoofPolygons.clear();
        m_cachedInnerRoofPolygons.clear();
        updatePolygons(*viewport, m_cachedOuterPolygons, m_cachedInnerPolygons, m_hasInnerBoundaries);
        if (m_cachedOuterPolygons.isEmpty()) {
            return;
        }
        paintFrame(painter, viewport);
    } else if (layer.endsWith(QLatin1StringView("/roof"))) {
        if (m_cachedOuterPolygons.isEmpty()) {
            return;
        }
        paintRoof(painter, viewport);
    } else {
        mDebug() << "Didn't expect to have to paint layer " << layer << ", ignoring it.";
    }
}

void BuildingGraphicsItem::paintRoof(GeoPainter *painter, const ViewportParams *viewport)
{
    bool drawAccurate3D;
    bool isCameraAboveBuilding;
    initializeBuildingPainting(painter, viewport, drawAccurate3D, isCameraAboveBuilding);
    if (!isCameraAboveBuilding) {
        return; // do not render roof if we look inside the building
    }

    bool isValid = true;
    if (s_previousStyle != style().data()) {
        isValid = configurePainter(painter, *viewport);

        QFont font = painter->font(); // TODO: better font configuration
        if (font.pointSize() != 10) {
            font.setPointSize(10);
            painter->setFont(font);
        }
    }
    s_previousStyle = style().data();

    if (!isValid)
        return;

    // first paint the area (and the outline if there are no inner boundaries)

    if (drawAccurate3D) {
        if (m_hasInnerBoundaries) {
            QPen const currentPen = painter->pen();

            painter->setPen(Qt::NoPen);
            QList<QPolygonF *> fillPolygons = painter->createFillPolygons(m_cachedOuterRoofPolygons, m_cachedInnerRoofPolygons);

            for (const QPolygonF *fillPolygon : std::as_const(fillPolygons)) {
                painter->drawPolygon(*fillPolygon);
            }

            painter->setPen(currentPen);

            for (const QPolygonF *outerRoof : std::as_const(m_cachedOuterRoofPolygons)) {
                painter->drawPolyline(*outerRoof);
            }
            for (const QPolygonF *innerRoof : std::as_const(m_cachedInnerRoofPolygons)) {
                painter->drawPolyline(*innerRoof);
            }
            qDeleteAll(fillPolygons);
        } else {
            for (const QPolygonF *outerRoof : std::as_const(m_cachedOuterRoofPolygons)) {
                painter->drawPolygon(*outerRoof);
            }
        }
    } else {
        QPointF const offset = buildingOffset(m_cachedOuterPolygons[0]->boundingRect().center(), viewport);
        painter->translate(offset);

        if (m_hasInnerBoundaries) {
            QPen const currentPen = painter->pen();

            painter->setPen(Qt::NoPen);
            QList<QPolygonF *> fillPolygons = painter->createFillPolygons(m_cachedOuterPolygons, m_cachedInnerPolygons);

            for (const QPolygonF *fillPolygon : std::as_const(fillPolygons)) {
                painter->drawPolygon(*fillPolygon);
            }

            painter->setPen(currentPen);

            for (const QPolygonF *outerPolygon : std::as_const(m_cachedOuterPolygons)) {
                painter->drawPolyline(*outerPolygon);
            }
            for (const QPolygonF *innerPolygon : std::as_const(m_cachedInnerPolygons)) {
                painter->drawPolyline(*innerPolygon);
            }
            qDeleteAll(fillPolygons);
        } else {
            for (const QPolygonF *outerPolygon : std::as_const(m_cachedOuterPolygons)) {
                painter->drawPolygon(*outerPolygon);
            }
        }
        painter->translate(-offset);
    }

    qreal maxSize(0.0);
    double maxArea = 0.0;

    for (int i = 0; i < m_cachedOuterRoofPolygons.size(); ++i) {
        const QPolygonF *outerRoof = m_cachedOuterRoofPolygons[i];

        QPointF roofCenter;

        // Label position calculation
        if (!building()->name().isEmpty() || !building()->entries().isEmpty()) {
            QSizeF const polygonSize = outerRoof->boundingRect().size();
            qreal size = polygonSize.width() * polygonSize.height();
            if (size > maxSize) {
                maxSize = size;
                double area;
                roofCenter = centroid(*outerRoof, area);
                maxArea = qMax(area, maxArea);
            }
        }

        // Draw the housenumber labels
        if (drawAccurate3D && !building()->name().isEmpty() && !roofCenter.isNull()) {
            double const w2 = 0.5 * painter->fontMetrics().horizontalAdvance(building()->name());
            double const ascent = painter->fontMetrics().ascent();
            double const descent = painter->fontMetrics().descent();
            double const a2 = 0.5 * painter->fontMetrics().ascent();
            QPointF const textPosition = roofCenter - QPointF(w2, -a2);
            if (outerRoof->containsPoint(textPosition + QPointF(-2, -ascent), Qt::OddEvenFill)
                && outerRoof->containsPoint(textPosition + QPointF(-2, descent), Qt::OddEvenFill)
                && outerRoof->containsPoint(textPosition + QPointF(2 + 2 * w2, descent), Qt::OddEvenFill)
                && outerRoof->containsPoint(textPosition + QPointF(2 + 2 * w2, -ascent), Qt::OddEvenFill)) {
                painter->drawTextFragment(roofCenter.toPoint(), building()->name(), painter->font().pointSize(), painter->brush().color());
            }
        }
    }

    // Render additional housenumbers at building entries
    if (!building()->entries().isEmpty() && maxArea > 1600 * building()->entries().size()) {
        for (const auto &entry : building()->entries()) {
            qreal x, y;
            viewport->screenCoordinates(entry.point, x, y);
            QPointF point(x, y);
            point += buildingOffset(point, viewport);
            painter->drawTextFragment(point.toPoint(), building()->name(), painter->font().pointSize(), painter->brush().color(), GeoPainter::RoundFrame);
        }
    }
}

void BuildingGraphicsItem::paintFrame(GeoPainter *painter, const ViewportParams *viewport)
{
    // TODO: how does this match the Q_ASSERT in the constructor?
    if (building()->height() == 0.0) {
        return;
    }

    if ((polygon() && !viewport->resolves(polygon()->outerBoundary().latLonAltBox(), 4)) || (ring() && !viewport->resolves(ring()->latLonAltBox(), 4))) {
        return;
    }

    bool drawAccurate3D;
    bool isCameraAboveBuilding;
    initializeBuildingPainting(painter, viewport, drawAccurate3D, isCameraAboveBuilding);

    bool isValid = true;
    if (s_previousStyle != style().data()) {
        isValid = configurePainterForFrame(painter);
    }
    s_previousStyle = style().data();

    if (!isValid)
        return;

    if (drawAccurate3D && isCameraAboveBuilding) {
        for (const QPolygonF *outline : std::as_const(m_cachedOuterPolygons)) {
            if (outline->isEmpty()) {
                continue;
            }
            // draw the building sides
            int const size = outline->size();
            auto outerRoof = new QPolygonF;
            outerRoof->reserve(outline->size());
            QPointF a = (*outline)[0];
            QPointF shiftA = a + buildingOffset(a, viewport);
            outerRoof->append(shiftA);
            for (int i = 1; i < size; ++i) {
                QPointF const &b = (*outline)[i];
                QPointF const shiftB = b + buildingOffset(b, viewport);
                // perform backface culling
                bool backface = (b.x() - a.x()) * (shiftA.y() - a.y()) - (b.y() - a.y()) * (shiftA.x() - a.x()) >= 0;
                if (!backface) {
                    QPolygonF buildingSide;
                    buildingSide.reserve(4);
                    buildingSide << a << shiftA << shiftB << b;
                    painter->drawPolygon(buildingSide);
                }
                a = b;
                shiftA = shiftB;
                outerRoof->append(shiftA);
            }
            m_cachedOuterRoofPolygons.append(outerRoof);
        }
        for (const QPolygonF *outline : std::as_const(m_cachedInnerPolygons)) {
            if (outline->isEmpty()) {
                continue;
            }
            // draw the building sides
            int const size = outline->size();
            auto innerRoof = new QPolygonF;
            innerRoof->reserve(outline->size());
            QPointF a = (*outline)[0];
            QPointF shiftA = a + buildingOffset(a, viewport);
            innerRoof->append(shiftA);
            for (int i = 1; i < size; ++i) {
                QPointF const &b = (*outline)[i];
                QPointF const shiftB = b + buildingOffset(b, viewport);
                // perform backface culling
                bool backface = (b.x() - a.x()) * (shiftA.y() - a.y()) - (b.y() - a.y()) * (shiftA.x() - a.x()) >= 0;
                if (backface) {
                    QPolygonF buildingSide;
                    buildingSide.reserve(4);
                    buildingSide << a << shiftA << shiftB << b;
                    painter->drawPolygon(buildingSide);
                }
                a = b;
                shiftA = shiftB;
                innerRoof->append(shiftA);
            }
            m_cachedInnerRoofPolygons.append(innerRoof);
        }
    } else {
        // don't draw the building sides - just draw the base frame instead
        QList<QPolygonF *> fillPolygons = painter->createFillPolygons(m_cachedOuterPolygons, m_cachedInnerPolygons);

        for (QPolygonF *fillPolygon : std::as_const(fillPolygons)) {
            painter->drawPolygon(*fillPolygon);
        }
        qDeleteAll(fillPolygons);
    }
}

void BuildingGraphicsItem::screenPolygons(const ViewportParams &viewport,
                                          const GeoDataPolygon *polygon,
                                          QList<QPolygonF *> &innerPolygons,
                                          QList<QPolygonF *> &outerPolygons)
{
    Q_ASSERT(polygon);

    viewport.screenCoordinates(polygon->outerBoundary(), outerPolygons);

    QList<GeoDataLinearRing> const &innerBoundaries = polygon->innerBoundaries();
    for (const GeoDataLinearRing &innerBoundary : innerBoundaries) {
        QList<QPolygonF *> innerPolygonsPerBoundary;
        viewport.screenCoordinates(innerBoundary, innerPolygonsPerBoundary);

        innerPolygons.reserve(innerPolygons.size() + innerPolygonsPerBoundary.size());
        for (QPolygonF *innerPolygonPerBoundary : std::as_const(innerPolygonsPerBoundary)) {
            innerPolygons << innerPolygonPerBoundary;
        }
    }
}

bool BuildingGraphicsItem::contains(const QPoint &screenPosition, const ViewportParams *viewport) const
{
    if (m_cachedOuterPolygons.isEmpty()) {
        // Level 17
        return AbstractGeoPolygonGraphicsItem::contains(screenPosition, viewport);
    }

    QPointF const point = screenPosition;
    for (auto polygon : m_cachedOuterRoofPolygons) {
        if (polygon->containsPoint(point, Qt::OddEvenFill)) {
            for (auto polygon : m_cachedInnerRoofPolygons) {
                if (polygon->containsPoint(point, Qt::OddEvenFill)) {
                    return false;
                }
            }
            return true;
        }
    }
    for (auto polygon : m_cachedOuterPolygons) {
        if (polygon->containsPoint(point, Qt::OddEvenFill)) {
            for (auto polygon : m_cachedInnerPolygons) {
                if (polygon->containsPoint(point, Qt::OddEvenFill)) {
                    return false;
                }
            }
            return true;
        }
    }
    return false;
}

bool BuildingGraphicsItem::configurePainterForFrame(GeoPainter *painter) const
{
    QPen currentPen = painter->pen();

    GeoDataStyle::ConstPtr style = this->style();
    if (!style) {
        painter->setPen(QPen());
    } else {
        const GeoDataPolyStyle &polyStyle = style->polyStyle();

        if (currentPen.style() != Qt::NoPen) {
            painter->setPen(Qt::NoPen);
        }

        if (!polyStyle.fill()) {
            return false;
        } else {
            const QColor paintedColor = polyStyle.paintedColor().darker(150);
            if (painter->brush().color() != paintedColor) {
                painter->setBrush(paintedColor);
            }
        }
    }

    return true;
}

}
