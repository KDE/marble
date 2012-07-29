//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_GEOGRAPHICSITEMPRIVATE_H
#define MARBLE_GEOGRAPHICSITEMPRIVATE_H


// Marble
#include "GeoDataLatLonAltBox.h"
#include "GeoDataStyle.h"
#include "ViewportParams.h"

namespace Marble
{

class GeoGraphicsItemPrivate
{
 public:
    explicit GeoGraphicsItemPrivate()
        : m_zValue( 0 ),
          m_minZoomLevel( 0 ),
          m_latLonAltBox(),
          m_style( 0 )
    {
    }
    
    virtual ~GeoGraphicsItemPrivate()
    {
    }

    GeoDataCoordinates m_coordinate;
    qreal m_zValue;
    GeoGraphicsItem::GeoGraphicsItemFlags m_flags;

    int m_minZoomLevel;
    
    GeoDataLatLonAltBox m_latLonAltBox;
    const GeoDataStyle *m_style;
};

}

#endif
