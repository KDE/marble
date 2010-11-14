//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007   Andrew Manson   <g.real.ate@gmail.com>
// Copyright 2009   Eckhart Wörner  <ewoerner@kde.org>
// Copyright 2010   Thibaut Gridel  <tgridel@free.fr>
//

#include "PositionMarker.h"

#include "MarbleDebug.h"
#include <QtCore/QRect>
#include <QtGui/QColor>

#include "AbstractProjection.h"
#include "MarbleDataFacade.h"
#include "GeoPainter.h"
#include "PositionTracking.h"
#include "ViewportParams.h"

namespace Marble
{

PositionMarker::PositionMarker ()
    : RenderPlugin(),
      m_isInitialized( false ),
      m_viewport( 0 )
{
}

PositionMarker::~PositionMarker ()
{
}

QStringList PositionMarker::renderPosition() const
{
    return QStringList( "HOVERS_ABOVE_SURFACE" );
}

QString PositionMarker::renderPolicy() const
{
    return "ALWAYS";
}

QStringList PositionMarker::backendTypes() const
{
    return QStringList( "positionmarker" );
}

QString PositionMarker::name() const
{
    return tr( "Position Marker" );
}

QString PositionMarker::guiString() const
{
    return tr( "&Position Marker" );
}

QString PositionMarker::nameId() const
{
    return QString( "positionMarker" );
}

QString PositionMarker::description() const
{
    return tr( "draws a marker at the current position" );
}

QIcon PositionMarker::icon() const
{
    return QIcon();
}


void PositionMarker::initialize()
{
    if ( dataFacade() ) {
        connect( dataFacade()->positionTracking(), SIGNAL( gpsLocation( GeoDataCoordinates,qreal ) ),
                this, SLOT( setPosition( GeoDataCoordinates ) ) );
        m_isInitialized = true;
    }
}

bool PositionMarker::isInitialized() const
{
    return m_isInitialized;
}

void PositionMarker::update()
{
    if ( !m_viewport ) {
        return;
    }
    if( ! ( m_currentPosition == m_previousPosition ) )
    {
        QPointF position;
        QPointF previousPosition;

        m_viewport->currentProjection()->screenCoordinates( m_currentPosition,
                                                            m_viewport,
                                                            position );
        m_viewport->currentProjection()->screenCoordinates( m_previousPosition,
                                                            m_viewport,
                                                            previousPosition );

        // calculate the arrow shape, oriented by the heading
        // and with constant size
        QPointF unitVector = position - previousPosition;
        // check that some screen progress was made
        if( unitVector.x() || unitVector.y() ) {
            // magnitude should be >0
            qreal magnitude = sqrt( ( unitVector.x() * unitVector.x() )
                                    + ( unitVector.y() * unitVector.y() ) );
            unitVector = unitVector / magnitude;
            QPointF unitVector2 = QPointF ( -unitVector.y(), unitVector.x() );
            QPointF relativeLeft = - ( unitVector * 9   ) + ( unitVector2 * 9 );
            QPointF relativeRight = - ( unitVector * 9 ) - ( unitVector2 * 9 );
            QPointF relativeTip =  unitVector * 19.0 ;

            m_arrow.clear();
            m_arrow << position
                    << position + relativeLeft
                    << position + relativeTip
                    << position + relativeRight;

            m_dirtyRegion = QRegion();
            m_dirtyRegion += ( m_arrow.boundingRect().toRect() );
            m_dirtyRegion += ( m_previousArrow.boundingRect().toRect() );
        }
    }
}

bool PositionMarker::render( GeoPainter *painter,
                           ViewportParams *viewport,
                           const QString& renderPos,
                           GeoSceneLayer * layer )
{
    Q_UNUSED( layer )
    if (renderPosition().contains(renderPos))
    {
        if ( m_viewport != viewport ) {
            m_viewport = viewport;
        }
        update();
        painter->save();
        painter->autoMapQuality();

        GeoDataAccuracy accuracy = dataFacade()->positionTracking()->accuracy();
        if ( accuracy.horizontal > 0 && accuracy.horizontal < 1000 ) {
            // Paint a red circle indicating the position accuracy
            painter->setPen( Qt::transparent );
            QColor transparentRed = oxygenBrickRed4;
            int width = qRound( accuracy.horizontal * viewport->radius() / EARTH_RADIUS );
            if ( MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen ) {
                transparentRed.setAlpha( 80 );
                int arrowSize = qMax<int>( m_arrow.boundingRect().width(), m_arrow.boundingRect().height() );
                width = qMax<int>( width, arrowSize + 10 );
            } else {
                transparentRed.setAlpha( 40 );
            }

            painter->setBrush( transparentRed );
            painter->drawEllipse( m_currentPosition, width, width );
        }

        painter->setPen( Qt::black );
        painter->setBrush( Qt::white );
        painter->drawPolygon( m_arrow );

        painter->restore();
        m_previousArrow = m_arrow;
    }
    return true;
}

void PositionMarker::setPosition( const GeoDataCoordinates &position )
{
    m_previousPosition = m_currentPosition;
    m_currentPosition = position;
    if ( m_viewport->viewLatLonAltBox().contains( m_currentPosition ) )
    {
        emit repaintNeeded( m_dirtyRegion );
    }
}

qreal PositionMarker::zValue() const
{
    return 1.0;
}

}

Q_EXPORT_PLUGIN2( PositionMarker, Marble::PositionMarker )

#include "PositionMarker.moc"
