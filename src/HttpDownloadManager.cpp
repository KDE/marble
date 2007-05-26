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

#include "httpfetchfile.h"


HttpDownloadManager::HttpDownloadManager() 
    : m_activatedJobsLimit(5)
    , m_jobQueueLimit(1000)
{
    m_jobQueue.clear();
    m_activatedJobList.clear();

    m_fetchFile = new HttpFetchFile(this);
}


HttpDownloadManager::~HttpDownloadManager()
{
    /* NOOP */
}


// void HttpDownloadManager::addJob(const QString& ServerUrl, const QString& SourceUrl, const QString& TargetDir, Priority priority)
void HttpDownloadManager::addJob( QString relativeUrl)
{
    qDebug() << "Accepting Job: " << relativeUrl;

    HttpJob  *job = new HttpJob();
    job->SourceUrl = relativeUrl;
    // job->SourceUrl = SourceUrl;
    // job->TargetDir = TargetDir;
    // job->priority  = priority;

    if ( !m_jobQueue.contains( job ) ) {
        m_jobQueue.enqueue( job );
        job->status = Pending;
        activateJobs();
    }
}


void HttpDownloadManager::removeJob(HttpJob* job)
{
    Q_UNUSED( job );
    // m_activatedJobList
    // removeAll( job );
}


void HttpDownloadManager::activateJobs()
{
    while ( m_jobQueue.count() > 0
            && m_activatedJobList.count() < m_activatedJobsLimit )
    {
        HttpJob  *job = m_jobQueue.dequeue();
        qDebug() << "On activatedJobList: " << job->SourceUrl;

        m_activatedJobList.push_back( job );
        job->status = Activated;
        m_fetchFile->downloadFile( job->SourceUrl );
    }
}


#ifndef Q_OS_MACX
#include "HttpDownloadManager.moc"
#endif
