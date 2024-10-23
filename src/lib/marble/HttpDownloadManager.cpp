// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2006-2007 Torsten Rahn <tackat@kde.org>
// SPDX-FileCopyrightText: 2007 Inge Wallin <ingwa@kde.org>
// SPDX-FileCopyrightText: 2008, 2009 Jens-Michael Hoffmann <jensmh@gmx.de>
//

#include "HttpDownloadManager.h"

#include <QCoreApplication>
#include <QList>
#include <QMap>
#include <QNetworkAccessManager>
#include <QTimer>

#include "DownloadPolicy.h"
#include "DownloadQueueSet.h"
#include "HttpJob.h"
#include "MarbleDebug.h"
#include "StoragePolicy.h"

using namespace Marble;

// Time before a failed download job is requeued in ms
const quint32 requeueTime = 60000;

class Q_DECL_HIDDEN HttpDownloadManager::Private
{
public:
    Private(HttpDownloadManager *parent, StoragePolicy *policy);
    ~Private();

    void connectDefaultQueueSets();
    void connectQueueSet(DownloadQueueSet *);
    bool hasDownloadPolicy(const DownloadPolicy &policy) const;
    void finishJob(const QByteArray &, const QString &, const QString &id);
    void requeue();
    void startRetryTimer();

    DownloadQueueSet *findQueues(const QString &hostName, const DownloadUsage usage);

    HttpDownloadManager *m_downloadManager;
    QTimer m_requeueTimer;
    /**
     * Contains per download policy a queue set containing of
     * - a queue where jobs are waiting for being activated (=downloaded)
     * - a queue containing currently being downloaded
     * - a queue for retries of failed downloads */
    QList<QPair<DownloadPolicyKey, DownloadQueueSet *>> m_queueSets;
    QMap<DownloadUsage, DownloadQueueSet *> m_defaultQueueSets;
    StoragePolicy *const m_storagePolicy;
    QNetworkAccessManager m_networkAccessManager;
    bool m_acceptJobs;
};

HttpDownloadManager::Private::Private(HttpDownloadManager *parent, StoragePolicy *policy)
    : m_downloadManager(parent)
    , m_requeueTimer()
    , m_storagePolicy(policy)
    , m_networkAccessManager()
    , m_acceptJobs(true)
{
    // setup default download policy and associated queue set
    DownloadPolicy defaultBrowsePolicy;
    defaultBrowsePolicy.setMaximumConnections(20);
    m_defaultQueueSets[DownloadBrowse] = new DownloadQueueSet(defaultBrowsePolicy);
    DownloadPolicy defaultBulkDownloadPolicy;
    defaultBulkDownloadPolicy.setMaximumConnections(2);
    m_defaultQueueSets[DownloadBulk] = new DownloadQueueSet(defaultBulkDownloadPolicy);
}

HttpDownloadManager::Private::~Private()
{
    QMap<DownloadUsage, DownloadQueueSet *>::iterator pos = m_defaultQueueSets.begin();
    QMap<DownloadUsage, DownloadQueueSet *>::iterator const end = m_defaultQueueSets.end();
    for (; pos != end; ++pos)
        delete pos.value();
}

DownloadQueueSet *HttpDownloadManager::Private::findQueues(const QString &hostName, const DownloadUsage usage)
{
    DownloadQueueSet *result = nullptr;
    QList<QPair<DownloadPolicyKey, DownloadQueueSet *>>::iterator pos = m_queueSets.begin();
    QList<QPair<DownloadPolicyKey, DownloadQueueSet *>>::iterator const end = m_queueSets.end();
    for (; pos != end; ++pos) {
        if ((*pos).first.matches(hostName, usage)) {
            result = (*pos).second;
            break;
        }
    }
    if (!result) {
        mDebug() << "No download policy found for" << hostName << usage << ", using default policy.";
        result = m_defaultQueueSets[usage];
    }
    return result;
}

HttpDownloadManager::HttpDownloadManager(StoragePolicy *policy)
    : d(new Private(this, policy))
{
    d->m_requeueTimer.setInterval(requeueTime);
    connect(&d->m_requeueTimer, SIGNAL(timeout()), this, SLOT(requeue()));
    d->connectDefaultQueueSets();
}

HttpDownloadManager::~HttpDownloadManager()
{
    delete d;
}

void HttpDownloadManager::setDownloadEnabled(const bool enable)
{
    /*
        PORT_QT6
        d->m_networkAccessManager.setNetworkAccessible( enable ? QNetworkAccessManager::Accessible : QNetworkAccessManager::NotAccessible );
    */
    d->m_acceptJobs = enable;
    QList<QPair<DownloadPolicyKey, DownloadQueueSet *>>::iterator pos = d->m_queueSets.begin();
    QList<QPair<DownloadPolicyKey, DownloadQueueSet *>>::iterator const end = d->m_queueSets.end();
    for (; pos != end; ++pos) {
        pos->second->purgeJobs();
    }
}

