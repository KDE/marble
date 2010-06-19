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

#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "ClipPainter.h"
#include "AbstractProjection.h"
#include "MarbleMath.h"
#include "MarbleDebug.h"
#include "MarbleGeometryModel.h"
#include "ViewParams.h"

using namespace Marble;

PositionTracking::PositionTracking( MarbleGeometryModel *geometryModel,
                          QObject *parent ) 
     : QObject( parent ), m_geometryModel(geometryModel), m_positionProvider(0)
{
    m_document     = new GeoDataDocument();

    GeoDataPlacemark placemark;
    GeoDataMultiGeometry multiGeometry;
    GeoDataLineString lineString;

    multiGeometry.append(lineString);
    placemark.setGeometry(multiGeometry);
    m_document->append(placemark);

    m_geometryModel->setGeoDataRoot(m_document);
    connect(this, SIGNAL(gpsLocation(GeoDataCoordinates,qreal)),
            m_geometryModel, SLOT(update()));
}


PositionTracking::~PositionTracking()
{
}


void PositionTracking::construct( const QSize &canvasSize,
                             ViewParams *viewParams )
{
    QPointF position;
    QPointF previousPosition;

    viewParams->currentProjection()->screenCoordinates(m_gpsCurrentPosition,
                                                       viewParams->viewport(),
                                                       position);
    viewParams->currentProjection()->screenCoordinates(m_gpsPreviousPosition,
                                                       viewParams->viewport(),
                                                       previousPosition);

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


void PositionTracking::updateSpeed( GeoDataCoordinates& previous, GeoDataCoordinates next )
{
    //This function makes the assumption that the update stage happens once
    //every second.
    qreal distance = distanceSphere( previous,
                                     next );
    m_speed = distance * 60 * 60;
}

void PositionTracking::setPosition( GeoDataCoordinates position,
                                    GeoDataAccuracy accuracy )
{
    if ( m_positionProvider && m_positionProvider->status() ==
        PositionProviderStatusAvailable )
    {

        GeoDataPlacemark placemark = m_document->features().last();
        GeoDataMultiGeometry *geometry = static_cast<GeoDataMultiGeometry*>(placemark.geometry());
        GeoDataLineString &lineString = static_cast<GeoDataLineString&>(geometry->last());
        lineString.append(position);

        if (m_geometryModel->geoDataRoot() != m_document) {
            mDebug() << "setting geometrymodel";
            m_geometryModel->setGeoDataRoot(m_document);
        }
        mDebug() << "geometry size " << lineString.size();

        //if the position has moved then update the current position
        if ( !( m_gpsCurrentPosition ==
                position ) )
        {

            m_gpsCurrentPosition = position;
            emit gpsLocation( position, m_speed );
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
        updateSpeed( m_gpsPreviousPosition, m_gpsCurrentPosition );
        m_speed *= viewParams->radius();

        //if the position has moved then update the current position
        if ( !( m_gpsPreviousPosition ==
                m_gpsCurrentPosition ) )
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
            m_gpsPreviousPosition = m_gpsCurrentPosition;
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
        connect( m_positionProvider, SIGNAL( positionChanged( GeoDataCoordinates,GeoDataAccuracy ) ),
                 this, SLOT( setPosition( GeoDataCoordinates,GeoDataAccuracy ) ) );
        m_positionProvider->initialize();
    }
}

QString PositionTracking::error() const
{
    return m_positionProvider ? m_positionProvider->error() : QString();
}

#include "PositionTracking.moc"
