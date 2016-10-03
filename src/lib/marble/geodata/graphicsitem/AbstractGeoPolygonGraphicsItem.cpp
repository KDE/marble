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

namespace Marble
{

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

const GeoDataLatLonAltBox& AbstractGeoPolygonGraphicsItem::latLonAltBox() const
{
    if( m_polygon ) {
        return m_polygon->latLonAltBox();
    }

    return m_ring->latLonAltBox();
}

void AbstractGeoPolygonGraphicsItem::paint( GeoPainter* painter, const ViewportParams* viewport, const QString &layer )
{
    Q_UNUSED(layer);

    painter->save();
    configurePainter(painter, viewport);
    if ( m_polygon ) {
        painter->drawPolygon( *m_polygon );
    } else if ( m_ring ) {
        painter->drawPolygon( *m_ring );
    }

    const GeoDataIconStyle& iconStyle = style()->iconStyle();
    bool const hasIcon = !iconStyle.iconPath().isEmpty();
    if (hasIcon) {
        QImage const icon = iconStyle.scaledIcon();
        painter->drawImage(latLonAltBox().center(), icon);
    }
    painter->restore();
}

QPen AbstractGeoPolygonGraphicsItem::configurePainter(GeoPainter *painter, const ViewportParams *viewport)
{
    QPen currentPen = painter->pen();

    GeoDataStyle::ConstPtr style = this->style();
    if (!style) {
        painter->setPen( QPen() );
    }
    else {
        const GeoDataPolyStyle& polyStyle = style->polyStyle();

        if (!polyStyle.outline()) {
            currentPen.setColor( Qt::transparent );
        }
        else {
            const GeoDataLineStyle& lineStyle = style->lineStyle();

            currentPen.setColor(lineStyle.paintedColor());
            currentPen.setWidthF(lineStyle.width());
            currentPen.setCapStyle(lineStyle.capStyle());
            currentPen.setStyle(lineStyle.penStyle());
        }

        painter->setPen(currentPen);

        if (!polyStyle.fill()) {
            painter->setBrush(QColor(Qt::transparent));
        }
        else {
            const QColor paintedColor = polyStyle.paintedColor();
            if (painter->brush().color() != paintedColor ||
                painter->brush().style() != polyStyle.brushStyle()) {
                const QImage textureImage = polyStyle.textureImage();
                if (!textureImage.isNull()) {
                    GeoDataCoordinates coords = latLonAltBox().center();
                    qreal x, y;
                    viewport->screenCoordinates(coords, x, y);
                    const QString texturePath = polyStyle.texturePath();
                    if (m_cachedTexturePath != texturePath ||
                        m_cachedTextureColor != paintedColor) {
                        if (textureImage.hasAlphaChannel()) {
                            m_cachedTexture = QImage ( textureImage.size(), QImage::Format_ARGB32_Premultiplied );
                            m_cachedTexture.fill(paintedColor);
                            QPainter imagePainter(&m_cachedTexture );
                            imagePainter.drawImage(0, 0, textureImage);
                        }
                        else {
                            m_cachedTexture = textureImage;
                        }
                        m_cachedTexturePath = texturePath;
                        m_cachedTextureColor = paintedColor;
                    }
                    QBrush brush(m_cachedTexture);
                    painter->setBrush(brush);
                    painter->setBrushOrigin(QPoint(x,y));
                }
                else {
                    painter->setBrush(QBrush(paintedColor, polyStyle.brushStyle()));
                }
            }
        }
    }

    return currentPen;
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

}
