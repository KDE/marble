// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2009 Torsten Rahn <tackat@kde.org>

#include "GeoPainter.h"
#include "GeoPainter_p.h"

#include <QList>
#include <QPainterPath>
#include <QPixmapCache>
#include <QRegion>
#include <qmath.h>

#include "MarbleDebug.h"

#include "GeoDataCoordinates.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"

#include "AbstractProjection.h"
#include "ViewportParams.h"

// #define MARBLE_DEBUG

using namespace Marble;

GeoPainterPrivate::GeoPainterPrivate(GeoPainter *q, const ViewportParams *viewport, MapQuality mapQuality)
    : m_viewport(viewport)
    , m_mapQuality(mapQuality)
    , m_x(new qreal[100])
    , m_parent(q)
{
}

GeoPainterPrivate::~GeoPainterPrivate()
{
    delete[] m_x;
}

void GeoPainterPrivate::createAnnotationLayout(qreal x,
                                               qreal y,
                                               const QSizeF &bubbleSize,
                                               qreal bubbleOffsetX,
                                               qreal bubbleOffsetY,
                                               qreal xRnd,
                                               qreal yRnd,
                                               QPainterPath &path,
                                               QRectF &rect)
{
    // TODO: MOVE this into an own Annotation class
    qreal arrowPosition = 0.3;
    qreal arrowWidth = 12.0;

    qreal width = bubbleSize.width();
    qreal height = bubbleSize.height();

    qreal dx = (bubbleOffsetX > 0) ? 1.0 : -1.0; // x-Mirror
    qreal dy = (bubbleOffsetY < 0) ? 1.0 : -1.0; // y-Mirror

    qreal x0 = (x + bubbleOffsetX) - dx * (1.0 - arrowPosition) * (width - 2.0 * xRnd) - xRnd * dx;
    qreal x1 = (x + bubbleOffsetX) - dx * (1.0 - arrowPosition) * (width - 2.0 * xRnd);
    qreal x2 = (x + bubbleOffsetX) - dx * (1.0 - arrowPosition) * (width - 2.0 * xRnd) + xRnd * dx;
    qreal x3 = (x + bubbleOffsetX) - dx * arrowWidth / 2.0;
    qreal x4 = (x + bubbleOffsetX) + dx * arrowWidth / 2.0;
    qreal x5 = (x + bubbleOffsetX) + dx * arrowPosition * (width - 2.0 * xRnd) - xRnd * dx;
    qreal x6 = (x + bubbleOffsetX) + dx * arrowPosition * (width - 2.0 * xRnd);
    qreal x7 = (x + bubbleOffsetX) + dx * arrowPosition * (width - 2.0 * xRnd) + xRnd * dx;

    qreal y0 = (y + bubbleOffsetY);
    qreal y1 = (y + bubbleOffsetY) - dy * yRnd;
    qreal y2 = (y + bubbleOffsetY) - dy * 2 * yRnd;
    qreal y5 = (y + bubbleOffsetY) - dy * (height - 2 * yRnd);
    qreal y6 = (y + bubbleOffsetY) - dy * (height - yRnd);
    qreal y7 = (y + bubbleOffsetY) - dy * height;

    QPointF p1(x, y); // pointing point
    QPointF p2(x4, y0);
    QPointF p3(x6, y0);
    QPointF p4(x7, y1);
    QPointF p5(x7, y6);
    QPointF p6(x6, y7);
    QPointF p7(x1, y7);
    QPointF p8(x0, y6);
    QPointF p9(x0, y1);
    QPointF p10(x1, y0);
    QPointF p11(x3, y0);

    QRectF bubbleBoundingBox(QPointF(x0, y7), QPointF(x7, y0));

    path.moveTo(p1);
    path.lineTo(p2);

    path.lineTo(p3);
    QRectF bottomRight(QPointF(x5, y2), QPointF(x7, y0));
    path.arcTo(bottomRight, 270.0, 90.0);

    path.lineTo(p5);
    QRectF topRight(QPointF(x5, y7), QPointF(x7, y5));
    path.arcTo(topRight, 0.0, 90.0);

    path.lineTo(p7);
    QRectF topLeft(QPointF(x0, y7), QPointF(x2, y5));
    path.arcTo(topLeft, 90.0, 90.0);

    path.lineTo(p9);
    QRectF bottomLeft(QPointF(x0, y2), QPointF(x2, y0));
    path.arcTo(bottomLeft, 180.0, 90.0);

    path.lineTo(p10);
    path.lineTo(p11);
    path.lineTo(p1);

    qreal left = (dx > 0) ? x1 : x6;
    qreal right = (dx > 0) ? x6 : x1;
    qreal top = (dy > 0) ? y6 : y1;
    qreal bottom = (dy > 0) ? y1 : y6;

    rect.setTopLeft(QPointF(left, top));
    rect.setBottomRight(QPointF(right, bottom));
}

