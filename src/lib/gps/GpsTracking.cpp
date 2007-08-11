//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007   Andrew Manson   <g.real.ate@gmail.com>
//

#include "GpsTracking.h"
#include "TrackPoint.h"
#include "Track.h"
#include "TrackSegment.h"
#include "AbstractLayer/AbstractLayer.h"

#include <QDebug>

GpsTracking::GpsTracking( Track *track, TrackingMethod method)
{
    
    m_gpsCurrentPosition = new TrackPoint( 0,0 );
    m_gpsPreviousPosition = new TrackPoint( 0,0 );
    m_gpsTracking           = new TrackPoint( 0,0 );
    
    m_gpsTrack = track;
    m_gpsTrackSeg = 0;
    
#ifdef HAVE_LIBGPS
    m_gpsd = new gpsmm();
    m_gpsdData = m_gpsd -> open( "127.0.0.1", "2947" );
    
#endif
}


GpsTracking::~GpsTracking()
{
}

void GpsTracking::construct( const QSize &canvasSize, double radius,
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
    if ( !draw ) {
        return;
    }
    
    QPointF unitVector = ( position - previousPosition ) 
   / ( sqrt( AbstractLayer::distance( position, previousPosition) ) );
    //the perpindicular of the unit vector between first and second
    QPointF unitVector2 = QPointF ( -unitVector.y(), unitVector.x());
    
    qDebug()<< unitVector << unitVector2;
    
    previousDraw = currentDraw;
    
    currentDraw.clear();
    currentDraw   << position
                  << ( position - ( unitVector * 9 ) 
                                + ( unitVector2 * 9 ) )
                  << ( position + ( unitVector * 19.0 ) )
                  << ( position - ( unitVector * 9 ) 
                                - ( unitVector2 * 9 ) );
}

QRegion GpsTracking::update(const QSize &canvasSize, double radius,
                            Quaternion invRotAxis) 
{
#ifdef HAVE_LIBGPS
    if( m_gpsdData !=0 ){
        m_gpsdData =m_gpsd->query( "p" );
        
        m_gpsTracking ->setPosition( m_gpsdData->fix.latitude,
                                     m_gpsdData->fix.longitude );
       
        if (m_gpsTrackSeg == 0 ){
            m_gpsTrackSeg = new TrackSegment();
        }
        if ( !( m_gpsPreviousPosition->position()  
                == m_gpsTracking->position() ) )
        {
            m_gpsTrackSeg->append( m_gpsPreviousPosition );
            m_gpsPreviousPosition = m_gpsCurrentPosition;
            m_gpsCurrentPosition = new TrackPoint( *m_gpsTracking );
        }
            
        
    } else {
        
        if (m_gpsTrackSeg != 0  && (m_gpsTrackSeg->size() >0)) {
            m_gpsTrack->append( m_gpsTrackSeg );
            m_gpsTrackSeg = 0;
        } 
    }
    
    construct( canvasSize, radius, invRotAxis );
    
    QRegion temp1(currentDraw.boundingRect().toRect());
    QRegion temp2(previousDraw.boundingRect().toRect());
    
    return temp1.united( temp2 );
    
#endif
    return QRegion();

}

void GpsTracking::draw( ClipPainter *painter,
                        const QSize &canvasSize, double radius,
                        Quaternion invRotAxis )
{
    painter->setPen( Qt::black );
    painter->setBrush( Qt::white );
    painter->drawPolygon( currentDraw, Qt::OddEvenFill );
    
    if( m_gpsTrackSeg != 0) {
       m_gpsTrackSeg -> draw ( painter, canvasSize, radius, 
                               invRotAxis );
    
    }
    
}


