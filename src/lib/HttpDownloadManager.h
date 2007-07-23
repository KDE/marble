//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

//
// The HttpDownloadManager manages http downloads.
//


#ifndef __MARBLE__HTTPDOWNLOADMANAGER_H
#define __MARBLE__HTTPDOWNLOADMANAGER_H


#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QDir>
#include <QtCore/QQueue>
#include <QtCore/QList>
#include <QtCore/QDebug>

#include "HttpFetchFile.h"

/*
 * @Short This class manages scheduled downloads. 

 * The downloadmanager offers a maximum number of active jobs and a
 * limit for pending jobs.  it also takes care that the job queue
 * won't be polluted by jobs that timed out already.
 *
 * @author Torsten Rahn
 */

class HttpDownloadManager : public QObject 
{
    Q_OBJECT
public:
    HttpDownloadManager( const QUrl& serverUrl );
    virtual ~HttpDownloadManager();

    // void addJob(HttpJob*){};
    // void killJob(HttpJob*){};
    // void killAllJobs(){};

    void setTargetDir(const QString& targetDir)
    {
        m_targetDir = targetDir;
        m_fetchFile -> setTargetDir( m_targetDir );
    }

    void setServerUrl(const QUrl& serverUrl)
    {
        m_serverUrl = serverUrl;
    }

    void setJobQueueLimit(int jobQueueLimit)
    {
        m_jobQueueLimit = jobQueueLimit;
    }

    void setActivatedJobsLimit(int activatedJobsLimit)
    {
        m_activatedJobsLimit = activatedJobsLimit;
    }

 public Q_SLOTS:
    void addJob( const QString& relativeUrlString, int id );

    void removeJob(HttpJob*);

    void reportResult( HttpJob*, bool );

 Q_SIGNALS:
    void downloadComplete( QString, int id );
    void statusMessage( QString );

private Q_SLOTS:

    void activateJobs();

private:
    bool              m_downloadEnabled;

    QQueue<HttpJob*>  m_jobQueue;
    QList<HttpJob*>   m_activatedJobList;

    HttpFetchFile    *m_fetchFile;

    int               m_activatedJobsLimit;
    int               m_jobQueueLimit;

    QString           m_targetDir;
    QUrl              m_serverUrl;

    // QTimer* timer; // the timer needs to be set for each item in the activatedJobsList
};


#endif // __MARBLE__HTTPDOWNLOADMANAGER_H
