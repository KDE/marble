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

#include "AbstractLayer.h"
#include "ClipPainter.h"
#include "GmlSax.h"
#include "GpxFile.h"
#include "Track.h"
#include "TrackPoint.h"
#include "TrackSegment.h"

#include <QtXml/QXmlInputSource>
#include <QtXml/QXmlSimpleReader>

#include <QtCore/QDebug>

using namespace Marble;

GpsTracking::GpsTracking( GpxFile *currentGpx, TrackingMethod method, 
                          QObject *parent ) 
     :QObject( parent )
{
    m_trackingMethod = method;

    m_gpsCurrentPosition  = new TrackPoint( 0,0 );
    m_gpsPreviousPosition = new TrackPoint( 0,0 );
    m_gpsTracking         = new TrackPoint( 0,0 );

    m_gpsTrack    = new Track();
    currentGpx->addTrack( m_gpsTrack );
    m_gpsTrackSeg = 0;
    m_updateDelay =0;

    //for ip address evaluation
    connect( &host, SIGNAL( done(  bool ) ),
             this,  SLOT( getData( bool ) ) ) ;
    connect( this, SIGNAL(gpsLocation(GeoDataCoordinates)),
             this, SLOT(notifyPosition(GeoDataCoordinates)) );
    m_downloadFinished = false;

#ifdef HAVE_LIBGPS
    m_gpsd     = new gpsmm();
    m_gpsdData = m_gpsd->open( "127.0.0.1", "2947" );
#endif
}


GpsTracking::~GpsTracking()
{
    delete m_gpsCurrentPosition;
    delete m_gpsPreviousPosition;
    delete m_gpsTracking;

    delete m_gpsTrack;

#ifdef HAVE_LIBGPS
    delete m_gpsd;
#endif
}


void GpsTracking::construct( const QSize &canvasSize,
                             ViewParams *viewParams )
{
    //qreal const radius = viewParams->m_radius;

#ifdef HAVE_LIBGPS
    if( !m_gpsd ) {
        m_currentDraw.clear();
        return;
    }
#endif
    QPointF  position;
    QPointF  previousPosition;

    bool draw = false;

    draw = m_gpsCurrentPosition->getPixelPos( canvasSize, viewParams,
                                              &position );

    draw = m_gpsPreviousPosition->getPixelPos( canvasSize, viewParams,
                                               &previousPosition );

    if ( !draw ) {
        m_currentDraw.clear();
        return;
    }

    qreal distance = std::sqrt( AbstractLayer::distance( position,
                                                     previousPosition) );
    if (distance == 0) {
        return;
    }

    QPointF unitVector = ( ( position - previousPosition )
                           / distance );
    // The normal of the unit vector between first and second
    QPointF unitVector2 = QPointF ( -unitVector.y(), unitVector.x() );

    m_previousDraw = m_currentDraw;

    m_currentDraw.clear();
    m_currentDraw << position
                  << ( position - ( unitVector * 9 ) 
                                + ( unitVector2 * 9 ) )
                  << ( position + ( unitVector * 19.0 ) )
                  << ( position - ( unitVector * 9 ) 
                                - ( unitVector2 * 9 ) );
}


QRegion GpsTracking::genRegion( const QSize &canvasSize,
                                ViewParams *viewParams )
{
        construct( canvasSize, viewParams );

        QRect temp1( m_currentDraw.boundingRect().toRect() );
        QRect temp2( m_previousDraw.boundingRect().toRect() );

        temp1.adjust( -5, -5, 10, 10 );
        temp2.adjust( -5, -5, 10, 10 );

        return QRegion(temp1).united( QRegion(temp2) );
}

void GpsTracking::getData( bool error )
{
    if ( !error ) {
        m_data = QString( host.readAll() );
        updateIp();
        m_downloadFinished = true;
    }
}

void GpsTracking::notifyPosition( GeoDataCoordinates pos )
{
    qDebug() << "Position from gpsd: " << pos.toString();
}

