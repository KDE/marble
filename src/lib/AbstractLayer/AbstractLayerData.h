//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//

#include "GeoPoint.h"

class AbstractLayerData
{
 public:
    //!constructor
    AbstractData();
    //!destructor
    ~AbstractData();
    /*!
     * draw is intended to either provide the details nessary for the
     * view to draw, or given a pointer to the view it may draw itself
     */
    virtual void draw()=0;
    //!accessors
    GeoPoint getPositon()const {return position} 
    void setPosition(const Geopoint& posIn){position = posIn}
    
 private:
    //!the position of the Data item
    GeoPoint position;
    //insert generic (if any) view variables here
}