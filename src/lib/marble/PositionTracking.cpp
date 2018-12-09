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
#include "GeoDataMultiTrack.h"
#include "GeoDataPlacemark.h"
#include "GeoDataParser.h"
#include "GeoDataStyle.h"
#include "GeoDataLineStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataTrack.h"
#include "GeoDataTreeModel.h"
#include "GeoDataLineString.h"
#include "GeoDataAccuracy.h"
#include "GeoDataDocumentWriter.h"
#include "KmlElementDictionary.h"
#include "FileManager.h"
#include "MarbleColors.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "PositionProviderPlugin.h"

#include <QFile>

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
        m_trackSegments( new GeoDataMultiTrack ),
        m_document(),
        m_currentTrack( nullptr ),
        m_positionProvider( nullptr ),
        m_length( 0.0 )
    {
    }

    void updatePosition();

    void updateStatus();

    static QString statusFile();

    PositionTracking *const q;

    GeoDataTreeModel *const m_treeModel;

    GeoDataPlacemark *const m_currentPositionPlacemark;
    GeoDataPlacemark *m_currentTrackPlacemark;
    GeoDataMultiTrack *m_trackSegments;
    GeoDataDocument m_document;

    GeoDataCoordinates  m_gpsPreviousPosition;
    GeoDataTrack  *m_currentTrack;

    PositionProviderPlugin* m_positionProvider;

    qreal m_length;
};

