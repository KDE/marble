//
// This file is part of the Marble Virtual Globe.
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

#include <QtCore/QList>
#include <QtCore/QMap>
#include <QtCore/QTimer>

#include "DownloadPolicy.h"
#include "DownloadQueueSet.h"
#include "HttpJob.h"
#include "MarbleDebug.h"
#include "StoragePolicy.h"
#include "NetworkPlugin.h"
#include "PluginManager.h"

using namespace Marble;

// Time before a failed download job is requeued in ms
const quint32 requeueTime = 60000;

class HttpDownloadManager::Private
{
  public:
    explicit Private( StoragePolicy *policy,
                      PluginManager *pluginManager );
    ~Private();

    HttpJob *createJob( const QUrl& sourceUrl, const QString& destFileName,
                        const QString &id );
    DownloadQueueSet *findQueues( const QString& hostName, const DownloadUsage usage );

    bool m_downloadEnabled;
    QTimer *m_requeueTimer;
    /**
     * Contains per download policy a queue set containing of
     * - a queue where jobs are waiting for being activated (=downloaded)
     * - a queue containing currently being downloaded
     * - a queue for retries of failed downloads */
    QList<QPair<DownloadPolicyKey, DownloadQueueSet *> > m_queueSets;
    QMap<DownloadUsage, DownloadQueueSet *> m_defaultQueueSets;
    StoragePolicy *const m_storagePolicy;
    PluginManager *const m_pluginManager;
    NetworkPlugin *m_networkPlugin;

};

HttpDownloadManager::Private::Private( StoragePolicy *policy,
                                       PluginManager *pluginManager )
    : m_downloadEnabled( true ), //enabled for now
      m_requeueTimer( 0 ),
      m_storagePolicy( policy ),
      m_pluginManager( pluginManager ),
      m_networkPlugin( 0 )
{
    // setup default download policy and associated queue set
    DownloadPolicy defaultBrowsePolicy;
    defaultBrowsePolicy.setMaximumConnections( 20 );
    m_defaultQueueSets[ DownloadBrowse ] = new DownloadQueueSet( defaultBrowsePolicy );
    DownloadPolicy defaultBulkDownloadPolicy;
    defaultBulkDownloadPolicy.setMaximumConnections( 2 );
    m_defaultQueueSets[ DownloadBulk ] = new DownloadQueueSet( defaultBulkDownloadPolicy );
}

HttpDownloadManager::Private::~Private()
{
    QMap<DownloadUsage, DownloadQueueSet *>::iterator pos = m_defaultQueueSets.begin();
    QMap<DownloadUsage, DownloadQueueSet *>::iterator const end = m_defaultQueueSets.end();
    for (; pos != end; ++pos )
        delete pos.value();
    delete m_networkPlugin;
}

HttpJob *HttpDownloadManager::Private::createJob( const QUrl& sourceUrl,
                                                  const QString& destFileName,
                                                  const QString &id )
{
    if ( !m_networkPlugin ) {
        QList<NetworkPlugin *> networkPlugins = m_pluginManager->createNetworkPlugins();
        if ( !networkPlugins.isEmpty() ) {
            // FIXME: not just take the first plugin, but use some configuration setting
            // take the first plugin and delete the rest
            m_networkPlugin = networkPlugins.takeFirst();
            qDeleteAll( networkPlugins );
        }
        else {
            m_downloadEnabled = false;
            return 0;
        }
    }
    Q_ASSERT( m_networkPlugin );
    return m_networkPlugin->createJob( sourceUrl, destFileName, id );
}

DownloadQueueSet *HttpDownloadManager::Private::findQueues( const QString& hostName,
                                                            const DownloadUsage usage )
{
    DownloadQueueSet * result = 0;
    QList<QPair<DownloadPolicyKey, DownloadQueueSet*> >::iterator pos = m_queueSets.begin();
    QList<QPair<DownloadPolicyKey, DownloadQueueSet*> >::iterator const end = m_queueSets.end();
    for (; pos != end; ++pos ) {
        if ( (*pos).first.matches( hostName, usage )) {
            result = (*pos).second;
            break;
        }
    }
    if ( !result ) {
        mDebug() << "No download policy found for" << hostName << usage
                 << ", using default policy.";
        result = m_defaultQueueSets[ usage ];
    }
    return result;
}