GeoDataLinearRing GeoPainterPrivate::createLinearRingFromGeoRect(const GeoDataCoordinates &centerCoordinates, qreal width, qreal height)
{
    qreal lon = 0.0;
    qreal lat = 0.0;
    qreal altitude = centerCoordinates.altitude();
    centerCoordinates.geoCoordinates(lon, lat, GeoDataCoordinates::Degree);

    lon = GeoDataCoordinates::normalizeLon(lon, GeoDataCoordinates::Degree);
    lat = GeoDataCoordinates::normalizeLat(lat, GeoDataCoordinates::Degree);

    qreal west = GeoDataCoordinates::normalizeLon(lon - width * 0.5, GeoDataCoordinates::Degree);
    qreal east = GeoDataCoordinates::normalizeLon(lon + width * 0.5, GeoDataCoordinates::Degree);

    qreal north = GeoDataCoordinates::normalizeLat(lat + height * 0.5, GeoDataCoordinates::Degree);
    qreal south = GeoDataCoordinates::normalizeLat(lat - height * 0.5, GeoDataCoordinates::Degree);

    GeoDataCoordinates southWest(west, south, altitude, GeoDataCoordinates::Degree);
    GeoDataCoordinates southEast(east, south, altitude, GeoDataCoordinates::Degree);
    GeoDataCoordinates northEast(east, north, altitude, GeoDataCoordinates::Degree);
    GeoDataCoordinates northWest(west, north, altitude, GeoDataCoordinates::Degree);

    GeoDataLinearRing rectangle(Tessellate | RespectLatitudeCircle);

    // If the width of the rect is larger as 180 degree, we have to enforce the long way.
    if (width >= 180) {
        qreal center = lon;
        GeoDataCoordinates southCenter(center, south, altitude, GeoDataCoordinates::Degree);
        GeoDataCoordinates northCenter(center, north, altitude, GeoDataCoordinates::Degree);

        rectangle << southWest << southCenter << southEast << northEast << northCenter << northWest;
    } else {
        rectangle << southWest << southEast << northEast << northWest;
    }

    return rectangle;
}

bool GeoPainterPrivate::doClip(const ViewportParams *viewport)
{
    if (!viewport->currentProjection()->isClippedToSphere())
        return true;

    const qint64 radius = viewport->radius() * viewport->currentProjection()->clippingRadius();

    return (radius > viewport->width() / 2 || radius > viewport->height() / 2);
}

qreal GeoPainterPrivate::normalizeAngle(qreal angle)
{
    angle = fmodf(angle, 360);
    return angle < 0 ? angle + 360 : angle;
}

void GeoPainterPrivate::drawTextRotated(const QPointF &startPoint, qreal angle, const QString &text)
{
    QRectF textRect(startPoint, m_parent->fontMetrics().size(0, text));
    QTransform const oldTransform = m_parent->transform();
    m_parent->translate(startPoint);
    m_parent->rotate(angle);
    m_parent->translate(-startPoint - QPointF(0.0, m_parent->fontMetrics().height() / 2.0));

    m_parent->drawText(textRect, text);
    m_parent->setTransform(oldTransform);
}

// -------------------------------------------------------------------------------------------------

GeoPainter::GeoPainter(QPaintDevice *pd, const ViewportParams *viewport, MapQuality mapQuality)
    : ClipPainter(pd, GeoPainterPrivate::doClip(viewport))
    , d(new GeoPainterPrivate(this, viewport, mapQuality))
{
    const bool antialiased = mapQuality == HighQuality || mapQuality == PrintQuality;
    setRenderHint(QPainter::Antialiasing, antialiased);
    ClipPainter::setScreenClip(false);
}

GeoPainter::~GeoPainter()
{
    delete d;
}

MapQuality GeoPainter::mapQuality() const
{
    return d->m_mapQuality;
}

void GeoPainter::drawAnnotation(const GeoDataCoordinates &position,
                                const QString &text,
                                QSizeF bubbleSize,
                                qreal bubbleOffsetX,
                                qreal bubbleOffsetY,
                                qreal xRnd,
                                qreal yRnd)
{
    int pointRepeatNum;
    qreal y;
    bool globeHidesPoint;

    if (bubbleSize.height() <= 0) {
        QRectF rect = QRectF(QPointF(0.0, 0.0), bubbleSize - QSizeF(2 * xRnd, 0.0));
        qreal idealTextHeight = boundingRect(rect, Qt::TextWordWrap, text).height();
        bubbleSize.setHeight(2 * yRnd + idealTextHeight);
    }

    bool visible = d->m_viewport->screenCoordinates(position, d->m_x, y, pointRepeatNum, QSizeF(), globeHidesPoint);

    if (visible) {
        // Draw all the x-repeat-instances of the point on the screen
        for (int it = 0; it < pointRepeatNum; ++it) {
            QPainterPath path;
            QRectF rect;
            d->createAnnotationLayout(d->m_x[it], y, bubbleSize, bubbleOffsetX, bubbleOffsetY, xRnd, yRnd, path, rect);
            QPainter::drawPath(path);
            QPainter::drawText(rect, Qt::TextWordWrap, text, &rect);
        }
    }
}

