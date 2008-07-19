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


HttpJob::HttpJob ( const QUrl & sourceUrl, const QString & destFileName, QString const id )
        : m_sourceUrl ( sourceUrl ),
        m_destinationFileName ( destFileName ),
        m_originalDestinationFileName ( destFileName ),
        m_data(),
        m_buffer( 0 ),
        m_initiatorId ( id ),
        m_status ( NoStatus ),
        m_priority ( NoPriority ),
        m_http( 0 )
{
}

HttpJob::~HttpJob()
{

    delete m_http;
    delete m_buffer;
}

void HttpJob::prepareExecution()
{
    // job can be executed more than once because of redirection
    // perhaps better to make a new job than (FIXME)
    if ( !m_http ) {
        m_http = new QHttp;
        m_buffer = new QBuffer ( &m_data );
        m_buffer->open ( QIODevice::WriteOnly );
    }
}

int HttpJob::execute()
{
    m_http->setHost( m_sourceUrl.host(),
                     m_sourceUrl.port() != -1 ? m_sourceUrl.port() : 80 );
    if ( !m_sourceUrl.userName().isEmpty() )
        m_http->setUser( m_sourceUrl.userName(), m_sourceUrl.password() );
    // if Url has query item like in panoramio API requests source.path()
    // chops it , this "if" gurantees its correct treatement
    QString cleanupPath;
    if ( m_sourceUrl.hasQuery() == true ) {
        cleanupPath = QString( m_sourceUrl.toString( QUrl::RemoveAuthority | QUrl::RemoveScheme ) );
    }
    else {
        cleanupPath = QUrl::toPercentEncoding( m_sourceUrl.path(), "/", " -" );
    }

    qDebug() << m_sourceUrl.host() << "and path=" << cleanupPath;
    QHttpRequestHeader header ( QLatin1String ( "GET" ), cleanupPath );
    header.setValue ( "Connection", "Keep-Alive" );
    header.setValue ( "User-Agent", "Marble TinyWebBrowser" );
    header.setValue ( "Host", m_sourceUrl.host() );

    const int httpGetId = m_http->request( header, 0, m_buffer );
    return httpGetId;
}

HttpFetchFile::HttpFetchFile ( StoragePolicy *policy, QObject *parent )
        : QObject ( parent ),
        m_storagePolicy ( policy )
{
}

HttpFetchFile::~HttpFetchFile()
{
}

void HttpFetchFile::executeJob ( HttpJob* job )
{
    // FIXME: this is a little bit ugly, but it resolves the following issues:
    // 1. not all jobs in the queue should have QHttp allocated,
    //    but only the active ones
    // 2. we have to connect before execution, even if it is asynchronously,
    //    I think.
    job->prepareExecution();

    connect( job->m_http, SIGNAL( requestFinished( int, bool ) ),
	     this, SLOT( httpRequestFinished( int, bool ) ) );

    int httpGetId = job->execute();
    m_pJobMap.insert ( httpGetId, job );

    emit statusMessage ( tr ( "Downloading data..." ) );
}

void HttpFetchFile::httpRequestFinished ( int requestId, bool error )
{
    if ( !m_pJobMap.contains ( requestId ) )
        return;

    HttpJob * job = m_pJobMap[ requestId ];

    QHttpResponseHeader responseHeader = job->m_http->lastResponse();
//     qDebug() << "responseHeader.statusCode():" << responseHeader.statusCode()
//              << responseHeader.reasonPhrase();

//    FIXME: Check whether this assumption is a safe on:
//    ( Problem: Conditional jump later on depends on uninitialised value )
//
//    if ( responseHeader.isValid() == false )
//        return;

    if ( responseHeader.statusCode() == 301 )
    {
        QUrl newLocation ( responseHeader.value ( "Location" ) );
        job->setSourceUrl ( newLocation );
        job->setDestinationFileName ( newLocation.path() );

        // Let's try again
        executeJob ( job );
        return;
    }

    if ( responseHeader.statusCode() != 200 )
    {
        emit statusMessage ( tr ( "Download failed: %1." )
                             .arg ( responseHeader.reasonPhrase() ) );
        emit jobDone ( m_pJobMap[ requestId ], 1 );

        m_pJobMap.remove ( requestId );
        return;
    }

    if ( error != 0 )
    {
        emit statusMessage ( tr ( "Download failed: %1." )
                             .arg ( job->m_http->errorString() ) );
        emit jobDone ( m_pJobMap[ requestId ], error );

        m_pJobMap.remove ( requestId );
        return;

    }

    if ( !m_storagePolicy->updateFile ( job->originalDestinationFileName(), job->data() ) )
    {
        emit statusMessage ( tr ( "Download failed: %1." )
                             .arg ( m_storagePolicy->lastErrorMessage() ) );
        emit jobDone ( m_pJobMap[ requestId ], error );

        m_pJobMap.remove ( requestId );
        return;
    }

    emit statusMessage ( tr ( "Download finished." ) );

    emit jobDone ( m_pJobMap[ requestId ], 0 );
    m_pJobMap.remove ( requestId );
}

#include "HttpFetchFile.moc"
