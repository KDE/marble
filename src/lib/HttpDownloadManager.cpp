//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
// Copyright 2008,2009 Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "HttpDownloadManager.h"

#include <QtCore/QDebug>
#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QTimer>

#include "DownloadPolicy.h"
#include "DownloadQueueSet.h"
#include "HttpJob.h"
#include "StoragePolicy.h"
#include "NetworkPlugin.h"
#include "PluginManager.h"

using namespace Marble;

// Time before a failed download job is requeued in ms
const quint32 requeueTime = 60000;

class HttpDownloadManager::Private
{
  public:
    Private( const QUrl& serverUrl, StoragePolicy *policy );
    ~Private();

    bool m_downloadEnabled;
    QTimer *m_requeueTimer;
    /**
     * Contains per download policy a queue set containing of
     * - a queue where jobs are waiting for being activated (=dowloaded)
     * - a queue containing currently being dowloaded
     * - a queue for retries of failed downloads */
    QMap<DownloadPolicyKey, DownloadQueueSet *> m_queueSets;
    int m_jobQueueLimit;
    QUrl m_serverUrl;
    StoragePolicy *m_storagePolicy;
    NetworkPlugin *m_networkPlugin;
};

HttpDownloadManager::Private::Private( const QUrl& serverUrl, StoragePolicy *policy )
    : m_downloadEnabled( true ), //enabled for now
      m_requeueTimer( 0 ),
      m_jobQueueLimit( 1000 ),
      m_serverUrl( serverUrl ),
      m_storagePolicy( policy ),
      m_networkPlugin( 0 )
{
}

HttpDownloadManager::Private::~Private()
{
    delete m_storagePolicy;
    delete m_networkPlugin;
}


HttpDownloadManager::HttpDownloadManager( const QUrl& serverUrl,
                                          StoragePolicy *policy )
    : d( new Private( serverUrl, policy ))
{
      d->m_requeueTimer = new QTimer( this );
      d->m_requeueTimer->setInterval( requeueTime );
      connect( d->m_requeueTimer, SIGNAL( timeout() ), this, SLOT( requeue() ) );

      // default download policy
      DownloadPolicy defaultDownloadPolicy;
      defaultDownloadPolicy.setMaximumConnections( 20 );
      addDownloadPolicy( defaultDownloadPolicy );
}


HttpDownloadManager::~HttpDownloadManager()
{
    d->m_downloadEnabled = false;
    delete d;
}

void HttpDownloadManager::setServerUrl( const QUrl& serverUrl )
{
    d->m_serverUrl = serverUrl;
}

void HttpDownloadManager::setJobQueueLimit( int jobQueueLimit )
{
    d->m_jobQueueLimit = jobQueueLimit;
}

// FIXME: remove this method
void HttpDownloadManager::setActivatedJobsLimit( int activatedJobsLimit )
{
}

void HttpDownloadManager::setDownloadEnabled( const bool enable )
{
    d->m_downloadEnabled = enable;
}

void HttpDownloadManager::addDownloadPolicy( const DownloadPolicy& policy )
{
    DownloadQueueSet * const queueSet = new DownloadQueueSet( policy );
    connect( queueSet, SIGNAL( jobFinished( QByteArray, QString, QString )),
             SLOT( finishJob( QByteArray, QString, QString )));
    connect( queueSet, SIGNAL( jobRetry() ), SLOT( startRetryTimer() ));
    connect( queueSet, SIGNAL( jobRedirected( QUrl, QString, QString )),
             SLOT( addJob( QUrl, QString, QString )));
    // relay jobAdded/jobRemoved signals (interesting for progress bar)
    connect( queueSet, SIGNAL( jobAdded() ), SIGNAL( jobAdded() ));
    connect( queueSet, SIGNAL( jobRemoved() ), SIGNAL( jobRemoved() ));
    d->m_queueSets.insert( queueSet->downloadPolicy().key(), queueSet );
}

StoragePolicy* HttpDownloadManager::storagePolicy() const
{
    return d->m_storagePolicy;
}