void GeoPainter::drawPoint(const GeoDataCoordinates &position)
{
    int pointRepeatNum;
    qreal y;
    bool globeHidesPoint;

    bool visible = d->m_viewport->screenCoordinates(position, d->m_x, y, pointRepeatNum, QSizeF(), globeHidesPoint);

    if (visible) {
        // Draw all the x-repeat-instances of the point on the screen
        for (int it = 0; it < pointRepeatNum; ++it) {
            QPainter::drawPoint(QPointF(d->m_x[it], y));
        }
    }
}

QRegion GeoPainter::regionFromPoint(const GeoDataCoordinates &position, qreal width) const
{
    return regionFromRect(position, width, width, false, 3);
}

void GeoPainter::drawPoint(const GeoDataPoint &point)
{
    drawPoint(point.coordinates());
}

QRegion GeoPainter::regionFromPoint(const GeoDataPoint &point, qreal width) const
{
    return regionFromRect(point.coordinates(), width, width, false, 3);
}

void GeoPainter::drawText(const GeoDataCoordinates &position,
                          const QString &text,
                          qreal xOffset,
                          qreal yOffset,
                          qreal width,
                          qreal height,
                          const QTextOption &option)
{
    // Of course in theory we could have the "isGeoProjected" parameter used
    // for drawText as well. However this would require us to convert all
    // glyphs to PainterPaths / QPolygons. From QPolygons we could create
    // GeoDataPolygons which could get painted on screen. Any patches appreciated ;-)

    int pointRepeatNum;
    qreal y;
    bool globeHidesPoint;

    QSizeF textSize(fontMetrics().horizontalAdvance(text), fontMetrics().height());

    bool visible = d->m_viewport->screenCoordinates(position, d->m_x, y, pointRepeatNum, textSize, globeHidesPoint);

    if (visible) {
        // Draw all the x-repeat-instances of the point on the screen
        const qreal posY = y - yOffset;
        for (int it = 0; it < pointRepeatNum; ++it) {
            const qreal posX = d->m_x[it] + xOffset;
            if (width == 0.0 && height == 0.0) {
                QPainter::drawText(QPointF(posX, posY), text);
            } else {
                const QRectF boundingRect(posX, posY, width, height);
                QPainter::drawText(boundingRect, text, option);
            }
        }
    }
}

void GeoPainter::drawEllipse(const GeoDataCoordinates &centerPosition, qreal width, qreal height, bool isGeoProjected)
{
    if (!isGeoProjected) {
        int pointRepeatNum;
        qreal y;
        bool globeHidesPoint;

        bool visible = d->m_viewport->screenCoordinates(centerPosition, d->m_x, y, pointRepeatNum, QSizeF(width, height), globeHidesPoint);

        if (visible) {
            // Draw all the x-repeat-instances of the point on the screen
            const qreal rx = width / 2.0;
            const qreal ry = height / 2.0;
            for (int it = 0; it < pointRepeatNum; ++it) {
                QPainter::drawEllipse(QPointF(d->m_x[it], y), rx, ry);
            }
        }
    } else {
        // Initialize variables
        const qreal centerLon = centerPosition.longitude(GeoDataCoordinates::Degree);
        const qreal centerLat = centerPosition.latitude(GeoDataCoordinates::Degree);
        const qreal altitude = centerPosition.altitude();

        // Ensure a valid latitude range:
        if (centerLat + 0.5 * height > 90.0 || centerLat - 0.5 * height < -90.0) {
            return;
        }

        // Don't show the ellipse if it's too small:
        GeoDataLatLonBox ellipseBox(centerLat + 0.5 * height,
                                    centerLat - 0.5 * height,
                                    centerLon + 0.5 * width,
                                    centerLon - 0.5 * width,
                                    GeoDataCoordinates::Degree);
        if (!d->m_viewport->viewLatLonAltBox().intersects(ellipseBox) || !d->m_viewport->resolves(ellipseBox))
            return;

        GeoDataLinearRing ellipse;

        // Optimizing the precision by determining the size which the
        // ellipse covers on the screen:
        const qreal degreeResolution = d->m_viewport->angularResolution() * RAD2DEG;
        // To create a circle shape even for very small precision we require uneven numbers:
        const int precision = qMin<qreal>(width / degreeResolution / 8 + 1, 81);

        // Calculate the shape of the upper half of the ellipse:
        for (int i = 0; i <= precision; ++i) {
            const qreal t = 1.0 - 2.0 * (qreal)(i) / (qreal)(precision);
            const qreal lat = centerLat + 0.5 * height * sqrt(1.0 - t * t);
            const qreal lon = centerLon + 0.5 * width * t;
            ellipse << GeoDataCoordinates(lon, lat, altitude, GeoDataCoordinates::Degree);
        }
        // Calculate the shape of the lower half of the ellipse:
        for (int i = 0; i <= precision; ++i) {
            const qreal t = 2.0 * (qreal)(i) / (qreal)(precision)-1.0;
            const qreal lat = centerLat - 0.5 * height * sqrt(1.0 - t * t);
            const qreal lon = centerLon + 0.5 * width * t;
            ellipse << GeoDataCoordinates(lon, lat, altitude, GeoDataCoordinates::Degree);
        }

        drawPolygon(ellipse);
    }
}

