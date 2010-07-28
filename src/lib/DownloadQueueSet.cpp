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

#include "DownloadQueueSet.h"

#include "MarbleDebug.h"

#include "HttpJob.h"

namespace Marble
{

DownloadQueueSet::DownloadQueueSet( QObject * const parent )
    : QObject( parent )
{
}

DownloadQueueSet::DownloadQueueSet( DownloadPolicy const & policy, QObject * const parent )
    : QObject( parent ),
      m_downloadPolicy( policy )
{
}

DownloadQueueSet::~DownloadQueueSet()
{
    // todo: delete HttpJobs
}

DownloadPolicy DownloadQueueSet::downloadPolicy() const
{
    return m_downloadPolicy;
}

void DownloadQueueSet::setDownloadPolicy( DownloadPolicy const & policy )
{
    m_downloadPolicy = policy;
}

bool DownloadQueueSet::canAcceptJob( const QUrl& sourceUrl,
                                     const QString& destinationFileName ) const
{
    if ( jobIsQueued( destinationFileName )) {
        mDebug() << "Download rejected: It's in the queue already:"
                 << destinationFileName;
        return false;
    }
    if ( jobIsWaitingForRetry( destinationFileName )) {
        mDebug() << "Download rejected: Will try to download again in some time:"
                 << destinationFileName;
        return false;
    }
    if ( jobIsActive( destinationFileName )) {
        mDebug() << "Download rejected: It's being downloaded already:"
                 << destinationFileName;
        return false;
    }
    if ( jobIsBlackListed( sourceUrl )) {
        mDebug() << "Download rejected: Blacklisted.";
        return false;
    }
    return true;
}

void DownloadQueueSet::addJob( HttpJob * const job )
{
    m_jobs.push( job );
    mDebug() << "addJob: new job queue size:" << m_jobs.count();
    emit jobAdded();
    activateJobs();
}

void DownloadQueueSet::activateJobs()
{
    while ( !m_jobs.isEmpty()
            && m_activeJobs.count() < m_downloadPolicy.maximumConnections() )
    {
        HttpJob * const job = m_jobs.pop();
        activateJob( job );
    }
}

void DownloadQueueSet::retryJobs()
{
    while ( !m_retryQueue.isEmpty() ) {
        HttpJob * const job = m_retryQueue.dequeue();
        mDebug() << "Requeuing" << job->destinationFileName();
        // FIXME: addJob calls activateJobs every time
        addJob( job );
    }
}

void DownloadQueueSet::finishJob( HttpJob * job, const QByteArray& data )
{
    mDebug() << "finishJob: " << job->sourceUrl() << job->destinationFileName();

    deactivateJob( job );
    emit jobRemoved();
    emit jobFinished( data, job->destinationFileName(), job->initiatorId() );
    job->deleteLater();
    activateJobs();
}

void DownloadQueueSet::redirectJob( HttpJob * job, const QUrl& newSourceUrl )
{
    mDebug() << "jobRedirected:" << job->sourceUrl() << " -> " << newSourceUrl;

    deactivateJob( job );
    emit jobRemoved();
    emit jobRedirected( newSourceUrl, job->destinationFileName(), job->initiatorId(),
                        job->downloadUsage() );
    job->deleteLater();
}

void DownloadQueueSet::retryOrBlacklistJob( HttpJob * job, const int errorCode )
{
    Q_ASSERT( errorCode != 0 );
    Q_ASSERT( !m_retryQueue.contains( job ));

    deactivateJob( job );
    emit jobRemoved();

    if ( job->tryAgain() ) {
        mDebug() << QString( "Download of %1 to %2 failed, but trying again soon" )
            .arg( job->sourceUrl().toString() ).arg( job->destinationFileName() );
        m_retryQueue.enqueue( job );
        emit jobRetry();
    }
    else {
        mDebug() << "JOB-address: " << job
                 << "Blacklist-size:" << m_jobBlackList.size()
                 << "err:" << errorCode;
        m_jobBlackList.insert( job->sourceUrl().toString() );
        mDebug() << QString( "Download of %1 Blacklisted. "
                             "Number of blacklist items: %2" )
            .arg( job->destinationFileName() )
            .arg( m_jobBlackList.size() );

        job->deleteLater();
    }
    activateJobs();
}

void DownloadQueueSet::activateJob( HttpJob * const job )
{
    m_activeJobs.push_back( job );

    connect( job, SIGNAL( jobDone( HttpJob *, int )),
             SLOT( retryOrBlacklistJob( HttpJob *, int )));
    connect( job, SIGNAL( redirected( HttpJob *, QUrl )),
             SLOT( redirectJob( HttpJob *, QUrl )));
    connect( job, SIGNAL( dataReceived( HttpJob *, QByteArray )),
             SLOT( finishJob( HttpJob *, QByteArray )));

    job->execute();
}

/**
   pre condition: - job is in m_activeJobs
                  - job's signal are connected to our slots
   post condition: - job is not in m_activeJobs anymore (and btw not
                     in any other queue)
                   - job's signals are disconnected from our slots
 */
void DownloadQueueSet::deactivateJob( HttpJob * const job )
{
    const bool disconnected = job->disconnect();
    Q_ASSERT( disconnected );
    Q_UNUSED( disconnected ); // for Q_ASSERT in release mode
    const bool removed = m_activeJobs.removeOne( job );
    Q_ASSERT( removed );
    Q_UNUSED( removed ); // for Q_ASSERT in release mode
}

bool DownloadQueueSet::jobIsActive( QString const & destinationFileName ) const
{
    QList<HttpJob*>::const_iterator pos = m_activeJobs.constBegin();
    QList<HttpJob*>::const_iterator const end = m_activeJobs.constEnd();
    for (; pos != end; ++pos) {
        if ( (*pos)->destinationFileName() == destinationFileName ) {
            return true;
        }
    }
    return false;
}

inline bool DownloadQueueSet::jobIsQueued( QString const & destinationFileName ) const
{
    return m_jobs.contains( destinationFileName );
}

bool DownloadQueueSet::jobIsWaitingForRetry( QString const & destinationFileName ) const
{
    QList<HttpJob*>::const_iterator pos = m_retryQueue.constBegin();
    QList<HttpJob*>::const_iterator const end = m_retryQueue.constEnd();
    for (; pos != end; ++pos) {
        if ( (*pos)->destinationFileName() == destinationFileName ) {
            return true;
        }
    }
    return false;
}

bool DownloadQueueSet::jobIsBlackListed( const QUrl& sourceUrl ) const
{
    QSet<QString>::const_iterator const pos =
        m_jobBlackList.constFind( sourceUrl.toString() );
    return pos != m_jobBlackList.constEnd();
}


inline bool DownloadQueueSet::JobStack::contains( const QString& destinationFileName ) const
{
    return m_jobsContent.contains( destinationFileName );
}

inline int DownloadQueueSet::JobStack::count() const
{
    return m_jobs.count();
}

inline bool DownloadQueueSet::JobStack::isEmpty() const
{
    return m_jobs.isEmpty();
}

inline HttpJob * DownloadQueueSet::JobStack::pop()
{
    HttpJob * const job = m_jobs.pop();
    bool const removed = m_jobsContent.remove( job->destinationFileName() );
    Q_UNUSED( removed ); // for Q_ASSERT in release mode
    Q_ASSERT( removed );
    return job;
}

inline void DownloadQueueSet::JobStack::push( HttpJob * const job )
{
    m_jobs.push( job );
    m_jobsContent.insert( job->destinationFileName() );
}


}

#include "DownloadQueueSet.moc"
