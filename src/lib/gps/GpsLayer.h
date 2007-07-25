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
class TrackContainer;
class TrackPoint;
class Track;
class TrackSegment;


class GpsLayer : public AbstractLayer
{
    
 public:
    GpsLayer( QObject *parent =0 );
    ~GpsLayer();
    virtual void paintLayer( ClipPainter *painter, 
                            const QSize &canvasSize, double radius,
                            Quaternion rotAxis );
    virtual void paintCurrentPosition( ClipPainter *painter, 
                            const QSize &canvasSize, 
                            double radius, Quaternion invRotAxis, 
                            AbstractLayerData *point );
    
    void changeCurrentPosition( double lat, double lon );
    
    void loadGpx(const QString &fileName);
 public slots:
    void updateGps();

    
 private:
    Waypoint            *m_currentPosition;
    WaypointContainer   *m_waypoints;
    TrackContainer      *m_tracks;
    
#ifdef HAVE_LIBGPS
    gpsmm               *m_gpsd;
    struct gps_data_t   *m_gpsdData;
    TrackPoint          *m_gpsTracking;
    TrackPoint          *m_gpsPrevious;
    Track               *m_gpsTrack;
    TrackSegment        *m_gpsTrackSeg;
#endif
};

#endif
