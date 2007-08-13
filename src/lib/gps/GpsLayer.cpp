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
#include "GpsTracking.h"
#include "GpxFileModel.h"

#include <QtGui/QPixmap>
#include <QtCore/QString>
#include <QtCore/QFile>
#include <QtXml/QXmlInputSource>
#include <QtXml/QXmlSimpleReader>
#include <QtCore/QTime>
#include <QtCore/QDebug>
#include <QtGui/QRegion>
#include <cmath>

GpsLayer::GpsLayer( GpxFileModel *fileModel, QObject *parent ) 
                :AbstractLayer( parent )
{
    m_currentPosition = new Waypoint( 0,0 );
    
    /*
    m_waypoints = new WaypointContainer();
    m_tracks = new TrackContainer();*/
    
//     m_files = new QVector<GpxFile*>();
    m_fileModel = fileModel;
    
    m_gpsTrack = new Track();
    m_tracking = new GpsTracking( m_gpsTrack );

}

GpsLayer::~GpsLayer()
{
}

QRegion GpsLayer::updateGps( const QSize &canvasSize, double radius,
                             Quaternion rotAxis )
{
    return  m_tracking->update( canvasSize, radius, 
                                rotAxis.inverse() );
//     return QRegion();
}

void GpsLayer::paintLayer( ClipPainter *painter, 
                          const QSize &canvasSize, double radius,
                          Quaternion rotAxis, BoundingBox box )
{
    
    Quaternion invRotAxis = rotAxis.inverse();

    m_currentPosition->draw( painter, canvasSize, 
                             radius, invRotAxis );

    
    qDebug() << "just before updating in GpsLayer::paintLayer";
    updateGps( canvasSize, radius, rotAxis );
    paintCurrentPosition( painter, canvasSize, radius, invRotAxis );
    
    
    QPoint *previous=0;
    
    QVector<GpxFile*>::const_iterator it;
    for( it = m_fileModel->allFiles()->constBegin(); 
         it < m_fileModel->allFiles()->constEnd(); ++it ){
        (*it)->draw( painter, canvasSize, radius, invRotAxis, box );
    }
    
    delete previous;
}

void GpsLayer::paintCurrentPosition( ClipPainter *painter, 
                                     const QSize &canvasSize, 
                                     double radius,
                                     Quaternion invRotAxis)
{
    m_tracking->draw( painter, canvasSize, radius, invRotAxis );
}

void GpsLayer::changeCurrentPosition( double lat, double lon )
{
    m_currentPosition->setPosition( lat, lon );
}

void GpsLayer::loadGpx( const QString &fileName )
{
    GpxFile *tempFile = new GpxFile();
//     m_files->append( tempFile );
   
    
    QFile gpxFile( fileName );
    QXmlInputSource gpxInput( &gpxFile );
    
    QXmlSimpleReader gpxReader;
    GpxSax gpxSaxHandler( tempFile );
    
    gpxReader.setContentHandler( &gpxSaxHandler );
    gpxReader.setErrorHandler( &gpxSaxHandler );
    
    gpxReader.parse( &gpxInput );
    
    m_fileModel->addFile( tempFile );
}

