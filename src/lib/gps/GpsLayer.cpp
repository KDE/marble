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
#include "GpxFile.h"
#include "PositionTracking.h"
#include "GpxFileModel.h"

#include <QtCore/QString>
#include <QtCore/QDebug>
#include <QtGui/QRegion>
#include <cmath>

using namespace Marble;

GpsLayer::GpsLayer( GpxFileModel *fileModel, QObject *parent ) 
                :AbstractLayer( parent )
{
    m_currentPosition = new Waypoint( 0,0 );

    /*
    m_waypoints = new WaypointContainer();
    m_tracks = new TrackContainer();*/

//     m_files = new QVector<GpxFile*>();
//    m_fileModel = fileModel;

//     m_gpsTrack = new Track();
    m_currentGpx = new GpxFile();
//    m_fileModel->addFile( m_currentGpx );
    m_ownedGpxFiles.append( m_currentGpx );
    m_tracking = new PositionTracking( m_currentGpx ); 

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

PositionTracking* GpsLayer::getPositionTracking()
{
    return m_tracking;
}

void GpsLayer::paintLayer( ClipPainter *painter, 
                          const QSize &canvasSize, ViewParams *viewParams )
{
    painter->save();
    if ( visible() ) {
        m_currentPosition->draw( painter, canvasSize, 
                                 viewParams );
        QRegion temp; // useless variable
        updateGps( canvasSize, viewParams, temp);
        paintCurrentPosition( painter, canvasSize, viewParams );
        if( !m_currentGpx ) {
            m_currentGpx = new GpxFile();
            m_currentGpx->addTrack( m_tracking->currentGpsTrack() );
            m_ownedGpxFiles.append( m_currentGpx );
        }
        m_currentGpx->draw( painter, canvasSize, viewParams );
    }

//    const QVector<GpxFile*> * const allFiles = m_fileModel->allFiles();
    QList<GpxFile*>::const_iterator it;
    for( it = m_externalGpxFiles.constBegin();
         it != m_externalGpxFiles.constEnd(); ++it ) {
             if( (*it) != m_currentGpx ) {
                 (*it)->draw( painter, canvasSize, viewParams );
             }
    }

    for( it = m_ownedGpxFiles.constBegin();
         it != m_ownedGpxFiles.constEnd(); ++it ) {
             if( (*it) != m_currentGpx ) {
                 (*it)->draw( painter, canvasSize, viewParams );
             }
    }
    painter->restore();
}

void GpsLayer::paintCurrentPosition( ClipPainter *painter, 
                                     const QSize &canvasSize, 
                                     ViewParams *viewParams )
{
    m_tracking->draw( painter, canvasSize, viewParams );
}

void GpsLayer::changeCurrentPosition( qreal lat, qreal lon )
{
    m_currentPosition->setPosition( lat, lon );
}

void GpsLayer::loadGpx( const QString &fileName )
{
    GpxFile *tempFile = new GpxFile( fileName );

//    m_fileModel->addFile( tempFile );
    m_ownedGpxFiles.append( tempFile );
}

void GpsLayer::addGpxFile( GpxFile* file )
{
//    m_fileModel->addFile( file );
    m_externalGpxFiles.append( file );
}

void GpsLayer::clearModel()
{
    while( !m_ownedGpxFiles.isEmpty() ) {
        GpxFile* file = m_ownedGpxFiles.takeFirst();
        m_currentGpx = 0;
        delete file;
    }

    m_externalGpxFiles.clear();
}
