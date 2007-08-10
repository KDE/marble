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
#include "BoundingBox.h"
#include "GpxFile.h"

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
    
    /*
    m_waypoints = new WaypointContainer();
    m_tracks = new TrackContainer();*/
    
    m_files = new QVector<GpxFile*>();
    
#ifdef HAVE_LIBGPS
    
    m_gpsCurrentPosition = new TrackPoint( 0,0 );
    m_gpsPreviousPosition = new TrackPoint( 0,0 );
    m_gpsTracking           = new TrackPoint( 0,0 );
    
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
        
        qDebug()<< m_gpsTracking->toString()
                << m_gpsPreviousPosition->toString();
        
        m_gpsTracking ->setPosition( m_gpsdData->fix.latitude,
                                     m_gpsdData->fix.longitude );
       
        if (m_gpsTrackSeg == 0 ){
            m_gpsTrackSeg = new TrackSegment();
        }
        
        qDebug("checking position");
        //if the current point has moved
        qDebug()<< m_gpsTracking->toString()
                << m_gpsPreviousPosition->toString();
        if ( !( m_gpsPreviousPosition->position()  
             == m_gpsTracking->position() ) )
        {
            qDebug()<<"position changed";
            m_gpsTrackSeg->append( m_gpsPreviousPosition );
            qDebug()<< "previous pointer" <<m_gpsPreviousPosition;
            m_gpsPreviousPosition = m_gpsCurrentPosition;
            
            qDebug()<< "previous pointer" <<m_gpsPreviousPosition;
            m_gpsCurrentPosition = new TrackPoint( *m_gpsTracking );
        }
        qDebug() << m_gpsPreviousPosition << m_gpsTracking;
            
        
    } else {
        if (m_gpsTrackSeg != 0  && (m_gpsTrackSeg->size() >0)) {
            m_gpsTrack->append( m_gpsTrackSeg );
            m_gpsTrackSeg = 0;
        } 
    }
#endif
}

void GpsLayer::paintLayer( ClipPainter *painter, 
                          const QSize &canvasSize, double radius,
                          Quaternion rotAxis, BoundingBox box )
{
    
    Quaternion invRotAxis = rotAxis.inverse();

    m_currentPosition->draw( painter, canvasSize, 
                             radius, invRotAxis );
#ifdef HAVE_LIBGPS
    if( m_gpsd != 0 ) {
        paintCurrentPosition( painter, canvasSize, radius, 
                              invRotAxis );
    }
    
    if ( m_gpsTrack !=0 ) {
         m_gpsTrack ->draw( painter, canvasSize, radius, invRotAxis);
    }
    
    if ( m_gpsTrackSeg !=0 ) {
         m_gpsTrackSeg->draw( painter, canvasSize, radius, 
                              invRotAxis);
    }
#endif
    QPoint *previous=0;
    
    

//     m_waypoints->draw(painter, canvasSize, radius, invRotAxis );
    /*
    if ( m_tracks != 0 ) {
        m_tracks->draw(painter, canvasSize, radius, invRotAxis, box);
    }*/
    
    QVector<GpxFile*>::const_iterator it;
    for( it = m_files->constBegin(); it < m_files->constEnd(); ++it ){
        (*it)->draw( painter, canvasSize, radius, invRotAxis, box );
    }
    
    delete previous;
}

void GpsLayer::paintCurrentPosition( ClipPainter *painter,
                      const QSize &canvasSize, double radius,
                      Quaternion invRotAxis )
{
    
    QPointF position;
    QPointF previousPosition;
    
    bool draw = false;
    
    draw = m_gpsCurrentPosition -> getPixelPos( canvasSize,
                                                invRotAxis,
                                             (int)radius, &position );
    
    draw = m_gpsPreviousPosition -> getPixelPos( canvasSize, 
                                                 invRotAxis,
                                                 (int)radius, 
                                                 &previousPosition );
    
    qDebug()<< m_gpsPreviousPosition->toString();
    qDebug()<< m_gpsCurrentPosition->toString();
    qDebug("-----");
    
    QPointF unitVector = ( position - previousPosition ) 
                  / ( sqrt( distance( position, previousPosition) ) );
    //the perpindicular of the unit vector between first and second
    QPointF unitVector2 = QPointF ( -unitVector.y(), unitVector.x());
    
    qDebug() << unitVector << QPointF(0.0,0.0);
    qDebug() << sqrt(distance(unitVector, QPointF(0.0,0.0))) ;
    
    QPolygonF arrow;
    arrow   << position
            << ( position - ( unitVector * 9 ) + ( unitVector2 * 9 ) )
            << ( position + ( unitVector * 19.0 ) )
            << position - ( unitVector * 9 ) - ( unitVector2 * 9 );
    
    qDebug() << arrow;
    if ( draw ) {
        painter->setPen( Qt::black );
        painter->setBrush( Qt::white );
        painter->drawPolygon( arrow, Qt::OddEvenFill );
    }
}

void GpsLayer::changeCurrentPosition( double lat, double lon )
{
    m_currentPosition->setPosition( lat, lon );
}

void GpsLayer::loadGpx( const QString &fileName )
{
    GpxFile *tempFile = new GpxFile();
    m_files->append( tempFile );
    
    QFile gpxFile( fileName );
    QXmlInputSource gpxInput( &gpxFile );
    
    QXmlSimpleReader gpxReader;
    GpxSax gpxSaxHandler( tempFile );
    
    gpxReader.setContentHandler( &gpxSaxHandler );
    gpxReader.setErrorHandler( &gpxSaxHandler );
    
    gpxReader.parse( &gpxInput );
}

