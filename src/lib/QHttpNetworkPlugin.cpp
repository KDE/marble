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
// Copyright 2008      Pino Toscano <pino@kde.org>
//

#include "QHttpNetworkPlugin.h"

#include <QtCore/QBuffer>
#include <QtCore/QMetaObject>
#include <QtNetwork/QHttp>
#include <QtNetwork/QHttpResponseHeader>

#include "StoragePolicy.h"

using namespace Marble;

QHttpHttpJob::QHttpHttpJob( const QUrl & sourceUrl, const QString & destFileName,
                            const QString &id )
        : HttpJob( sourceUrl, destFileName, id ),
        m_data(),
        m_buffer( 0 ),
        m_http( 0 ),
        m_currentRequest( -1 )
{
}

QHttpHttpJob::~QHttpHttpJob()
{
    delete m_http;
    delete m_buffer;
}

void QHttpHttpJob::prepareExecution()
{
    // job can be executed more than once because of redirection
    // perhaps better to make a new job than (FIXME)
    if ( !m_http ) {
        m_http = new QHttp;
        m_buffer = new QBuffer( &m_data );
        m_buffer->open( QIODevice::WriteOnly );

        connect( m_http, SIGNAL( requestFinished( int, bool ) ),
                 this, SLOT( httpRequestFinished( int, bool ) ) );
    }
}

void QHttpHttpJob::execute()
{
    emit statusMessage( tr( "Downloading data..." ) );

    m_http->setHost( sourceUrl().host(),
                     sourceUrl().port() != -1 ? sourceUrl().port() : 80 );
    if ( !sourceUrl().userName().isEmpty() )
        m_http->setUser( sourceUrl().userName(), sourceUrl().password() );
    // if Url has query item like in panoramio API requests source.path()
    // chops it , this "if" gurantees its correct treatement
    QString cleanupPath;
    if ( sourceUrl().hasQuery() == true ) {
        cleanupPath = QString( sourceUrl().toString( QUrl::RemoveAuthority | QUrl::RemoveScheme ) );
    }
    else {
        cleanupPath = QUrl::toPercentEncoding( sourceUrl().path(), "/", " -" );
    }

//    qDebug() << sourceUrl().host() << "and path=" << cleanupPath;
    QHttpRequestHeader header( QLatin1String( "GET" ), cleanupPath );
    header.setValue( "Connection", "Keep-Alive" );
    header.setValue( "User-Agent", "Marble TinyWebBrowser" );
    header.setValue( "Host", sourceUrl().host() );

    m_currentRequest = m_http->request( header, 0, m_buffer );
}

void QHttpHttpJob::httpRequestFinished( int requestId, bool error )
{
    if ( requestId != m_currentRequest )
        return;

    QHttpResponseHeader responseHeader = m_http->lastResponse();
//     qDebug() << "responseHeader.statusCode():" << responseHeader.statusCode()
//              << responseHeader.reasonPhrase();

//    FIXME: Check whether this assumption is a safe on:
//    ( Problem: Conditional jump later on depends on uninitialised value )
//
//    if ( responseHeader.isValid() == false )
//        return;

    if ( responseHeader.statusCode() == 301 )
    {
        QUrl newLocation( responseHeader.value( "Location" ) );
        setSourceUrl( newLocation );
        setDestinationFileName( newLocation.path() );
        m_currentRequest = -1;

        // Let's try again
        QMetaObject::invokeMethod( this, "execute", Qt::QueuedConnection );
        return;
    }

    if ( responseHeader.statusCode() != 200 )
    {
        emit statusMessage( tr( "Download failed: %1." )
                            .arg( responseHeader.reasonPhrase() ) );
        emit jobDone( this, 1 );
        return;
    }

    if ( error != 0 )
    {
        emit statusMessage( tr( "Download failed: %1." )
                            .arg( m_http->errorString() ) );
        emit jobDone( this, error );
        return;
    }

    if ( storagePolicy() && !storagePolicy()->updateFile( originalDestinationFileName(), data() ) )
    {
        emit statusMessage( tr( "Download failed: %1." )
                            .arg( storagePolicy()->lastErrorMessage() ) );
        emit jobDone( this, error );
        return;
    }

    emit statusMessage( tr( "Download finished." ));
    emit jobDone( this, 0 );
}


QHttpNetworkPlugin::QHttpNetworkPlugin()
{
}

QHttpNetworkPlugin::~QHttpNetworkPlugin()
{
}

QString QHttpNetworkPlugin::nameId() const
{
    return QString::fromLatin1( "QHttp" );
}

HttpJob *QHttpNetworkPlugin::createJob( const QUrl &source, const QString &destination,
                                        const QString &id )
{
    return new QHttpHttpJob( source, destination, id );
}


Q_EXPORT_PLUGIN2( QHttpNetworkPlugin, Marble::QHttpNetworkPlugin )

#include "QHttpNetworkPlugin.moc"