void HttpDownloadManager::addJob( const QString& relativeUrlString, const QString &id )
{
    QUrl sourceUrl( d->m_serverUrl );
    QString path = sourceUrl.path();
    sourceUrl.setPath( path + relativeUrlString );
    addJob( sourceUrl, relativeUrlString, id );
}

void HttpDownloadManager::addJob( const QUrl& sourceUrl, const QString& destFileName,
                                  const QString &id )
{
    if ( !d->m_downloadEnabled )
        return;

    DownloadQueueSet * const queueSet = findQueues( sourceUrl.host() );
    if ( queueSet->canAcceptJob( sourceUrl, destFileName )) {
        HttpJob * const job = createJob( sourceUrl, destFileName, id );
        if ( job ) {
            queueSet->addJob( job );
        }
    }
}

void HttpDownloadManager::finishJob( const QByteArray& data, const QString& destinationFileName,
                                     const QString& id )
{
    qDebug() << "emitting downloadComplete( QByteArray, " << id << ")";
    emit downloadComplete( data, id );
    if ( d->m_storagePolicy ) {
        const bool saved = d->m_storagePolicy->updateFile( destinationFileName, data );
        if ( saved ) {
            qDebug() << "emitting downloadComplete( " << destinationFileName << ", " << id << ")";
            emit downloadComplete( destinationFileName, id );
        } else {
            qWarning() << "Could not save:" << destinationFileName;
        }
    }
}

void HttpDownloadManager::requeue()
{
    d->m_requeueTimer->stop();

    QMap<DownloadPolicyKey, DownloadQueueSet *>::iterator pos = d->m_queueSets.begin();
    QMap<DownloadPolicyKey, DownloadQueueSet *>::iterator const end = d->m_queueSets.end();
    for (; pos != end; ++pos ) {
        pos.value()->retryJobs();
    }
}

void HttpDownloadManager::startRetryTimer()
{
    if ( !d->m_requeueTimer->isActive() )
        d->m_requeueTimer->start();
}

HttpJob *HttpDownloadManager::createJob( const QUrl& sourceUrl, const QString& destFileName,
                                         const QString &id )
{
    if ( !d->m_networkPlugin ) {
        PluginManager pluginManager;
        QList<NetworkPlugin *> networkPlugins = pluginManager.createNetworkPlugins();
        if ( !networkPlugins.isEmpty() ) {
            // FIXME: not just take the first plugin, but use some configuration setting
            // take the first plugin and delete the rest
            d->m_networkPlugin = networkPlugins.takeFirst();
            qDeleteAll( networkPlugins );
            d->m_networkPlugin->setParent( this );
        }
        else {
            d->m_downloadEnabled = false;
            return 0;
        }
    }
    Q_ASSERT( d->m_networkPlugin );
    return d->m_networkPlugin->createJob( sourceUrl, destFileName, id );
}

DownloadQueueSet const * HttpDownloadManager::findQueues( const QString& hostName ) const
{
    DownloadQueueSet const * result = 0;
    QMap<DownloadPolicyKey, DownloadQueueSet*>::const_iterator pos = d->m_queueSets.constBegin();
    QMap<DownloadPolicyKey, DownloadQueueSet*>::const_iterator const end = d->m_queueSets.constEnd();
    for (; pos != end; ++pos ) {
        if ( pos.key().matches( hostName )) {
            result = pos.value();
            break;
        }
    }
    // FIXME:
    if ( !result ) {
        Q_ASSERT( !d->m_queueSets.isEmpty() );
        result = d->m_queueSets.constBegin().value();
    }
    return result;
}

DownloadQueueSet *HttpDownloadManager::findQueues( const QString& hostName )
{
    DownloadQueueSet * result = 0;
    QMap<DownloadPolicyKey, DownloadQueueSet*>::iterator pos = d->m_queueSets.begin();
    QMap<DownloadPolicyKey, DownloadQueueSet*>::iterator const end = d->m_queueSets.end();
    for (; pos != end; ++pos ) {
        if ( pos.key().matches( hostName )) {
            result = pos.value();
            break;
        }
    }
    // FIXME:
    if ( !result ) {
        Q_ASSERT( !d->m_queueSets.isEmpty() );
        result = d->m_queueSets.begin().value();
    }
    return result;
}


#include "HttpDownloadManager.moc"
