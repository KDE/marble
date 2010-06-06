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
#include "GpxFile.h"
#include "Track.h"
#include "TrackPoint.h"
#include "TrackSegment.h"
#include "MarbleMath.h"
#include "MarbleDebug.h"
#include "ViewParams.h"

#include <QtXml/QXmlInputSource>
#include <QtXml/QXmlSimpleReader>

using namespace Marble;

PositionTracking::PositionTracking( GpxFile *currentGpx,
                          QObject *parent ) 
     : QObject( parent ), m_positionProvider(0)
{
    m_gpsCurrentPosition  = new TrackPoint( 0,0 );
    m_gpsPreviousPosition = new TrackPoint( 0,0 );
    m_gpsTracking         = new TrackPoint( 0,0 );

    m_gpsTrack    = new Track();
    currentGpx->addTrack( m_gpsTrack );
    m_gpsTrackSeg = 0;
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
    QPointF position;
    QPointF previousPosition;

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

    m_currentDraw.clear();
    m_currentDraw << position
            << position + m_relativeLeft
            << position + m_relativeTip
            << position + m_relativeRight;
}


void PositionTracking::updateSpeed( TrackPoint* previous, TrackPoint* next )
{
    //This function makes the assumption that the update stage happens once
    //every second.
    qreal distance = distanceSphere( previous->position().longitude(),
                                     previous->position().latitude(),
                                     next->position().longitude(),
                                     next->position().latitude() );
    m_speed = distance * 60 * 60;
}

void PositionTracking::setPosition( GeoDataCoordinates position,
                                    GeoDataAccuracy accuracy )
{
    if ( m_positionProvider && m_positionProvider->status() ==
        PositionProviderStatusAvailable )
    {
        m_gpsTracking->setPosition( position );
        m_gpsTracking->setPosition( GeoDataCoordinates ( m_gpsTracking->position().longitude(GeoDataCoordinates::Degree),
                                                         m_gpsTracking->position().latitude( GeoDataCoordinates::Degree ),
                                                         m_gpsTracking->position().altitude(), GeoDataCoordinates::Degree ) );


        if (m_gpsTrackSeg == 0 ) {
            m_gpsTrackSeg = new TrackSegment();
            m_gpsTrack->append( m_gpsTrackSeg );
        }

        //if the position has moved then update the current position
        if ( !( m_gpsPreviousPosition->position() ==
                m_gpsTracking->position() ) )
        {
            m_gpsTrackSeg->append( m_gpsPreviousPosition );
            m_gpsPreviousPosition = m_gpsCurrentPosition;
            m_gpsCurrentPosition = new TrackPoint( *m_gpsTracking );
            emit gpsLocation( m_gpsTracking->position(), m_speed );
        }
    } else {
        if ( m_gpsTrackSeg && m_gpsTrackSeg->count() > 0 ) {
            m_gpsTrackSeg = 0;
        }
    }
}

bool PositionTracking::update(const QSize &canvasSize, ViewParams *viewParams,
                         QRegion &reg) 
{
    if ( m_positionProvider && m_positionProvider->status() ==
        PositionProviderStatusAvailable )
    {
        //updateSpeed updates the speed to radians and needs
        //to be multiplied by the radius
        updateSpeed( m_gpsPreviousPosition, m_gpsTracking );
        m_speed *= viewParams->radius();

        //if the position has moved then update the current position
        if ( !( m_gpsPreviousPosition->position() ==
                m_gpsTracking->position() ) )
        {
            construct( canvasSize, viewParams );

            QList<QPolygonF> items;
            items << m_previousDraw << m_currentDraw;
            foreach(const QPolygonF &polygon, items)
            {
                QRect rect = polygon.boundingRect().toRect();
                rect.adjust( -5, -5, 10, 10 );
                reg |= rect ;
            }

            return true;
        } else {
            return false;
        }
    }
    return false;
}

void PositionTracking::draw( ClipPainter *painter,
                        const QSize &canvasSize, 
                        ViewParams *viewParams )
{
    // Needed to have the current position stick to the gps position
    // when dragging the map. See bug 199858.
    construct( canvasSize, viewParams );

    painter->save();
    painter->setPen( Qt::black );
    painter->setBrush( Qt::white );
    painter->drawPolygon( m_currentDraw );
    painter->restore();
    m_previousDraw = m_currentDraw;
}

void PositionTracking::setPositionProviderPlugin( PositionProviderPlugin* plugin )
{
    if ( m_positionProvider ) {
        m_positionProvider->deleteLater();
    }

    m_positionProvider = plugin;

    if ( m_positionProvider ) {
        m_positionProvider->setParent( this );
        mDebug() << "Initializing position provider:" << m_positionProvider->name();
        connect( m_positionProvider, SIGNAL( statusChanged( PositionProviderStatus ) ),
                this, SIGNAL( statusChanged(PositionProviderStatus ) ) );
        connect( m_positionProvider, SIGNAL(positionChanged(GeoDataCoordinates,GeoDataAccuracy)),
                 this, SLOT(setPosition(GeoDataCoordinates,GeoDataAccuracy)));
        m_positionProvider->initialize();
    }
}

QString PositionTracking::error() const
{
    return m_positionProvider ? m_positionProvider->error() : QString();
}

#include "PositionTracking.moc"
