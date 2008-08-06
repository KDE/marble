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

// Marble
#include "global.h"
#include "ClipPainter.h"


using namespace Marble;

class ViewportParams;
class GeoDataCoordinates;
class GeoPainterPrivate;


/**
 * @short a painter that makes it easy to draw geometric items on the map
 *
 * This class enables application developers to paint simple geometric
 * shapes and objects on the map. The API is inspired by and
 * compatible to QPainter.
 *
 * All methods are using Geographic coordinates to position the item.
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
//    Ideally we'd like to have this constructor:

//    GeoPainter( MarbleMap * map, bool clip );
//    .. however I don't see a way to implement it in a way that
//    would serialize API graphic calls on one hand and would stay
//    compatible with the Qt Painter API on the other hand.

    ~GeoPainter();
    void autoMapQuality();

//  We don't go for the percentual approach that Qt takes for rounded corners for
//  reasons of bad aesthetics here.

    void drawAnnotation (  const GeoDataCoordinates & position, const QString & text, QSize bubbleSize = QSize( 130, 100 ), int bubbleOffsetX = -10, int bubbleOffsetY = -30, int xRnd = 5, int yRnd = 5 );

    void drawPoint (  const GeoDataCoordinates & position );
    void drawPoints (  const GeoDataCoordinates * points, int pointCount );

    // Of course in theory we could have the "isGeoProjected" parameter used
    // for drawText as well. However this would largely complicate and slow
    // down things to a crawl as we'd need to convert all glyphs to PainterPaths
    // and convert thos ... . Given that for decent maps you don't really want
    // this anyways we leave it out for now ...
    void drawText ( const GeoDataCoordinates & position, const QString & text );

//    void drawPlaceMark ( const GeoDataCoordinates & position, const QString& name );
//    void drawPlaceMark ( const GeoDataPlaceMark & placemark );

    // all cases for isGeoProjected = false get implemented first, as this is much easier to implement.

    void drawEllipse ( const GeoDataCoordinates & centerPoint, int width, int height, bool isGeoProjected = false );

    // isGeoProjected = true would project the image/pixmap onto the globe. This requires
    // to deal with the TextureMapping classes -> should get implemented later on
/*
    void drawImage ( const GeoDataCoordinates & point, const QImage & image, const QRect & source, Qt::ImageConversionFlags flags = Qt::AutoColor, bool isGeoProjected = false );
    void drawImage ( const GeoDataCoordinates & point, const QImage & image, int sx = 0, int sy = 0, int sw = -1, int sh = -1, Qt::ImageConversionFlags flags = Qt::AutoColor, bool isGeoProjected = false ); */

    void drawImage ( const GeoDataCoordinates & centerPoint, const QImage & image, bool isGeoProjected = false );

    void drawPixmap ( const GeoDataCoordinates & centerPoint, const QPixmap & pixmap, bool isGeoProjected = false );
    // In the following cases isGeoProjected = true lets the line segments be bent according
    // to the projection. This requires slerp to be used to interpolate the points inbetween
    // (similar to how measure lines get calculated).-> should get implemented once lmc is in place.

    void drawLine (  const GeoDataCoordinates & p1,  const GeoDataCoordinates & p2, bool isGeoProjected = false );

    void drawPolygon ( const GeoDataCoordinates * points, int pointCount, Qt::FillRule fillRule = Qt::OddEvenFill, bool isGeoProjected = false );
    void drawPolygon ( QVector<GeoDataCoordinates*> points, Qt::FillRule fillRule = Qt::OddEvenFill, bool isGeoProjected = false );

    void drawPolyline ( const GeoDataCoordinates * points, int pointCount, bool isGeoProjected = false );
    void drawPolyline ( QVector<GeoDataCoordinates*> points, bool isGeoProjected = false );

    void drawRect ( const GeoDataCoordinates & centerPoint, int width, int height, bool isGeoProjected = false );
    void drawRoundRect ( const GeoDataCoordinates & centerPoint, int w, int h, int xRnd = 25, int yRnd = 25, bool isGeoProjected = false );


    // Reenabling QPainter methods.

    void drawText ( int x, int y, const QString & text );
    void drawText ( const QPoint & position, const QString & text );
    void drawEllipse ( int x, int y, int width, int height );
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

#endif // GEOPAINTER_H