void PositionTrackingPrivate::updatePosition()
{
    Q_ASSERT( m_positionProvider != nullptr );

    const GeoDataAccuracy accuracy = m_positionProvider->accuracy();
    const GeoDataCoordinates position = m_positionProvider->position();
    const QDateTime timestamp = m_positionProvider->timestamp();

    if ( m_positionProvider->status() == PositionProviderStatusAvailable ) {
        if ( accuracy.horizontal < 250 ) {
            if ( m_currentTrack->size() ) {
                m_length += m_currentTrack->coordinatesAt(m_currentTrack->size() - 1).sphericalDistanceTo(position);
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
    Q_ASSERT( m_positionProvider != nullptr );

    const PositionProviderStatus status = m_positionProvider->status();

    if (status == PositionProviderStatusAvailable) {
        m_currentTrack = new GeoDataTrack;
        m_treeModel->removeFeature( m_currentTrackPlacemark );
        m_trackSegments->append( m_currentTrack );
        m_treeModel->addFeature( &m_document, m_currentTrackPlacemark );
    }

    emit q->statusChanged( status );
}

QString PositionTrackingPrivate::statusFile()
{
    QString const subdir = "tracking";
    QDir dir( MarbleDirs::localPath() );
    if ( !dir.exists( subdir ) ) {
        if ( !dir.mkdir( subdir ) ) {
            mDebug() << "Unable to create dir " << dir.absoluteFilePath( subdir );
            return dir.absolutePath();
        }
    }

    if ( !dir.cd( subdir ) ) {
        mDebug() << "Cannot change into " << dir.absoluteFilePath( subdir );
    }

    return dir.absoluteFilePath( "track.kml" );
}

PositionTracking::PositionTracking( GeoDataTreeModel *model )
     : QObject( model ),
       d( new PositionTrackingPrivate( model, this ) )
{
    d->m_document.setDocumentRole( TrackingDocument );
    d->m_document.setName(QStringLiteral("Position Tracking"));

    // First point is current position
    d->m_currentPositionPlacemark->setName(QStringLiteral("Current Position"));
    d->m_currentPositionPlacemark->setVisible(false);
    d->m_document.append( d->m_currentPositionPlacemark );

    // Second point is position track
    d->m_currentTrack = new GeoDataTrack;
    d->m_trackSegments->append(d->m_currentTrack);

    d->m_currentTrackPlacemark->setGeometry(d->m_trackSegments);
    d->m_currentTrackPlacemark->setName(QStringLiteral("Current Track"));

    GeoDataStyle::Ptr style(new GeoDataStyle);
    GeoDataLineStyle lineStyle;
    QColor transparentRed = Oxygen::brickRed4;
    transparentRed.setAlpha( 200 );
    lineStyle.setColor( transparentRed );
    lineStyle.setWidth( 4 );
    style->setLineStyle(lineStyle);
    style->setId(QStringLiteral("track"));

    GeoDataStyleMap styleMap;
    styleMap.setId(QStringLiteral("map-track"));
    styleMap.insert(QStringLiteral("normal"), QLatin1Char('#') + style->id());
    d->m_document.addStyleMap(styleMap);
    d->m_document.addStyle(style);
    d->m_document.append( d->m_currentTrackPlacemark );

    d->m_currentTrackPlacemark->setStyleUrl(QLatin1Char('#') + styleMap.id());

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
        connect( d->m_positionProvider, SIGNAL(statusChanged(PositionProviderStatus)),
                this, SLOT(updateStatus()) );
        connect( d->m_positionProvider, SIGNAL(positionChanged(GeoDataCoordinates,GeoDataAccuracy)),
                 this, SLOT(updatePosition()) );

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

bool PositionTracking::saveTrack( const QString& fileName )
{

    if ( fileName.isEmpty() ) {
        return false;
    }

    GeoDataDocument *document = new GeoDataDocument;
    QFileInfo fileInfo( fileName );
    QString name = fileInfo.baseName();
    document->setName( name );
    for( const GeoDataStyle::Ptr &style: d->m_document.styles() ) {
        document->addStyle( style );
    }
    for( const GeoDataStyleMap &map: d->m_document.styleMaps() ) {
        document->addStyleMap( map );
    }
    GeoDataPlacemark *track = new GeoDataPlacemark( *d->m_currentTrackPlacemark );
    track->setName(QLatin1String("Track ") + name);
    document->append( track );

    bool const result = GeoDataDocumentWriter::write(fileName, *document);
    delete document;
    return result;
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

void PositionTracking::readSettings()
{
    QFile file( d->statusFile() );
    if ( !file.open( QIODevice::ReadOnly ) ) {
        mDebug() << "Can not read track from " << file.fileName();
        return;
    }

    GeoDataParser parser( GeoData_KML );
    if ( !parser.read( &file ) ) {
        mDebug() << "Could not parse tracking file: " << parser.errorString();
        return;
    }

    GeoDataDocument *doc = dynamic_cast<GeoDataDocument*>( parser.releaseDocument() );
    file.close();

    if( !doc ){
        mDebug() << "tracking document not available";
        return;
    }

    GeoDataPlacemark *track = dynamic_cast<GeoDataPlacemark*>( doc->child( 0 ) );
    if( !track ) {
        mDebug() << "tracking document doesn't have a placemark";
        delete doc;
        return;
    }

    d->m_trackSegments = dynamic_cast<GeoDataMultiTrack*>( track->geometry() );
    if( !d->m_trackSegments ) {
        mDebug() << "tracking document doesn't have a multitrack";
        delete doc;
        return;
    }
    if( d->m_trackSegments->size() < 1 ) {
        mDebug() << "tracking document doesn't have a track";
        delete doc;
        return;
    }

    d->m_currentTrack = dynamic_cast<GeoDataTrack*>( d->m_trackSegments->child( d->m_trackSegments->size() - 1 ) );
    if( !d->m_currentTrack ) {
        mDebug() << "tracking document doesn't have a last track";
        delete doc;
        return;
    }

    doc->remove( 0 );
    delete doc;

    d->m_treeModel->removeDocument( &d->m_document );
    d->m_document.remove( 1 );
    delete d->m_currentTrackPlacemark;
    d->m_currentTrackPlacemark = track;
    d->m_currentTrackPlacemark->setName(QStringLiteral("Current Track"));
    d->m_document.append( d->m_currentTrackPlacemark );
    d->m_currentTrackPlacemark->setStyleUrl( d->m_currentTrackPlacemark->styleUrl() );

    d->m_treeModel->addDocument( &d->m_document );
    d->m_length = 0.0;
    for ( int i = 0; i < d->m_trackSegments->size(); ++i ) {
        d->m_length += d->m_trackSegments->at( i ).lineString()->length( 1 );
    }
}

void PositionTracking::writeSettings()
{
    saveTrack( d->statusFile() );
}

bool PositionTracking::isTrackEmpty() const
{
    if ( d->m_trackSegments->size() < 1 ) {
        return true;
    }

    if ( d->m_trackSegments->size() == 1 ) {
        return ( d->m_currentTrack->size() == 0 );
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

#include "moc_PositionTracking.cpp"
