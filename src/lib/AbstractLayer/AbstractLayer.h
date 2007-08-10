//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2004-2007 Torsten Rahn  <tackat@kde.org>"
// Copyright 2007      Inge Wallin   <ingwa@kde.org>"
// Copyright 2007      Andrew Manson <g.real.ate@gmail.com>"
//


#ifndef ABSTRACTLAYER_H
#define ABSTRACTLAYER_H


#include <QtCore/QObject>
#include <QtCore/QSize>

#include "../Quaternion.h"
#include "AbstractLayerContainer.h"


class ClipPainter;
class QPoint;
class QRectF;


/**
 * @brief framework class for each display layer in Marble
 *
 * This class is intended to be used as a base class for all layers in
 * Marble. It contains all parts nessary to draw a layer on a Marble
 * Widget.
 **/
class AbstractLayer: public QObject
{
    Q_OBJECT
 public slots:
    /**
     * @brief method to paint the whole Layer
     * 
     * This method is intended to keep the painting of an entire
     * layer a simple function call.
     * @param painter pointer to the painter that will paint this
     *                layer
     * @param screenSize used to calculate the pixel position
     * @param radius globe radius, used as a measure of zoom level
     * @param rotAxis quaternion that represents the current rotation
     *                of the globe
     */
    virtual void paintLayer(ClipPainter* painter, 
                            const QSize& screenSize,
                            double radius, Quaternion rotAxis);
    
    /**
     * @brief method to paint the whole Layer
     * 
     * This overloaded method is intended to add the functionality of
     * having a bounding box implementation. 
     * @param painter pointer to the painter that will paint this
     *                layer
     * @param screenSize used to calculate the pixel position
     * @param radius globe radius, used as a measure of zoom level
     * @param rotAxis quaternion that represents the current rotation
     *                of the globe
     */
    virtual void paintLayer(ClipPainter* painter, 
                            const QSize& screenSize,
                            double radius, Quaternion rotAxis, 
                            QRectF bounding );

 public:
    /**
     * @brief Construct this layer with a parent
     */
    AbstractLayer( QObject * parent=0 );
    
    /**
     * @brief distructor
     */
    ~AbstractLayer();
    
    /** 
     * @brief get screen pixel position from a geographical position
     * 
     * Method to simplify the retrieval of the screen pixel position 
     * from a longditude and latitude.
     * @param _lon the longitude of the point we want to find
     * @param _lat the latitude of the point we want to find 
     * @param invRotAxis inversion of the Quaternion status of the
     *                   globe
     * @param screenSize size of the screen
     * @param xOut where the x value of the point will be stored
     * @param yOut where the y value of the point will be stored
     * @param radius the radius of the globe, a measure of zoom level
     * @return @c true if the pixel is visible on the screen
     *         @c false if the pixel is outside the screen
     *
     * 
     **/
    bool getPixelPosFromGeoPoint(double _lon, double _lat, 
                                 const QSize &screenSize, 
                                 Quaternion invRotAxis, 
                                 int radius,
                                 QPoint *position);

     /**
      * @brief get screen pixel position. 
      * 
      * Method to simplify the retreval of the screen pixel position
      * from a Quaternion.
      * @param position the position of the point we want to find
      * @param invRotAxis inversion of the Quaternion status of the
      *                   globe
      * @param screenSize size of the screen
      * @param xOut where the x value of the point will be stored
      * @param yOut where the y value of the point will be sotred
      * @param radius the radius of the globe, a measure of zoom level
      * @return @c true if the pixel is visible on the screen
      *         @c false if the pixel is outside the screen
      **/ 
    bool getPixelPosFromGeoPoint(GeoPoint geoPosition,
                                 const QSize &screenSize,
                                 Quaternion invRotAxis,
                                 int radius,
                                 QPoint *pos2);
    
    /**
     * @brief  Return whether the Layer is visible.
     * @return The Layer visibility.
     */
    bool  visible() const;
    
    /**
     * @brief  Set whether the Layer is visible
     * @param  visible  visibility of the Layer
     */
    void setVisible( bool visible );
    
    /**
     * @brief method to paint an the entire layer
     *
     * All this method does it itterates through all the 
     * AbstractLayerContainer objects in @c m_containers and calls
     * their @c draw() methods
     */
    virtual void paint(ClipPainter*, const QSize& screenSize,
                       double radius, Quaternion rotAxis);
    
    /**
     * @brief convenience method to find the distance between 2 points
     */
    double distance ( const QPoint &, const QPoint & );
    double distance ( const QPointF &, const QPointF & );

    

 private:
    /**
     * @brief visibility of this layer
     * 
     * Any layer should only be drawn if this variable is @c true, it
     * defaults as @c false.
     */
    bool m_visible;
    
 protected:
    /**
     * @brief a collection of all containers that hold the data for
     *        this layer
     * 
     * To draw a layer you should be able to iterate through this
     * collection and call each container's @c draw() function
     */
    QVector<AbstractLayerContainer *> *m_containers;
};
#endif //ABSTRACTLAYER_H

