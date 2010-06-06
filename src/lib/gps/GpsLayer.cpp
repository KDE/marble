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
#include "MarbleDebug.h"
#include <QtGui/QRegion>
#include <cmath>

using namespace Marble;

GpsLayer::GpsLayer( GpxFileModel *fileModel,
                    PositionTracking *positionTracking,
                    QObject *parent )
                :AbstractLayer( parent )
{
    m_currentPosition = new Waypoint( 0,0 );

    /*
    m_waypoints = new WaypointContainer();
    m_tracks = new TrackContainer();*/

//     m_files = new QVector<GpxFile*>();
    m_fileModel = fileModel;

    m_tracking = positionTracking;

}

GpsLayer::~GpsLayer()
{
    // leaks m_fileModel, see comment in clearModel()
    delete m_currentPosition;
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
        updateGps( canvasSize, viewParams, temp );
        paintCurrentPosition( painter, canvasSize, viewParams );
    }

    const QVector<GpxFile*> * const allFiles = m_fileModel->allFiles();
    QVector<GpxFile*>::const_iterator it;
    for( it = allFiles->constBegin();
         it != allFiles->constEnd(); ++it ) {
             (*it)->draw( painter, canvasSize, viewParams );
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

void GpsLayer::clearModel()
{
    // Here is a know memory leak, because m_fileModel does not get
    // deleted in the destructor.
    // It is recommended not to touch the code because any attempt to fix it
    // would create a new leak. This stuff is conceptually broken.
    delete m_fileModel;
    m_fileModel = 0;
    m_fileModel = new GpxFileModel();
}
