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

#include "HttpFetchFile.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QFileInfo>
#include <QtGui/QMessageBox>
#include <QtCore/QTemporaryFile>

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
}

HttpFetchFile::~HttpFetchFile()
{
    qDebug() << "Deleting Temporary Files ...";
    m_pHttp->disconnect();

    QFile* jobTargetFile;
    QMap<int, HttpJob*>::const_iterator i = m_pFileIdMap.constBegin();
    while (i != m_pFileIdMap.constEnd()) {
        qDebug() << "Deleting Item";
        HttpJob* job = i.value();
        jobTargetFile = job->targetFile;
        jobTargetFile->remove();
        ++i;
    }
    qDebug() << "Done.";
}

void HttpFetchFile::executeJob( HttpJob* job )
{
    QString localFileUrlString = job->targetDirString + job->relativeUrlString;

    if ( QFile::exists( localFileUrlString ) ) {
        qDebug( "File already exists" );
        emit jobDone( job, 1 );

        return;
    }

    QTemporaryFile* jobTargetFile = new QTemporaryFile(  QDir::tempPath() + "marble-tile.XXXXXX" );
    jobTargetFile->setAutoRemove( false );
    job->targetFile = (QFile*) jobTargetFile;

    if ( !jobTargetFile->open() ) {
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

    int httpGetId = m_pHttp->get( sourceUrl.path(), jobTargetFile );
    qDebug() << " job id: " << httpGetId << " source: " << sourceUrl.toString();
    m_pFileIdMap.insert( httpGetId, job );

    emit statusMessage( tr("Downloading data...") );
}

void HttpFetchFile::httpRequestFinished(int requestId, bool error)
{
    if ( !m_pFileIdMap.contains( requestId ) )
        return;

    QHttpResponseHeader responseHeader = m_pHttp->lastResponse();

    HttpJob* job = m_pFileIdMap[requestId];

    QFile* jobTargetFile = job->targetFile;

    if (responseHeader.statusCode() != 200) {
        qDebug() << QString( " response: %1" ).arg( responseHeader.statusCode() );
        jobTargetFile->remove();
        emit statusMessage( tr( "Download failed: %1." )
                            .arg( responseHeader.reasonPhrase() ) );
        emit jobDone( m_pFileIdMap[requestId], 1 );

        m_pFileIdMap.remove( requestId );
        return;
    }

    if ( error != 0 ) {
        qDebug() << "An error occurred! The Temporary file will be REMOVED!";
        jobTargetFile->remove();
        emit statusMessage( tr( "Download failed: %1." )
                            .arg( m_pHttp->errorString() ) );
        emit jobDone( m_pFileIdMap[requestId], error );

        m_pFileIdMap.remove( requestId );
        return;

    }

    jobTargetFile->close();

    QString localFileUrlString = job->targetDirString + job->relativeUrlString;

    QString localFilePath = localFileUrlString.section( QDir::separator(), 0, -2 ); 
    qDebug() << "Moving download to: " << localFileUrlString << " in: " << localFilePath;

    if ( !QDir( localFilePath ).exists() )
        ( QDir::root() ).mkpath( localFilePath );

    jobTargetFile->rename( localFileUrlString );

    emit statusMessage( tr( "Download finished." ) );

    emit jobDone( m_pFileIdMap[requestId], 0 );
    m_pFileIdMap.remove( requestId );
	
    delete jobTargetFile;
    jobTargetFile = 0;

}

#include "HttpFetchFile.moc"
