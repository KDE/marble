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


#include "config-libgps.h" // krazy:exclude=includes


#ifdef HAVE_LIBGPS
#include <libgpsmm.h>
#endif

namespace Marble
{

class ClipPainter;
class GpxFile;
class Track;
class TrackPoint;
class TrackSegment;
class ViewParams;
class Waypoint;
class GeoDataCoordinates;

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

    /**
     * @brief update the gps object
     * 
     * this function is designed to be polled at a regular interval so
     * that the gps object can update its position and construct the 
     * new polygon that needs to be painted to screen. 
     * 
     * @param canvasSize current size of view
     * @param radius current radius of globe
     * @param invRotAxis current Quaternion representation of globe.
     * @param reg the region on screen that needs to be re-painted
     *            following this update
     * @return @c true the region of the screen needs to be repainted
     *         @c false the gps object was not updated so no repaint
     *                  necessary.
     */
    bool update(const QSize &canvasSize, ViewParams *viewParams,
                QRegion &reg);

    void construct ( const QSize &canvasSize,
                     ViewParams *viewParams );
    /**
     * generates the region of the view that needs to be updated when 
     * the position of the gps changes. it is a union of the old 
     * position with the new one. 
     * 
     * @param canvasSize current size of view. 
     * @param radius    current radius
     * @param invRotAxis current Quaternion representation of globe.
     * @return QRegion representation of where on the view needs to be
     *         updated.
     */
    QRegion     genRegion( const QSize &canvasSize, 
                           ViewParams *viewParams );

    void draw( ClipPainter *painter,
                const QSize &canvasSize, 
                ViewParams *viewParams );


 public slots:
    void  updateIp( );
    void  getData( bool error );
    void  notifyPosition( GeoDataCoordinates );

Q_SIGNALS:
    void  gpsLocation( GeoDataCoordinates );

 private:
    //used to draw the arrow in gps tracking
    GpxFile             *m_currentGpx;
    QPolygonF           m_currentDraw;
    QPolygonF           m_previousDraw;
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

}

#endif
