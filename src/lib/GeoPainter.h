//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008-2009 Torsten Rahn <tackat@kde.org>
//

#ifndef MARBLE_GEOPAINTER_H
#define MARBLE_GEOPAINTER_H


#include "marble_export.h"

#include <QtCore/QSize>
#include <QtGui/QRegion>

// Marble
#include "MarbleGlobal.h"
#include "ClipPainter.h"


class QImage;
class QPaintDevice;
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
class GeoDataPoint;
class GeoDataPolygon;


/*!
    \class GeoPainter
    \brief A painter that allows to draw geometric primitives on the map.

    This class allows application developers to draw simple geometric shapes
    and objects onto the map.

    The API is modeled after the QPainter API.

    The GeoPainter provides a wide range of methods that are using geographic
    ("geodesic") coordinates to position the item.
    For example a point or the nodes of a polygon can fully be described in
    geographic coordinates.

    In all these cases the position of the object is specified in geographic
    coordinates.

    There are however some cases where there are two viable cases:
    \li the shape of the object could still use screen coordinates (like a label
    or an icon).
    \li Alternatively the shape of the object can get projected according
    to the current projection (e.g. a texture projected onto the spherical
    surface)
    
    If screen coordinates are used then e.g. width and height are assumed to be
    expressed in pixels, otherwise degrees are used.

    Painter transformations (e.g. translate) always happen in screen
    coordinates.

    Like in QPainter drawing objects onto a widget should always considered to
    be a volatile operation. This means that e.g. placemarks don't get added to
    the globe permanently.
    So the drawing needs to be done on every repaint to prevent that drawings
    will disappear during the next paint event.

    So if you want to add placemarks to your map widget permanently (i.e. you
    don't want to take care of repainting) then you need to use other solutions
    such as the KML import of the Marble framework or Marble's GeoGraphicsItems.

    \note By default the GeoPainter automatically filters geographical content
    in order to provide fast painting:
    \li Geographically positioned objects which are outside the viewport are not
    drawn at all.
    Parts of objects which are specified through geographic coordinates
    (like polygons, ellipses etc.) get cut off if they are not placed within the
    viewport.
    \li Objects which have a shape that is specified through geographic
    coordinates get filtered according to the viewport resolution:
    If the object is much smaller than a pixel then it won't get drawn at all.
*/


