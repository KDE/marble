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

class StoragePolicy;

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
    friend class HttpJob;

    Q_OBJECT
 public:
    /**
     * Creates a new http download manager.
     *
     * @param serverUrl The url of the server to download from.
     * @param policy The storage policy for this manager.
     */
    HttpDownloadManager( const QUrl& serverUrl, StoragePolicy *policy );

    /**
     * Destroys the http download manager.
     */
    virtual ~HttpDownloadManager();

    /**
     * Sets the url of the server to download from.
     */
    void setServerUrl( const QUrl& serverUrl );

    /**
     * Sets the limit of jobs to be queued.
     */
    void setJobQueueLimit( int jobQueueLimit );

    /**
     * Sets the limit of active jobs.
     */
    void setActivatedJobsLimit( int activatedJobsLimit );

 public Q_SLOTS:
    /**
     * Adds a new job with the relative url and the given id.
     */
    void addJob( const QString& relativeUrlString, const QString &id );

    /**
     * Adds a new job with a server, relative url and given id.
     */
    void addJob( const QString& server, const QString& relativeUrlString, const QString &id );

    /**
     * Removes the @p job from the manager.
     */
    void removeJob( HttpJob *job );


 Q_SIGNALS:
    void downloadComplete( QString, QString );
    void statusMessage( QString );

 private Q_SLOTS:
    void activateJobs();
    void reportResult( HttpJob *job, int id );

 private:
    // Check whether the job gets processed already or whether it got blacklisted
    bool              acceptJob( HttpJob  *job );
    bool              m_downloadEnabled;

    QQueue<HttpJob*>  m_jobQueue;
    QList<HttpJob*>   m_activatedJobList;
    QList<HttpJob*>   m_jobBlackList;

    HttpFetchFile    *m_fetchFile;

    int               m_activatedJobsLimit;
    int               m_jobQueueLimit;

    QUrl              m_serverUrl;
    StoragePolicy    *m_storagePolicy;
};


#endif // __MARBLE__HTTPDOWNLOADMANAGER_H
