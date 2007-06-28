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
    m_targetDirString = KAtlasDirs::localDir() + "/cache/";

    if ( QDir( m_targetDirString ).exists() == false ) 
        ( QDir::root() ).mkpath( m_targetDirString );
    // What if we don't succeed in creating the path?

    connect( m_pHttp, SIGNAL( requestFinished( int, bool ) ),
             this,    SLOT( httpRequestFinished( int, bool ) ) );
//    connect(m_pHttp, SIGNAL(responseHeaderReceived( const QHttpResponseHeader & ) ),
//            this,    SLOT( checkResponseHeader( const QHttpResponseHeader & ) ) );
}

void HttpFetchFile::executeJob( HttpJob* job )
{

    QString localFileUrlString = job->targetDirString + job->relativeUrlString;

    if ( QFile::exists( localFileUrlString ) ) {
        qDebug( "File already exists" );
        emit jobDone( job, false );

        return;
    }

    QString localFilePath = localFileUrlString.section( QDir::separator(), 0, -2 ); 
    qDebug() << "Download to: " << localFileUrlString << " in: " << localFilePath;

    if ( !QDir( localFilePath ).exists() )
        ( QDir::root() ).mkpath( localFilePath );

    QFile* jobTargetFile = new QFile( localFileUrlString + ".tmp" );
    job->targetFile = jobTargetFile;

    if ( !jobTargetFile->open( QIODevice::WriteOnly ) ) {
        emit statusMessage( tr( "Unable to save the file %1: %2." )
                            .arg( localFileUrlString ).arg( jobTargetFile->errorString() ) );
        delete jobTargetFile;
        jobTargetFile = 0;

        return;
    }

    QUrl sourceUrl = QUrl( (job->serverUrl).toString() + job->relativeUrlString ); 

    m_pHttp->setHost( sourceUrl.host(), sourceUrl.port() != -1 ? sourceUrl.port() : 80 );
    if ( !sourceUrl.userName().isEmpty() )
        m_pHttp->setUser( sourceUrl.userName(), sourceUrl.password() );

    m_httpRequestAborted = false;

    int httpGetId = m_pHttp->get( sourceUrl.path(), jobTargetFile );
    qDebug() << " job id: " << httpGetId;
    m_pFileIdMap.insert( httpGetId, job );

    emit statusMessage( tr("Downloading data ...") );
}

void HttpFetchFile::cancelJob( HttpJob* job )
{
    emit statusMessage( tr( "Download cancelled." ) );
    job->status = Aborted;
    m_httpRequestAborted = true;
    m_pHttp->abort();
}


void HttpFetchFile::httpRequestFinished(int requestId, bool error)
{

    if ( !m_pFileIdMap.contains( requestId ) )
        return;

    HttpJob* job = m_pFileIdMap[requestId];

    QFile* jobTargetFile = job->targetFile;

    if ( job->status == Aborted ) {
        if ( jobTargetFile ) {
            jobTargetFile->close();
            jobTargetFile->remove();

            delete jobTargetFile;
            jobTargetFile = 0;
        }
        m_pFileIdMap.remove( requestId );

        return;
    }

    jobTargetFile->close();

    if ( error ) {
        jobTargetFile->remove();
        emit statusMessage( tr( "Download failed: %1." )
                            .arg( m_pHttp->errorString() ) );
    } else {
        QString localFileUrlString = job->targetDirString + job->relativeUrlString;
        jobTargetFile->rename( localFileUrlString );
        emit statusMessage( tr( "Download finished." ) );
    }

    emit jobDone( m_pFileIdMap[requestId], error );
    m_pFileIdMap.remove( requestId );
	
    delete jobTargetFile;
    jobTargetFile = 0;

}

/*
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
*/
#include "httpfetchfile.moc"