QRegion GeoPainter::regionFromEllipse(const GeoDataCoordinates &centerPosition, qreal width, qreal height, bool isGeoProjected, qreal strokeWidth) const
{
    if (!isGeoProjected) {
        int pointRepeatNum;
        qreal y;
        bool globeHidesPoint;

        bool visible = d->m_viewport->screenCoordinates(centerPosition, d->m_x, y, pointRepeatNum, QSizeF(width, height), globeHidesPoint);

        QRegion regions;

        if (visible) {
            // only a hint, a backend could still ignore it, but we cannot know more
            const bool antialiased = testRenderHint(QPainter::Antialiasing);

            const qreal halfStrokeWidth = strokeWidth / 2.0;
            const int startY = antialiased ? (qFloor(y - halfStrokeWidth)) : (qFloor(y + 0.5 - halfStrokeWidth));
            const int endY = antialiased ? (qCeil(y + height + halfStrokeWidth)) : (qFloor(y + 0.5 + height + halfStrokeWidth));
            // Draw all the x-repeat-instances of the point on the screen
            for (int it = 0; it < pointRepeatNum; ++it) {
                const qreal x = d->m_x[it];
                const int startX = antialiased ? (qFloor(x - halfStrokeWidth)) : (qFloor(x + 0.5 - halfStrokeWidth));
                const int endX = antialiased ? (qCeil(x + width + halfStrokeWidth)) : (qFloor(x + 0.5 + width + halfStrokeWidth));

                regions += QRegion(startX, startY, endX - startX, endY - startY, QRegion::Ellipse);
            }
        }
        return regions;
    } else {
        // Initialize variables
        const qreal centerLon = centerPosition.longitude(GeoDataCoordinates::Degree);
        const qreal centerLat = centerPosition.latitude(GeoDataCoordinates::Degree);
        const qreal altitude = centerPosition.altitude();

        // Ensure a valid latitude range:
        if (centerLat + 0.5 * height > 90.0 || centerLat - 0.5 * height < -90.0) {
            return {};
        }

        // Don't show the ellipse if it's too small:
        GeoDataLatLonBox ellipseBox(centerLat + 0.5 * height,
                                    centerLat - 0.5 * height,
                                    centerLon + 0.5 * width,
                                    centerLon - 0.5 * width,
                                    GeoDataCoordinates::Degree);
        if (!d->m_viewport->viewLatLonAltBox().intersects(ellipseBox) || !d->m_viewport->resolves(ellipseBox))
            return {};

        GeoDataLinearRing ellipse;

        // Optimizing the precision by determining the size which the
        // ellipse covers on the screen:
        const qreal degreeResolution = d->m_viewport->angularResolution() * RAD2DEG;
        // To create a circle shape even for very small precision we require uneven numbers:
        const int precision = qMin<qreal>(width / degreeResolution / 8 + 1, 81);

        // Calculate the shape of the upper half of the ellipse:
        for (int i = 0; i <= precision; ++i) {
            const qreal t = 1.0 - 2.0 * (qreal)(i) / (qreal)(precision);
            const qreal lat = centerLat + 0.5 * height * sqrt(1.0 - t * t);
            const qreal lon = centerLon + 0.5 * width * t;
            ellipse << GeoDataCoordinates(lon, lat, altitude, GeoDataCoordinates::Degree);
        }
        // Calculate the shape of the lower half of the ellipse:
        for (int i = 0; i <= precision; ++i) {
            const qreal t = 2.0 * (qreal)(i) / (qreal)(precision)-1.0;
            const qreal lat = centerLat - 0.5 * height * sqrt(1.0 - t * t);
            const qreal lon = centerLon + 0.5 * width * t;
            ellipse << GeoDataCoordinates(lon, lat, altitude, GeoDataCoordinates::Degree);
        }

        return regionFromPolygon(ellipse, Qt::OddEvenFill, strokeWidth);
    }
}

void GeoPainter::drawImage(const GeoDataCoordinates &centerPosition, const QImage &image /*, bool isGeoProjected */)
{
    // isGeoProjected = true would project the image/pixmap onto the globe. This
    // requires to deal with the TextureMapping classes -> should get
    // implemented later on

    int pointRepeatNum;
    qreal y;
    bool globeHidesPoint;

    //    if ( !isGeoProjected ) {
    bool visible = d->m_viewport->screenCoordinates(centerPosition, d->m_x, y, pointRepeatNum, image.size(), globeHidesPoint);

    if (visible) {
        // Draw all the x-repeat-instances of the point on the screen
        const qreal posY = y - (image.height() / 2.0);
        for (int it = 0; it < pointRepeatNum; ++it) {
            const qreal posX = d->m_x[it] - (image.width() / 2.0);
            QPainter::drawImage(QPointF(posX, posY), image);
        }
    }
    //    }
}

