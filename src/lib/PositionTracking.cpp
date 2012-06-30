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

#include "PositionTracking.h"

#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataTrack.h"
#include "GeoDataTreeModel.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "FileManager.h"
#include "MarbleMath.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "PositionProviderPlugin.h"

#include <QtCore/QFile>

namespace Marble
{

class PositionTrackingPrivate
{
 public:
    PositionTrackingPrivate( GeoDataTreeModel *model, PositionTracking *parent ) :
        q( parent ),
        m_treeModel( model ),
        m_currentPositionPlacemark( new GeoDataPlacemark ),
        m_currentTrackPlacemark( new GeoDataPlacemark ),
        m_trackSegments( new GeoDataMultiGeometry ),
        m_document(),
        m_positionProvider( 0 ),
        m_length( 0.0 )
    {
    }

    void updatePosition();

    void updateStatus();

    PositionTracking *const q;

    GeoDataTreeModel *const m_treeModel;

    GeoDataPlacemark *const m_currentPositionPlacemark;
    GeoDataPlacemark *const m_currentTrackPlacemark;
    GeoDataMultiGeometry *const m_trackSegments;
    GeoDataDocument m_document;

    GeoDataCoordinates  m_gpsPreviousPosition;
    GeoDataTrack  *m_currentTrack;

    PositionProviderPlugin* m_positionProvider;

