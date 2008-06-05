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
#include "ClipPainter.h"
#include "Waypoint.h"
#include "BoundingBox.h"
#include "GpxFile.h"
#include "GpsTracking.h"
#include "GpxFileModel.h"

#include <QtCore/QString>
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

//     m_gpsTrack = new Track();
    m_currentGpx = new GpxFile();
    m_fileModel->addFile( m_currentGpx );
    m_tracking = new GpsTracking( m_currentGpx ); 

}

GpsLayer::~GpsLayer()
{
    delete m_currentPosition;
    delete m_tracking;
    delete m_currentGpx;
}

bool GpsLayer::updateGps( const QSize &canvasSize, ViewParams *viewParams,
                          QRegion &reg )
{
    return  m_tracking->update( canvasSize, viewParams, reg );
//     return QRegion();
}

void GpsLayer::paintLayer( ClipPainter *painter, 
                          const QSize &canvasSize, ViewParams *viewParams,
                          const BoundingBox &box )
{
    if ( visible() ) {
        m_currentPosition->draw( painter, canvasSize, 
                                 viewParams );
        QRegion temp; // useless variable
        updateGps( canvasSize, viewParams, temp);
        paintCurrentPosition( painter, canvasSize, viewParams );
    }

    QVector<GpxFile*>::const_iterator it;
    for( it = m_fileModel->allFiles()->constBegin(); 
         it < m_fileModel->allFiles()->constEnd(); ++it ){
        (*it)->draw( painter, canvasSize, viewParams, box );
    }
}

void GpsLayer::paintCurrentPosition( ClipPainter *painter, 
                                     const QSize &canvasSize, 
                                     ViewParams *viewParams )
{
    m_tracking->draw( painter, canvasSize, viewParams );
}

void GpsLayer::changeCurrentPosition( double lat, double lon )
{
    m_currentPosition->setPosition( lat, lon );
}

void GpsLayer::loadGpx( const QString &fileName )
{
    GpxFile *tempFile = new GpxFile( fileName );

//     QTextStream test(stderr);
//     test << *tempFile;

    m_fileModel->addFile( tempFile );
}
