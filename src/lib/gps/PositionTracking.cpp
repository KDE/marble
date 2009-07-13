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
#include "MarbleMath.h"
#include "ViewParams.h"

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
    } else {
        // useful when there is no plugin available.
        m_positionProvider = 0;
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
    //qDebug() << "Position from gpsd: " << pos.toString();
}

void PositionTracking::updateSpeed( TrackPoint* previous, TrackPoint* next )
{
    //This function makes the asumption that the update stage happens once
    //every second.
    qreal distance = distanceSphere( previous->position().longitude(),
                                     previous->position().latitude(),
                                     next->position().longitude(),
                                     next->position().latitude() );
    m_speed = distance * 60 * 60;
}

bool PositionTracking::update(const QSize &canvasSize, ViewParams *viewParams,
                         QRegion &reg) 
{
    if ( m_positionProvider && m_positionProvider->status() ==
        PositionProviderStatusAvailable )
    {
        m_gpsTracking->setPosition( m_positionProvider->position() );
        m_gpsTracking->setPosition( m_gpsTracking->position().latitude(GeoDataCoordinates::Degree),
                                       m_gpsTracking->position().longitude( GeoDataCoordinates::Degree ) );


        if (m_gpsTrackSeg == 0 ) {
            m_gpsTrackSeg = new TrackSegment();
            m_gpsTrack->append( m_gpsTrackSeg );
        }

        //updateSpeed updates the speed to radians and needs
        //to be multiplied by the radius
        updateSpeed( m_gpsPreviousPosition, m_gpsTracking );
        m_speed *= viewParams->radius();

        //if the position has not moved then update the current position
        if (!( m_gpsPreviousPosition->position() ==
            m_gpsTracking->position() ) )
        {
            notifyPosition( m_gpsTracking->position() );
            m_gpsTrackSeg->append( m_gpsPreviousPosition );
            m_gpsPreviousPosition = m_gpsCurrentPosition;
            m_gpsCurrentPosition = new TrackPoint( *m_gpsTracking );
            reg = genRegion( canvasSize, viewParams );
            emit gpsLocation( m_gpsTracking->position(), m_speed );
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
    Q_UNUSED( canvasSize )
    Q_UNUSED( viewParams )

    QPointF position;
    QPointF previousPosition;

    //FIXME: this is a workaround for dealing with NAN values. we need to protect against that in the future
    m_gpsCurrentPosition->setPosition( m_gpsCurrentPosition->position().latitude(GeoDataCoordinates::Degree),
                                       m_gpsCurrentPosition->position().longitude( GeoDataCoordinates::Degree ) );
    m_gpsPreviousPosition->setPosition( m_gpsPreviousPosition->position().latitude(GeoDataCoordinates::Degree),
                                       m_gpsPreviousPosition->position().longitude( GeoDataCoordinates::Degree ) );


    m_gpsCurrentPosition->getPixelPos( canvasSize, viewParams, &position );
    m_gpsPreviousPosition->getPixelPos( canvasSize, viewParams, &previousPosition );


    QPointF unitVector = ( position - previousPosition  ) ;

    if( unitVector.x() || unitVector.y() ) {
        qreal magnitude = sqrt( (unitVector.x() * unitVector.x() )
                          + ( unitVector.y() * unitVector.y() ) );
        unitVector = unitVector / magnitude;
        QPointF unitVector2 = QPointF ( -unitVector.y(), unitVector.x() );
        m_relativeLeft = ( position - ( unitVector * 9   ) + ( unitVector2 * 9 ) ) - position ;
        m_relativeRight = ( position - ( unitVector * 9 ) - ( unitVector2 * 9 ) ) - position;
        m_relativeTip = ( position + ( unitVector * 19.0 ) ) - position;
    }

    QPolygonF arrow;

    arrow   << position
            << position + m_relativeLeft
            << position + m_relativeTip
            << position + m_relativeRight;

    QPoint temp;
    painter->save();
    painter->setPen( Qt::black );
    painter->setBrush( Qt::white );
    painter->drawPolygon( arrow );
    painter->restore();
}



#include "PositionTracking.moc"