void GeoPainter::drawPixmap(const GeoDataCoordinates &centerPosition, const QPixmap &pixmap /* , bool isGeoProjected */)
{
    int pointRepeatNum;
    qreal y;
    bool globeHidesPoint;

    //    if ( !isGeoProjected ) {
    // FIXME: Better visibility detection that takes the circle geometry into account
    bool visible = d->m_viewport->screenCoordinates(centerPosition, d->m_x, y, pointRepeatNum, pixmap.size(), globeHidesPoint);

    if (visible) {
        // Draw all the x-repeat-instances of the point on the screen
        const qreal posY = y - (pixmap.height() / 2.0);
        for (int it = 0; it < pointRepeatNum; ++it) {
            const qreal posX = d->m_x[it] - (pixmap.width() / 2.0);
            QPainter::drawPixmap(QPointF(posX, posY), pixmap);
        }
    }
    //    }
}

QRegion GeoPainter::regionFromPixmapRect(const GeoDataCoordinates &centerCoordinates, int width, int height, int margin) const
{
    const int fullWidth = width + 2 * margin;
    const int fullHeight = height + 2 * margin;
    int pointRepeatNum;
    qreal y;
    bool globeHidesPoint;

    const bool visible = d->m_viewport->screenCoordinates(centerCoordinates, d->m_x, y, pointRepeatNum, QSizeF(fullWidth, fullHeight), globeHidesPoint);

    QRegion regions;

    if (visible) {
        // cmp. GeoPainter::drawPixmap() position calculation
        // QPainter::drawPixmap seems to qRound the passed position
        const int posY = qRound(y - (height / 2.0)) - margin;
        for (int it = 0; it < pointRepeatNum; ++it) {
            const int posX = qRound(d->m_x[it] - (width / 2.0)) - margin;
            regions += QRegion(posX, posY, width, height);
        }
    }

    return regions;
}

void GeoPainter::polygonsFromLineString(const GeoDataLineString &lineString, QList<QPolygonF *> &polygons) const
{
    // Immediately leave this method now if:
    // - the object is not visible in the viewport or if
    // - the size of the object is below the resolution of the viewport
    if (!d->m_viewport->viewLatLonAltBox().intersects(lineString.latLonAltBox()) || !d->m_viewport->resolves(lineString.latLonAltBox())) {
        // mDebug() << "LineString doesn't get displayed on the viewport";
        return;
    }

    d->m_viewport->screenCoordinates(lineString, polygons);
}

void GeoPainter::drawPolyline(const GeoDataLineString &lineString, const QString &labelText, LabelPositionFlags labelPositionFlags, const QColor &labelColor)
{
    // no labels to draw?
    // TODO: !labelColor.isValid() || labelColor.alpha() == 0 does not work,
    // something injects invalid labelColor for city streets
    if (labelText.isEmpty() || labelPositionFlags.testFlag(NoLabel) || labelColor == Qt::transparent) {
        drawPolyline(lineString);
        return;
    }

    QList<QPolygonF *> polygons;
    polygonsFromLineString(lineString, polygons);
    if (polygons.empty())
        return;

    for (const QPolygonF *itPolygon : std::as_const(polygons)) {
        ClipPainter::drawPolyline(*itPolygon);
    }

    drawLabelsForPolygons(polygons, labelText, labelPositionFlags, labelColor);

    qDeleteAll(polygons);
}

