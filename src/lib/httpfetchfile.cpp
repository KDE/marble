//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "httpfetchfile.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtGui/QMessageBox>

#include "katlasdirs.h"


HttpFetchFile::HttpFetchFile( QObject *parent )
    : QObject( parent )
{
    m_pHttp     = new QHttp(this);
    m_targetDir = KAtlasDirs::localDir() + "/cache/";

    if ( QDir( m_targetDir ).exists() == false ) 
        ( QDir::root() ).mkpath( m_targetDir );
    // What if we don't succeed in creating the path?

    connect( m_pHttp, SIGNAL( requestFinished( int, bool ) ),
             this,    SLOT( httpRequestFinished( int, bool ) ) );
    connect(m_pHttp, SIGNAL(responseHeaderReceived( const QHttpResponseHeader & ) ),
            this,    SLOT( checkResponseHeader( const QHttpResponseHeader & ) ) );
}


void HttpFetchFile::downloadFile( const QUrl& url )
{
    QString fileName = QFileInfo(url.path()).fileName();

    if ( QFile::exists( fileName ) ) {
        qDebug( "File already exists" );
        emit downloadDone( fileName, false );

        return;
    }

    m_pFile = new QFile( m_targetDir + fileName );
    if ( !m_pFile->open( QIODevice::WriteOnly ) ) {
        emit statusMessage( tr( "Unable to save the file %1: %2." )
                            .arg( m_targetDir + fileName ).arg( m_pFile->errorString() ) );
        delete m_pFile;
        m_pFile = 0;

        return;
    }

    m_pHttp->setHost( url.host(), url.port() != -1 ? url.port() : 80 );
    if ( !url.userName().isEmpty() )
        m_pHttp->setUser( url.userName(), url.password() );

    m_httpRequestAborted = false;
    m_httpGetId = m_pHttp->get( url.path(), m_pFile );
    m_pFileIdMap.insert( m_httpGetId, fileName );

    emit statusMessage( tr("Downloading data from Wikipedia.") );
}


void HttpFetchFile::cancelDownload()
{
    emit statusMessage( tr( "Download cancelled." ) );
    m_httpRequestAborted = true;
    m_pHttp->abort();
}


void HttpFetchFile::httpRequestFinished(int requestId, bool error)
{
    if ( m_httpRequestAborted ) {
        if ( m_pFile ) {
            m_pFile->close();
            m_pFile->remove();

            delete m_pFile;
            m_pFile = 0;
        }
        m_pFileIdMap.remove( requestId );

        return;
    }

    if ( requestId != m_httpGetId )
        return;

    m_pFile->close();

    if ( error ) {
        m_pFile->remove();
        emit statusMessage( tr( "Download failed: %1." )
                            .arg( m_pHttp->errorString() ) );
    } else {
        emit statusMessage( tr( "Download from Wikipedia finished." ) );
    }

    emit downloadDone( m_pFileIdMap[requestId], error );
    m_pFileIdMap.remove( requestId );
	
    delete m_pFile;
    m_pFile = 0;
}


void HttpFetchFile::checkResponseHeader(const QHttpResponseHeader &responseHeader)
{
    if (responseHeader.statusCode() != 200) {
        qDebug() << QString( " response: %1" ).arg( responseHeader.statusCode() );
        emit statusMessage( tr( "Download failed: %1." )
                            .arg( responseHeader.reasonPhrase() ) );
        m_httpRequestAborted = true;
        m_pHttp->clearPendingRequests();
//        m_pHttp->abort();
        return;
    }
}

#include "httpfetchfile.moc"
