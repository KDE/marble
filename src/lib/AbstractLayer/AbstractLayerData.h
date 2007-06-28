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


#include "../GeoPoint.h"

class QPixmap;


class AbstractLayerData
{
 public:
    //!make a Data Object from GeoPoint
    AbstractLayerData( const GeoPoint &position );
    //!make a Data object from latitude and londitude
    AbstractLayerData( double lat, double lon);
    
    //!destructor
    virtual ~AbstractLayerData();
    
    /*!\brief allow for drawing this Layer Data object
     * 
     * draw is intended to either provide the details necessary for the
     * view to draw, or given a pointer to the view it may draw itself
     */
   // virtual void draw() = 0;
    
    virtual QPixmap symbolPixmap() = 0;
    
    /**
     * @brief check if this Layer Data is visible on screen
     * @return the visibliliy of this Data
     */
    bool visible() const;
    /**
     * @brief set whether this Layer Data is visible
     * @param visible the visiblity of this Data
     */
    void setVisible( bool visible );
    
    //!accessor
    GeoPoint position() const { return *m_position; } 
   
    void setPosition( const GeoPoint &posIn );
    void setPosition( const double &lat, const double &lon);
    
 private:
    //!the position of the Data item
    GeoPoint *m_position;
    
    bool m_visible;
    
    //insert generic (if any) view variables here
};

#endif //ABSTRACTLAYERDATA_H

