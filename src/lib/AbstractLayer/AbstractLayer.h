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


/*! \brief This class is to represent the base class for each of the
 * levels in the rendering of the Marble display.
 * 
 * It will contain all information nessary to draw a layer.
 * */
class AbstractLayer: public QObject
{
    Q_OBJECT
 public slots:
    /**
     * @brief method to paint the whole Layer
     */
    virtual void paintLayer(ClipPainter*, const QSize& screenSize,
                            double radius, Quaternion rotAxis);

 public:
    ///an empty constructor, so far.
    AbstractLayer( QObject * parent=0, 
                  AbstractLayerContainer * container=0 );
    
    /** \brief get screen pixel position from a geographical position
     * 
     * Method to simplify the retrieval of the screen pixel position 
     * from a longditude and latitude.
     * \param _lon the longitude of the point we want to find
     * \param _lat the latitude of the point we want to find 
     * \param invRotAxis inversion of the Quaternion status of the globe
     * \param screenSize size of the screen
     * \param xOut where the x value of the point will be stored
     * \param yOut where the y value of the point will be stored
     * \param radius FIXME: add roll of the radius in this comment
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
      *@brief get screen pixel position. 
      * Method to simplify the retreval of the screen pixel position
      * from a Quaternion.
      * @param position the position of the point we want to find
      * @param invRotAxis inversion of the Quaternion status of the
      * globe
      * @param screenSize size of the screen
      * @param xOut where the x value of the point will be stored
      * @param yOut where the y value of the point will be sotred
      * @param radius FIXME: add roll of the radius in this comment
      * @return boolean value as to whether the point is visable on 
      * screen
      **/ 
    bool getPixelPosFromGeoPoint(GeoPoint position, 
                                 const QSize &screenSize, 
                                 Quaternion invRotAxis, 
                                 int radius,
                                 QPoint *position);
    
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
     * @brief method to paint an AbstractLayerData 
     * this method takes the previously painted point and does not
     * draw the new one if it is the same.
     */
    virtual QPoint * paint(ClipPainter*, const QSize& screenSize,
                       double radius, Quaternion rotAxis, 
                       QPoint *previous, AbstractLayerData *point);
    /**
     * @brief method to paint an AbstractLayerData 
     * this method is the un-optimised version that does not check if 
     * the previousd Abstract Layer Data has already been drawn.
     */
    virtual void paint( ClipPainter*, const QSize& screenSize,
                       double radius, Quaternion rotAxis, 
                       AbstractLayerData *point );

    

 private:
    /**
     * AbstractLayerContainer with all the nessary data for
     * this layer.
     */
    AbstractLayerContainer *m_layerContainer;
    
    bool m_visible;
};
#endif //ABSTRACTLAYER_H

