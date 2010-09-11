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
#include "ViewportParams.h"
#include "AbstractProjection.h"

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
    m_updateDelay = 0;
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

    //FIXME: this is a workaround for dealing with NAN values. we need to protect against that in the future
    m_gpsCurrentPosition->setPosition( GeoDataCoordinates ( m_gpsCurrentPosition->position().longitude(GeoDataCoordinates::Degree),
                                       m_gpsCurrentPosition->position().latitude( GeoDataCoordinates::Degree ),
                                       m_gpsCurrentPosition->position().altitude(), GeoDataCoordinates::Degree ) );
    m_gpsPreviousPosition->setPosition( GeoDataCoordinates ( m_gpsPreviousPosition->position().longitude(GeoDataCoordinates::Degree),
                                       m_gpsPreviousPosition->position().latitude( GeoDataCoordinates::Degree ),
                                       m_gpsPreviousPosition->position().altitude(), GeoDataCoordinates::Degree) );


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


QRegion PositionTracking::genRegion( const QSize &canvasSize,
                                ViewParams *viewParams )
{
        construct( canvasSize, viewParams );

        QRegion dirty;
        QList<QPolygonF> items;
        items << m_previousDraw << m_currentDraw;
        foreach(const QPolygonF &polygon, items)
        {
            QRect rect = polygon.boundingRect().toRect();
            rect.adjust( -5, -5, 10, 10 );
            dirty |= rect ;
        }

        dirty |= accuracyIndicatorRegion( viewParams ).toRect();
        return dirty;
}

void PositionTracking::notifyPosition( GeoDataCoordinates pos )
{
    //mDebug() << "Position from gpsd: " << pos.toString();
    // FIXME: Unused parameters should get fixed during refactoring of this class
    Q_UNUSED( pos )
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

bool PositionTracking::update(const QSize &canvasSize, ViewParams *viewParams,
                         QRegion &reg) 
{
    if ( m_positionProvider && m_positionProvider->status() ==
        PositionProviderStatusAvailable )
    {
        m_gpsTracking->setPosition( m_positionProvider->position() );
        m_gpsTracking->setPosition( GeoDataCoordinates ( m_gpsTracking->position().longitude(GeoDataCoordinates::Degree),
                                    m_gpsTracking->position().latitude( GeoDataCoordinates::Degree ),
                                    m_gpsTracking->position().altitude(), GeoDataCoordinates::Degree ) );


        if (m_gpsTrackSeg == 0 ) {
            m_gpsTrackSeg = new TrackSegment();
            m_gpsTrack->append( m_gpsTrackSeg );
        }

        //updateSpeed updates the speed to radians and needs
        //to be multiplied by the radius
        updateSpeed( m_gpsPreviousPosition, m_gpsTracking );
        m_speed *= viewParams->radius();

        //if the position has moved then update the current position
        if ( !( m_gpsPreviousPosition->position() ==
                m_gpsTracking->position() ) )
        {
            notifyPosition( m_gpsTracking->position() );
            m_gpsTrackSeg->append( m_gpsPreviousPosition );
            m_gpsPreviousPosition = m_gpsCurrentPosition;
            m_gpsCurrentPosition = new TrackPoint( *m_gpsTracking );
            reg = genRegion( canvasSize, viewParams );
            // mDebug() << "Dirty region: " << reg;
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
    // Needed to have the current position stick to the gps position
    // when dragging the map. See bug 199858.
    construct( canvasSize, viewParams );

    painter->save();
    
    QRectF accuracyIndicator;
    if ( m_gpsCurrentPosition && accuracy().horizontal > 0 && accuracy().horizontal < 1000 ) {
        // Paint a red circle indicating the position accuracy
        QColor transparentRed = QColor::fromRgb( 226, 8, 0 );
        if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
            transparentRed.setAlpha( 80 );
        } else {
            transparentRed.setAlpha( 40 );
        }

        painter->setPen( transparentRed );
        painter->setBrush( transparentRed );
        accuracyIndicator = accuracyIndicatorRegion( viewParams );
        painter->drawEllipse( accuracyIndicator );
    }
    
    
    painter->setPen( Qt::black );
    painter->setBrush( Qt::white );
    painter->drawPolygon( m_currentDraw );
    painter->restore();
    m_previousDraw = m_currentDraw;
    
    // Make a combined polygon of the previous position and the accuracy indicator circle
    // This is ok as they are painted at the same position. It avoids adding another class
    // member (which had to be static for ABI compatibility)
    if ( !accuracyIndicator.isNull() ) {
        m_previousDraw << accuracyIndicator.topLeft() << accuracyIndicator.topRight();
        m_previousDraw << accuracyIndicator.bottomLeft() << accuracyIndicator.bottomRight();
    }
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
        m_positionProvider->initialize();
    }
}

QString PositionTracking::error() const
{
    return m_positionProvider ? m_positionProvider->error() : QString();
}

GeoDataAccuracy PositionTracking::accuracy() const
{
    return m_positionProvider ? m_positionProvider->accuracy() : GeoDataAccuracy();
}

QRectF PositionTracking::accuracyIndicatorRegion( ViewParams *viewParams ) const
{
    QRectF result;
    if ( m_gpsCurrentPosition ) {
        qreal width = qRound( accuracy().horizontal * viewParams->viewport()->radius() / EARTH_RADIUS );
        if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
            qreal arrowSize = qMax<qreal>( m_currentDraw.boundingRect().width(), m_currentDraw.boundingRect().height() );
            width = qMax<qreal>( width, arrowSize + 10 );
        }
        
        qreal x(0), y(0);
        AbstractProjection * projection = viewParams->viewport()->currentProjection();
        if ( projection->screenCoordinates( m_gpsCurrentPosition->position(), viewParams->viewport(), x, y ) ) {
            result = QRectF( x - width / 2.0, y - width / 2.0, width, width );
        }
    }
    
    return result;
}  

#include "PositionTracking.moc"
