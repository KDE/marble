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

AbstractLayerData::AbstractLayerData( const GeoPoint &position ):
                    m_visible(true)
{
    m_position = new GeoPoint;
    *m_position = position;
}

AbstractLayerData::AbstractLayerData( double lat, 
                                      double lon): m_visible(true)
{
    m_position = new GeoPoint( lon, lat, GeoPoint::Degree);
}

AbstractLayerData::~AbstractLayerData()
{
    delete m_position;
}

bool AbstractLayerData::visible() const
{
    return m_visible;
}

void AbstractLayerData::setVisible( bool visible )
{
    m_visible = visible;
}

void AbstractLayerData::setPosition( const GeoPoint &position )
{
    delete m_position;
    m_position = new GeoPoint(position);
}

void AbstractLayerData::setPosition( const double &lat,
                                     const double &lon )
{
    int detail = m_position->detail();
    delete m_position;
    m_position = new GeoPoint( lon, lat, GeoPoint::Degree);
}