void HttpDownloadManager::addDownloadPolicy(const DownloadPolicy &policy)
{
    if (d->hasDownloadPolicy(policy))
        return;
    auto const queueSet = new DownloadQueueSet(policy, this);
    d->connectQueueSet(queueSet);
    d->m_queueSets.append(QPair<DownloadPolicyKey, DownloadQueueSet *>(queueSet->downloadPolicy().key(), queueSet));
}

void HttpDownloadManager::addJob(const QUrl &sourceUrl, const QString &destFileName, const QString &id, const DownloadUsage usage)
{
    if (!d->m_acceptJobs) {
        mDebug() << "Working offline, not adding job";
        return;
    }

    DownloadQueueSet *const queueSet = d->findQueues(sourceUrl.host(), usage);
    if (queueSet->canAcceptJob(sourceUrl, destFileName)) {
        auto const job = new HttpJob(sourceUrl, destFileName, id, &d->m_networkAccessManager);
        job->setUserAgentPluginId(QStringLiteral("QNamNetworkPlugin"));
        job->setDownloadUsage(usage);
        mDebug() << "adding job " << sourceUrl;
        queueSet->addJob(job);
    }
}

void HttpDownloadManager::Private::finishJob(const QByteArray &data, const QString &destinationFileName, const QString &id)
{
    mDebug() << "emitting downloadComplete( QByteArray, " << id << ")";
    Q_EMIT m_downloadManager->downloadComplete(data, id);
    if (m_storagePolicy) {
        const bool saved = m_storagePolicy->updateFile(destinationFileName, data);
        if (saved) {
            mDebug() << "emitting downloadComplete( " << destinationFileName << ", " << id << ")";
            Q_EMIT m_downloadManager->downloadComplete(destinationFileName, id);
        } else {
            qWarning() << "Could not save:" << destinationFileName;
        }
    }
}

void HttpDownloadManager::Private::requeue()
{
    m_requeueTimer.stop();

    QList<QPair<DownloadPolicyKey, DownloadQueueSet *>>::iterator pos = m_queueSets.begin();
    QList<QPair<DownloadPolicyKey, DownloadQueueSet *>>::iterator const end = m_queueSets.end();
    for (; pos != end; ++pos) {
        (*pos).second->retryJobs();
    }
}

void HttpDownloadManager::Private::startRetryTimer()
{
    if (!m_requeueTimer.isActive())
        m_requeueTimer.start();
}

void HttpDownloadManager::Private::connectDefaultQueueSets()
{
    QMap<DownloadUsage, DownloadQueueSet *>::iterator pos = m_defaultQueueSets.begin();
    QMap<DownloadUsage, DownloadQueueSet *>::iterator const end = m_defaultQueueSets.end();
    for (; pos != end; ++pos)
        connectQueueSet(pos.value());
}

void HttpDownloadManager::Private::connectQueueSet(DownloadQueueSet *queueSet)
{
    connect(queueSet, SIGNAL(jobFinished(QByteArray, QString, QString)), m_downloadManager, SLOT(finishJob(QByteArray, QString, QString)));
    connect(queueSet, SIGNAL(jobRetry()), m_downloadManager, SLOT(startRetryTimer()));
    connect(queueSet, SIGNAL(jobRedirected(QUrl, QString, QString, DownloadUsage)), m_downloadManager, SLOT(addJob(QUrl, QString, QString, DownloadUsage)));
    // relay jobAdded/jobRemoved signals (interesting for progress bar)
    connect(queueSet, SIGNAL(jobAdded()), m_downloadManager, SIGNAL(jobAdded()));
    connect(queueSet, SIGNAL(jobRemoved()), m_downloadManager, SIGNAL(jobRemoved()));
    connect(queueSet, SIGNAL(progressChanged(int, int)), m_downloadManager, SIGNAL(progressChanged(int, int)));
}

bool HttpDownloadManager::Private::hasDownloadPolicy(const DownloadPolicy &policy) const
{
    bool found = false;
    QList<QPair<DownloadPolicyKey, DownloadQueueSet *>>::const_iterator pos = m_queueSets.constBegin();
    QList<QPair<DownloadPolicyKey, DownloadQueueSet *>>::const_iterator const end = m_queueSets.constEnd();
    for (; pos != end; ++pos) {
        if ((*pos).second->downloadPolicy() == policy) {
            found = true;
            break;
        }
    }
    return found;
}

QByteArray HttpDownloadManager::userAgent(const QString &platform, const QString &component)
{
    QString result(QStringLiteral("Mozilla/5.0 (compatible; Marble/%1; %2; %3; %4; %5)"));
    bool const smallScreen = MarbleGlobal::getInstance()->profiles() & MarbleGlobal::SmallScreen;
    QString const device = smallScreen ? QStringLiteral("MobileDevice") : QStringLiteral("DesktopDevice");
    QString const app = QCoreApplication::applicationName();
    result = result.arg(QString::fromLatin1(MARBLE_VERSION_STRING), device, platform, component, app);
    return result.toLatin1();
}

#include "moc_HttpDownloadManager.cpp"
