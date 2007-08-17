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
#include "MarbleDirs.h"

HttpDownloadManager::HttpDownloadManager( const QUrl& serverUrl ) 
    : m_activatedJobsLimit(5)
    , m_jobQueueLimit(1000)
    , m_serverUrl(serverUrl)
{
    m_downloadEnabled = true; //disabled for now

    m_jobQueue.clear();
    m_activatedJobList.clear();
    m_jobBlackList.clear();

    m_fetchFile = new HttpFetchFile(this);

    setTargetDir( MarbleDirs::localPath() + '/' );

    connect( m_fetchFile, SIGNAL( jobDone( HttpJob*, int ) ), this, SLOT( reportResult( HttpJob*, int ) ) );
    connect( m_fetchFile, SIGNAL( statusMessage( QString ) ), this, SIGNAL( statusMessage( QString ) ) );
}


HttpDownloadManager::~HttpDownloadManager()
{
    qDebug() << "Deleting HttpDownloadManager";
//    m_fetchFile->disconnect();
//    delete m_fetchFile;
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

    if ( acceptJob( job ) ) {
//    if ( !m_jobQueue.contains( job ) && !m_activatedJobList.contains(job) ) {
        m_jobQueue.enqueue( job );
        job->status = Pending;
        activateJobs();
    }
    else
    {
        qDebug() << "The download was not activated. Deleting it for now.";
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
        if ( job->relativeUrlString == (*i)->relativeUrlString )
        {
            qDebug() << "Download rejected: It's in the queue already.";
            (*i)->initiatorId = job->initiatorId;
            return false;
        }
    }
    for (i = m_activatedJobList.begin(); i != m_activatedJobList.end(); ++i)
    {
        if ( job->relativeUrlString == (*i)->relativeUrlString )
        {
            qDebug() << "Download rejected: It's being downloaded already.";
            (*i)->initiatorId = job->initiatorId;
            return false;
        }
    }
    for (i = m_jobBlackList.begin(); i != m_jobBlackList.end(); ++i)
    {
        if ( job->relativeUrlString == (*i)->relativeUrlString )
        {
            qDebug() << "Download rejected: Blacklisted.";
            (*i)->initiatorId = job->initiatorId;
            return false;
        }
    }

    return true;
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

void HttpDownloadManager::reportResult( HttpJob* job, int err )
{
    if ( err != 0 )
    {
        int pos = m_activatedJobList.indexOf( job );

        if ( pos > 0 )
        {
            m_activatedJobList.removeAt( pos );
        }
        m_jobBlackList.push_back( job );

        qDebug() << QString( "Download of %1 Blacklisted. Number of blacklist items: %2" ).arg( job->relativeUrlString ).arg( m_jobBlackList.size() );
    }
    else 
    {
        emit downloadComplete( job->relativeUrlString, job->initiatorId );
        removeJob( job );

        qDebug() << "Download Complete!";
    }

}

#include "HttpDownloadManager.moc"
