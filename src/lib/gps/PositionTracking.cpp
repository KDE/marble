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
#include "MarbleMath.h"
#include "MarbleDebug.h"
#include "MarbleGeometryModel.h"

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
