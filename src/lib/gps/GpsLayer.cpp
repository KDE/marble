//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//

#include "GpsLayer.h"
#include "Waypoint.h"
#include "clippainter.h"
#include <QPixmap>

GpsLayer::GpsLayer( QObject *parent ) : AbstractLayer( parent )
{
    m_currentPosition = new Waypoint( 0,0 );
  //  m_gpsd = new gpsmm();
  //  m_gpsdData = m_gpsd -> open();
}

GpsLayer::~GpsLayer()
{
}

void GpsLayer::updateGps(){
   /* if(m_gpsdData !=0){
        m_gpsdData =m_gpsd->poll();
        m_gpsTracking-> setPosition( m_gpsdData->fix.latitude, 
                                 m_gpsdData->fix.longitude);
    }*/
}

void GpsLayer::paint( ClipPainter *painter, const QSize &canvasSize, 
                      double radius, Quaternion rotAxis )
{
    Quaternion invRotAxis = rotAxis.inverse();
    
    QPoint position;
    bool draw = false;
    
    draw = getPixelPosFromGeoPoint(m_currentPosition->position(),
                                   canvasSize, invRotAxis, 
                                   (int)radius, &position);
    
    if (draw){
        painter->drawPixmap( position,
                             m_currentPosition->symbolPixmap() );
    }
}

void GpsLayer::changeCurrentPosition( double lat, double lon )
{
    m_currentPosition->setPosition( lat, lon );
}

