// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef BOOKMARKSYNCMANAGER_H
#define BOOKMARKSYNCMANAGER_H

#include "MergeItem.h"
#include "marble_export.h"

#include <QObject>

namespace Marble {

class CloudSyncManager;
class BookmarkManager;

class MARBLE_EXPORT BookmarkSyncManager : public QObject
{
    Q_OBJECT

    Q_PROPERTY( bool bookmarkSyncEnabled READ isBookmarkSyncEnabled WRITE setBookmarkSyncEnabled NOTIFY bookmarkSyncEnabledChanged )

public:
    explicit BookmarkSyncManager( CloudSyncManager *cloudSyncManager );
    ~BookmarkSyncManager() override;

    /**
     * Last time Marble synced everything up
     */
    QDateTime lastSync() const;

    /**
     * Checks if the user enabled bookmark synchronization.
     * @return true if bookmark synchronization enabled
     */
    bool isBookmarkSyncEnabled() const;

    /**
     * Setter for enabling/disabling bookmark synchronization.
     * @param enabled Status of bookmark synchronization
     */
    void setBookmarkSyncEnabled( bool enabled );

    void setBookmarkManager(BookmarkManager *manager);

Q_SIGNALS:
    void bookmarkSyncEnabledChanged(bool enabled);
    void uploadProgress( qint64 sent, qint64 total );
    void downloadProgress( qint64 received, qint64 total );
    void mergeConflict( MergeItem *item );
    void syncComplete();

public Q_SLOTS:
    /**
     * Initiates running of synchronization "method chain".
     */
    void startBookmarkSync();

    /**
     *
     * @param item
     */
    void resolveConflict( MergeItem *item );

private:
    Q_PRIVATE_SLOT(d, void saveDownloadedToCache( const QByteArray &kml ) )
    Q_PRIVATE_SLOT(d, void parseTimestamp() )
    Q_PRIVATE_SLOT(d, void copyLocalToCache() )

    Q_PRIVATE_SLOT(d, void continueSynchronization() )
    Q_PRIVATE_SLOT(d, void completeSynchronization() )
    Q_PRIVATE_SLOT(d, void completeMerge() )
    Q_PRIVATE_SLOT(d, void completeUpload() )

private:
    class Private;
    Private *d;
};

}

#endif // BOOKMARKSYNCMANAGER_H