HttpDownloadManager::HttpDownloadManager( StoragePolicy *policy,
                                          PluginManager *pluginManager )
    : d( new Private( policy, pluginManager ))
{
    d->m_requeueTimer = new QTimer( this );
    d->m_requeueTimer->setInterval( requeueTime );
    connect( d->m_requeueTimer, SIGNAL( timeout() ), this, SLOT( requeue() ) );
    connectDefaultQueueSets();
}

HttpDownloadManager::~HttpDownloadManager()
{
    delete d;
}

void HttpDownloadManager::setDownloadEnabled( const bool enable )
{
    d->m_downloadEnabled = enable;
}

void HttpDownloadManager::addDownloadPolicy( const DownloadPolicy& policy )
{
    if ( hasDownloadPolicy( policy ))
        return;
    DownloadQueueSet * const queueSet = new DownloadQueueSet( policy, this );
    connectQueueSet( queueSet );
    d->m_queueSets.append( QPair<DownloadPolicyKey, DownloadQueueSet *>
                           ( queueSet->downloadPolicy().key(), queueSet ));
}

void HttpDownloadManager::addJob( const QUrl& sourceUrl, const QString& destFileName,
                                  const QString &id, const DownloadUsage usage )
{
    if ( !d->m_downloadEnabled )
        return;

    DownloadQueueSet * const queueSet = d->findQueues( sourceUrl.host(), usage );
    if ( queueSet->canAcceptJob( sourceUrl, destFileName )) {
        HttpJob * const job = d->createJob( sourceUrl, destFileName, id );
        if ( job ) {
            job->setDownloadUsage( usage );
            queueSet->addJob( job );
        }
    }
}

void HttpDownloadManager::finishJob( const QByteArray& data, const QString& destinationFileName,
                                     const QString& id )
{
    mDebug() << "emitting downloadComplete( QByteArray, " << id << ")";
    emit downloadComplete( data, id );
    if ( d->m_storagePolicy ) {
        const bool saved = d->m_storagePolicy->updateFile( destinationFileName, data );
        if ( saved ) {
            mDebug() << "emitting downloadComplete( " << destinationFileName << ", " << id << ")";
            emit downloadComplete( destinationFileName, id );
        } else {
            qWarning() << "Could not save:" << destinationFileName;
        }
    }
}

void HttpDownloadManager::requeue()
{
    d->m_requeueTimer->stop();

    QList<QPair<DownloadPolicyKey, DownloadQueueSet *> >::iterator pos = d->m_queueSets.begin();
    QList<QPair<DownloadPolicyKey, DownloadQueueSet *> >::iterator const end = d->m_queueSets.end();
    for (; pos != end; ++pos ) {
        (*pos).second->retryJobs();
    }
}

void HttpDownloadManager::startRetryTimer()
{
    if ( !d->m_requeueTimer->isActive() )
        d->m_requeueTimer->start();
}

void HttpDownloadManager::connectDefaultQueueSets()
{
    QMap<DownloadUsage, DownloadQueueSet *>::iterator pos = d->m_defaultQueueSets.begin();
    QMap<DownloadUsage, DownloadQueueSet *>::iterator const end = d->m_defaultQueueSets.end();
    for (; pos != end; ++pos )
        connectQueueSet( pos.value() );
}

void HttpDownloadManager::connectQueueSet( DownloadQueueSet * queueSet )
{
    connect( queueSet, SIGNAL( jobFinished( QByteArray, QString, QString )),
             SLOT( finishJob( QByteArray, QString, QString )));
    connect( queueSet, SIGNAL( jobRetry() ), SLOT( startRetryTimer() ));
    connect( queueSet, SIGNAL( jobRedirected( QUrl, QString, QString, DownloadUsage )),
             SLOT( addJob( QUrl, QString, QString, DownloadUsage )));
    // relay jobAdded/jobRemoved signals (interesting for progress bar)
    connect( queueSet, SIGNAL( jobAdded() ), SIGNAL( jobAdded() ));
    connect( queueSet, SIGNAL( jobRemoved() ), SIGNAL( jobRemoved() ));
}

bool HttpDownloadManager::hasDownloadPolicy( const DownloadPolicy& policy ) const
{
    bool found = false;
    QList<QPair<DownloadPolicyKey, DownloadQueueSet*> >::iterator pos = d->m_queueSets.begin();
    QList<QPair<DownloadPolicyKey, DownloadQueueSet*> >::iterator const end = d->m_queueSets.end();
    for (; pos != end; ++pos ) {
        if ( (*pos).second->downloadPolicy() == policy ) {
            found = true;
            break;
        }
    }
    return found;
}

#include "HttpDownloadManager.moc"
