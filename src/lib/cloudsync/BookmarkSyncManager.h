//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#ifndef BOOKMARKSYNCMANAGER_H
#define BOOKMARKSYNCMANAGER_H

#include "MergeItem.h"
#include "marble_export.h"
#include "GeoDataPlacemark.h"

#include <QObject>
#include <QNetworkReply>

namespace Marble {

class CloudSyncManager;

class MARBLE_EXPORT BookmarkSyncManager : public QObject
{
    Q_OBJECT

public:
    BookmarkSyncManager( CloudSyncManager *cloudSyncManager );
    ~BookmarkSyncManager();

Q_SIGNALS:
    void uploadProgress( qint64 sent, qint64 total );
    void downloadProgress( qint64 received, qint64 total );
    void mergeConflict( Marble::MergeItem *item );
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