class MARBLE_EXPORT GeoPainter : public ClipPainter
{
 public:
     
/*!
    \brief Creates a new geo painter.

    To create a geo painter it's necessary to provide \a paintDevice
    as a canvas and the viewportParams to specify the map projection
    inside the viewport.
*/
    GeoPainter( QPaintDevice * paintDevice,
                const ViewportParams *viewportParams,
                MapQuality mapQuality = NormalQuality,
                bool clip = true );

                
/*!
    \brief Destroys the geo painter.
*/
    ~GeoPainter();

    
/*!
    \brief Automatically adjusts the painter to the current map quality.

    Automatically adjusts the usage of antialiasing according to the current
    map quality. It's considered good practice to call this method before
    starting to draw with a painter. This avoids that objects don't get
    painted in the wrong quality and it ensures that performance is always
    optimal.
*/
    void autoMapQuality();

    
/*!
    \brief Returns the map quality.
    \return The map quality that got assigned to the painter.
*/
    MapQuality mapQuality() const;


/*!
    \brief Draws a text annotation that points to a geodesic position.

    The annotation consists of a bubble with the specified \a text inside.
    By choosing an appropriate pen for the painter it's possible to change
    the color and line style of the bubble outline and the text. The brush
    chosen for the painter is used to paint the background of the bubble

    The optional parameters which describe the layout of the bubble are
    similar to those used by QPainter::drawRoundRect().
    Unlike in QPainter the rounded corners are not specified in percentage
    but in pixels to provide for optimal aesthetics.
    By choosing a positive or negative bubbleOffset it's possible to
    place the annotation on top, bottom, left or right of the annotated
    position.
    
    \param position The geodesic position
    \param text The text contained by the bubble
    \param bubbleSize The size of the bubble that holds the annotation text.
                      A height of 0 can be used to have the height calculated
                      automatically to fit the needed text height.
    \param bubbleOffsetX The x-axis offset between the annotated position and
                         the "root" of the speech bubble's "arrow".
    \param bubbleOffsetY The y-axis offset between the annotated position and
                         the "root" of the speech bubble's "arrow".
    \param xRnd Specifies the geometry of the rounded corners in pixels along
                the x-axis.
    \param yRnd Specifies the geometry of the rounded corners in pixels along
                the y-axis.

    \see GeoDataCoordinates
*/
    void drawAnnotation( const GeoDataCoordinates & position,
                         const QString & text,
                         QSizeF bubbleSize = QSizeF( 130, 100 ),
                         qreal bubbleOffsetX = -10, qreal bubbleOffsetY = -30,
                         qreal xRnd = 5, qreal yRnd = 5 );

                         
/*!
    \brief Draws a single point at a given geographic position.
    The point is drawn using the painter's pen color.

    \see GeoDataCoordinates
*/
    void drawPoint ( const GeoDataCoordinates & position );

    
/*!
    \brief Creates a region for a given geographic position.

    A QRegion object is created that represents the area covered by
    GeoPainter::drawPoint( GeoDataCoordinates ). It can be used e.g. for
    input event handling of objects that have been painted using
    GeoPainter::drawPoint( GeoDataCoordinates ).

    The width allows to set the "stroke width" for the region. For input
    event handling it's always advisable to use a width that is slightly
    bigger than the width of the painter's pen.

    \see GeoDataCoordinates
*/
    QRegion regionFromPoint ( const GeoDataCoordinates & position,
                              qreal strokeWidth = 3) const;

    
/*!
    \brief Draws a single point at a given geographic position.
    The point is drawn using the painter's pen color.

    \see GeoDataPoint
*/
    void drawPoint ( const GeoDataPoint & point );


/*!
    \brief Create a region for a given geographic position.

    A QRegion object is created that represents the area covered by
    GeoPainter::drawPoint( GeoDataPoint ). It can be used e.g. for
    input event handling of objects that have been painted using
    GeoPainter::drawPoint( GeoDataPoint ).

    The width allows to set the "stroke width" for the region. For input
    event handling it's always advisable to use a width that is slightly
    bigger than the width of the painter's pen.
*/
    QRegion regionFromPoint ( const GeoDataPoint & point,
                              qreal strokeWidth = 3) const;
    
    
/*!
    \brief Draws a number of points at the given geographic positions.
    All \a positions are stored in an array. The first \a pointCount points are
    drawn using the painter's pen color.
*/
    void drawPoints ( const GeoDataCoordinates * positions, int pointCount );

    
/*!
    \brief Draws the given text at a given geographic position.
    The \a text is drawn starting at the given \a position using the painter's
    font property. The text rendering is performed in screen coordinates and is
    not subject to the current projection.
*/
    void drawText ( const GeoDataCoordinates & position, const QString & text );

    
/*!
    \brief Draws an ellipse at the given position.
    The ellipse is placed with its center located at the given \a centerPosition.

    For the outline it uses the painter's pen and for the background the
    painter's brush.

    If \a isGeoProjected is true then the outline of the ellipse is drawn
    in geographic coordinates. In this case the \a width and the \a height
    are interpreted to be degrees.
    If \a isGeoProjected is false then the outline of the ellipse is drawn
    in screen coordinates. In this case the \a width and the \a height
    are interpreted to be pixels.

    \see GeoDataCoordinates
*/
    void drawEllipse ( const GeoDataCoordinates & centerPosition,
                       qreal width, qreal height, bool isGeoProjected = false );


/*!
    \brief Creates a region for an ellipse at a given position

    A QRegion object is created that represents the area covered by
    GeoPainter::drawEllipse(). As such it can be used e.g. for input event
    handling for objects that have been painted using GeoPainter::drawEllipse().

    The \a strokeWidth allows to extrude the QRegion by half the amount of
    "stroke width" pixels. For input event handling it's always advisable to use
    a width that is slightly bigger than the width of the painter's pen.

    \see GeoDataCoordinates
*/
    QRegion regionFromEllipse ( const GeoDataCoordinates & centerPosition,
                                qreal width, qreal height, bool isGeoProjected = false,
                                qreal strokeWidth = 3 ) const;


/*!
    \brief Draws an image at the given position.
    The image is placed with its center located at the given \a centerPosition.

    The image rendering is performed in screen coordinates and is
    not subject to the current projection.

    \see GeoDataCoordinates
*/
    void drawImage ( const GeoDataCoordinates & centerPosition,
                     const QImage & image /* , bool isGeoProjected = false */ );


/*!
    \brief Draws a pixmap at the given position.
    The pixmap is placed with its center located at the given \a centerPosition.

    The image rendering is performed in screen coordinates and is
    not subject to the current projection.

    \see GeoDataCoordinates
*/
    void drawPixmap ( const GeoDataCoordinates & centerPosition,
                      const QPixmap & pixmap /*, bool isGeoProjected = false */ );


/*!
    \brief Draws a line from the given start position to the end position.

    The line is drawn using the current pen.
    If \a isGeoProjected is true then the line is bent across the surface.
    If \a isGeoProjected is false then a straight line in screen coordinates is
    the result.

    \see GeoDataCoordinates
*/
    void drawLine (  const GeoDataCoordinates & startPosition,
                     const GeoDataCoordinates & endPosition,
                     bool isGeoProjected = false );


/*!
    \brief Creates a region for a given line.

    A QRegion object is created that represents the area covered by
    GeoPainter::drawLine(). As such it can be used e.g. for input event handling
    for objects that have been painted using
    GeoPainter::drawLine( GeoDataLineString ).

    The \a strokeWidth allows to extrude the QRegion by half the amount of
    "stroke width" pixels. For input event handling it's always advisable to use
    a width that is slightly bigger than the width of the painter's pen.

    \see GeoDataCoordinates
*/
    QRegion regionFromLine ( const GeoDataCoordinates & startPosition,
                             const GeoDataCoordinates & endPosition,
                             bool isGeoProjected = false,
                             qreal strokeWidth = 3 ) const;


/*!
    \brief Draws a given line string (a "polyline").

    The \a lineString is drawn using the current pen. It's possible to
    provide a \a labelText for the \a lineString. The text is rendered using
    the painter's font property.
    The position of the \a labelText can be specified using the
    \a labelPositionFlags.

    \see GeoDataLineString
*/
    void drawPolyline ( const GeoDataLineString & lineString,
                        const QString& labelText = QString(),
                        LabelPositionFlags labelPositionFlags = LineCenter );


/*!
    \brief Creates a region for a given line string (a "polyline").

    A QRegion object is created that represents the area covered by
    GeoPainter::drawPolyline( GeoDataLineString ). As such it can be used
    e.g. for input event handling for objects that have been painted using
    GeoPainter::drawPolyline( GeoDataLineString ).

    The \a strokeWidth allows to extrude the QRegion by half the amount of
    "stroke width" pixels. For input event handling it's always advisable to use
    a width that is slightly bigger than the width of the painter's pen.

    \see GeoDataLineString
*/
    QRegion regionFromPolyline ( const GeoDataLineString & lineString,
                                 qreal strokeWidth = 3 ) const;


/*!
    \brief Draws a given linear ring (a "polygon without holes").

    The outline of the \a linearRing is drawn using the current pen. The
    background is painted using the current brush of the painter.
    Like in QPainter::drawPolygon() the \a fillRule specifies the
    fill algorithm that is used to fill the polygon.

    \see GeoDataLinearRing
*/
    void drawPolygon ( const GeoDataLinearRing & linearRing,
                       Qt::FillRule fillRule = Qt::OddEvenFill );


/*!
    \brief Creates a region for a given linear ring (a "polygon without holes").

    A QRegion object is created that represents the area covered by
    GeoPainter::drawPolygon( GeoDataLinearRing ). As such it can be used
    e.g. for input event handling for objects that have been painted using
    GeoPainter::drawPolygon( GeoDataLinearRing ).

    Like in drawPolygon() the \a fillRule specifies the fill algorithm that is
    used to fill the polygon.

    The \a strokeWidth allows to extrude the QRegion by half the amount of
    "stroke width" pixels. For input event handling it's always advisable to use
    a width that is slightly bigger than the width of the painter's pen.

    For the polygon case a "cosmetic" strokeWidth of zero should provide the
    best performance.

    \see GeoDataLinearRing
*/
    QRegion regionFromPolygon ( const GeoDataLinearRing & linearRing,
                                Qt::FillRule fillRule, qreal strokeWidth = 3 ) const;


/*!
    \brief Draws a given polygon (which may contain holes).

    The outline of the \a polygon is drawn using the current pen. The
    background is painted using the current brush of the painter.
    Like in QPainter::drawPolygon() the \a fillRule specifies the
    fill algorithm that is used to fill the polygon.

    \see GeoDataPolygon
*/    
    void drawPolygon ( const GeoDataPolygon & polygon,
                       Qt::FillRule fillRule = Qt::OddEvenFill );

    
/*!
    \brief Draws a rectangle at the given position.
    The rectangle is placed with its center located at the given
    \a centerPosition.

    For the outline it uses the painter's pen and for the background the
    painter's brush.

    If \a isGeoProjected is true then the outline of the rectangle is drawn
    in geographic coordinates. In this case the \a width and the \a height
    are interpreted to be degrees.
    If \a isGeoProjected is false then the outline of the rectangle is drawn
    in screen coordinates. In this case the \a width and the \a height
    are interpreted to be pixels.

    \see GeoDataCoordinates
*/
    void drawRect ( const GeoDataCoordinates & centerPosition,
                    qreal width, qreal height,
                    bool isGeoProjected = false );


/*!
    \brief Creates a region for a rectangle at a given position.

    A QRegion object is created that represents the area covered by
    GeoPainter::drawRect(). This can be used e.g. for input event handling
    for objects that have been painted using GeoPainter::drawRect().

    The isGeoProjected parameter is used the same way as for
    GeoPainter::drawRect().

    The \a strokeWidth allows to extrude the QRegion by half the amount of
    "stroke width" pixels. For input event handling it's always advisable to use
    a width that is slightly bigger than the width of the painter's pen. This is
    especially true for small objects.

    \see GeoDataCoordinates
*/
    QRegion regionFromRect ( const GeoDataCoordinates & centerPosition,
                             qreal width, qreal height,
                             bool isGeoProjected = false,
                             qreal strokeWidth = 3 ) const;

   
/*!
    \brief Draws a rectangle with rounded corners at the given position.
    The rectangle is placed with its center located at the given
    \a centerPosition.

    For the outline it uses the painter's pen and for the background the
    painter's brush.
    Unlike in QPainter::drawRoundedRect() the rounded corners are not specified
    in percentage but in pixels to provide for optimal aesthetics.

    \param xRnd Specifies the geometry of the rounded corners in pixels along
                the x-axis.
    \param yRnd Specifies the geometry of the rounded corners in pixels along
                the y-axis.

    If \a isGeoProjected is true then the outline of the rectangle is drawn
    in geographic coordinates. In this case the \a width and the \a height
    are interpreted to be degrees.
    If \a isGeoProjected is false then the outline of the rectangle is drawn
    in screen coordinates. In this case the \a width and the \a height
    are interpreted to be pixels.

    \see GeoDataCoordinates
*/
    void drawRoundRect ( const GeoDataCoordinates & centerPosition,
                         int width, int height,
                         int xRnd = 25, int yRnd = 25,
                         bool isGeoProjected = false );



    // Reenabling QPainter methods.

    void drawText ( int x, int y, const QString & text );
    void drawText ( const QPointF & position, const QString & text );
    void drawText ( const QPoint & position, const QString & text );
    void drawText(  const QRect & rectangle, int flags, const QString & text,
                    QRect * boundingRect = 0 );
    void drawEllipse ( int x, int y, int width, int height );
    void drawEllipse ( const QRectF & rectangle );
    void drawImage ( const QRect & target, const QImage & image,
                     const QRect & source,
                     Qt::ImageConversionFlags flags = Qt::AutoColor );
    void drawImage ( const QRect& rect, const QImage& image );
    void drawImage ( const QRectF& rect, const QImage& image );
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

#endif
