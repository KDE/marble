//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//

#include "AbstractLayerData.h"

AbstractLayerData::AbstractLayerData( GeoPoint position )
{
    m_position = new GeoPoint;
    *m_position = position;
}

AbstractLayerData::AbstractLayerData( double lat, double lon)
{
    m_position = new GeoPoint( lon, lat );
}

AbstractLayerData::~AbstractLayerData()
{
    delete m_position;
}