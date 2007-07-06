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
#include "WaypointContainer.h"
#include "GpxSax.h"

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
    m_gpsTracking = new Waypoint( 0,0 );
    m_waypoints = new WaypointContainer();
    
#ifdef HAVE_LIBGPS
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
    }
#endif
}

void GpsLayer::paintLayer(ClipPainter *painter, 
                          const QSize &canvasSize, double radius,
                          Quaternion rotAxis)
{
   // painter->setRenderHint(QPainter::Antialiasing, true);
    Quaternion invRotAxis = rotAxis.inverse();

    paint( painter, canvasSize, radius, invRotAxis, 
           m_currentPosition );
#ifdef HAVE_LIBGPS
    if( m_gpsd!=0 ) {
        paint( painter, canvasSize, radius, invRotAxis, 
               m_gpsTracking );
    }
#endif
    QPoint *previous=0;
    
    QTime t;
    t.start();

    WaypointContainer::const_iterator it;
    for( it = m_waypoints->begin(); it < m_waypoints->constEnd();
         it++ )
    {
        previous = paint( painter, canvasSize, radius, invRotAxis, 
              (*it) , previous);
    }
    
    delete previous;
    qDebug("Time elapsed: %d ms", t.elapsed());
}

QPoint * GpsLayer::paint( ClipPainter *painter,
                      const QSize &canvasSize, double radius,
                      Quaternion invRotAxis, 
                      AbstractLayerData *point, 
                      QPoint *previous )
{
    QPoint position;
    bool draw = false;
    
    draw = getPixelPosFromGeoPoint( point->position(),
                                    canvasSize, invRotAxis, 
                                    (int)radius, &position );
   
    if ( previous == 0 ) {
        painter->drawPixmap( position ,
                            point->symbolPixmap() );
        previous = new QPoint(position);
        *previous = position;
        return previous;
    }

   // qDebug() << ( pow( ( position.x() - previous->x() ), 2) 
     //       + pow( ( position.y() - previous->y() ), 2 ) );
    if ( (( position.x() - previous->x() ) 
           * ( position.x() - previous->x() )) 
         
        + (( position.y() - previous->y() ) 
                * ( position.y() - previous->y() ))  
        < 25.0 )
    {
        draw = false;
    }

    if ( draw ) {
     //   painter->drawPixmap( position ,
     //                        point->symbolPixmap() );
        painter->drawEllipse( position.x(), position.y(), 3, 3 );
        *previous= position;
    }
    return previous;
}

void GpsLayer::paint(ClipPainter *painter, 
                     const QSize &canvasSize, 
                     double radius, Quaternion invRotAxis, 
                     AbstractLayerData *point)
{
    QPoint position;
    bool draw = false;
    
    draw = getPixelPosFromGeoPoint( point->position(),
                                    canvasSize, invRotAxis, 
                                    (int)radius, &position );

    if ( draw ) {
        painter->drawPixmap( position ,
                point->symbolPixmap() );
    }
}

void GpsLayer::changeCurrentPosition( double lat, double lon )
{
    m_currentPosition->setPosition( lat, lon );
}

void GpsLayer::loadGpx( const QString &fileName )
{
    qDebug("trying to parse at least");
    QFile gpxFile( fileName );
    QXmlInputSource gpxInput( &gpxFile );
    
    if(gpxFile.exists()){
        qDebug("its ther!!!!!!!!!!!!!!");
    }
    
    QXmlSimpleReader gpxReader;
    GpxSax gpxSaxHandler( m_waypoints );
    
    gpxReader.setContentHandler( &gpxSaxHandler );
    
    gpxReader.parse( gpxInput );
}

