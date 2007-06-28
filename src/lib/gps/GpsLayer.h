//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Andrew Manson    <g.real.ate@gmail.com>
//

#ifndef GPSLAYER_H
#define GPSLAYER_H

#include "AbstractLayer/AbstractLayer.h"
//#include "gpsd/libgpsmm.h"

class Waypoint;

class GpsLayer : public AbstractLayer
{
    
 public:
    GpsLayer( QObject *parent =0 );
    ~GpsLayer();
    virtual void paint(ClipPainter*, const QSize& ,double,
                       Quaternion);
    void changeCurrentPosition( double lat, double lon );
 public slots:
    void updateGps();

    
 private:
    Waypoint *m_currentPosition;
    Waypoint *m_gpsTracking;
  //  gpsmm *m_gpsd;
    //struct gps_data_t *m_gpsdData;
    
};

#endif
