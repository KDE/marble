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
        m_document( 0 ),
        m_treeModel( model ),
        m_positionProvider( 0 )
    {
    }

    void setPosition( GeoDataCoordinates position, GeoDataAccuracy accuracy );

    void setStatus( PositionProviderStatus status );

    PositionTracking *const q;

    GeoDataDocument     *m_document;
    GeoDataTreeModel    *m_treeModel;

    GeoDataCoordinates  m_gpsCurrentPosition;
    GeoDataCoordinates  m_gpsPreviousPosition;
    GeoDataLineString  *m_currentLineString;

    PositionProviderPlugin* m_positionProvider;

    GeoDataAccuracy m_accuracy;
};

void PositionTrackingPrivate::setPosition( GeoDataCoordinates position,
                                           GeoDataAccuracy accuracy )
{
    m_accuracy = accuracy;
    if ( m_positionProvider && m_positionProvider->status() ==
        PositionProviderStatusAvailable )
    {
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(m_document->child(m_document->size()-1));

        if ( accuracy.horizontal < 250 ) {
            m_currentLineString->append(position);
        }

        //if the position has moved then update the current position
        if ( m_gpsCurrentPosition != position )
        {
            placemark = static_cast<GeoDataPlacemark*>(m_document->child(0));
            placemark->setCoordinate(position);
            m_gpsCurrentPosition = position;
            qreal speed = m_positionProvider->speed();
            emit q->gpsLocation( position, speed );
        }
    }
}


void PositionTrackingPrivate::setStatus( PositionProviderStatus status )
{
    if (status == PositionProviderStatusAvailable) {
        Q_ASSERT(m_document);
        m_treeModel->removeDocument( m_document );
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(m_document->child(m_document->size()-1));
        GeoDataMultiGeometry *multiGeometry = static_cast<GeoDataMultiGeometry*>(placemark->geometry());
        m_currentLineString = new GeoDataLineString;
        multiGeometry->append(m_currentLineString);
        m_treeModel->addDocument( m_document );
    }

    emit q->statusChanged( status );
}

PositionTracking::PositionTracking( GeoDataTreeModel *model )
     : QObject( model ),
       d( new PositionTrackingPrivate( model, this ) )
{

    d->m_document     = new GeoDataDocument();
    d->m_document->setDocumentRole( TrackingDocument );
    d->m_document->setName("Position Tracking");

    // First point is current position
    GeoDataPlacemark *placemark = new GeoDataPlacemark;
    placemark->setName("Current Position");
    placemark->setVisible(false);
    d->m_document->append(placemark);

    // Second point is position track
    placemark = new GeoDataPlacemark;
    GeoDataMultiGeometry *multiGeometry = new GeoDataMultiGeometry;
    d->m_currentLineString = new GeoDataLineString;

    multiGeometry->append(d->m_currentLineString);
    placemark->setGeometry(multiGeometry);
    placemark->setName("Current Track");

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
    d->m_document->addStyleMap(styleMap);
    d->m_document->addStyle(style);
    d->m_document->append(placemark);

    placemark->setStyleUrl(QString("#").append(styleMap.styleId()));

    d->m_treeModel->addDocument(d->m_document);
}


PositionTracking::~PositionTracking()
{
    delete d;
}

void PositionTracking::setPositionProviderPlugin( PositionProviderPlugin* plugin )
{
    if ( d->m_positionProvider ) {
        d->m_positionProvider->deleteLater();
    }

    d->m_positionProvider = plugin;

    if ( d->m_positionProvider ) {
        d->m_positionProvider->setParent( this );
        mDebug() << "Initializing position provider:" << d->m_positionProvider->name();
        connect( d->m_positionProvider, SIGNAL( statusChanged( PositionProviderStatus ) ),
                this, SLOT( setStatus( PositionProviderStatus ) ) );
        connect( d->m_positionProvider, SIGNAL( positionChanged( GeoDataCoordinates,GeoDataAccuracy ) ),
                 this, SLOT( setPosition( GeoDataCoordinates,GeoDataAccuracy ) ) );

        d->m_positionProvider->initialize();
    }
    emit positionProviderPluginChanged( plugin );
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

bool PositionTracking::trackVisible() const
{
    GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(d->m_document->child(d->m_document->size()-1));
    return placemark->isVisible();
}

void PositionTracking::setTrackVisible( bool visible )
{
    d->m_treeModel->removeDocument(d->m_document);
    GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(d->m_document->child(d->m_document->size()-1));
    placemark->setVisible( visible );
    d->m_treeModel->addDocument(d->m_document);
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
        foreach( const GeoDataStyle &style, d->m_document->styles() ) {
            document->addStyle( style );
        }
        foreach( const GeoDataStyleMap &map, d->m_document->styleMaps() ) {
            document->addStyleMap( map );
        }
        GeoDataFeature *track = new GeoDataFeature(d->m_document->last());
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
    GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(d->m_document->child(d->m_document->size()-1));
    GeoDataMultiGeometry *multiGeometry = static_cast<GeoDataMultiGeometry*>(placemark->geometry());
    d->m_currentLineString = new GeoDataLineString;
    d->m_treeModel->removeDocument( d->m_document );
    multiGeometry->clear();
    multiGeometry->append(d->m_currentLineString);
    d->m_treeModel->addDocument( d->m_document );
}

bool PositionTracking::isTrackEmpty() const
{
    GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(d->m_document->child(d->m_document->size()-1));
    GeoDataMultiGeometry *multiGeometry = static_cast<GeoDataMultiGeometry*>(placemark->geometry());
    if ( multiGeometry->size() < 1 ) {
        return true;
    }

    if ( multiGeometry->size() == 1 ) {
        return d->m_currentLineString->isEmpty();
    }

    return false;
}

GeoDataAccuracy PositionTracking::accuracy() const
{
    return d->m_accuracy;
}

GeoDataCoordinates PositionTracking::currentLocation() const
{
    return d->m_gpsCurrentPosition;
}

PositionProviderStatus PositionTracking::status() const
{
    return d->m_positionProvider ? d->m_positionProvider->status() : PositionProviderStatusUnavailable;
}

}

#include "PositionTracking.moc"
