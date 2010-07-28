//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007   Andrew Manson   <g.real.ate@gmail.com>
// Copyright 2009   Eckhart Wörner  <ewoerner@kde.org>
// Copyright 2010   Thibaut Gridel  <tgridel@free.fr>
//

#include "PositionTracking.h"

#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "AbstractProjection.h"
#include "FileManager.h"
#include "MarbleMath.h"
#include "MarbleDebug.h"

using namespace Marble;

PositionTracking::PositionTracking( FileManager *fileManager,
                          QObject *parent ) 
     : QObject( parent ), m_fileManager(fileManager), m_positionProvider(0)
{
    m_document     = new GeoDataDocument();
    m_document->setName("Position Tracking");

    GeoDataPlacemark *placemark = new GeoDataPlacemark;
    GeoDataMultiGeometry *multiGeometry = new GeoDataMultiGeometry;
    GeoDataLineString *lineString = new GeoDataLineString;

    multiGeometry->append(lineString);
    placemark->setGeometry(multiGeometry);
    m_document->append(placemark);

    m_fileManager->addGeoDataDocument(m_document);
}


PositionTracking::~PositionTracking()
{
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
    Q_UNUSED( accuracy );
    if ( m_positionProvider && m_positionProvider->status() ==
        PositionProviderStatusAvailable )
    {

        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(m_document->child(m_document->size()-1));
        GeoDataMultiGeometry *geometry = static_cast<GeoDataMultiGeometry*>(placemark->geometry());
        GeoDataLineString *lineString = static_cast<GeoDataLineString*>(geometry->child(geometry->size()-1));
        lineString->append(position);

        //if the position has moved then update the current position
        if ( !( m_gpsCurrentPosition ==
                position ) )
        {

            updateSpeed( m_gpsCurrentPosition, position );
            m_gpsCurrentPosition = position;
            emit gpsLocation( position, m_speed );
        }
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
