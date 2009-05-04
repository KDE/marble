//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007   Andrew Manson   <g.real.ate@gmail.com>
// Copyright 2009   Eckhart Wörner  <ewoerner@kde.org>
//

#include "PositionTracking.h"

#include "AbstractLayer.h"
#include "ClipPainter.h"
#include "GmlSax.h"
#include "GpxFile.h"
#include "Track.h"
#include "TrackPoint.h"
#include "TrackSegment.h"
#include "PluginManager.h"
#include "PositionProviderPlugin.h"

#include <QtXml/QXmlInputSource>
#include <QtXml/QXmlSimpleReader>

#include <QtCore/QDebug>



using namespace Marble;

PositionTracking::PositionTracking( GpxFile *currentGpx,
                          QObject *parent ) 
     : QObject( parent )
{
    m_gpsCurrentPosition  = new TrackPoint( 0,0 );
    m_gpsPreviousPosition = new TrackPoint( 0,0 );
    m_gpsTracking         = new TrackPoint( 0,0 );

    m_gpsTrack    = new Track();
    currentGpx->addTrack( m_gpsTrack );
    m_gpsTrackSeg = 0;
    m_updateDelay = 0;

    PluginManager pluginManager;
    QList<PositionProviderPlugin *> plugins = pluginManager.createPositionProviderPlugins();
    if ( !plugins.isEmpty() ) {
        // FIXME: not just take the first plugin, but use some configuration setting
        // take the first plugin and delete the rest
        m_positionProvider = plugins.takeFirst();
        qDeleteAll( plugins );
        m_positionProvider->setParent( this );
        m_positionProvider->initialize();
    }
}


PositionTracking::~PositionTracking()
{
    delete m_gpsCurrentPosition;
    delete m_gpsPreviousPosition;
    delete m_gpsTracking;

    delete m_gpsTrack;
}


void PositionTracking::construct( const QSize &canvasSize,
                             ViewParams *viewParams )
{
    //qreal const radius = viewParams->m_radius;

    // FIXME: Review
//#ifdef HAVE_LIBGPS
//    if( !m_gpsd ) {
//        m_currentDraw.clear();
//        return;
//    }
//#endif
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


QRegion PositionTracking::genRegion( const QSize &canvasSize,
                                ViewParams *viewParams )
{
        construct( canvasSize, viewParams );

        QRect temp1( m_currentDraw.boundingRect().toRect() );
        QRect temp2( m_previousDraw.boundingRect().toRect() );

        temp1.adjust( -5, -5, 10, 10 );
        temp2.adjust( -5, -5, 10, 10 );

        return QRegion(temp1).united( QRegion(temp2) );
}

void PositionTracking::notifyPosition( GeoDataCoordinates pos )
{
    qDebug() << "Position from gpsd: " << pos.toString();
}

bool PositionTracking::update(const QSize &canvasSize, ViewParams *viewParams,
                         QRegion &reg) 
{
    if ( m_positionProvider && m_positionProvider->status() ==
        PositionProviderStatusAvailable )
    {
        m_gpsTracking->setPosition( m_positionProvider->position() );

        if (m_gpsTrackSeg == 0 ) {
            m_gpsTrackSeg = new TrackSegment();
            m_gpsTrack->append( m_gpsTrackSeg );
        }
        if (!( m_gpsPreviousPosition->position() ==
            m_gpsTracking->position() ) )
        {
            notifyPosition( m_gpsTracking->position() );
            m_gpsTrackSeg->append( m_gpsPreviousPosition );
            m_gpsPreviousPosition = m_gpsCurrentPosition;
            m_gpsCurrentPosition = new TrackPoint( *m_gpsTracking );
            reg = genRegion( canvasSize, viewParams );
            emit gpsLocation( m_gpsTracking->position() );
            return true;
        } else {
            return false;
        }
    } else {
        if ( m_gpsTrackSeg && m_gpsTrackSeg->count() > 0 ) {
            m_gpsTrackSeg = 0;
        }
    }

    return false;
}

void PositionTracking::draw( ClipPainter *painter,
                        const QSize &canvasSize, 
                        ViewParams *viewParams )
{
    QPoint temp;
    painter->setPen( Qt::black );
    painter->setBrush( Qt::white );
    painter->drawPolygon( m_currentDraw, Qt::OddEvenFill );
}



#include "PositionTracking.moc"
