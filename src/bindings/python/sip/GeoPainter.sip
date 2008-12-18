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
                            GeoPainter (QPaintDevice* pd, Marble::ViewportParams* params, Marble::MapQuality mapQuality, bool clip = 1);
    void                    autoMapQuality ();
    void                    drawAnnotation (const Marble::GeoDataCoordinates& position, const QString& text, QSize bubbleSize = QSize(130,100), int bubbleOffsetX = -10, int bubbleOffsetY = -30, int xRnd = 5, int yRnd = 5);
    void                    drawPoint (const Marble::GeoDataCoordinates& position);
    void                    drawPoints (const Marble::GeoDataCoordinates* points, int pointCount);
    void                    drawText (const Marble::GeoDataCoordinates& position, const QString& text);
    void                    drawEllipse (const Marble::GeoDataCoordinates& centerPoint, qreal width, qreal height, bool isGeoProjected = 0);
    void                    drawImage (const Marble::GeoDataCoordinates& centerPoint, const QImage& image, bool isGeoProjected = 0);
    void                    drawPixmap (const Marble::GeoDataCoordinates& centerPoint, const QPixmap& pixmap, bool isGeoProjected = 0);
    void                    drawLine (const Marble::GeoDataCoordinates& p1, const Marble::GeoDataCoordinates& p2, bool isGeoProjected = 0);
    void                    drawPolyline (const Marble::GeoDataLineString& lineString);
//FIXME    void                    drawPolygon (const Marble::GeoDataLinearRing& linearRing, Qt::FillRule fillRule = Qt::OddEvenFill);
    void                    drawPolygon (const Marble::GeoDataPolygon& polygon, Qt::FillRule fillRule = Qt::OddEvenFill);
    void                    drawRect (const Marble::GeoDataCoordinates& centerPoint, qreal width, qreal height, bool isGeoProjected = 0);
    void                    drawRoundRect (const Marble::GeoDataCoordinates& centerPoint, int w, int h, int xRnd = 25, int yRnd = 25, bool isGeoProjected = 0);
    void                    drawText (int x, int y, const QString& text);
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

private:
                            GeoPainter (const Marble::GeoPainter&);
};   // GeoPainter

};   // Marble


