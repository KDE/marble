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
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "AbstractProjection.h"
#include "FileManager.h"
#include "MarbleMath.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "PositionProviderPlugin.h"

#include "PositionTracking_p.h"

#include <QtCore/QFile>

using namespace Marble;

void PositionTrackingPrivate::setPosition( GeoDataCoordinates position,
                                           GeoDataAccuracy accuracy )
{
    m_accuracy = accuracy;
    if ( m_positionProvider && m_positionProvider->status() ==
        PositionProviderStatusAvailable )
    {
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(m_document->child(m_document->size()-1));
        //GeoDataMultiGeometry *geometry = static_cast<GeoDataMultiGeometry*>(placemark->geometry());
        m_currentLineString->append(position);

        //if the position has moved then update the current position
        if ( !( m_gpsCurrentPosition ==
                position ) )
        {
            placemark = static_cast<GeoDataPlacemark*>(m_document->child(0));
            placemark->setCoordinate(position);
            m_gpsCurrentPosition = position;
            qreal speed = m_positionProvider->speed();
            emit gpsLocation( position, speed );
        }
    }
}


void PositionTrackingPrivate::setStatus( PositionProviderStatus status )
{
    if (status == PositionProviderStatusAvailable) {
        Q_ASSERT(m_document);
        GeoDataPlacemark *placemark = static_cast<GeoDataPlacemark*>(m_document->child(m_document->size()-1));
        GeoDataMultiGeometry *multiGeometry = static_cast<GeoDataMultiGeometry*>(placemark->geometry());
        m_currentLineString = new GeoDataLineString;
        multiGeometry->append(m_currentLineString);
    }

    emit statusChanged( status );
}

PositionTracking::PositionTracking( FileManager *fileManager,
                          QObject *parent )
     : QObject( parent ), d (new PositionTrackingPrivate(fileManager, parent))
{

    connect( d, SIGNAL( gpsLocation(GeoDataCoordinates,qreal) ),
             this, SIGNAL( gpsLocation(GeoDataCoordinates,qreal) ));
    connect( d, SIGNAL( statusChanged(PositionProviderStatus)),
             this, SIGNAL( statusChanged(PositionProviderStatus) ) );

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

    placemark->setStyleUrl(QString("#").append(styleMap.styleId()));
    placemark->setStyle( &d->m_document->style(style.styleId()));
    d->m_document->append(placemark);

    d->m_fileManager->addGeoDataDocument(d->m_document);
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
                d, SLOT( setStatus(PositionProviderStatus) ) );
        connect( d->m_positionProvider, SIGNAL( positionChanged( GeoDataCoordinates,GeoDataAccuracy ) ),
                 d, SLOT( setPosition( GeoDataCoordinates,GeoDataAccuracy ) ) );

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
    return d->m_document->isVisible();
}

void PositionTracking::setTrackVisible( bool visible )
{
    d->m_document->setVisible( visible );
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
        foreach( GeoDataStyle style, d->m_document->styles() ) {
            document->addStyle( style );
        }
        foreach( GeoDataStyleMap map, d->m_document->styleMaps() ) {
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
    multiGeometry->clear();
    multiGeometry->append(d->m_currentLineString);
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

#include "PositionTracking.moc"
#include "PositionTracking_p.moc"
