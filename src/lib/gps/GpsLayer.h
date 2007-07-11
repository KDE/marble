//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Carlos Licea     <carlos _licea@hotmail.com>
//


#ifndef GPSLAYER_H
#define GPSLAYER_H

#include "AbstractLayer/AbstractLayer.h"
#include "config-libgps.h"

#ifdef HAVE_LIBGPS
#include <libgpsmm.h>
#endif

class WaypointContainer;
class Waypoint;


class GpsLayer : public AbstractLayer
{
    
 public:
    GpsLayer( QObject *parent =0 );
    ~GpsLayer();
    virtual void paintLayer( ClipPainter *painter, 
                            const QSize &canvasSize, double radius,
                            Quaternion rotAxis );
    virtual QPoint * paint( ClipPainter *painter, 
                            const QSize &canvasSize, 
                            double radius, Quaternion invRotAxis, 
                            AbstractLayerData *point, 
                            QPoint *previous);
    virtual void paint(ClipPainter *painter, 
                       const QSize &canvasSize, 
                       double radius, Quaternion invRotAxis, 
                       AbstractLayerData *point);
    void changeCurrentPosition( double lat, double lon );
    
    void loadGpx(const QString &fileName);
 public slots:
    void updateGps();

    
 private:
    Waypoint            *m_currentPosition;
    Waypoint            *m_gpsTracking;
    WaypointContainer   *m_waypoints;
    
#ifdef HAVE_LIBGPS
    gpsmm               *m_gpsd;
    struct gps_data_t   *m_gpsdData;
#endif
};

#endif
