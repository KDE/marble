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

#include "HttpDownloadManager.h"

#include <QtCore/QDebug>

#include "HttpFetchFile.h"
#include "MarbleDirs.h"
#include "StoragePolicy.h"

HttpDownloadManager::HttpDownloadManager( const QUrl& serverUrl,
                                          StoragePolicy *policy )
    : m_activatedJobsLimit( 40 ),
      m_jobQueueLimit( 1000 ),
      m_serverUrl( serverUrl ),
      m_storagePolicy( policy )
{
    m_downloadEnabled = true; //enabled for now

    m_jobQueue.clear();
    m_activatedJobList.clear();
    m_jobBlackList.clear();

    m_fetchFile = new HttpFetchFile( m_storagePolicy, this );

    connect( m_fetchFile, SIGNAL( jobDone( HttpJob*, int ) ),
             this, SLOT( reportResult( HttpJob*, int ) ) );
    connect( m_fetchFile, SIGNAL( statusMessage( QString ) ),
             this, SIGNAL( statusMessage( QString ) ) );
}


HttpDownloadManager::~HttpDownloadManager()
{
    m_downloadEnabled = false;

    qDeleteAll( m_jobQueue );
    m_jobQueue.clear();

    // activated jobs have to be deleted using deleteLater()
    // because they may be connected to signals
    QList<HttpJob*>::const_iterator pos = m_activatedJobList.begin();
    QList<HttpJob*>::const_iterator const end = m_activatedJobList.end();
    for (; pos != end; ++pos ) {
        (*pos)->deleteLater();
    }
    m_activatedJobList.clear();

    qDeleteAll( m_jobBlackList );
    m_jobBlackList.clear();

    delete m_storagePolicy;
}

void HttpDownloadManager::setServerUrl( const QUrl& serverUrl )
{
    m_serverUrl = serverUrl;
}

void HttpDownloadManager::setJobQueueLimit( int jobQueueLimit )
{
    m_jobQueueLimit = jobQueueLimit;
}

void HttpDownloadManager::setActivatedJobsLimit( int activatedJobsLimit )
{
    m_activatedJobsLimit = activatedJobsLimit;
}

StoragePolicy* HttpDownloadManager::storagePolicy() const
{
    return m_storagePolicy;
}

void HttpDownloadManager::addJob( const QString& relativeUrlString, const QString &id )
{
    if ( !m_downloadEnabled )
        return;

    QUrl sourceUrl( m_serverUrl );
    QString path = sourceUrl.path();
    sourceUrl.setPath( path + relativeUrlString );

    HttpJob *job = new HttpJob( sourceUrl, relativeUrlString, id );
    if ( acceptJob( job ) ) {
        m_jobQueue.push( job );
        job->setStatus( Pending );
        activateJobs();
    }
    else {
        job->deleteLater();
    }
}

void HttpDownloadManager::addJob( const QUrl& sourceUrl, const QString& destFileName,
                                  const QString &id )
{
    if ( !m_downloadEnabled )
        return;

    HttpJob *job = new HttpJob( sourceUrl, destFileName, id );
    if ( acceptJob( job ) ) {
        m_jobQueue.push( job );
        job->setStatus( Pending );
        activateJobs();
    }
    else {
        job->deleteLater();
    }
}

bool HttpDownloadManager::acceptJob( HttpJob  *job )
{
    // We update the initiatorId as the previous initiator
    // likely doesn't exist anymore

    QStack<HttpJob*>::iterator j;
    for ( j = m_jobQueue.begin(); j != m_jobQueue.end(); ++j ) {
        if ( job->originalDestinationFileName() == (*j)->originalDestinationFileName() ) {
            qDebug() << "Download rejected: It's in the queue already.";
            (*j)->setInitiatorId( job->initiatorId() );
            return false;
        }
    }

    QList<HttpJob*>::iterator i;
    for ( i = m_activatedJobList.begin(); i != m_activatedJobList.end(); ++i ) {
        if ( job->originalDestinationFileName() == (*i)->originalDestinationFileName() ) {
            qDebug() << "Download rejected: It's being downloaded already.";
            (*i)->setInitiatorId( job->initiatorId() );
            return false;
        }
    }
    for ( i = m_jobBlackList.begin(); i != m_jobBlackList.end(); ++i ) {
        if ( job->originalDestinationFileName() == (*i)->originalDestinationFileName() ) {
            qDebug() << "Download rejected: Blacklisted.";
            (*i)->setInitiatorId( job->initiatorId() );
            return false;
        }
    }

    return true;
}

void HttpDownloadManager::removeJob( HttpJob* job )
{
    int pos = m_activatedJobList.indexOf( job );

    if ( pos > 0 ) {
        m_activatedJobList.removeAt( pos );
//        qDebug() << "Removing: " << job->initiatorId();
        job->deleteLater();
    }

    activateJobs();
}


void HttpDownloadManager::activateJobs()
{
    while ( m_jobQueue.count() > 0
            && m_activatedJobList.count() < m_activatedJobsLimit )
    {
        HttpJob *job = m_jobQueue.pop();

//         qDebug() << "On activatedJobList: " << job->sourceUrl().toString()
//                  << job->destinationFileName();
        m_activatedJobList.push_back( job );
        job->setStatus( Activated );
        m_fetchFile->executeJob( job );
    }
}

void HttpDownloadManager::reportResult( HttpJob* job, int err )
{
    if ( err != 0 ) {
        int pos = m_activatedJobList.indexOf( job );

        if ( pos >= 0 ) {
            m_activatedJobList.removeAt( pos );
            m_jobBlackList.push_back( job );

            qDebug() << QString( "Download of %1 Blacklisted. Number of blacklist items: %2" )
                .arg( job->destinationFileName() ).arg( m_jobBlackList.size() );
        }
    }
    else {
//         qDebug() << "HttpDownloadManager: Download Complete:"
//                  << job->originalDestinationFileName() << job->initiatorId();
        emit downloadComplete( job->originalDestinationFileName(), job->initiatorId() );
        removeJob( job );
    }

}

#include "HttpDownloadManager.moc"