void GeoPainter::drawLabelsForPolygons(const QList<QPolygonF *> &polygons,
                                       const QString &labelText,
                                       LabelPositionFlags labelPositionFlags,
                                       const QColor &labelColor)
{
    if (labelText.isEmpty()) {
        return;
    }
    QPen const oldPen = pen();

    if (labelPositionFlags.testFlag(FollowLine)) {
        const qreal maximumLabelFontSize = 20;
        qreal fontSize = pen().widthF() * 0.45;
        fontSize = qMin(fontSize, maximumLabelFontSize);

        if (fontSize < 6.0 || labelColor == "transparent") {
            return;
        }
        QFont font = this->font();
        font.setPointSizeF(fontSize);
        setFont(font);
        int labelWidth = fontMetrics().horizontalAdvance(labelText);
        if (labelText.size() < 20) {
            labelWidth *= (20.0 / labelText.size());
        }
        setPen(labelColor);

        QList<QPointF> labelNodes;
        QRectF viewportRect = QRectF(QPointF(0, 0), d->m_viewport->size());
        for (QPolygonF *itPolygon : polygons) {
            if (!itPolygon->boundingRect().intersects(viewportRect)) {
                continue;
            }

            labelNodes.clear();

            QPainterPath path;
            path.addPolygon(*itPolygon);
            qreal pathLength = path.length();
            if (pathLength == 0)
                continue;

            int maxNumLabels = static_cast<int>(pathLength / labelWidth);

            if (maxNumLabels > 0) {
                qreal textRelativeLength = labelWidth / pathLength;
                int numLabels = 1;
                if (maxNumLabels > 1) {
                    numLabels = maxNumLabels / 2;
                }
                qreal offset = (1.0 - numLabels * textRelativeLength) / numLabels;
                qreal startPercent = offset / 2.0;

                for (int k = 0; k < numLabels; ++k, startPercent += textRelativeLength + offset) {
                    QPointF point = path.pointAtPercent(startPercent);
                    QPointF endPoint = path.pointAtPercent(startPercent + textRelativeLength);

                    if (viewport().contains(point.toPoint()) || viewport().contains(endPoint.toPoint())) {
                        qreal angle = -path.angleAtPercent(startPercent);
                        qreal angle2 = -path.angleAtPercent(startPercent + textRelativeLength);
                        angle = GeoPainterPrivate::normalizeAngle(angle);
                        angle2 = GeoPainterPrivate::normalizeAngle(angle2);
                        bool upsideDown = angle > 90.0 && angle < 270.0;

                        if (qAbs(angle - angle2) < 3.0) {
                            if (upsideDown) {
                                angle += 180.0;
                                point = path.pointAtPercent(startPercent + textRelativeLength);
                            }

                            d->drawTextRotated(point, angle, labelText);
                        } else {
                            for (int i = 0; i < labelText.length(); ++i) {
                                qreal currentGlyphTextLength = fontMetrics().horizontalAdvance(labelText.left(i)) / pathLength;

                                if (!upsideDown) {
                                    angle = -path.angleAtPercent(startPercent + currentGlyphTextLength);
                                    point = path.pointAtPercent(startPercent + currentGlyphTextLength);
                                } else {
                                    angle = -path.angleAtPercent(startPercent + textRelativeLength - currentGlyphTextLength) + 180;
                                    point = path.pointAtPercent(startPercent + textRelativeLength - currentGlyphTextLength);
                                }

                                d->drawTextRotated(point, angle, labelText.at(i));
                            }
                        }
                    }
                }
            }
        }
    } else {
        setPen(labelColor);

        int labelWidth = fontMetrics().horizontalAdvance(labelText);
        int labelAscent = fontMetrics().ascent();

        QList<QPointF> labelNodes;
        for (QPolygonF *itPolygon : polygons) {
            labelNodes.clear();
            ClipPainter::labelPosition(*itPolygon, labelNodes, labelPositionFlags);
            if (!labelNodes.isEmpty()) {
                for (const QPointF &labelNode : std::as_const(labelNodes)) {
                    QPointF labelPosition = labelNode + QPointF(3.0, -2.0);

                    // FIXME: This is a Q&D fix.
                    qreal xmax = viewport().width() - 10.0 - labelWidth;
                    if (labelPosition.x() > xmax)
                        labelPosition.setX(xmax);
                    qreal ymin = 10.0 + labelAscent;
                    if (labelPosition.y() < ymin)
                        labelPosition.setY(ymin);
                    qreal ymax = viewport().height() - 10.0 - labelAscent;
                    if (labelPosition.y() > ymax)
                        labelPosition.setY(ymax);

                    drawText(QRectF(labelPosition, fontMetrics().size(0, labelText)), labelText);
                }
            }
        }
    }
    setPen(oldPen);
}

void GeoPainter::drawPolyline(const GeoDataLineString &lineString)
{
    QList<QPolygonF *> polygons;
    polygonsFromLineString(lineString, polygons);
    if (polygons.empty())
        return;

    for (const QPolygonF *itPolygon : std::as_const(polygons)) {
        ClipPainter::drawPolyline(*itPolygon);
    }

    qDeleteAll(polygons);
}

QRegion GeoPainter::regionFromPolyline(const GeoDataLineString &lineString, qreal strokeWidth) const
{
    // Immediately leave this method now if:
    // - the object is not visible in the viewport or if
    // - the size of the object is below the resolution of the viewport
    if (!d->m_viewport->viewLatLonAltBox().intersects(lineString.latLonAltBox()) || !d->m_viewport->resolves(lineString.latLonAltBox())) {
        // mDebug() << "LineString doesn't get displayed on the viewport";
        return {};
    }

    QPainterPath painterPath;

    QList<QPolygonF *> polygons;
    d->m_viewport->screenCoordinates(lineString, polygons);

    for (QPolygonF *itPolygon : std::as_const(polygons)) {
        painterPath.addPolygon(*itPolygon);
    }

    qDeleteAll(polygons);

    QPainterPathStroker stroker;
    stroker.setWidth(strokeWidth);
    QPainterPath strokePath = stroker.createStroke(painterPath);

    return QRegion(strokePath.toFillPolygon().toPolygon(), Qt::WindingFill);
}

void GeoPainter::drawPolygon(const GeoDataLinearRing &linearRing, Qt::FillRule fillRule)
{
    // Immediately leave this method now if:
    // - the object is not visible in the viewport or if
    // - the size of the object is below the resolution of the viewport
    if (!d->m_viewport->viewLatLonAltBox().intersects(linearRing.latLonAltBox()) || !d->m_viewport->resolves(linearRing.latLonAltBox())) {
        // mDebug() << "Polygon doesn't get displayed on the viewport";
        return;
    }

    QList<QPolygonF *> polygons;
    d->m_viewport->screenCoordinates(linearRing, polygons);

    for (QPolygonF *itPolygon : std::as_const(polygons)) {
        ClipPainter::drawPolygon(*itPolygon, fillRule);
    }

    qDeleteAll(polygons);
}

