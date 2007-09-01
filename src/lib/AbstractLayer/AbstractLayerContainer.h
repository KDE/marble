//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//


#ifndef ABSTRACTLAYERCONTAINER_H
#define ABSTRACTLAYERCONTAINER_H


#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QBitArray>

#include "AbstractLayerData.h"
#include "AbstractLayerInterface.h"


class ClipPainter;
class QPointF;
class BoundingBox;


/*!
 * @brief Container to hold AbstractLayerData objects
 *
 * This container will hold all data that can be drawn on the Marble
 * Widget. The Container inherits the @c AbstractLayerInterface Class
 * and provides sensible default implementations for its methods using
 * the @c draw() implementations in the @c AbstractLayerData objects.
 * Because of the presence of pretty large Containers the
 * container itself will only represent @b visible data and will
 * contain a reference to a binary file that represents all of the
 * layer data. The intention of this class is to keep the complicated
 * memory management away from any new developers making it easier to 
 * implement a new layer Each time the view changes the visible data
 * should be  altered by a thread that will monitor and deal with
 * this process.
 */
class AbstractLayerContainer : public QVector<AbstractLayerData*>,
                               public virtual AbstractLayerInterface
{
    
 public:
    /**
     * @brief simple constructor with optional starting size.
     * @param size the amount of Data objects this container will have
     *             at first.
     */
    AbstractLayerContainer( int size =0 );
    
    /**
     * @brief simple constructor with optional starting size and name
     *        for the container.
     * @param size the amount of Data objects this container will have
     *             at first.
     * @param name the name of the Container
     */
    explicit AbstractLayerContainer(const QString &name, int size =0);
    
    /**
     * @brief destructor
     */
    virtual ~AbstractLayerContainer();
    
    /**
     * @brief default implementation of @c AbstractLayerInterface 
     *        function.
     * 
     * this method does not apply to AbstractLayerContainer and should
     * never be called.
     * @param painter only provided to match AbstractLayerInterface
     * @param point only provided to match AbstractLayerInterface
     */
    virtual void draw ( ClipPainter *painter, const QPoint &point );
    
    /**
     * @brief  draw all AbstraceLayerData objects in this container
     * 
     * @c draw() is intended to deal with drawing all visable Data
     * Objects in this Container by calling their @c draw() function.
     * @param painter this will be used to draw the contianer
     * @param canvasSize the size of the marble widget view
     * @param radius measure of zoom level
     * @param invRotAxis inversion of the rotation Quaternion that
     *                   represents the rotation of the globe
     */
    virtual void draw(ClipPainter *painter, 
                      const QSize &canvasSize, double radius,
                      Quaternion invRotAxis);
    
    /**
     * @brief draw this Container after evaluating its bounding Box
     * 
     * this container will only get drawn if the bounding box of the
     * view intersects with the bounding box of the container
     * @param painter this will be used to draw the contianer
     * @param canvasSize the size of the marble widget view
     * @param radius measure of zoom level
     * @param invRotAxis inversion of the rotation Quaternion that
     *                   represents the rotation of the globe
     * @param box the bounding box that holds the view
     */
    virtual void draw(ClipPainter *painter, 
                      const QSize &canvasSize, double radius,
                      Quaternion invRotAxis, BoundingBox box);
    
    /**
     * @brief m_name getter
     */
    QString name() const;
    
    /**
     * @brief get the distance between 2 points
     *
     * This is just a convience method to make certain optimisations 
     * easier, such as not drawing an @c AbstractLayerData if it is 
     * too close to another
     * @return the square of the distance between the points, if the 
     *         actual distance is needed get the square root.
     */
    double distance ( const QPoint &a, const QPoint &b );
    
    /**
     * @brief get the distance between 2 points
     * 
     * Overloaded method to work with @c QPointF objects and allow for
     * floating point Points. 
     * @return the square of the distance between the points, if the 
     *         actual distance is needed get the square root.
     */
    double distance ( const QPointF &a, const QPointF &b );
    
    /**
     * @brief creates a bouding box from this Contianer
     */
    void createBoundingBox();
    
    /**
     * @brief generate a QVector of QPointF representing the x and y
     *        values of each AbstractLayerData's Quaternion
     * 
     * this method is used to collect the information nessary to 
     * create the bounding box from this container. 
     */
    QVector<QPointF> geoCoord();
 protected:
    /** 
     * @brief Method to process what Data Objects need to be in
     *        memory.
     *
     * Intended to be implemented by each subclass of
     * AbstractLayerContainer with specific needs but a very basic 
     * implementation is available.
     */
    virtual void processVisible();
    
    /**
     * @brief print a text representation of this class to a stream
     */
    virtual void printToStream( QTextStream& ) const;
    
    /**
     * @brief brings data from file into memory
     * 
     * This method reads the Bit Array to find out which items need
     * to be in memory and processes this information.
     * FIXME: make this work with a binary file
     */
    void manageMemory();
    
    /**
     * @brief bounding box to quickly identify if this should be drawn
     */
    BoundingBox *m_boundingBox;
    
 private:
    /** 
     * @brief data is intended to be a binary swap file to minimize
     *        memory footprint.
     * 
     * This can be implemented at a later stage as an optimization
     * FIXME: make this a binary file cache
     */
    QVector<AbstractLayerData*> *m_data;
    
    /** 
     * @brief A representation of which items in m_data should be in
     *        memory.
     * 
     * This is intended to be the only read/write accessable part of
     * this container. processVisable() will deal with updating this
     * and the actuall memory swapping will be done elswere.
     */
    QBitArray *m_visible;
    
    /**
     * @brief name of this container
     * 
     * taken from placecontainer for compatibility.
     */
    QString *m_name;
};


#endif //ABSTRACTLAYERCONTAINER_H

