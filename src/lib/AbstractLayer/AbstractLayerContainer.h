//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//

#include <QVector>
/*!
 * Abstract LayerContainer is designed to resemble PlaceContainer
 * and be a base class for all Layer Containers. The container 
 * itself will only represent visible data and will contain a 
 * reference to a binary file that represents all of the layer 
 * data. Each time the view changes the visible data will be altered
 * by a thread that will monitor and deal with this process. 
 */
class AbstractLayerContainer:QVector<AbstractData*>
{
 public:
    //!constructor
    AbstractLayerContainer();
    //!destructor
    ~AbstractLayerContainer();
    //!m_name accessor
    QString name() const { return m_name}
    
 private:
    /*!
     * data is intended to be a binary swap file to minimize memory 
     * footprint. This can be implemented at a later stage as an 
     * optimization
     */
    QVector<AbstractLayerData*> data;
    //! taken from placecontainer, 
    QString m_name;
};