QRegion GeoPainter::regionFromPolygon(const GeoDataLinearRing &linearRing, Qt::FillRule fillRule, qreal strokeWidth) const
{
    // Immediately leave this method now if:
    // - the object is not visible in the viewport or if
    // - the size of the object is below the resolution of the viewport
    if (!d->m_viewport->viewLatLonAltBox().intersects(linearRing.latLonAltBox()) || !d->m_viewport->resolves(linearRing.latLonAltBox())) {
        return {};
    }

    QRegion regions;

    QList<QPolygonF *> polygons;
    d->m_viewport->screenCoordinates(linearRing, polygons);

    if (strokeWidth == 0) {
        // This is the faster way
        for (QPolygonF *itPolygon : std::as_const(polygons)) {
            regions += QRegion((*itPolygon).toPolygon(), fillRule);
        }
    } else {
        QPainterPath painterPath;
        for (QPolygonF *itPolygon : std::as_const(polygons)) {
            painterPath.addPolygon(*itPolygon);
        }

        QPainterPathStroker stroker;
        stroker.setWidth(strokeWidth);
        QPainterPath strokePath = stroker.createStroke(painterPath);
        painterPath = painterPath.united(strokePath);
        regions = QRegion(painterPath.toFillPolygon().toPolygon());
    }

    qDeleteAll(polygons);

    return regions;
}

void GeoPainter::drawPolygon(const GeoDataPolygon &polygon, Qt::FillRule fillRule)
{
    // If the object is not visible in the viewport return
    if (!d->m_viewport->viewLatLonAltBox().intersects(polygon.outerBoundary().latLonAltBox()) ||
        // If the size of the object is below the resolution of the viewport then return
        !d->m_viewport->resolves(polygon.outerBoundary().latLonAltBox())) {
        // mDebug() << "Polygon doesn't get displayed on the viewport";
        return;
    }
    // mDebug() << "Drawing Polygon";

    QList<QPolygonF *> outerPolygons;
    QList<QPolygonF *> innerPolygons;
    d->m_viewport->screenCoordinates(polygon.outerBoundary(), outerPolygons);

    QPen const currentPen = pen();

    bool const hasInnerBoundaries = !polygon.innerBoundaries().isEmpty();
    bool innerBoundariesOnScreen = false;

    if (hasInnerBoundaries) {
        QList<GeoDataLinearRing> const &innerBoundaries = polygon.innerBoundaries();

        const GeoDataLatLonAltBox &viewLatLonAltBox = d->m_viewport->viewLatLonAltBox();
        for (const GeoDataLinearRing &itInnerBoundary : innerBoundaries) {
            if (viewLatLonAltBox.intersects(itInnerBoundary.latLonAltBox()) && d->m_viewport->resolves(itInnerBoundary.latLonAltBox()), 4) {
                innerBoundariesOnScreen = true;
                break;
            }
        }

        if (innerBoundariesOnScreen) {
            // Create the inner screen polygons
            for (const GeoDataLinearRing &itInnerBoundary : innerBoundaries) {
                QList<QPolygonF *> innerPolygonsPerBoundary;

                d->m_viewport->screenCoordinates(itInnerBoundary, innerPolygonsPerBoundary);

                for (QPolygonF *innerPolygonPerBoundary : std::as_const(innerPolygonsPerBoundary)) {
                    innerPolygons << innerPolygonPerBoundary;
                }
            }

            setPen(Qt::NoPen);
            QList<QPolygonF *> fillPolygons = createFillPolygons(outerPolygons, innerPolygons);

            for (const QPolygonF *fillPolygon : std::as_const(fillPolygons)) {
                ClipPainter::drawPolygon(*fillPolygon, fillRule);
            }

            setPen(currentPen);

            for (const QPolygonF *outerPolygon : std::as_const(outerPolygons)) {
                ClipPainter::drawPolyline(*outerPolygon);
            }
            for (const QPolygonF *innerPolygon : std::as_const(innerPolygons)) {
                ClipPainter::drawPolyline(*innerPolygon);
            }

            qDeleteAll(fillPolygons);
        }
    }

    if (!hasInnerBoundaries || !innerBoundariesOnScreen) {
        drawPolygon(polygon.outerBoundary(), fillRule);
    }

    qDeleteAll(outerPolygons);
    qDeleteAll(innerPolygons);
}

QList<QPolygonF *> GeoPainter::createFillPolygons(const QList<QPolygonF *> &outerPolygons, const QList<QPolygonF *> &innerPolygons) const
{
    QList<QPolygonF *> fillPolygons;
    fillPolygons.reserve(outerPolygons.size());

    for (const QPolygonF *outerPolygon : outerPolygons) {
        auto fillPolygon = new QPolygonF;
        *fillPolygon << *outerPolygon;
        *fillPolygon << outerPolygon->first();

        for (const QPolygonF *innerPolygon : innerPolygons) {
            *fillPolygon << *innerPolygon;
            *fillPolygon << innerPolygon->first();
            *fillPolygon << outerPolygon->first();
        }

        fillPolygons << fillPolygon;
    }

    return fillPolygons;
}

