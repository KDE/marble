// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#ifndef MARBLE_FILESTORAGEWATCHER_H
#define MARBLE_FILESTORAGEWATCHER_H

#include <QDateTime>
#include <QMultiMap>
#include <QMutex>
#include <QThread>

namespace Marble
{

// Worker object that lives inside the new Thread
class FileStorageWatcherThread : public QObject
{
    Q_OBJECT

public:
    explicit FileStorageWatcherThread(const QString &dataDirectory, QObject *parent = nullptr);

    ~FileStorageWatcherThread() override;

    quint64 cacheLimit();

Q_SIGNALS:
    /**
     * Is emitted when a variable has changed.
     */
    void variableChanged();

public Q_SLOTS:
    /**
     * Sets the limit of the cache in @p bytes.
     */
    void setCacheLimit(quint64 bytes);

    /**
     * Add @p bytes to the current cache size.
     * So FileStorageWatcher is aware of the current cache size.
     */
    void addToCurrentSize(qint64 bytes);

    /**
     * Setting current cache size to 0.
     */
    void resetCurrentSize();

    /**
     * Stop doing things that take a long time to quit.
     */
    void prepareQuit();

    /**
     * Getting the current size of the data stored on the disc
     */
    void getCurrentCacheSize();

private Q_SLOTS:
    /**
     * Ensures that the cache doesn't exceed limits.
     */
    void ensureCacheSize();

private:
    Q_DISABLE_COPY(FileStorageWatcherThread)

    /**
     * Returns true if it is necessary to delete files.
     */
    bool keepDeleting() const;

    QString m_dataDirectory;
    QMultiMap<QDateTime, QString> m_filesCache;
    quint64 m_cacheLimit;
    quint64 m_cacheSoftLimit;
    quint64 m_currentCacheSize;
    int m_filesDeleted;
    bool m_deleting;
    QMutex m_limitMutex;
    bool m_willQuit;
};

// Lives inside main thread
class FileStorageWatcher : public QThread
{
    Q_OBJECT

public:
    /**
     * Creates a new FileStorageWatcher, which is a thread watching the
     * space Marble takes on the hard drive and deletes files if necessary.
     *
     * @param dataDirectory The directory where the data is stored
     * @param parent The parent of the object.
     */
    explicit FileStorageWatcher(const QString &dataDirectory = QString(), QObject *parent = nullptr);

    ~FileStorageWatcher() override;

    /**
     * Returns the limit of the cache in bytes.
     */
    quint64 cacheLimit();

public Q_SLOTS:
    /**
     * Sets the limit of the cache in @p bytes.
     */
    void setCacheLimit(quint64 bytes);

    /**
     * Add @p bytes to the current cache size.
     * So FileStorageWatcher is aware of the current cache size.
     */
    void addToCurrentSize(qint64 bytes);

    /**
     * Setting current cache size to 0.
     */
    void resetCurrentSize();

Q_SIGNALS:
    void sizeChanged(qint64 bytes);
    void cleared();

protected:
    /**
     * The function being called at starting Thread.
     * The thread is started by QThread::start().
     */
    void run() override;

private:
    Q_DISABLE_COPY(FileStorageWatcher)

    QString m_dataDirectory;
    FileStorageWatcherThread *m_thread;
    QMutex *m_limitMutex;
    quint64 m_limit;
    bool m_started;
    bool m_quitting;
};

}

#endif
