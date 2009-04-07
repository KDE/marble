//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2009      Jens-Michael Hoffmann <jensmh@gmx.de>
//

//
// The HttpDownloadManager manages http downloads.
//


#ifndef MARBLE_HTTPDOWNLOADMANAGER_H
#define MARBLE_HTTPDOWNLOADMANAGER_H


#include <QtCore/QString>
#include <QtCore/QUrl>
#include <QtCore/QList>
#include <QtCore/QSet>
#include <QtCore/QStack>
#include <QtCore/QQueue>

#include "marble_export.h"

class QTimer;

namespace Marble
{

class HttpJob;
class StoragePolicy;
class NetworkPlugin;

/*
 * @Short This class manages scheduled downloads. 

 * The downloadmanager offers a maximum number of active jobs and a
 * limit for pending jobs.  it also takes care that the job queue
 * won't be polluted by jobs that timed out already.
 *
 * @author Torsten Rahn
 */

class MARBLE_EXPORT HttpDownloadManager : public QObject 
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

    StoragePolicy* storagePolicy() const;

 public Q_SLOTS:
    /**
     * Adds a new job with the relative url and the given id.
     */
    void addJob( const QString& relativeUrlString, const QString &id );

    /**
     * Adds a new job with a sourceUrl, destination file name and given id.
     */
    void addJob( const QUrl& sourceUrl, const QString& destFilename, const QString &id );

    /**
     * Removes the @p job from the manager.
     */
    void removeJob( HttpJob *job );


 Q_SIGNALS:
    void downloadComplete( QString, QString );

    /**
     * Signal is emitted when a new job is added to the queue.
     */
    void jobAdded();

    /**
     * Signal is emitted when a job is removed from the queue.
     * The job might be completed or blacklisted.
     */
    void jobRemoved();


 private Q_SLOTS:
    void activateJobs();
    void reportResult( Marble::HttpJob *job, int id );
    void requeue();
    void jobRedirected( HttpJob *job, QUrl newLocation );
    void jobDataReceived( HttpJob *job, QByteArray data );

 private:
    Q_DISABLE_COPY( HttpDownloadManager )
    // Check whether the job gets processed already or whether it got blacklisted
    bool              acceptJob( HttpJob *job );
    HttpJob          *createJob( const QUrl& sourceUrl, const QString& destFileName,
                                 const QString &id );

    /**
     * Helper method for activateJobs(), also used in handling of redirections,
     * where the new job should be activated imediately.
     */
    void activateJob( HttpJob * const job );

    /**
     * Helper method for the public addJob methods which contains shared code.
     */
    void              addJob( HttpJob* );

    bool              m_downloadEnabled;

    QTimer           *m_requeueTimer;
    QStack<HttpJob*>  m_jobQueue;
    QQueue<HttpJob*>  m_waitingQueue;
    QList<HttpJob*>   m_activatedJobList;

    /// Contains the blacklisted source urls
    QSet<QString>     m_jobBlackList;

    int               m_activatedJobsLimit;
    int               m_jobQueueLimit;

    QUrl              m_serverUrl;
    StoragePolicy    *m_storagePolicy;

    NetworkPlugin    *m_networkPlugin;
};

}

#endif // MARBLE_HTTPDOWNLOADMANAGER_H