void GeoPainter::drawRect(const GeoDataCoordinates &centerCoordinates, qreal width, qreal height, bool isGeoProjected)
{
    if (!isGeoProjected) {
        int pointRepeatNum;
        qreal y;
        bool globeHidesPoint;

        bool visible = d->m_viewport->screenCoordinates(centerCoordinates, d->m_x, y, pointRepeatNum, QSizeF(width, height), globeHidesPoint);

        if (visible) {
            // Draw all the x-repeat-instances of the point on the screen
            const qreal posY = y - height / 2.0;
            for (int it = 0; it < pointRepeatNum; ++it) {
                const qreal posX = d->m_x[it] - width / 2.0;
                QPainter::drawRect(QRectF(posX, posY, width, height));
            }
        }
    } else {
        drawPolygon(d->createLinearRingFromGeoRect(centerCoordinates, width, height), Qt::OddEvenFill);
    }
}

QRegion GeoPainter::regionFromRect(const GeoDataCoordinates &centerCoordinates, qreal width, qreal height, bool isGeoProjected, qreal strokeWidth) const
{
    if (!isGeoProjected) {
        int pointRepeatNum;
        qreal centerY;
        bool globeHidesPoint;

        bool visible = d->m_viewport->screenCoordinates(centerCoordinates, d->m_x, centerY, pointRepeatNum, QSizeF(width, height), globeHidesPoint);

        QRegion regions;

        if (visible) {
            // only a hint, a backend could still ignore it, but we cannot know more
            const bool antialiased = testRenderHint(QPainter::Antialiasing);

            const qreal halfStrokeWidth = strokeWidth / 2.0;
            const int topY = centerY - height / 2.0;
            const int startY = antialiased ? (qFloor(topY - halfStrokeWidth)) : (qFloor(topY + 0.5 - halfStrokeWidth));
            const int endY = antialiased ? (qCeil(topY + height + halfStrokeWidth)) : (qFloor(centerY + 0.5 + height + halfStrokeWidth));
            // Draw all the x-repeat-instances of the point on the screen
            for (int it = 0; it < pointRepeatNum; ++it) {
                const qreal leftX = d->m_x[it] - width / 2.0;
                const int startX = antialiased ? (qFloor(leftX - halfStrokeWidth)) : (qFloor(leftX + 0.5 - halfStrokeWidth));
                const int endX = antialiased ? (qCeil(leftX + width + halfStrokeWidth)) : (qFloor(leftX + 0.5 + width + halfStrokeWidth));
                regions += QRegion(startX, startY, endX - startX, endY - startY);
            }
        }
        return regions;
    } else {
        return regionFromPolygon(d->createLinearRingFromGeoRect(centerCoordinates, width, height), Qt::OddEvenFill, strokeWidth);
    }
}

void GeoPainter::drawRoundedRect(const GeoDataCoordinates &centerPosition, qreal width, qreal height, qreal xRnd, qreal yRnd)
{
    int pointRepeatNum;
    qreal y;
    bool globeHidesPoint;

    // FIXME: Better visibility detection that takes the circle geometry into account
    bool visible = d->m_viewport->screenCoordinates(centerPosition, d->m_x, y, pointRepeatNum, QSizeF(width, height), globeHidesPoint);

    if (visible) {
        // Draw all the x-repeat-instances of the point on the screen
        const qreal posY = y - height / 2.0;
        for (int it = 0; it < pointRepeatNum; ++it) {
            const qreal posX = d->m_x[it] - width / 2.0;
            QPainter::drawRoundedRect(QRectF(posX, posY, width, height), xRnd, yRnd);
        }
    }
}

void GeoPainter::drawTextFragment(const QPoint &position, const QString &text, const qreal fontSize, const QColor &color, const Frames &flags)
{
    const QString key = text + QString::fromLatin1(":") + QString::number(static_cast<int>(flags));

    QPixmap pixmap;

    if (!QPixmapCache::find(key, &pixmap)) {
        const bool hasRoundFrame = flags.testFlag(RoundFrame);

        QPixmap pixmap(10, 10);
        QPainter textPainter;

        textPainter.begin(&pixmap);
        const QFontMetrics metrics = textPainter.fontMetrics();
        textPainter.end();

        const int width = metrics.horizontalAdvance(text);
        const int height = metrics.height();
        const QSize size = hasRoundFrame ? QSize(qMax(1.2 * width, 1.1 * height), 1.2 * height) : QSize(width, height);
        pixmap = QPixmap(size);
        pixmap.fill(Qt::transparent);
        const QRect labelRect(QPoint(), size);
        textPainter.begin(&pixmap);
        QFont textFont = textPainter.font();
        textFont.setPointSize(fontSize);
        textPainter.setFont(textFont);
        textPainter.setRenderHint(QPainter::Antialiasing, true);

        const QColor brushColor = color;
        if (hasRoundFrame) {
            QColor lighterColor = brushColor.lighter(110);
            lighterColor.setAlphaF(0.9);
            textPainter.setBrush(lighterColor);
            textPainter.drawRoundedRect(labelRect, 3, 3);
        }

        textPainter.setBrush(brushColor);
        textPainter.drawText(labelRect, Qt::AlignHCenter, text);

        if (hasRoundFrame) {
            textPainter.setBrush(brushColor);
        }

        textPainter.end();
        QPixmapCache::insert(key, pixmap);
    }

    QPainter::drawPixmap(position.x() - pixmap.width() / 2, position.y() - pixmap.height() / 2, pixmap);
}
