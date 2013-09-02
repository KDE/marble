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

class GeoDataFolder;
class GeoDataDocument;
class GeoDataContainer;
class CloudSyncManager;

class DiffItem
{
public:
    enum Action {
        NoAction,
        Created,
        Changed,
        Deleted
    };

    enum Status {
        Source,
        Destination
    };

    QString m_path;
    Action m_action;
    Status m_origin;
    GeoDataPlacemark m_placemarkA;
    GeoDataPlacemark m_placemarkB;
};

class MARBLE_EXPORT BookmarkSyncManager : public QObject
{
    Q_OBJECT

public:
    BookmarkSyncManager( CloudSyncManager *cloudSyncManager );
    ~BookmarkSyncManager();

private:
    /**
     * Returns an API endpoint
     * @param endpoint Endpoint itself without server info
     * @return Complete API URL as QUrl
     */
    QUrl endpointUrl( const QString &endpoint );

    /**
     * Uploads local bookmarks.kml to cloud.
     */
    void uploadBookmarks();

    /**
     * Downloads bookmarks.kml from cloud.
     */
    void downloadBookmarks();

    /**
     * Gets cloud bookmarks.kml's timestamp from cloud.
     */
    void downloadTimestamp();

    /**
     * Compares cloud bookmarks.kml's timestamp to last synced bookmarks.kml's timestamp.
     * @return true if cloud one is different from last synced one.
     */
    bool cloudBookmarksModified( const QString &cloudTimestamp );

    /**
     * Removes all KMLs in the cache except the
     * one with yougnest timestamp.
     */
    void clearCache();

    /**
     * Finds the last synced bookmarks.kml file and returns its path
     * @return Path of last synced bookmarks.kml file.
     */
    QString lastSyncedKmlPath();

    /**
     * Gets all placemarks in a document as DiffItems, compares them to another document and puts the result in a list.
     * @param document The document whose placemarks will be compared to another document's placemarks.
     * @param other The document whose placemarks will be compared to the first document's placemarks.
     * @param diffDirection Direction of comparison, e.g. must be DiffItem::Destination if direction is source to destination.
     * @return A list of DiffItems
     */
    QList<DiffItem> getPlacemarks(GeoDataDocument *document, GeoDataDocument *other, DiffItem::Status diffDirection );

    /**
     * Gets all placemarks in a document as DiffItems, compares them to another document and puts the result in a list.
     * @param folder The folder whose placemarks will be compared to another document's placemarks.
     * @param path Path of the folder.
     * @param other The document whose placemarks will be compared to the first document's placemarks.
     * @param diffDirection Direction of comparison, e.g. must be DiffItem::Destination if direction is source to destination.
     * @return A list of DiffItems
     */
    QList<DiffItem> getPlacemarks( GeoDataFolder *folder, QString &path, GeoDataDocument *other, DiffItem::Status diffDirection );

    /**
     * Finds the placemark which has the same coordinates with given bookmark
     * @param container Container of placemarks which will be compared. Can be document or folder.
     * @param bookmark The bookmark whose counterpart will be searched in the container.
     * @return Counterpart of the given placemark.
     */
    GeoDataPlacemark* findPlacemark( GeoDataContainer* container, const GeoDataPlacemark &bookmark ) const;

    /**
     * Determines the status (created, deleted, changed or unchanged) of given DiffItem
     * by comparing the item's placemark with placemarks of given GeoDataDocument.
     * @param item The item whose status will be determined.
     * @param document The document whose placemarks will be used to determine DiffItem's status.
     */
    void determineDiffStatus( DiffItem &item, GeoDataDocument* document );

    /**
     * Finds differences between two bookmark files.
     * @param sourcePath Source bookmark
     * @param destinationPath Destination bookmark
     * @return A list of differences
     */
    QList<DiffItem> diff( QString &sourcePath, QString &destinationPath );

    /**
     * Merges two diff lists.
     * @param diffListA First diff list.
     * @param diffListB Second diff list.
     * @return Merged DiffItems.
     */
    void merge();

    /**
     * Creates GeoDataFolders using strings in path list.
     * @param container Container which created GeoDataFolder will be attached to.
     * @param pathList Names of folders. Note that each item will be the child of the previous one.
     * @return A pointer to created folder.
     */
    GeoDataFolder* createFolders( GeoDataContainer *container, QStringList &pathList );

    /**
     * Creates a GeoDataDocument using a list of DiffItems.
     * @param mergedList DiffItems which will be used as placemarks.
     * @return A pointer to created document.
     */
    GeoDataDocument* constructDocument( const QList<DiffItem> &mergedList );

public slots:
    void resolveConflict( MergeItem *item );

    /**
     * Initiates running of synchronization "method chain".
     */
    void startBookmarkSync();

private slots:
    void saveDownloadedToCache( const QByteArray &kml );
    void parseTimestamp();
    void copyLocalToCache();

    // Bookmark synchronization steps, not intended for other uses
    void continueSynchronization();
    void completeSynchronization();
    void completeMerge();
    void completeUpload();

signals:
    void uploadProgress( qint64 sent, qint64 total );
    void downloadProgress( qint64 received, qint64 total );
    void timestampDownloaded();
    void bookmarksDownloaded();
    void mergeConflict( Marble::MergeItem *item );
    void syncComplete();

private:
    class Private;
    Private *d;
};

}

#endif // BOOKMARKSYNCMANAGER_H