void GpsTracking::updateIp( )
{
//         QTextStream out(&gmlFile);
//         gmlFile.write( host.readAll() );
//         out << host.readAll();
//         qDebug() << gmlFile.readAll();
//         qDebug() << host.readAll();
//         
    qreal lon;
    qreal lat;
    QXmlInputSource gmlInput/*( &gmlFile )*/;
    gmlInput.setData( m_data );

    QXmlSimpleReader gmlReader;
    GmlSax gmlSaxHandler( &lon, &lat );

    gmlReader.setContentHandler( &gmlSaxHandler );
    gmlReader.setErrorHandler( &gmlSaxHandler );

    gmlReader.parse( &gmlInput );

    qDebug() << "in the real world" << lon << lat;
    m_gpsCurrentPosition->setPosition( lat, lon );
}

bool GpsTracking::update(const QSize &canvasSize, ViewParams *viewParams,
                         QRegion &reg) 
{
    switch ( m_trackingMethod ) {
    case MobilePhone:
        qDebug("GpsTracking::update - MobilePhone case not handled in %s, line %d",
               __FILE__, __LINE__);
        exit(1); //force fail
        break;

    case IP:

        if ( m_updateDelay > 0 ) {
            --m_updateDelay;
            //removed need for returning empty regions
            //return QRegion();
            return false;
        }

        host.setHost( "api.hostip.info" );
        host.get( "http://api.hostip.info/");
        m_updateDelay = 15000;

        //removed empty return
        //return QRegion();

        return false;
        break;
    case Gps:
#ifndef HAVE_LIBGPS
        Q_UNUSED( canvasSize );
#else
        //m_gpsdData has been successully set
        if ( m_gpsdData != 0 ){
            m_gpsdData =m_gpsd->query( "p" );
            m_gpsTracking->setPosition( m_gpsdData->fix.latitude,
                                        m_gpsdData->fix.longitude,
                                        m_gpsdData->fix.altitude );

            if (m_gpsTrackSeg == 0 ){
                m_gpsTrackSeg = new TrackSegment();
                m_gpsTrack->append( m_gpsTrackSeg );
            }
            if (!( m_gpsPreviousPosition->position() ==
                   m_gpsTracking->position() ) )
            {
                m_gpsTrackSeg->append( m_gpsPreviousPosition );
                m_gpsPreviousPosition = m_gpsCurrentPosition;
                m_gpsCurrentPosition = new TrackPoint( *m_gpsTracking);
                reg = genRegion( canvasSize, viewParams );
                emit gpsLocation( m_gpsTracking->position() );
                return true;
            } else {
                return false;
            }
        } else {

            if ( m_gpsTrackSeg != 0  
                 && (m_gpsTrackSeg->count() > 0 ) )
            {
                m_gpsTrackSeg = 0;
            }
        }

        /*
        construct( canvasSize, radius, invRotAxis );

        QRect temp1(m_currentDraw.boundingRect().toRect());
        QRect temp2(m_previousDraw.boundingRect().toRect());

        temp1.adjust( -5, -5, 10, 10);
        temp2.adjust( -5, -5, 10, 10);

        return QRegion(temp1).united( QRegion(temp2) );
        */

#endif
    }

    return false;
}

void GpsTracking::draw( ClipPainter *painter,
                        const QSize &canvasSize, 
                        ViewParams *viewParams )
{
    QPoint temp;
    switch( m_trackingMethod ){ 
    case IP: 
        if( m_gpsCurrentPosition->getPixelPos( canvasSize,
                                               viewParams,
                                               &temp ) )
        {
            painter->drawEllipse( temp.x(), temp.y(), 10, 10 );
        }
        break;
    case Gps:
        painter->setPen( Qt::black );
        painter->setBrush( Qt::white );
        painter->drawPolygon( m_currentDraw, Qt::OddEvenFill );
        break;
    case MobilePhone:
        break;
    }
}

#include "GpsTracking.moc"
