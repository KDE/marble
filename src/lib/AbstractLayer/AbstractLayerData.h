//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//


#ifndef ABSTRACTLAYERDATA_H
#define ABSTRACTLAYERDATA_H

#include "GeoDataCoordinates.h"
#include "AbstractLayerInterface.h"

class QPoint;
class QPointF;
class QSize;

namespace Marble
{

class ClipPainter;
class ViewParams;

class AbstractLayerData : public virtual AbstractLayerInterface
{
 public:
    /**
     * @brief make a Data Object from GeoDataCoordinates
     * @param position the position that the intended
     *                 AbstractLayerData will be at.
     */
    explicit AbstractLayerData( const GeoDataCoordinates &position );
    
    /**
     * @brief make a Data object from latitude and londitude
     * @param lat longitude of the object
     * @param lon latitude of the object
     */
    AbstractLayerData( qreal lat, qreal lon);
    
    /**
     * @brief destructor
     */
    virtual ~AbstractLayerData();
    
    /**
     * @brief get the string of the internal geoPoint
     */
    QString toString();
    
    /**
     * @brief allow for drawing this Layer Data object
     * 
     * Draw is a pure virtual function that deals with drawing this
     * @c AbstractLayerData from a @c QPoint. in this case the
     * calculations of where to draw are done elsewhere
     * @param painter used to draw the AbstractLayerData object
     * @param point where to paint on the screen 
     */
    virtual void draw ( ClipPainter *painter, 
                        const QPoint &point );
    
    /**
     * @brief calculate where to draw this object and draw it
     * 
     * This is a convience method that allows for the calculations of
     * where to draw this AbstractLayerObject on screen to be done
     * localy. The other @c draw() method is intended to do the actual
     * drawing and can be called from within this one. 
     * @param painter used to draw the AbstractLayerData object
     * @param canvasSize the size of the Marble Widget screen
     * @param viewParams parameters for painting the map
     */
    virtual void draw(ClipPainter *painter, 
                      const QSize &canvasSize,
                      ViewParams *viewParams);

    /**
     * @brief check if this Layer Data is visible on screen
     * @return the visibililiy of this Data
     */
    bool visible() const;
    
    /**
     * @brief set whether this Layer Data is visible
     * @param visible the visibility of this Data
     */
    void setVisible( bool visible );
    
    /**
     * @brief m_position getter
     * @return GeoDataCoordinates representation of the position of this Object
     */
    GeoDataCoordinates position() const ;
    
    /**
     * @brief evaluate the latitude of this LayerData object
     */
    qreal lat() const;
    
    /**
     * @brief evaluate the longitude of this LayerData object
     */
    qreal lon() const;
   
    /**
     * @brief m_position setter
     * @param posIn the new GeoDataCoordinates
     * 
     * coppies the Geopoint posIn
     */
    void setPosition( const GeoDataCoordinates &posIn );
    
    /**
     * @brief m_position setter
     * @param lat the latitude of the new position
     * @param lon the longitde of the new position
     * 
     * convience method to set the position from latitude and
     * longitude
     */
    void setPosition( const qreal &lat, const qreal &lon);
    
    /**
     * @brief get the position on screen
     * 
     * get the pixel position on screen of this @c AbstractLayerData
     * @param screenSize the size of the Widget Canvas
     * @param invRotAxis Quaternion representation of the rotation of
     *                   the globe, previously inverted
     * @param radius the current radius of the globe, measuer of zoom
     *               level
     * @param [out]position the QPoint that will be updated with the 
     *                      position on screen 
     * @return @c true if the position is on the visable side of the
     *                 globe
     *         @c false if the position is on the other side of the 
     *                  globe
     */
    bool getPixelPos(const QSize &screenSize, ViewParams *viewParams,
                     QPoint *position );
    bool getPixelPos(const QSize &screenSize, ViewParams *viewParams, 
                     QPointF *position );
    
 protected:
    /**
     * @brief print a text representation of this class to a stream
     */
    virtual void printToStream( QTextStream & ) const;
    
 private:
    /**
     * @brief position of the AbstractLayerData
     */
    GeoDataCoordinates *m_position;
    
    /**
     * @brief visability of the AbstractLayerData
     */
    bool m_visible;
};

}

#endif //ABSTRACTLAYERDATA_H

