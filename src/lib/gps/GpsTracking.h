//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007   Andrew Manson   <g.real.ate@gmail.com>
//

#ifndef GPSTRACKING_H
#define GPSTRACKING_H

#include <QtGui/QRegion>
#include <QtGui/QPolygonF>
#include <QtCore/QObject>
#include <QtCore/QTemporaryFile>

#include <QtNetwork/QHttp>
#include "ClipPainter.h"
#include "Quaternion.h"

#include "config-libgps.h"


#ifdef HAVE_LIBGPS
#include <libgpsmm.h>
#endif

class Waypoint;
class TrackPoint;
class Track;
class TrackSegment;
class GpxFile;

/**
	@author Andrew Manson <g.real.ate@gmail.com>
*/
class GpsTracking : public QObject 
{
    Q_OBJECT
            
public:
    
    enum TrackingMethod { Gps, IP, MobilePhone };
    
    explicit GpsTracking( GpxFile *currentGpx,
                          TrackingMethod method = Gps, 
                          QObject *parent = 0 );
    ~GpsTracking();

    
    QRegion update(const QSize &canvasSize, double radius,
                   Quaternion invRotAxis);
    
    void construct ( const QSize &canvasSize, double radius,
                     Quaternion invRotAxis );
    
    void draw( ClipPainter *painter,
                const QSize &canvasSize, double radius,
                Quaternion invRotAxis );
    
    
 public slots:
    void  updateIp( );
    void  getData( bool error );

 private:
    //used to draw the arrow in gps tracking
    GpxFile             *m_currentGpx;
    QPolygonF           currentDraw;
    QPolygonF           previousDraw;
    QPointF             m_previousDistancePosition;
    
    //used to get info from ip address
    QTemporaryFile gmlFile;
    bool                m_downloadFinished;
    
    
    TrackingMethod      m_trackingMethod;
    
   
    Waypoint            *m_currentPosition;
    TrackPoint          *m_gpsCurrentPosition;
    TrackPoint          *m_gpsPreviousPosition;

#ifdef HAVE_LIBGPS
    gpsmm               *m_gpsd;
    struct gps_data_t   *m_gpsdData;
#endif
    
    TrackPoint          *m_gpsTracking;
    Track               *m_gpsTrack;
    TrackSegment        *m_gpsTrackSeg;

    int                 m_updateDelay;
    QTemporaryFile      m_tempFile;
    QString             m_data;
    
    QHttp host;


};


#endif
