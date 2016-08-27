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

#include "GeoDataFeature.h"
#include "GeoDataLineString.h"
#include "GeoDataLineStyle.h"
#include "GeoPainter.h"
#include "StyleBuilder.h"
#include "ViewportParams.h"
#include "GeoDataStyle.h"
#include "MarbleDebug.h"

#include <qmath.h>

namespace Marble
{

GeoLineStringGraphicsItem::GeoLineStringGraphicsItem( const GeoDataFeature *feature,
                                                      const GeoDataLineString* lineString )
        : GeoGraphicsItem( feature ),
          m_lineString( lineString )
{
    QString const category = StyleBuilder::visualCategoryName(feature->visualCategory());
    QStringList paintLayers;
    paintLayers << QLatin1String("LineString/") + category + QLatin1String("/outline");
    paintLayers << QLatin1String("LineString/") + category + QLatin1String("/inline");
    paintLayers << QLatin1String("LineString/") + category + QLatin1String("/label");
    setPaintLayers(paintLayers);
}


void GeoLineStringGraphicsItem::setLineString( const GeoDataLineString* lineString )
{
    m_lineString = lineString;
}

const GeoDataLatLonAltBox& GeoLineStringGraphicsItem::latLonAltBox() const
{
    return m_lineString->latLonAltBox();
}

void GeoLineStringGraphicsItem::paint(GeoPainter* painter, const ViewportParams* viewport , const QString &layer)
{
    int const tileLevel = qLn( viewport->radius() * 4 / 256 ) / qLn( 2.0 );
    setRenderContext(RenderContext(tileLevel));

    if (layer.endsWith(QLatin1String("/outline"))) {
        if (painter->mapQuality() == HighQuality || painter->mapQuality() == PrintQuality) {
            paintOutline(painter, viewport);
        }
    } else if (layer.endsWith(QLatin1String("/label"))) {
        paintLabel(painter, viewport);
    } else if (layer.endsWith(QLatin1String("/inline"))) {
        paintInline(painter, viewport);
    } else {
        painter->drawPolyline(*m_lineString);
    }
}

void GeoLineStringGraphicsItem::paintInline(GeoPainter* painter, const ViewportParams* viewport)
{
    if ( ( !viewport->resolves( m_lineString->latLonAltBox(), 2) ) ) {
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
    painter->drawPolyline(*m_lineString);

    painter->restore();
}

void GeoLineStringGraphicsItem::paintOutline(GeoPainter *painter, const ViewportParams *viewport)
{
    if ( ( !viewport->resolves( m_lineString->latLonAltBox(), 2) ) ) {
        return;
    }

    painter->save();
    LabelPositionFlags labelPositionFlags = NoLabel;
    QPen currentPen = configurePainter(painter, viewport, labelPositionFlags);
    if (!( currentPen.widthF() < 2.5f )) {
        painter->drawPolyline(*m_lineString);
    }
    painter->restore();
}

void GeoLineStringGraphicsItem::paintLabel(GeoPainter *painter, const ViewportParams *viewport)
{
    if ( ( !viewport->resolves( m_lineString->latLonAltBox(), 2) ) ) {
        return;
    }

    painter->save();
    LabelPositionFlags labelPositionFlags = NoLabel;
    QPen currentPen = configurePainter(painter, viewport, labelPositionFlags);

    if (!( currentPen.widthF() < 2.5f )) {
        GeoDataStyle::ConstPtr style = this->style();

        QPen pen = QPen(QColor(Qt::transparent));
        pen.setWidthF(currentPen.widthF());
        painter->setPen(pen);
        // Activate the lines below to paint a label background which
        // prevents antialiasing overpainting glitches, but leads to
        // other glitches.
        //QColor const color = style->polyStyle().paintedColor();
        //painter->setBackground(QBrush(color));
        //painter->setBackgroundMode(Qt::OpaqueMode);
        const GeoDataLabelStyle& labelStyle = style->labelStyle();
        painter->setFont(labelStyle.font());
        painter->drawPolyline( *m_lineString, feature()->name(), FollowLine,
                               labelStyle.paintedColor());
    }

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

}
