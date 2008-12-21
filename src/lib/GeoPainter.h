//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008 Torsten Rahn <tackat@kde.org>"
//


#ifndef GEOPAINTER_H
#define GEOPAINTER_H


#include "marble_export.h"

#include <QtCore/QSize>

// Marble
#include "global.h"
#include "ClipPainter.h"

class QImage;
class QPaintDevice;
class QPixMap;
class QPolygonF;
class QRect;
class QRectF;
class QString;

namespace Marble
{

class ViewportParams;
class GeoPainterPrivate;
class GeoDataCoordinates;
class GeoDataLineString;
class GeoDataLinearRing;
class GeoDataPolygon;

/**
 * @short a painter that makes it easy to draw geometric items on the map
 *
 * This class enables application developers to paint simple geometric
 * shapes and objects on the map. The API is inspired by and is
 * compatible to QPainter.
 *
 * All methods are using geographic coordinates to position the item.
 * However while being placed in geographic coordinates the
 * shape of the objects could still use the screen coordinates
 * ( isGeoProjected = false ).
 * Alternatively the shape of the object can get projected according
 * to Marble's current projection (e.g. spherical projection)
 * ( isGeoProjected = true ).
 * If screen coordinates get used then e.g. width and height
 * are assumed to be expressed in pixels, otherwise degrees are
 * used.
 *
 * Painter transformations (e.g. translate) always happen in screen
 * coordinates.
 *
 * NOTE: Like in QPainter drawing objects should always be a
 * VOLATILE operation! This means that e.g. placemarks don't get
 * added to the globe permanently. So unless you do the drawing on
 * every repaint the objects you draw will disappear on the next
 * paint event.
 *
 * So if you want to add placemarks to your map PERMANENTLY
 * (i.e. you don't need to take care of repainting) then
 * please use other means such as the KML import.
 *
 * See docs/paintingmaps.txt for more information.
 */


class MARBLE_EXPORT GeoPainter : public ClipPainter
{
 public:
    GeoPainter( QPaintDevice*pd, ViewportParams *params,
		MapQuality mapQuality, bool clip = true );

    ~GeoPainter();
    void autoMapQuality();
    MapQuality mapQuality() const;


//  We don't go for the percentual approach that Qt takes for rounded corners for
//  reasons of bad aesthetics here.
    void drawAnnotation (  const GeoDataCoordinates & position, const QString & text, QSize bubbleSize = QSize( 130, 100 ), int bubbleOffsetX = -10, int bubbleOffsetY = -30, int xRnd = 5, int yRnd = 5 );

    void drawPoint (  const GeoDataCoordinates & position );
    void drawPoints (  const GeoDataCoordinates * points, int pointCount );

    // Of course in theory we could have the "isGeoProjected" parameter used
    // for drawText as well. However this would require us to convert all 
    // glyphs to PainterPaths / QPolygons. From QPolygons we could create
    // GeoDataPolygons which could get painted on screen. Any patches appreciated ;-)
    void drawText ( const GeoDataCoordinates & position, const QString & text );

//    void drawPlaceMark ( const GeoDataCoordinates & position, const QString& name );
//    void drawPlaceMark ( const GeoDataPlaceMark & placemark );

    void drawEllipse ( const GeoDataCoordinates & centerPoint, qreal width, qreal height, bool isGeoProjected = false );

    // isGeoProjected = true would project the image/pixmap onto the globe. This requires
    // to deal with the TextureMapping classes -> should get implemented later on

    void drawImage ( const GeoDataCoordinates & centerPoint, const QImage & image, bool isGeoProjected = false );

    void drawPixmap ( const GeoDataCoordinates & centerPoint, const QPixmap & pixmap, bool isGeoProjected = false );

    // In the following case isGeoProjected = true lets the line be bent according to the projection.
    void drawLine (  const GeoDataCoordinates & p1,  const GeoDataCoordinates & p2, bool isGeoProjected = false );

    // For these classes use setTesselate( true ) or the TesselationOptions on the line string / linear ring
    // to project the line segments onto the geographical coordinate system ("isGeoProjected").
    void drawPolyline ( const GeoDataLineString & lineString );
    void drawPolygon ( const GeoDataLinearRing & linearRing, Qt::FillRule fillRule = Qt::OddEvenFill );
    void drawPolygon ( const GeoDataPolygon & polygon, Qt::FillRule fillRule = Qt::OddEvenFill );

    // For isGeoProjected == false the width and height are given in pixels.
    // For isGeoProjected == true the width and height are given in degrees.
    void drawRect ( const GeoDataCoordinates & centerPoint, qreal width, qreal height, bool isGeoProjected = false );
    void drawRoundRect ( const GeoDataCoordinates & centerPoint, int w, int h, int xRnd = 25, int yRnd = 25, bool isGeoProjected = false );


    // Reenabling QPainter methods.

    void drawText ( int x, int y, const QString & text );
    void drawText ( const QPoint & position, const QString & text );
    void drawText(  const QRect & rectangle, int flags, const QString & text, QRect * boundingRect = 0 );    void drawEllipse ( int x, int y, int width, int height );
    void drawEllipse ( const QRectF & rectangle );
    void drawImage ( const QRect & target, const QImage & image, const QRect & source, Qt::ImageConversionFlags flags = Qt::AutoColor );
    void drawPixmap ( int x, int y, const QPixmap & pixmap );
    void drawPixmap ( const QPointF & point, const QPixmap & pixmap );
    void drawPixmap ( const QPoint & point, const QPixmap & pixmap );
    void drawPoint ( int x, int y );
    void drawPolyline( const QPolygonF & polyline );
    void drawPolygon( const QPolygonF & polygon );
    void drawLine ( int x1, int y1, int x2, int y2 );
    void drawRect ( const QRectF & rectangle );
    void drawRect ( const QRect & rectangle );
    void drawRect ( int x, int y, int width, int height );
 private:
    Q_DISABLE_COPY( GeoPainter )
    GeoPainterPrivate  * const d;
};

}

#endif // GEOPAINTER_H