    qreal m_length;
};

void PositionTrackingPrivate::updatePosition()
{
    Q_ASSERT( m_positionProvider != 0 );

    const GeoDataAccuracy accuracy = m_positionProvider->accuracy();
    const GeoDataCoordinates position = m_positionProvider->position();
    const QDateTime timestamp = m_positionProvider->timestamp();

    if ( m_positionProvider->status() == PositionProviderStatusAvailable ) {
        if ( accuracy.horizontal < 250 ) {
            if ( m_currentTrack->size() ) {
                m_length += distanceSphere( m_currentTrack->coordinatesAt( m_currentTrack->size() - 1 ), position );
            }
            m_currentTrack->addPoint( timestamp, position );
        }

        //if the position has moved then update the current position
        if ( m_gpsPreviousPosition != position ) {
            m_currentPositionPlacemark->setCoordinate( position );

            qreal speed = m_positionProvider->speed();
            emit q->gpsLocation( position, speed );
        }
    }
}


void PositionTrackingPrivate::updateStatus()
{
    Q_ASSERT( m_positionProvider != 0 );

    const PositionProviderStatus status = m_positionProvider->status();

    if (status == PositionProviderStatusAvailable) {
        m_currentTrack = new GeoDataTrack;
        m_treeModel->removeFeature( m_currentTrackPlacemark );
        m_trackSegments->append( m_currentTrack );
        m_treeModel->addFeature( &m_document, m_currentTrackPlacemark );
    }

    emit q->statusChanged( status );
}

PositionTracking::PositionTracking( GeoDataTreeModel *model )
     : QObject( model ),
       d( new PositionTrackingPrivate( model, this ) )
{
    d->m_document.setDocumentRole( TrackingDocument );
    d->m_document.setName("Position Tracking");

    // First point is current position
    d->m_currentPositionPlacemark->setName("Current Position");
    d->m_currentPositionPlacemark->setVisible(false);
    d->m_document.append( d->m_currentPositionPlacemark );

    // Second point is position track
    d->m_currentTrack = new GeoDataTrack;
    d->m_trackSegments->append(d->m_currentTrack);

    d->m_currentTrackPlacemark->setGeometry(d->m_trackSegments);
    d->m_currentTrackPlacemark->setName("Current Track");

    GeoDataStyle style;
    GeoDataLineStyle lineStyle;
    QColor transparentRed = oxygenBrickRed4;
    transparentRed.setAlpha( 200 );
    lineStyle.setColor( transparentRed );
    lineStyle.setWidth( 4 );
    style.setLineStyle(lineStyle);
    style.setStyleId("track");

    GeoDataStyleMap styleMap;
    styleMap.setStyleId("map-track");
    styleMap.insert("normal", QString("#").append(style.styleId()));
    d->m_document.addStyleMap(styleMap);
    d->m_document.addStyle(style);
    d->m_document.append( d->m_currentTrackPlacemark );

    d->m_currentTrackPlacemark->setStyleUrl(QString("#").append(styleMap.styleId()));

    d->m_treeModel->addDocument( &d->m_document );
}


PositionTracking::~PositionTracking()
{
    d->m_treeModel->removeDocument( &d->m_document );
    delete d;
}

void PositionTracking::setPositionProviderPlugin( PositionProviderPlugin* plugin )
{
    const PositionProviderStatus oldStatus = status();

    if ( d->m_positionProvider ) {
        delete d->m_positionProvider;
    }

    d->m_positionProvider = plugin;

    if ( d->m_positionProvider ) {
        d->m_positionProvider->setParent( this );
        mDebug() << "Initializing position provider:" << d->m_positionProvider->name();
        connect( d->m_positionProvider, SIGNAL( statusChanged( PositionProviderStatus ) ),
                this, SLOT( updateStatus() ) );
        connect( d->m_positionProvider, SIGNAL( positionChanged( GeoDataCoordinates,GeoDataAccuracy ) ),
                 this, SLOT( updatePosition() ) );

        d->m_positionProvider->initialize();
    }

    emit positionProviderPluginChanged( plugin );

    if ( oldStatus != status() ) {
        emit statusChanged( status() );
    }

    if ( status() == PositionProviderStatusAvailable ) {
        emit gpsLocation( d->m_positionProvider->position(), d->m_positionProvider->speed() );
    }
}

PositionProviderPlugin* PositionTracking::positionProviderPlugin()
{
    return d->m_positionProvider;
}

QString PositionTracking::error() const
{
    return d->m_positionProvider ? d->m_positionProvider->error() : QString();
}


//get speed from provider
qreal PositionTracking::speed() const
{
    return d->m_positionProvider ? d->m_positionProvider->speed() : 0 ;
}

//get direction from provider
qreal PositionTracking::direction() const
{
    return d->m_positionProvider ? d->m_positionProvider->direction() : 0 ;
}

QDateTime PositionTracking::timestamp() const
{
    return d->m_positionProvider ? d->m_positionProvider->timestamp() : QDateTime();
}

bool PositionTracking::trackVisible() const
{
    return d->m_currentTrackPlacemark->isVisible();
}

void PositionTracking::setTrackVisible( bool visible )
{
    d->m_currentTrackPlacemark->setVisible( visible );
    d->m_treeModel->updateFeature( d->m_currentTrackPlacemark );
}

bool PositionTracking::saveTrack(QString& fileName)
{

    if ( !fileName.isEmpty() )
    {
        if ( !fileName.endsWith(".kml", Qt::CaseInsensitive) )
        {
            fileName.append( ".kml" );
        }

        GeoWriter writer;
        //FIXME: a better way to do this?
        writer.setDocumentType( kml::kmlTag_nameSpace22 );

        GeoDataDocument *document = new GeoDataDocument;
        QFileInfo fileInfo( fileName );
        QString name = fileInfo.baseName();
        document->setName( name );
        foreach( const GeoDataStyle &style, d->m_document.styles() ) {
            document->addStyle( style );
        }
        foreach( const GeoDataStyleMap &map, d->m_document.styleMaps() ) {
            document->addStyleMap( map );
        }
        GeoDataFeature *track = new GeoDataFeature( *d->m_currentTrackPlacemark );
        track->setName( "Track " + name );
        document->append( track );

        QFile file( fileName );
        file.open( QIODevice::ReadWrite );
        bool const result = writer.write( &file, document );
        delete document;
        return result;
    }
    return false;
}

void PositionTracking::clearTrack()
{
    d->m_treeModel->removeFeature( d->m_currentTrackPlacemark );
    d->m_currentTrack = new GeoDataTrack;
    d->m_trackSegments->clear();
    d->m_trackSegments->append( d->m_currentTrack );
    d->m_treeModel->addFeature( &d->m_document, d->m_currentTrackPlacemark );
    d->m_length = 0.0;
}

bool PositionTracking::isTrackEmpty() const
{
    if ( d->m_trackSegments->size() < 1 ) {
        return true;
    }

    if ( d->m_trackSegments->size() == 1 ) {
        return ( d->m_currentTrack->size() > 0 );
    }

    return false;
}

qreal PositionTracking::length( qreal planetRadius ) const
{
    return d->m_length * planetRadius;
}

GeoDataAccuracy PositionTracking::accuracy() const
{
    return d->m_positionProvider ? d->m_positionProvider->accuracy() : GeoDataAccuracy();
}

GeoDataCoordinates PositionTracking::currentLocation() const
{
    return d->m_positionProvider ? d->m_positionProvider->position() : GeoDataCoordinates();
}

PositionProviderStatus PositionTracking::status() const
{
    return d->m_positionProvider ? d->m_positionProvider->status() : PositionProviderStatusUnavailable;
}

}

#include "PositionTracking.moc"
