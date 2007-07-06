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


/*!
 * \brief Abstract LayerContainer is designed to resemble
 * PlaceMarkContainer and be a base class for all Layer Containers.
 *
 * The container itself will only represent visible data and will
 * contain a reference to a binary file that represents all of the
 * layer data. Each time the view changes the visible data will be
 * altered by a thread that will monitor and deal with this process.
 */
class AbstractLayerContainer : public QVector<AbstractLayerData*>
{
 public:
    /*!\brief simple constructor with optional starting size.
     * 
     * \param size the amount of Data objects this container will have
     * at first.
     */
    AbstractLayerContainer( int size =0 );
    
    /*!\brief simple constructor with optional starting size and name
     *        for the container.
     * 
     * \param size the amount of Data objects this container will have
     * at first.
     * \param name the name of the Container
     */
    explicit AbstractLayerContainer( const QString &name, int size =0 );
    
    //!destructor
    virtual ~AbstractLayerContainer();
    
    /*! \brief draw is intended to deal with drawing all visable
     *         Data Objects in this layer.
     *
     * This method simplafies the interface for drawing the entire
     * layer but can also deal with specific layer drawing needs.
     */
    virtual void draw() = 0;
    
    //! \brief m_name accessor
    QString name() const { return *m_name; }
    
 protected:
    /*! \brief Method to process what Data Objects need to be in
     *         memory.
     *
     * intended to be implemented by each subclass of
     * AbstractLayerContainer with specific needs.
     */
    virtual void processVisible();
    
    /*! \brief brings data from file into memory
     * 
     * This method reads the Bit Array to find out which items need
     * to be in memory and processes this information.
     */
    void manageMemory();
    
 private:
    /*! \brief data is intended to be a binary swap file to minimize
     * memory footprint.
     * 
     * This can be implemented at a later stage as an optimization
     */
    QVector<AbstractLayerData*> *m_data;
    
    /*! \brief a representation of which items in m_data should be in
     *         memory.
     * 
     * This is intended to be the only read/write accessable part of
     * this container. processVisable() will deal with updating this
     * and the actuall memory swapping will be done elswere.
     */
    QBitArray *m_visible;
    
    //! taken from placecontainer.
    QString *m_name;
};

#endif //ABSTRACTLAYERCONTAINER_H

