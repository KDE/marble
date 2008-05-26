//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2008      Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "HttpFetchFile.h"

#include <QtNetwork/QHttp>
#include <QtNetwork/QHttpResponseHeader>

#include "MarbleDirs.h"
#include "StoragePolicy.h"


HttpJob::HttpJob( const QUrl & sourceUrl, const QString & destFileName, QString const id )
    : m_sourceUrl( sourceUrl ),
      m_destinationFileName( destFileName ),
      m_originalDestinationFileName( destFileName ),
      m_data(),
      m_initiatorId( id ),
      m_status( NoStatus ),
      m_priority( NoPriority )
{
    m_buffer = new QBuffer( &m_data );
    m_buffer->open( QIODevice::WriteOnly );
}

HttpJob::~HttpJob()
{
    m_buffer->close();
    delete m_buffer;
}


HttpFetchFile::HttpFetchFile( StoragePolicy *policy, QObject *parent )
    : QObject( parent ),
      m_storagePolicy( policy )
{
    m_pHttp = new QHttp(this);

    connect( m_pHttp, SIGNAL( requestFinished( int, bool ) ),
             this, SLOT( httpRequestFinished( int, bool ) ) );
}

HttpFetchFile::~HttpFetchFile()
{
    m_pHttp->abort();
    delete m_pHttp;
}

void HttpFetchFile::executeJob( HttpJob* job )
{
    const QUrl sourceUrl = job->sourceUrl();

    m_pHttp->setHost( sourceUrl.host(), sourceUrl.port() != -1 ? sourceUrl.port() : 80 );
    if ( !sourceUrl.userName().isEmpty() )
        m_pHttp->setUser( sourceUrl.userName(), sourceUrl.password() );

    const QString cleanupPath = QUrl::toPercentEncoding( sourceUrl.path(), "/", " -" );

    QHttpRequestHeader header( QLatin1String("GET"), cleanupPath );
    header.setValue( "Connection", "Keep-Alive" );
    header.setValue( "User-Agent", "Marble TinyWebBrowser" );
    header.setValue( "Host", sourceUrl.host() );

    int httpGetId = m_pHttp->request( header, 0, job->buffer() );
    m_pJobMap.insert( httpGetId, job );

    emit statusMessage( tr("Downloading data...") );
}

void HttpFetchFile::httpRequestFinished( int requestId, bool error )
{
    if ( !m_pJobMap.contains( requestId ) )
        return;

    QHttpResponseHeader responseHeader = m_pHttp->lastResponse();

//    FIXME: Check whether this assumption is a safe on:
//    ( Problem: Conditional jump later on depends on uninitialised value )
//
//    if ( responseHeader.isValid() == false )
//        return;

    HttpJob* job = m_pJobMap[ requestId ];

    if ( responseHeader.statusCode() == 301 ) {
        QUrl newLocation( responseHeader.value( "Location" ) );
        job->setSourceUrl( newLocation );
        job->setDestinationFileName( newLocation.path() );

        // Let's try again
        executeJob( job );
        return;
    }

    if ( responseHeader.statusCode() != 200 ) {
        emit statusMessage( tr( "Download failed: %1." )
                            .arg( responseHeader.reasonPhrase() ) );
        emit jobDone( m_pJobMap[ requestId ], 1 );

        m_pJobMap.remove( requestId );
        return;
    }

    if ( error != 0 ) {
        emit statusMessage( tr( "Download failed: %1." )
                            .arg( m_pHttp->errorString() ) );
        emit jobDone( m_pJobMap[ requestId ], error );

        m_pJobMap.remove( requestId );
        return;

    }

    if ( !m_storagePolicy->updateFile( job->originalDestinationFileName(), job->data() ) ) {
        emit statusMessage( tr( "Download failed: %1." )
                            .arg( m_storagePolicy->lastErrorMessage() ) );
        emit jobDone( m_pJobMap[ requestId ], error );

        m_pJobMap.remove( requestId );
        return;
    }

    emit statusMessage( tr( "Download finished." ) );

    emit jobDone( m_pJobMap[ requestId ], 0 );
    m_pJobMap.remove( requestId );
}

#include "HttpFetchFile.moc"
