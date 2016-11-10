//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Andrew Manson <g.real.ate@gmail.com>
//

#include "GeoLineStringGraphicsItem.h"

#include "GeoDataLineString.h"
#include "GeoDataLineStyle.h"
#include "GeoDataLabelStyle.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolyStyle.h"
#include "GeoPainter.h"
#include "StyleBuilder.h"
#include "ViewportParams.h"
#include "GeoDataStyle.h"
#include "MarbleDebug.h"
#include "MarbleMath.h"

#include <qmath.h>

namespace Marble
{

GeoLineStringGraphicsItem::GeoLineStringGraphicsItem(const GeoDataPlacemark *placemark,
                                                     const GeoDataLineString *lineString) :
    GeoGraphicsItem(placemark),
    m_lineString(lineString),
    m_renderLineString(lineString),
    m_renderLabel(false)
{
    QString const category = StyleBuilder::visualCategoryName(placemark->visualCategory());
    QStringList paintLayers;
    paintLayers << QLatin1String("LineString/") + category + QLatin1String("/outline");
    paintLayers << QLatin1String("LineString/") + category + QLatin1String("/inline");
    if (!feature()->name().isEmpty()) {
        paintLayers << QLatin1String("LineString/") + category + QLatin1String("/label");
    }
    setPaintLayers(paintLayers);
}


void GeoLineStringGraphicsItem::setLineString( const GeoDataLineString* lineString )
{
    m_lineString = lineString;
    m_renderLineString = lineString;
}

const GeoDataLineString *GeoLineStringGraphicsItem::lineString() const
{
    return m_lineString;
}

GeoDataLineString GeoLineStringGraphicsItem::merge(const QVector<const GeoDataLineString *> &lineStrings_)
{
    if (lineStrings_.isEmpty()) {
        return GeoDataLineString();
    }

    Q_ASSERT(!lineStrings_.isEmpty());
    auto lineStrings = lineStrings_;
    GeoDataLineString result = *lineStrings.first();
    lineStrings.pop_front();
    for (bool matched = true; matched && !lineStrings.isEmpty();) {
        matched = false;
        for (auto lineString: lineStrings) {
            if (canMerge(result.first(), lineString->first())) {
                result.remove(0);
                result.reverse();
                result << *lineString;
                lineStrings.removeOne(lineString);
                matched = true;
                break;
            } else if (canMerge(result.last(), lineString->first())) {
                result.remove(result.size()-1);
                result << *lineString;
                lineStrings.removeOne(lineString);
                matched = true;
                break;
            } else if (canMerge(result.first(), lineString->last())) {
                GeoDataLineString behind = result;
                result = *lineString;
                behind.remove(0);
                result << behind;
                lineStrings.removeOne(lineString);
                matched = true;
                break;
            } else if (canMerge(result.last(), lineString->last())) {
                GeoDataLineString behind = *lineString;
                behind.reverse();
                behind.remove(0);
                result << behind;
                lineStrings.removeOne(lineString);
                matched = true;
                break;
            }
        }

        if (!matched) {
            return GeoDataLineString();
        }
    }
    return lineStrings.isEmpty() ? result : GeoDataLineString();
}

void GeoLineStringGraphicsItem::setMergedLineString(const GeoDataLineString &mergedLineString)
{
    m_mergedLineString = mergedLineString;
    m_renderLineString = mergedLineString.isEmpty() ? m_lineString : &m_mergedLineString;
}

const GeoDataLatLonAltBox& GeoLineStringGraphicsItem::latLonAltBox() const
{
    return m_renderLineString->latLonAltBox();
}

void GeoLineStringGraphicsItem::paint(GeoPainter* painter, const ViewportParams* viewport , const QString &layer)
{
    int const tileLevel = qLn( viewport->radius() / 64.0 ) / qLn( 2.0 );
    setRenderContext(RenderContext(tileLevel));

    if (layer.endsWith(QLatin1String("/outline"))) {
        qDeleteAll(m_cachedPolygons);
        m_cachedPolygons.clear();
        painter->polygonsFromLineString(*m_renderLineString, m_cachedPolygons);
        if (m_cachedPolygons.empty()) {
            return;
        }
        if (painter->mapQuality() == HighQuality || painter->mapQuality() == PrintQuality) {
            paintOutline(painter, viewport, m_cachedPolygons);
        }
    } else if (layer.endsWith(QLatin1String("/inline"))) {
        if (m_cachedPolygons.empty()) {
            return;
        }
        paintInline(painter, viewport, m_cachedPolygons);
    } else if (layer.endsWith(QLatin1String("/label"))) {
        if (!m_cachedPolygons.empty()) {
            if (m_renderLabel) {
                paintLabel(painter, viewport, m_cachedPolygons);
            }
        }
        qDeleteAll(m_cachedPolygons);
        m_cachedPolygons.clear();
    } else {
        qDeleteAll(m_cachedPolygons);
        m_cachedPolygons.clear();
        painter->polygonsFromLineString(*m_renderLineString, m_cachedPolygons);
        if (m_cachedPolygons.empty()) {
            return;
        }
        foreach(const QPolygonF* itPolygon, m_cachedPolygons) {
            painter->drawPolyline(*itPolygon);
        }
        qDeleteAll(m_cachedPolygons);
        m_cachedPolygons.clear();
    }
}

void GeoLineStringGraphicsItem::paintInline(GeoPainter* painter, const ViewportParams* viewport, const QVector<QPolygonF*> &polygons)
{
    if ( ( !viewport->resolves( m_renderLineString->latLonAltBox(), 2) ) ) {
        return;
    }

    painter->save();
    LabelPositionFlags labelPositionFlags = NoLabel;
    QPen currentPen = configurePainter(painter, viewport, labelPositionFlags);

    GeoDataStyle::ConstPtr style = this->style();
    const GeoDataLineStyle& lineStyle = style->lineStyle();
    if (lineStyle.cosmeticOutline() && lineStyle.penStyle() == Qt::SolidLine) {
        const float currentPenWidth = currentPen.widthF();
        if (currentPenWidth > 2.5f) {
            currentPen.setWidthF(currentPenWidth - 2.0f);
        }
        currentPen.setColor(style->polyStyle().paintedColor());
        painter->setPen( currentPen );
    }
    foreach(const QPolygonF* itPolygon, polygons) {
        painter->drawPolyline(*itPolygon);
    }

    painter->restore();
}

void GeoLineStringGraphicsItem::paintOutline(GeoPainter *painter, const ViewportParams *viewport, const QVector<QPolygonF*> &polygons)
{
    if ( ( !viewport->resolves( m_renderLineString->latLonAltBox(), 2) ) ) {
        return;
    }

    painter->save();
    LabelPositionFlags labelPositionFlags = NoLabel;
    QPen currentPen = configurePainter(painter, viewport, labelPositionFlags);
    m_renderLabel = !( currentPen.widthF() < 8.0f );

    if (!( currentPen.widthF() < 2.5f )) {
        foreach(const QPolygonF* itPolygon, polygons) {
            painter->drawPolyline(*itPolygon);
        }
    }
    painter->restore();
}

void GeoLineStringGraphicsItem::paintLabel(GeoPainter *painter, const ViewportParams *viewport, const QVector<QPolygonF*> &polygons)
{
    if ( ( !viewport->resolves( m_renderLineString->latLonAltBox(), 2) ) ) {
        return;
    }

    painter->save();
    LabelPositionFlags labelPositionFlags = NoLabel;
    QPen currentPen = configurePainter(painter, viewport, labelPositionFlags);

    GeoDataStyle::ConstPtr style = this->style();
    const GeoDataLabelStyle& labelStyle = style->labelStyle();

    // Activate the lines below to paint a label background which
    // prevents antialiasing overpainting glitches, but leads to
    // other glitches.
    //QColor const color = style->polyStyle().paintedColor();
    //painter->setBackground(QBrush(color));
    //painter->setBackgroundMode(Qt::OpaqueMode);

    painter->drawLabelsForPolygons(polygons, feature()->name(), FollowLine,
                           labelStyle.paintedColor());

    painter->restore();
}

QPen GeoLineStringGraphicsItem::configurePainter(GeoPainter *painter, const ViewportParams *viewport, LabelPositionFlags &labelPositionFlags) const
{
    QPen currentPen = painter->pen();
    GeoDataStyle::ConstPtr style = this->style();
    if (!style) {
        painter->setPen( QPen() );
    }
    else {
        const GeoDataLineStyle& lineStyle = style->lineStyle();

        const QColor linePaintedColor = lineStyle.paintedColor();
        if (currentPen.color() != linePaintedColor) {
            currentPen.setColor(linePaintedColor);
        }

        const float lineWidth = lineStyle.width();
        const float linePhysicalWidth = lineStyle.physicalWidth();
        if (currentPen.widthF() != lineWidth || linePhysicalWidth != 0.0) {
            const float scaledLinePhysicalWidth = float(viewport->radius()) / EARTH_RADIUS * linePhysicalWidth;
            if (scaledLinePhysicalWidth < lineWidth) {
                currentPen.setWidthF(lineWidth);
            } else {
                currentPen.setWidthF(scaledLinePhysicalWidth);
            }
        }
        else if (lineWidth != 0.0 ) {
            currentPen.setWidthF(lineWidth);
        }

        const Qt::PenCapStyle lineCapStyle = lineStyle.capStyle();
        if (currentPen.capStyle() != lineCapStyle) {
            currentPen.setCapStyle(lineCapStyle);
        }

        if (painter->mapQuality() == HighQuality || painter->mapQuality() == PrintQuality) {
            const Qt::PenStyle linePenStyle = lineStyle.penStyle();
            if (currentPen.style() != linePenStyle) {
                currentPen.setStyle(linePenStyle);
            }

            if (linePenStyle == Qt::CustomDashLine) {
                currentPen.setDashPattern(lineStyle.dashPattern());
            }
        } else {
            currentPen.setStyle(Qt::SolidLine);
        }

        if ( painter->mapQuality() != Marble::HighQuality
                && painter->mapQuality() != Marble::PrintQuality ) {
            QColor penColor = currentPen.color();
            penColor.setAlpha( 255 );
            currentPen.setColor( penColor );
        }

        if ( painter->pen() != currentPen )
            painter->setPen( currentPen );

        if (lineStyle.background()) {
            QBrush brush = painter->background();
            brush.setColor(style->polyStyle().paintedColor());
            painter->setBackground( brush );

            painter->setBackgroundMode( Qt::OpaqueMode );
        }

        // label styles
        const GeoDataLabelStyle& labelStyle = style->labelStyle();
        painter->setFont(labelStyle.font() );
        switch (labelStyle.alignment()) {
        case GeoDataLabelStyle::Corner:
        case GeoDataLabelStyle::Right:
            labelPositionFlags |= LineStart;
            break;
        case GeoDataLabelStyle::Center:
            labelPositionFlags |= LineCenter;
            break;
        }
    }

    return currentPen;
}

bool GeoLineStringGraphicsItem::canMerge(const GeoDataCoordinates &a, const GeoDataCoordinates &b)
{
    return distanceSphere(a, b) * EARTH_RADIUS < 0.1;
}

}
