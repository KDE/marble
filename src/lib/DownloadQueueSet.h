// Copyright 2009  Jens-Michael Hoffmann <jmho@c-xx.com>
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public 
// License along with this library.  If not, see <http://www.gnu.org/licenses/>.

#ifndef MARBLE_DOWNLOADQUEUESET_H
#define MARBLE_DOWNLOADQUEUESET_H

#include <QtCore/QList>
#include <QtCore/QQueue>
#include <QtCore/QObject>
#include <QtCore/QSet>
#include <QtCore/QStack>
#include <QtCore/QUrl>

#include "DownloadPolicy.h"

namespace Marble
{

class HttpJob;

/**
   Life of a HttpJob
   =================
   - Job is added to the QueueSet (by calling addJob() )
     the HttpJob is put into the m_jobQueue where it waits for "activation"
     signal jobAdded is emitted
   - Job is activated
     Job is moved from m_jobQueue to m_activeJobs and signals of the job
     are connected to slots (local or HttpDownloadManager)
     Job is executed by calling the jobs execute() method

   now there are different possibilities:
   1) Job emits jobDone (some error occurred, or canceled (kio))
      Job is disconnected
      signal jobRemoved is emitted
      Job is either moved from m_activeJobs to m_retryQueue
        or destroyed and blacklisted

   2) Job emits redirected
      Job is removed from m_activeJobs, disconnected and destroyed
      signal jobRemoved is emitted
      (HttpDownloadManager creates new Job with new source url)

   3) Job emits dataReceived
      Job is removed from m_activeJobs, disconnected and destroyed
      signal jobRemoved is emitted

   so we can conclude following rules:
   - Job is only connected to signals when in "active" state


   questions:
   - update of initiatorId needed?
     "We update the initiatorId as the previous initiator
      likely doesn't exist anymore"
   - blacklist or black list?

 */

class DownloadQueueSet: public QObject
{
    Q_OBJECT

 public:
    explicit DownloadQueueSet( QObject * const parent = 0 );
    explicit DownloadQueueSet( const DownloadPolicy& policy, QObject * const parent = 0 );
    ~DownloadQueueSet();

    DownloadPolicy downloadPolicy() const;
    void setDownloadPolicy( const DownloadPolicy& );

    bool canAcceptJob( const QUrl& sourceUrl,
                       const QString& destinationFileName ) const;
    void addJob( HttpJob * const job );

    void activateJobs();
    void retryJobs();
    void purgeJobs();

 Q_SIGNALS:
    void jobAdded();
    void jobRemoved();
    void jobRetry();
    void jobFinished( const QByteArray& data, const QString& destinationFileName,
                      const QString& id );
    void jobRedirected( const QUrl& newSourceUrl, const QString& destinationFileName,
                        const QString& id, DownloadUsage );

 private Q_SLOTS:
    void finishJob( HttpJob * job, const QByteArray& data );
    void redirectJob( HttpJob * job, const QUrl& newSourceUrl );
    void retryOrBlacklistJob( HttpJob * job, const int errorCode );

 private:
    void activateJob( HttpJob * const job );
    void deactivateJob( HttpJob * const job );
    bool jobIsActive( const QString& destinationFileName ) const;
    bool jobIsQueued( const QString& destinationFileName ) const;
    bool jobIsWaitingForRetry( const QString& destinationFileName ) const;
    bool jobIsBlackListed( const QUrl& sourceUrl ) const;

    DownloadPolicy m_downloadPolicy;

    /** This is the first stage a job enters, from this queue it will get
     *  into the activatedJobs container.
     */
    class JobStack
    {
    public:
        bool contains( const QString& destinationFileName ) const;
        int count() const;
        bool isEmpty() const;
        HttpJob * pop();
        void push( HttpJob * const );
    private:
        QStack<HttpJob*> m_jobs;
        QSet<QString> m_jobsContent;
    };
    JobStack m_jobs;

    /// Contains the jobs which are currently being downloaded.
    QList<HttpJob*> m_activeJobs;

    /** Contains jobs which failed to download and which are scheduled for
     *  retry according to retry settings.
     */
    QQueue<HttpJob*> m_retryQueue;

    /// Contains the blacklisted source urls
    QSet<QString> m_jobBlackList;
};

}

#endif
