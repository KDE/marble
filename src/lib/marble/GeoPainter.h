// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2008-2009 Torsten Rahn <tackat@kde.org>
//

#ifndef MARBLE_GEOPAINTER_H
#define MARBLE_GEOPAINTER_H


#include "marble_export.h"

// Marble
#include "MarbleGlobal.h"
#include "ClipPainter.h"

#include <QSizeF>

class QImage;
class QPaintDevice;
class QRegion;
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
    enum Frame {
        NoOptions = 0x0,
        RoundFrame = 0x1
    };

    Q_DECLARE_FLAGS(Frames, Frame)

/*!
    \brief Creates a new geo painter.

    To create a geo painter it's necessary to provide \a paintDevice
    as a canvas and the viewportParams to specify the map projection
    inside the viewport.
*/
    GeoPainter( QPaintDevice * paintDevice,
                const ViewportParams *viewportParams,
                MapQuality mapQuality = NormalQuality );

                
/*!
    \brief Destroys the geo painter.
*/
    ~GeoPainter();

    
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
    similar to those used by QPainter::drawRoundedRect().
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
    \brief Draws the given text at a given geographic position.
    The \a text is drawn starting at the given \a position using the painter's
    font property. The text rendering is performed in screen coordinates and is
    not subject to the current projection.
    An offset given in screenPixels can be provided via xOffset and yOffset
    in order to tweak the text position.
    By optionally adding a width, height and text options the text flow can be
    further influenced.
*/
    void drawText ( const GeoDataCoordinates & position, const QString & text,
                    qreal xOffset = 0.0, qreal yOffset = 0.0,
                    qreal width = 0.0, qreal height = 0.0,
                    const QTextOption & option = QTextOption() );

    
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
    \brief Creates a region for a rectangle for a pixmap at a given position.

    A QRegion object is created that represents the area covered by
    GeoPainter::drawPixmap(). This can be used e.g. for input event handling
    for objects that have been painted using GeoPainter::drawPixmap().

    The \a margin allows to extrude the QRegion by "margin" pixels on every side.

    \see GeoDataCoordinates
*/
    QRegion regionFromPixmapRect(const GeoDataCoordinates &centerCoordinates,
                                 int width, int height,
                                 int margin = 0) const;

/*!
    \brief Helper method for safe and quick linestring conversion.

    In general drawPolyline() should be used instead. However
    in situations where the same linestring is supposed to be
    drawn multiple times it's a good idea to cache the
    screen polygons using this method.

    \see GeoDataLineString
*/
    void polygonsFromLineString( const GeoDataLineString &lineString,
                                       QVector<QPolygonF*> &polygons) const;


/*!
    \brief Draws a given line string (a "polyline") with a label.

    The \a lineString is drawn using the current pen. It's possible to
    provide a \a labelText for the \a lineString. The text is rendered using
    the painter's font property.
    The position of the \a labelText can be specified using the
    \a labelPositionFlags.

    \see GeoDataLineString
*/
    void drawPolyline ( const GeoDataLineString & lineString,
                        const QString& labelText,
                        LabelPositionFlags labelPositionFlags = LineCenter,
                        const QColor& labelcolor = Qt::black);

/*!
    \brief Draws Labels for a given set of screen polygons.

    In common cases the drawPolyline overload can be used instead.
    However in certain more complex cases this particular method
    might be helpful for further optimization.
*/

    void drawLabelsForPolygons( const QVector<QPolygonF*> &polygons,
                                const QString& labelText,
                                LabelPositionFlags labelPositionFlags,
                                const QColor& labelColor );

/*!
    \brief Draws a given line string (a "polyline").

    The \a lineString is drawn using the current pen.

    \see GeoDataLineString
*/
    void drawPolyline(const GeoDataLineString & lineString);

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


    QVector<QPolygonF*> createFillPolygons( const QVector<QPolygonF*> & outerPolygons,
                                            const QVector<QPolygonF*> & innerPolygons ) const;
    
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

    \param centerPosition Position of rectangle center
    \param width Width of the rectangle in pixels
    \param height Height of the rectangle in pixels
    \param xRnd Specifies the geometry of the rounded corners in pixels along
                the x-axis.
    \param yRnd Specifies the geometry of the rounded corners in pixels along
                the y-axis.

    \see GeoDataCoordinates
*/
    void drawRoundedRect(const GeoDataCoordinates &centerPosition,
                         qreal width, qreal height,
                         qreal xRnd = 25.0, qreal yRnd = 25.0);


    void drawTextFragment(const QPoint &position, const QString &text,
                          const qreal fontSize, const QColor &color = Qt::black,
                          const Frames &flags = Frames());


    // Reenabling QPainter+ClipPainter methods.
    using QPainter::drawText;
    using QPainter::drawEllipse;
    using QPainter::drawImage;
    using QPainter::drawPixmap;
    using QPainter::drawPoint;
    using ClipPainter::drawPolyline;
    using ClipPainter::drawPolygon;
    using QPainter::drawRect;
    using QPainter::drawRoundedRect;

 private:
    Q_DISABLE_COPY( GeoPainter )
    GeoPainterPrivate  * const d;
};

}

#endif
