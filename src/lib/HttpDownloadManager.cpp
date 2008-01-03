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

#include "HttpDownloadManager.h"

#include <QtCore/QDebug>

#include "FileStoragePolicy.h"
#include "HttpFetchFile.h"
#include "MarbleDirs.h"

HttpDownloadManager::HttpDownloadManager( const QUrl& serverUrl, StoragePolicy *policy ) 
    : m_activatedJobsLimit( 5 )
    , m_jobQueueLimit( 1000 )
    , m_serverUrl( serverUrl )
    , m_storagePolicy( policy )
{
    m_downloadEnabled = true; //enabled for now

    m_jobQueue.clear();
    m_activatedJobList.clear();
    m_jobBlackList.clear();

    m_fetchFile = new HttpFetchFile( m_storagePolicy, this );

    connect( m_fetchFile, SIGNAL( jobDone( HttpJob*, int ) ), this, SLOT( reportResult( HttpJob*, int ) ) );
    connect( m_fetchFile, SIGNAL( statusMessage( QString ) ), this, SIGNAL( statusMessage( QString ) ) );
}


HttpDownloadManager::~HttpDownloadManager()
{
    m_downloadEnabled = false;

    qDeleteAll( m_jobQueue );
    m_jobQueue.clear();

    qDeleteAll( m_activatedJobList );
    m_activatedJobList.clear();

    qDeleteAll( m_jobBlackList );
    m_jobBlackList.clear();

    if ( m_storagePolicy != 0 )
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

void HttpDownloadManager::addJob( const QString& relativeUrlString, const QString &id )
{
    if ( !m_downloadEnabled )
        return;

    HttpJob  *job = new HttpJob;
    job->serverUrl = m_serverUrl;
    job->initiatorId = id;
    job->originalRelativeUrlString = relativeUrlString;
    job->relativeUrlString = relativeUrlString;

    if ( acceptJob( job ) ) {
        m_jobQueue.enqueue( job );
        job->status = Pending;
        activateJobs();
    }
    else
    {
        delete job;
    }
}

void HttpDownloadManager::addJob( const QString& server, const QString& relativeUrlString, const QString &id )
{
    if ( !m_downloadEnabled )
        return;

    HttpJob  *job = new HttpJob;
    job->serverUrl = server;
    job->initiatorId = id;
    job->originalRelativeUrlString = relativeUrlString;
    job->relativeUrlString = relativeUrlString;

    if ( acceptJob( job ) ) {
        m_jobQueue.enqueue( job );
        job->status = Pending;
        activateJobs();
    }
    else
    {
        delete job;
    }
}

bool HttpDownloadManager::acceptJob( HttpJob  *job )
{
    QList<HttpJob*>::iterator i;

    // We update the initiatorId as the previous initiator 
    // likely doesn't exist anymore

    for (i = m_jobQueue.begin(); i != m_jobQueue.end(); ++i)
    {
        if ( job->originalRelativeUrlString == (*i)->originalRelativeUrlString )
        {
//            qDebug() << "Download rejected: It's in the queue already.";
            (*i)->initiatorId = job->initiatorId;
            return false;
        }
    }
    for (i = m_activatedJobList.begin(); i != m_activatedJobList.end(); ++i)
    {
        if ( job->originalRelativeUrlString == (*i)->originalRelativeUrlString )
        {
//            qDebug() << "Download rejected: It's being downloaded already.";
            (*i)->initiatorId = job->initiatorId;
            return false;
        }
    }
    for (i = m_jobBlackList.begin(); i != m_jobBlackList.end(); ++i)
    {
        if ( job->originalRelativeUrlString == (*i)->originalRelativeUrlString )
        {
//            qDebug() << "Download rejected: Blacklisted.";
            (*i)->initiatorId = job->initiatorId;
            return false;
        }
    }

    return true;
}

void HttpDownloadManager::removeJob( HttpJob* job )
{
    int pos = m_activatedJobList.indexOf( job );

    if ( pos > 0 )
    {
        m_activatedJobList.removeAt( pos );
//        qDebug() << "Removing: " << job->initiatorId;
        delete job;
    }

    activateJobs();
}


void HttpDownloadManager::activateJobs()
{
    while ( m_jobQueue.count() > 0
            && m_activatedJobList.count() < m_activatedJobsLimit )
    {
        HttpJob  *job = m_jobQueue.dequeue();

//        qDebug() << "On activatedJobList: " << m_serverUrl.path() + job->relativeUrlString;
        m_activatedJobList.push_back( job );
        job->status = Activated;
        m_fetchFile->executeJob( job );
    }
}

void HttpDownloadManager::reportResult( HttpJob* job, int err )
{
    if ( err != 0 )
    {
        int pos = m_activatedJobList.indexOf( job );

        if ( pos >= 0 )
        {
            m_activatedJobList.removeAt( pos );
            m_jobBlackList.push_back( job );

//            qDebug() << QString( "Download of %1 Blacklisted. Number of blacklist items: %2" ).arg( job->relativeUrlString ).arg( m_jobBlackList.size() );
        }
    }
    else 
    {
        emit downloadComplete( job->originalRelativeUrlString, job->initiatorId );
        removeJob( job );

//        qDebug() << "Download Complete!";
    }

}

#include "HttpDownloadManager.moc"
