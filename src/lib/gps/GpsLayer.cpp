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
#include "ClipPainter.h"
#include "WaypointContainer.h"
#include "GpxSax.h"
#include "TrackContainer.h"
#include "TrackPoint.h"
#include "Track.h"
#include "TrackSegment.h"

#include <QtGui/QPixmap>
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtXml/QXmlInputSource>
#include <QtXml/QXmlSimpleReader>
#include <QtCore/QTime>
#include <QtCore/QDebug>
#include <cmath>

GpsLayer::GpsLayer( QObject *parent ) : AbstractLayer( parent )
{
    m_currentPosition = new Waypoint( 0,0 );
    
    m_waypoints = new WaypointContainer();
    m_tracks = new TrackContainer();
    
#ifdef HAVE_LIBGPS
    m_gpsTracking = new TrackPoint( 0,0 );
    m_gpsPrevious = new TrackPoint( 0,0 );
    
    m_gpsTrack = new Track();
    m_gpsTrackSeg = 0;
    
    m_gpsd = new gpsmm();
    m_gpsdData = m_gpsd -> open( "127.0.0.1", "2947" );
#endif
    
   // parseGpx("test.gpx");
}

GpsLayer::~GpsLayer()
{
}

void GpsLayer::updateGps(){
#ifdef HAVE_LIBGPS
    if( m_gpsdData !=0 ){
        m_gpsdData =m_gpsd->query( "p" );
        m_gpsTracking-> setPosition( m_gpsdData->fix.latitude, 
                                 m_gpsdData->fix.longitude );
       
            if (m_gpsTrackSeg ==0 ){
                m_gpsTrackSeg = new TrackSegment();
            }
            if ( !((m_gpsTracking->position().quaternion()) 
                     ==
 (m_gpsPrevious->position().quaternion()))){
                m_gpsTrackSeg->append( m_gpsPrevious );
             }
            m_gpsPrevious = new TrackPoint( m_gpsdData->fix.latitude, 
                                          m_gpsdData->fix.longitude);
            
        
    } else {
        if (m_gpsTrackSeg != 0  && (m_gpsTrackSeg->size() >0)) {
            m_gpsTrack->append( m_gpsTrackSeg );
            m_gpsTrackSeg = 0;
        } 
    }
#endif
}

void GpsLayer::paintLayer(ClipPainter *painter, 
                          const QSize &canvasSize, double radius,
                          Quaternion rotAxis)
{
    
    Quaternion invRotAxis = rotAxis.inverse();

    m_currentPosition->draw( painter, canvasSize, 
                             radius, invRotAxis );
#ifdef HAVE_LIBGPS
    if( m_gpsd != 0 ) {
        paintCurrentPosition( painter, canvasSize, radius,
                              invRotAxis, m_gpsTracking );
    }
    
    if ( m_gpsTrack !=0 ) {
        m_gpsTrack ->draw( painter, canvasSize, radius, invRotAxis );
    }
    
    if ( m_gpsTrackSeg !=0 ) {
        m_gpsTrackSeg->draw( painter, canvasSize, radius, 
                             invRotAxis);
    }
#endif
    QPoint *previous=0;
    
    

    m_waypoints->draw(painter, canvasSize, radius, invRotAxis );
    
    if ( m_tracks != 0 ) {
        m_tracks->draw(painter, canvasSize, radius, invRotAxis);
    }
    
    delete previous;
}

void GpsLayer::paintCurrentPosition( ClipPainter *painter,
                      const QSize &canvasSize, double radius,
                      Quaternion invRotAxis, 
                      AbstractLayerData *point )
{
    QPoint position;
    bool draw = false;
    
    draw = point->getPixelPos( canvasSize, invRotAxis, (int)radius,
                               &position );
   
    if ( draw ) {
        painter->drawEllipse( position.x() -5, position.y() -5,
                              10, 10);
    }
}

void GpsLayer::changeCurrentPosition( double lat, double lon )
{
    m_currentPosition->setPosition( lat, lon );
}

void GpsLayer::loadGpx( const QString &fileName )
{
    QFile gpxFile( fileName );
    QXmlInputSource gpxInput( &gpxFile );
    
    QXmlSimpleReader gpxReader;
    GpxSax gpxSaxHandler( m_waypoints, m_tracks );
    
    gpxReader.setContentHandler( &gpxSaxHandler );
    gpxReader.setErrorHandler( &gpxSaxHandler );
    
    gpxReader.parse( &gpxInput );
}

