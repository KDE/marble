//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
// Copyright 2008,2009 Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "HttpDownloadManager.h"

#include <QtCore/QDebug>
#include <QtCore/QTimer>

#include "HttpJob.h"
#include "StoragePolicy.h"
#include "NetworkPlugin.h"
#include "PluginManager.h"

using namespace Marble;

// Time before a failed download job is requeued in ms
const quint32 requeueTime = 60000;

HttpDownloadManager::HttpDownloadManager( const QUrl& serverUrl,
                                          StoragePolicy *policy )
    : m_downloadEnabled( true ), //enabled for now
      m_activatedJobsLimit( 40 ),
      m_jobQueueLimit( 1000 ),
      m_serverUrl( serverUrl ),
      m_storagePolicy( policy ),
      m_networkPlugin( 0 )
{
      m_requeueTimer = new QTimer( this );
      m_requeueTimer->setInterval( requeueTime );
      connect( m_requeueTimer, SIGNAL( timeout() ), this, SLOT( requeue() ) );
}


HttpDownloadManager::~HttpDownloadManager()
{
    m_downloadEnabled = false;

    qDeleteAll( m_jobQueue );
    m_jobQueue.clear();

    // activated jobs have to be deleted using deleteLater()
    // because they may be connected to signals
    QList<HttpJob*>::const_iterator pos = m_activatedJobList.constBegin();
    QList<HttpJob*>::const_iterator const end = m_activatedJobList.constEnd();
    for (; pos != end; ++pos ) {
        (*pos)->deleteLater();
    }
    m_activatedJobList.clear();

    qDeleteAll( m_jobBlackList );
    m_jobBlackList.clear();

    qDeleteAll( m_waitingQueue );
    m_waitingQueue.clear();

    delete m_storagePolicy;
    delete m_networkPlugin;
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

void HttpDownloadManager::addJob( HttpJob * job )
{
    if ( acceptJob( job ) ) {
        m_jobQueue.push( job );
        emit jobAdded();
        activateJobs();
    }
    else {
        job->deleteLater();
    }
}

void HttpDownloadManager::addJob( const QString& relativeUrlString, const QString &id )
{
    if ( !m_downloadEnabled )
        return;

    QUrl sourceUrl( m_serverUrl );
    QString path = sourceUrl.path();
    sourceUrl.setPath( path + relativeUrlString );

    HttpJob *job = createJob( sourceUrl, relativeUrlString, id );
    addJob( job );
}

void HttpDownloadManager::addJob( const QUrl& sourceUrl, const QString& destFileName,
                                  const QString &id )
{
    if ( !m_downloadEnabled )
        return;

    HttpJob *job = createJob( sourceUrl, destFileName, id );
    addJob( job );
}

bool HttpDownloadManager::acceptJob( HttpJob  *job )
{
    // We update the initiatorId as the previous initiator
    // likely doesn't exist anymore

    QStack<HttpJob*>::iterator j = m_jobQueue.begin();
    QStack<HttpJob*>::iterator const jEnd = m_jobQueue.end();
    for (; j != jEnd; ++j ) {
        if ( job->destinationFileName() == (*j)->destinationFileName() ) {
            qDebug() << "Download rejected: It's in the queue already.";
            (*j)->setInitiatorId( job->initiatorId() );
            return false;
        }
    }
    
    QList<HttpJob*>::iterator i = m_waitingQueue.begin();
    QList<HttpJob*>::iterator iEnd = m_waitingQueue.end();
    for (; i != iEnd; ++i) {
	if ( job->destinationFileName() == (*i)->destinationFileName() ) {
	    qDebug() << "Download rejected: Will try to download again in some time.";
	    (*i)->setInitiatorId( job->initiatorId() );
	    return false;
	}
    }

    i = m_activatedJobList.begin();
    iEnd = m_activatedJobList.end();
    for (; i != iEnd; ++i ) {
        if ( job->destinationFileName() == (*i)->destinationFileName() ) {
            qDebug() << "Download rejected: It's being downloaded already.";
            (*i)->setInitiatorId( job->initiatorId() );
            return false;
        }
    }

    i = m_jobBlackList.begin();
    iEnd = m_jobBlackList.end();
    for (; i != iEnd; ++i ) {
        if ( job->destinationFileName() == (*i)->destinationFileName() ) {
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
    emit jobRemoved();

    activateJobs();
}

void HttpDownloadManager::activateJob( HttpJob * const job )
{
    // qDebug() << "On activatedJobList: " << job->sourceUrl().toString()
    //          << job->destinationFileName();
    m_activatedJobList.push_back( job );
    job->setStoragePolicy( storagePolicy() );

    // No duplicate connections please
    disconnect( job, SIGNAL( jobDone( Marble::HttpJob*, int ) ),
                this, SLOT( reportResult( Marble::HttpJob*, int ) ) );
    disconnect( job, SIGNAL( statusMessage( QString ) ),
                this, SIGNAL( statusMessage( QString ) ) );
    connect( job, SIGNAL( jobDone( Marble::HttpJob*, int ) ),
             this, SLOT( reportResult( Marble::HttpJob*, int ) ) );
    connect( job, SIGNAL( statusMessage( QString ) ),
             this, SIGNAL( statusMessage( QString ) ) );
    connect( job, SIGNAL( redirected( HttpJob *, QUrl ) ),
             SLOT( jobRedirected( HttpJob *, QUrl ) ) );

    job->execute();
}

void HttpDownloadManager::activateJobs()
{
    while ( m_jobQueue.count() > 0
            && m_activatedJobList.count() < m_activatedJobsLimit )
    {
        HttpJob *job = m_jobQueue.pop();
        activateJob( job );
    }
}

void HttpDownloadManager::reportResult( HttpJob* job, int err )
{
    if ( err != 0 ) {
        int pos = m_activatedJobList.indexOf( job );

        if ( pos >= 0 ) {
            m_activatedJobList.removeAt( pos );
	    emit jobRemoved();

	    // This should always return true
	    if( !m_waitingQueue.contains( job ) ) {
		if( job->tryAgain() ) {
		    m_waitingQueue.enqueue( job );
		    qDebug() << QString( "Download of %1 failed, but trying again soon" )
			.arg( job->destinationFileName() );
                    if( !m_requeueTimer->isActive() )
                        m_requeueTimer->start();
		}
		else {
		    m_jobBlackList.push_back( job );
		    
		    qDebug() << QString( "Download of %1 Blacklisted. Number of blacklist items: %2" )
			.arg( job->destinationFileName() ).arg( m_jobBlackList.size() );
		}
	    }
	}
    }
    else {
//         qDebug() << "HttpDownloadManager: Download Complete:"
//                  << job->destinationFileName() << job->initiatorId();
        emit downloadComplete( job->destinationFileName(), job->initiatorId() );
        removeJob( job );
    }
    
    activateJobs();
}

void HttpDownloadManager::requeue()
{
    m_requeueTimer->stop();
    while( !m_waitingQueue.isEmpty() ) {
	HttpJob* job = m_waitingQueue.dequeue();
	qDebug() << QString( "Requeuing %1." ).arg( job->destinationFileName() );
	addJob( job );
    }
}

HttpJob *HttpDownloadManager::createJob( const QUrl& sourceUrl, const QString& destFileName,
                                         const QString &id )
{
    if ( !m_networkPlugin ) {
        PluginManager pluginManager;
        QList<NetworkPlugin *> networkPlugins = pluginManager.createNetworkPlugins();
        if ( !networkPlugins.isEmpty() ) {
            // FIXME: not just take the first plugin, but use some configuration setting
            // take the first plugin and delete the rest
            m_networkPlugin = networkPlugins.takeFirst();
            qDeleteAll( networkPlugins );
            m_networkPlugin->setParent( this );
        }
    }
    Q_ASSERT( m_networkPlugin );
    return m_networkPlugin->createJob( sourceUrl, destFileName, id );
}

void HttpDownloadManager::jobRedirected( HttpJob *job, QUrl newLocation )
{
    qDebug() << "jobRedirected" << job->sourceUrl() << " -> " << newLocation;
    HttpJob * const newJob = m_networkPlugin->createJob( newLocation, job->destinationFileName(),
                                                         job->initiatorId() );
    removeJob( job );
    activateJob( newJob );
}

#include "HttpDownloadManager.moc"
