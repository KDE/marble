//
// Copyright 2008 Simon Edwards <simon@simonzone.com>
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.
//

namespace Marble
{

class GeoPainter : Marble::ClipPainter
{
%TypeHeaderCode
#include <GeoPainter.h>
%End

public:
    void                    autoMapQuality ();
    Marble::MapQuality      mapQuality () const;
    void                    drawAnnotation (const Marble::GeoDataCoordinates& position, const QString& text, QSizeF bubbleSize = QSizeF(130,100), qreal bubbleOffsetX = -10, qreal bubbleOffsetY = -30, qreal xRnd = 5, qreal yRnd = 5);
    void                    drawPoint (const Marble::GeoDataCoordinates& position);
    void                    drawPoints (const Marble::GeoDataCoordinates* positions, int pointCount);
    void                    drawText (const Marble::GeoDataCoordinates& position, const QString& text);
    void                    drawEllipse (const Marble::GeoDataCoordinates& centerPosition, qreal width, qreal height, bool isGeoProjected = 0);
    void                    drawImage (const Marble::GeoDataCoordinates& centerPosition, const QImage& image);
    void                    drawPixmap (const Marble::GeoDataCoordinates& centerPosition, const QPixmap& pixmap);
    void                    drawLine (const Marble::GeoDataCoordinates& startPosition, const Marble::GeoDataCoordinates& endPosition, bool isGeoProjected = 0);
    void                    drawPolyline (const Marble::GeoDataLineString& lineString, const QString& labelText = QString(), Marble::LabelPositionFlags labelPositionFlags = Marble::LineCenter);
    void                    drawPolygon (const Marble::GeoDataLinearRing& linearRing, Qt::FillRule fillRule = Qt::OddEvenFill);
    void                    drawPolygon (const Marble::GeoDataPolygon& polygon, Qt::FillRule fillRule = Qt::OddEvenFill);
    void                    drawRect (const Marble::GeoDataCoordinates& centerPosition, qreal width, qreal height, bool isGeoProjected = 0);
    void                    drawRoundRect (const Marble::GeoDataCoordinates& centerPosition, int width, int height, int xRnd = 25, int yRnd = 25, bool isGeoProjected = 0);
    void                    drawText (int x, int y, const QString& text);
    void                    drawText (const QPointF& position, const QString& text);
    void                    drawText (const QPoint& position, const QString& text);
    void                    drawText (const QRect& rectangle, int flags, const QString& text, QRect* boundingRect = 0);
    void                    drawEllipse (int x, int y, int width, int height);
    void                    drawEllipse (const QRectF& rectangle);
    void                    drawImage (const QRect& target, const QImage& image, const QRect& source, Qt::ImageConversionFlags flags = Qt::AutoColor);
    void                    drawPixmap (int x, int y, const QPixmap& pixmap);
    void                    drawPixmap (const QPointF& point, const QPixmap& pixmap);
    void                    drawPixmap (const QPoint& point, const QPixmap& pixmap);
    void                    drawPoint (int x, int y);
    void                    drawPolyline (const QPolygonF& polyline);
    void                    drawPolygon (const QPolygonF& polygon);
    void                    drawLine (int x1, int y1, int x2, int y2);
    void                    drawRect (const QRectF& rectangle);
    void                    drawRect (const QRect& rectangle);
    void                    drawRect (int x, int y, int width, int height);

    ~GeoPainter ();
    void                    drawPoint (const Marble::GeoDataPoint& point);
    void                    drawImage (const QRect& rect, const QImage& image);
    void                    drawImage (const QRectF& rect, const QImage& image);
                            GeoPainter (QPaintDevice* paintDevice, const Marble::ViewportParams* viewportParams, Marble::MapQuality mapQuality = Marble::NormalQuality, bool clip = 1);
    QRegion                 regionFromPoint (const Marble::GeoDataCoordinates& position, qreal strokeWidth = 3) const;
    QRegion                 regionFromPoint (const Marble::GeoDataPoint& point, qreal strokeWidth = 3) const;
    QRegion                 regionFromEllipse (const Marble::GeoDataCoordinates& centerPosition, qreal width, qreal height, bool isGeoProjected = 0, qreal strokeWidth = 3) const;
    QRegion                 regionFromLine (const Marble::GeoDataCoordinates& startPosition, const Marble::GeoDataCoordinates& endPosition, bool isGeoProjected = 0, qreal strokeWidth = 3) const;
    QRegion                 regionFromPolyline (const Marble::GeoDataLineString& lineString, qreal strokeWidth = 3) const;
    QRegion                 regionFromPolygon (const Marble::GeoDataLinearRing& linearRing, Qt::FillRule fillRule, qreal strokeWidth = 3) const;
    QRegion                 regionFromRect (const Marble::GeoDataCoordinates& centerPosition, qreal width, qreal height, bool isGeoProjected = 0, qreal strokeWidth = 3) const;
};
// GeoPainter

};
// Marble


