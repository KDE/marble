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

#include "HttpFetchFile.h"
#include "katlasdirs.h"

HttpDownloadManager::HttpDownloadManager( const QUrl& serverUrl ) 
    : m_activatedJobsLimit(5)
    , m_jobQueueLimit(1000)
    , m_serverUrl(serverUrl)
{
    m_downloadEnabled = true; //disabled for now

    m_jobQueue.clear();
    m_activatedJobList.clear();

    m_fetchFile = new HttpFetchFile(this);

    setTargetDir( KAtlasDirs::localDir() + '/' );

    connect( m_fetchFile, SIGNAL( jobDone( HttpJob*, bool ) ), this, SLOT( reportResult( HttpJob*, bool ) ) );
    connect( m_fetchFile, SIGNAL( statusMessage( QString ) ), this, SIGNAL( statusMessage( QString ) ) );
}


HttpDownloadManager::~HttpDownloadManager()
{
    /* NOOP */
}


// void HttpDownloadManager::addJob(const QString& ServerUrl, const QString& SourceUrl, const QString& TargetDir, Priority priority)
void HttpDownloadManager::addJob( const QString& relativeUrlString, int id )
{
    if ( !m_downloadEnabled )
        return;

    qDebug() << "Accepting Job: " << relativeUrlString;

    HttpJob  *job = new HttpJob();
    job->initiatorId = id;
    job->relativeUrlString = relativeUrlString;
    // job->SourceUrl = SourceUrl;
    // job->TargetDir = TargetDir;
    // job->priority  = priority;

    if ( !m_jobQueue.contains( job ) && !m_activatedJobList.contains(job) ) {
        m_jobQueue.enqueue( job );
        job->status = Pending;
        activateJobs();
    }
}


void HttpDownloadManager::removeJob(HttpJob* job)
{
    int pos = m_activatedJobList.indexOf( job );

    if ( pos > 0 )
    {
        m_activatedJobList.removeAt( pos );
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

        qDebug() << "On activatedJobList: " << m_serverUrl.path() + job->relativeUrlString;
        m_activatedJobList.push_back( job );
        job->serverUrl = m_serverUrl;
        job->targetDirString = m_targetDir;
        job->status = Activated;
        m_fetchFile->executeJob( job );
    }
}

void HttpDownloadManager::reportResult( HttpJob* job, bool err )
{
    // FIXME: Actually use err?
    Q_UNUSED( err );

    emit downloadComplete( job->relativeUrlString, job->initiatorId );

    removeJob( job );

    qDebug() << "Download Complete!";
}

#include "HttpDownloadManager.moc"
