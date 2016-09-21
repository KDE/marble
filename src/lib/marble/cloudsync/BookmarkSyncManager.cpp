//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2013   Utku Aydın <utkuaydin34@gmail.com>
//

#include "BookmarkSyncManager.h"

#include "GeoWriter.h"
#include "MarbleMath.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "GeoDataParser.h"
#include "GeoDataFolder.h"
#include "GeoDataDocument.h"
#include "GeoDataLookAt.h"
#include "CloudSyncManager.h"
#include "GeoDataCoordinates.h"
#include "OwncloudSyncBackend.h"
#include "MarbleModel.h"
#include "BookmarkManager.h"

#include <QFile>
#include <QBuffer>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTimer>

namespace Marble {

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

class Q_DECL_HIDDEN BookmarkSyncManager::Private
{
public:
    Private( BookmarkSyncManager* parent, CloudSyncManager *cloudSyncManager );

    BookmarkSyncManager* m_q;
    CloudSyncManager *m_cloudSyncManager;

    QNetworkAccessManager m_network;
    QString m_uploadEndpoint;
    QString m_downloadEndpoint;
    QString m_timestampEndpoint;

    QNetworkReply* m_uploadReply;
    QNetworkReply* m_downloadReply;
    QNetworkReply* m_timestampReply;

    QString m_cloudTimestamp;

    QString m_cachePath;
    QString m_localBookmarksPath;
    QString m_bookmarksTimestamp;

    QList<DiffItem> m_diffA;
    QList<DiffItem> m_diffB;
    QList<DiffItem> m_merged;
    DiffItem m_conflictItem;

    BookmarkManager* m_bookmarkManager;
    QTimer m_syncTimer;
    bool m_bookmarkSyncEnabled;

    /**
     * Returns an API endpoint
     * @param endpoint Endpoint itself without server info
     * @return Complete API URL as QUrl
     */
    QUrl endpointUrl( const QString &endpoint ) const;

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
    bool cloudBookmarksModified( const QString &cloudTimestamp ) const;

    /**
     * Removes all KMLs in the cache except the
     * one with yougnest timestamp.
     */
    void clearCache();

    /**
     * Finds the last synced bookmarks.kml file and returns its path
     * @return Path of last synced bookmarks.kml file.
     */
    QString lastSyncedKmlPath() const;

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
    const GeoDataPlacemark* findPlacemark( GeoDataContainer* container, const GeoDataPlacemark &bookmark ) const;

    /**
     * Determines the status (created, deleted, changed or unchanged) of given DiffItem
     * by comparing the item's placemark with placemarks of given GeoDataDocument.
     * @param item The item whose status will be determined.
     * @param document The document whose placemarks will be used to determine DiffItem's status.
     */
    void determineDiffStatus( DiffItem &item, GeoDataDocument* document ) const;

    /**
     * Finds differences between two bookmark files.
     * @param sourcePath Source bookmark
     * @param destinationPath Destination bookmark
     * @return A list of differences
     */
    QList<DiffItem> diff( QString &sourcePath, QString &destinationPath );
    QList<DiffItem> diff( QString &sourcePath, QIODevice* destination );
    QList<DiffItem> diff( QIODevice* source, QString &destinationPath );
    QList<DiffItem> diff( QIODevice *source, QIODevice* destination );

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

    void saveDownloadedToCache( const QByteArray &kml );

    void parseTimestamp();
    void copyLocalToCache();

    void continueSynchronization();
    void completeSynchronization();
    void completeMerge();
    void completeUpload();
};

BookmarkSyncManager::Private::Private(BookmarkSyncManager *parent, CloudSyncManager *cloudSyncManager ) :
  m_q( parent ),
  m_cloudSyncManager( cloudSyncManager ),
  m_bookmarkManager( 0 ),
  m_bookmarkSyncEnabled( false )
{
    m_cachePath = MarbleDirs::localPath() + QLatin1String("/cloudsync/cache/bookmarks");
    m_localBookmarksPath = MarbleDirs::localPath() + QLatin1String("/bookmarks/bookmarks.kml");
    m_downloadEndpoint = "bookmarks/kml";
    m_uploadEndpoint = "bookmarks/update";
    m_timestampEndpoint = "bookmarks/timestamp";
}

BookmarkSyncManager::BookmarkSyncManager( CloudSyncManager *cloudSyncManager ) :
  QObject(),
  d( new Private( this, cloudSyncManager ) )
{
    d->m_syncTimer.setInterval( 60 * 60 * 1000 ); // 1 hour. TODO: Make this configurable.
    connect( &d->m_syncTimer, SIGNAL(timeout()), this, SLOT(startBookmarkSync()) );
}

BookmarkSyncManager::~BookmarkSyncManager()
{
    delete d;
}

QDateTime BookmarkSyncManager::lastSync() const
{
    const QString last = d->lastSyncedKmlPath();
    if (last.isEmpty())
        return QDateTime();
    return QFileInfo(last).created();
}

bool BookmarkSyncManager::isBookmarkSyncEnabled() const
{
    return d->m_bookmarkSyncEnabled;
}

void BookmarkSyncManager::setBookmarkSyncEnabled( bool enabled )
{
    bool const old_state = isBookmarkSyncEnabled();
    d->m_bookmarkSyncEnabled = enabled;
    if ( old_state != isBookmarkSyncEnabled() ) {
        emit bookmarkSyncEnabledChanged( d->m_bookmarkSyncEnabled );
        if ( isBookmarkSyncEnabled() ) {
            startBookmarkSync();
        }
    }
}

void BookmarkSyncManager::setBookmarkManager(BookmarkManager *manager)
{
  d->m_bookmarkManager = manager;
  connect( manager, SIGNAL(bookmarksChanged()), this, SLOT(startBookmarkSync()) );
  startBookmarkSync();
}

void BookmarkSyncManager::startBookmarkSync()
{
    if ( !d->m_cloudSyncManager->isSyncEnabled() || !isBookmarkSyncEnabled() )
    {
        return;
    }

    d->m_syncTimer.start();
    d->downloadTimestamp();
}

QUrl BookmarkSyncManager::Private::endpointUrl( const QString &endpoint ) const
{
    return QUrl(m_cloudSyncManager->apiUrl().toString() + QLatin1Char('/') + endpoint);
}

void BookmarkSyncManager::Private::uploadBookmarks()
{
    QByteArray data;
    QByteArray lineBreak = "\r\n";
    QString word = "----MarbleCloudBoundary";
    QString boundary = QString( "--%0" ).arg( word );
    QNetworkRequest request( endpointUrl( m_uploadEndpoint ) );
    request.setHeader( QNetworkRequest::ContentTypeHeader, QString( "multipart/form-data; boundary=%0" ).arg( word ) );

    data.append( QString( boundary + lineBreak ).toUtf8() );
    data.append( "Content-Disposition: form-data; name=\"bookmarks\"; filename=\"bookmarks.kml\"" + lineBreak );
    data.append( "Content-Type: application/vnd.google-earth.kml+xml" + lineBreak + lineBreak );

    QFile bookmarksFile( m_localBookmarksPath );
    if( !bookmarksFile.open( QFile::ReadOnly ) ) {
        mDebug() << "Failed to open file" << bookmarksFile.fileName()
                 <<  ". It is either missing or not readable.";
        return;
    }

    QByteArray kmlContent = bookmarksFile.readAll();
    data.append( kmlContent + lineBreak + lineBreak );
    data.append( QString( boundary ).toUtf8() );
    bookmarksFile.close();

    m_uploadReply = m_network.post( request, data );
    connect( m_uploadReply, SIGNAL(uploadProgress(qint64,qint64)),
             m_q, SIGNAL(uploadProgress(qint64,qint64)) );
    connect( m_uploadReply, SIGNAL(finished()),
             m_q, SLOT(completeUpload()) );
}

void BookmarkSyncManager::Private::downloadBookmarks()
{
    QNetworkRequest request( endpointUrl( m_downloadEndpoint ) );
    m_downloadReply = m_network.get( request );
    connect( m_downloadReply, SIGNAL(finished()),
             m_q, SLOT(completeSynchronization()) );
    connect( m_downloadReply, SIGNAL(downloadProgress(qint64,qint64)),
             m_q, SIGNAL(downloadProgress(qint64,qint64)) );
}

void BookmarkSyncManager::Private::downloadTimestamp()
{
    mDebug() << "Determining remote bookmark state.";
    m_timestampReply = m_network.get( QNetworkRequest( endpointUrl( m_timestampEndpoint ) ) );
    connect( m_timestampReply, SIGNAL(finished()),
             m_q, SLOT(parseTimestamp()) );
}

bool BookmarkSyncManager::Private::cloudBookmarksModified( const QString &cloudTimestamp ) const
{
    QStringList entryList = QDir( m_cachePath ).entryList(
                // TODO: replace with regex filter that only
                // allows timestamp filenames
                QStringList() << "*.kml",
                QDir::NoFilter, QDir::Name );
    if( !entryList.isEmpty() ) {
        QString lastSynced = entryList.last();
        lastSynced.chop( 4 );
        return cloudTimestamp != lastSynced;
    } else {
        return true; // That will let cloud one get downloaded.
    }
}

void BookmarkSyncManager::Private::clearCache()
{
    QDir cacheDir( m_cachePath );
    QFileInfoList fileInfoList = cacheDir.entryInfoList(
                QStringList() << "*.kml",
                QDir::NoFilter, QDir::Name );
    if( !fileInfoList.isEmpty() ) {
        foreach ( const QFileInfo& fileInfo, fileInfoList ) {
            QFile file( fileInfo.absoluteFilePath() );
            bool removed = file.remove();
            if( !removed ) {
                mDebug() << "Could not delete" << file.fileName() <<
                         "Make sure you have sufficient permissions.";
            }
        }
    }
}

QString BookmarkSyncManager::Private::lastSyncedKmlPath() const
{
    QDir cacheDir( m_cachePath );
    QFileInfoList fileInfoList = cacheDir.entryInfoList(
                QStringList() << "*.kml",
                QDir::NoFilter, QDir::Name );
    if( !fileInfoList.isEmpty() ) {
        return fileInfoList.last().absoluteFilePath();
    } else {
        return QString();
    }
}

QList<DiffItem> BookmarkSyncManager::Private::getPlacemarks( GeoDataDocument *document, GeoDataDocument *other, DiffItem::Status diffDirection )
{
    QList<DiffItem> diffItems;
    foreach ( GeoDataFolder *folder, document->folderList() ) {
        QString path = QString( "/%0" ).arg( folder->name() );
        diffItems.append( getPlacemarks( folder, path, other, diffDirection ) );
    }

    return diffItems;
}

QList<DiffItem> BookmarkSyncManager::Private::getPlacemarks( GeoDataFolder *folder, QString &path, GeoDataDocument *other, DiffItem::Status diffDirection )
{
    QList<DiffItem> diffItems;
    foreach ( GeoDataFolder *folder, folder->folderList() ) {
        QString newPath = QString( "%0/%1" ).arg( path, folder->name() );
        diffItems.append( getPlacemarks( folder, newPath, other, diffDirection ) );
    }

    foreach( GeoDataPlacemark *placemark, folder->placemarkList() ) {
        DiffItem diffItem;
        diffItem.m_path = path;
        diffItem.m_placemarkA = *placemark;
        switch ( diffDirection ) {
        case DiffItem::Source:
            diffItem.m_origin = DiffItem::Destination;
            break;
        case DiffItem::Destination:
            diffItem.m_origin = DiffItem::Source;
            break;
        default:
            break;
        }

        determineDiffStatus( diffItem, other );

        if( !( diffItem.m_action == DiffItem::NoAction && diffItem.m_origin == DiffItem::Destination )
                && !( diffItem.m_action == DiffItem::Changed && diffItem.m_origin == DiffItem::Source ) ) {
            diffItems.append( diffItem );
        }
    }

    return diffItems;
}

const GeoDataPlacemark* BookmarkSyncManager::Private::findPlacemark( GeoDataContainer* container, const GeoDataPlacemark &bookmark ) const
{
    foreach( GeoDataPlacemark* placemark, container->placemarkList() ) {
        if ( EARTH_RADIUS * distanceSphere( placemark->coordinate(), bookmark.coordinate() ) <= 1 ) {
            return placemark;
        }
    }

    foreach( GeoDataFolder* folder, container->folderList() ) {
        const GeoDataPlacemark* placemark = findPlacemark( folder, bookmark );
        if ( placemark ) {
            return placemark;
        }
    }

    return 0;
}

void BookmarkSyncManager::Private::determineDiffStatus( DiffItem &item, GeoDataDocument *document ) const
{
    const GeoDataPlacemark *match = findPlacemark( document, item.m_placemarkA );

    if( match != 0 ) {
        item.m_placemarkB = *match;
        bool nameChanged = item.m_placemarkA.name() != item.m_placemarkB.name();
        bool descChanged = item.m_placemarkA.description() != item.m_placemarkB.description();
        bool lookAtChanged = item.m_placemarkA.lookAt()->latitude() != item.m_placemarkB.lookAt()->latitude() ||
                item.m_placemarkA.lookAt()->longitude() != item.m_placemarkB.lookAt()->longitude() ||
                item.m_placemarkA.lookAt()->altitude() != item.m_placemarkB.lookAt()->altitude() ||
                item.m_placemarkA.lookAt()->range() != item.m_placemarkB.lookAt()->range();
        if(  nameChanged || descChanged || lookAtChanged ) {
            item.m_action = DiffItem::Changed;
        } else {
            item.m_action = DiffItem::NoAction;
        }
    } else {
        switch( item.m_origin ) {
        case DiffItem::Source:
            item.m_action = DiffItem::Deleted;
            item.m_placemarkB = item.m_placemarkA; // for conflict purposes
            break;
        case DiffItem::Destination:
            item.m_action = DiffItem::Created;
            break;
        }

    }
}

QList<DiffItem> BookmarkSyncManager::Private::diff( QString &sourcePath, QString &destinationPath )
{
    QFile fileB( destinationPath );
    if( !fileB.open( QFile::ReadOnly ) ) {
        mDebug() << "Could not open file " << fileB.fileName();
    }
    return diff( sourcePath, &fileB );
}

QList<DiffItem> BookmarkSyncManager::Private::diff( QString &sourcePath, QIODevice *fileB )
{
    QFile fileA( sourcePath );
    if( !fileA.open( QFile::ReadOnly ) ) {
        mDebug() << "Could not open file " << fileA.fileName();
    }

    return diff( &fileA, fileB );
}

QList<DiffItem> BookmarkSyncManager::Private::diff( QIODevice *source, QString &destinationPath )
{
    QFile fileB( destinationPath );
    if( !fileB.open( QFile::ReadOnly ) ) {
        mDebug() << "Could not open file " << fileB.fileName();
    }

    return diff( source, &fileB );
}

QList<DiffItem> BookmarkSyncManager::Private::diff( QIODevice *fileA, QIODevice *fileB )
{
    GeoDataParser parserA( GeoData_KML );
    parserA.read( fileA );
    GeoDataDocument *documentA = dynamic_cast<GeoDataDocument*>( parserA.releaseDocument() );

    GeoDataParser parserB( GeoData_KML );
    parserB.read( fileB );
    GeoDataDocument *documentB = dynamic_cast<GeoDataDocument*>( parserB.releaseDocument() );

    QList<DiffItem> diffItems = getPlacemarks( documentA, documentB, DiffItem::Destination ); // Compare old to new
    diffItems.append( getPlacemarks( documentB, documentA, DiffItem::Source ) ); // Compare new to old

    // Compare paths
    for( int i = 0; i < diffItems.count(); i++ ) {
        for( int p = i + 1; p < diffItems.count(); p++ ) {
            if( ( diffItems[i].m_origin == DiffItem::Source )
                    && ( diffItems[i].m_action == DiffItem::NoAction )
                    && ( EARTH_RADIUS * distanceSphere( diffItems[i].m_placemarkA.coordinate(), diffItems[p].m_placemarkB.coordinate() ) <= 1 )
                    && ( EARTH_RADIUS * distanceSphere( diffItems[i].m_placemarkB.coordinate(), diffItems[p].m_placemarkA.coordinate() ) <= 1 )
                    && ( diffItems[i].m_path != diffItems[p].m_path ) ) {
                diffItems[p].m_action = DiffItem::Changed;
            }
        }
    }

    return diffItems;
}

void BookmarkSyncManager::Private::merge()
{
    foreach( const DiffItem &itemA, m_diffA ) {
        if( itemA.m_action == DiffItem::NoAction ) {
            bool deleted = false;
            bool changed = false;
            DiffItem other;

            foreach( const DiffItem &itemB, m_diffB ) {
                if( EARTH_RADIUS * distanceSphere( itemA.m_placemarkA.coordinate(), itemB.m_placemarkA.coordinate() ) <= 1 ) {
                    if( itemB.m_action == DiffItem::Deleted ) {
                        deleted = true;
                    } else if( itemB.m_action == DiffItem::Changed ) {
                        changed = true;
                        other = itemB;
                    }
                }
            }
            if( changed ) {
                m_merged.append( other );
            } else if( !deleted ) {
                m_merged.append( itemA );
            }
        } else if( itemA.m_action == DiffItem::Created ) {
            m_merged.append( itemA );
        } else if( itemA.m_action == DiffItem::Changed || itemA.m_action == DiffItem::Deleted ) {
            bool conflict = false;
            DiffItem other;

            foreach( const DiffItem &itemB, m_diffB ) {
                if( EARTH_RADIUS * distanceSphere( itemA.m_placemarkB.coordinate(), itemB.m_placemarkB.coordinate() ) <= 1 ) {
                    if( ( itemA.m_action == DiffItem::Changed && ( itemB.m_action == DiffItem::Changed || itemB.m_action == DiffItem::Deleted ) )
                            || ( itemA.m_action == DiffItem::Deleted && itemB.m_action == DiffItem::Changed ) ) {
                        conflict = true;
                        other = itemB;
                    }
                }
            }

            if( !conflict && itemA.m_action == DiffItem::Changed ) {
                m_merged.append( itemA );
            } else if ( conflict ) {
                m_conflictItem = other;
                MergeItem *mergeItem = new MergeItem();
                mergeItem->setPathA( itemA.m_path );
                mergeItem->setPathB( other.m_path );
                mergeItem->setPlacemarkA( itemA.m_placemarkA );
                mergeItem->setPlacemarkB( other.m_placemarkA );

                switch( itemA.m_action ) {
                case DiffItem::Changed:
                    mergeItem->setActionA( MergeItem::Changed );
                    break;
                case DiffItem::Deleted:
                    mergeItem->setActionA( MergeItem::Deleted );
                    break;
                default:
                    break;
                }

                switch( other.m_action ) {
                case DiffItem::Changed:
                    mergeItem->setActionB( MergeItem::Changed );
                    break;
                case DiffItem::Deleted:
                    mergeItem->setActionB( MergeItem::Deleted );
                    break;
                default:
                    break;
                }

                emit m_q->mergeConflict( mergeItem );
                return;
            }
        }

        if( !m_diffA.isEmpty() ) {
            m_diffA.removeFirst();
        }
    }

    foreach( const DiffItem &itemB, m_diffB ) {
        if( itemB.m_action == DiffItem::Created ) {
            m_merged.append( itemB );
        }
    }

    completeMerge();
}

GeoDataFolder* BookmarkSyncManager::Private::createFolders( GeoDataContainer *container, QStringList &pathList )
{
    GeoDataFolder *folder = 0;
    if( pathList.count() > 0 ) {
        QString name = pathList.takeFirst();

        foreach( GeoDataFolder *otherFolder, container->folderList() ) {
            if( otherFolder->name() == name ) {
                folder = otherFolder;
            }
        }

        if( folder == 0 ) {
            folder = new GeoDataFolder();
            folder->setName( name );
            container->append( folder );
        }

        if( pathList.count() == 0 ) {
            return folder;
        }
    }

    return createFolders( folder, pathList );
}

GeoDataDocument* BookmarkSyncManager::Private::constructDocument( const QList<DiffItem> &mergedList )
{
    GeoDataDocument *document = new GeoDataDocument();
    document->setName( tr( "Bookmarks" ) );

    foreach( const DiffItem &item, mergedList ) {
        GeoDataPlacemark *placemark = new GeoDataPlacemark( item.m_placemarkA );
        QStringList splitten = item.m_path.split(QLatin1Char('/'), QString::SkipEmptyParts);
        GeoDataFolder *folder = createFolders( document, splitten );
        folder->append( placemark );
    }

    return document;
}

void BookmarkSyncManager::resolveConflict( MergeItem *item )
{
    DiffItem diffItem;

    switch( item->resolution() ) {
    case MergeItem::A:
        if( !d->m_diffA.isEmpty() ) {
            diffItem = d->m_diffA.first();
            break;
        }
    case MergeItem::B:
        diffItem = d->m_conflictItem;
        break;
    default:
        return; // It shouldn't happen.
    }

    if( diffItem.m_action != DiffItem::Deleted ) {
        d->m_merged.append( diffItem );
    }

    if( !d->m_diffA.isEmpty() ) {
        d->m_diffA.removeFirst();
    }

    d->merge();
}

void BookmarkSyncManager::Private::saveDownloadedToCache( const QByteArray &kml )
{
    QString localBookmarksDir = m_localBookmarksPath;
    QDir().mkdir( localBookmarksDir.remove( "bookmarks.kml" ) );
    QFile bookmarksFile( m_localBookmarksPath );
    if( !bookmarksFile.open( QFile::ReadWrite ) ) {
        mDebug() << "Failed to open file" << bookmarksFile.fileName()
                 <<  ". It is either missing or not readable.";
        return;
    }

    bookmarksFile.write( kml );
    bookmarksFile.close();
    copyLocalToCache();
}

void BookmarkSyncManager::Private::parseTimestamp()
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(m_timestampReply->readAll());
    QJsonValue dataValue = jsonDoc.object().value(QStringLiteral("data"));

    m_cloudTimestamp = dataValue.toString();
    mDebug() << "Remote bookmark timestamp is " << m_cloudTimestamp;
    continueSynchronization();
}
void BookmarkSyncManager::Private::copyLocalToCache()
{
    QDir().mkpath( m_cachePath );
    clearCache();

    QFile bookmarksFile( m_localBookmarksPath );
    bookmarksFile.copy( QString( "%0/%1.kml" ).arg( m_cachePath, m_cloudTimestamp ) );
}

// Bookmark synchronization steps
void BookmarkSyncManager::Private::continueSynchronization()
{
    bool cloudModified = cloudBookmarksModified( m_cloudTimestamp );
    if( cloudModified ) {
        downloadBookmarks();
    } else {
        QString lastSyncedPath = lastSyncedKmlPath();
        if( lastSyncedPath.isEmpty() ) {
            mDebug() << "Never synced. Uploading bookmarks.";
            uploadBookmarks();
        } else {
            QList<DiffItem> diffList = diff( lastSyncedPath, m_localBookmarksPath );
            bool localModified = false;
            foreach( const DiffItem &item, diffList ) {
                if( item.m_action != DiffItem::NoAction ) {
                    localModified = true;
                }
            }

            if( localModified ) {
                mDebug() << "Local modifications, uploading.";
                uploadBookmarks();
            }
        }
    }
}

void BookmarkSyncManager::Private::completeSynchronization()
{
    mDebug() << "Merging remote and local bookmark file";
    QString lastSyncedPath = lastSyncedKmlPath();
    QFile localBookmarksFile( m_localBookmarksPath );
    QByteArray result = m_downloadReply->readAll();
    QBuffer buffer( &result );
    buffer.open( QIODevice::ReadOnly );

    if( lastSyncedPath.isEmpty() ) {
        if( localBookmarksFile.exists() ) {
            mDebug() << "Conflict between remote bookmarks and local ones";
            m_diffA = diff( &buffer, m_localBookmarksPath );
            m_diffB = diff( m_localBookmarksPath, &buffer );
        } else {
            saveDownloadedToCache( result );
            return;
        }
    }
    else
    {
        m_diffA = diff( lastSyncedPath, m_localBookmarksPath );
        m_diffB = diff( lastSyncedPath, &buffer );
    }

    m_merged.clear();
    merge();
}

void BookmarkSyncManager::Private::completeMerge()
{
    QFile localBookmarksFile( m_localBookmarksPath );
    GeoDataDocument *doc = constructDocument( m_merged );
    GeoWriter writer;
    localBookmarksFile.remove();
    localBookmarksFile.open( QFile::ReadWrite );
    writer.write( &localBookmarksFile, doc );
    localBookmarksFile.close();
    uploadBookmarks();
}

void BookmarkSyncManager::Private::completeUpload()
{
    QJsonDocument jsonDoc = QJsonDocument::fromJson(m_uploadReply->readAll());
    QJsonValue dataValue = jsonDoc.object().value(QStringLiteral("data"));

    m_cloudTimestamp = dataValue.toString();
    mDebug() << "Uploaded bookmarks to remote server. Timestamp is " << m_cloudTimestamp;
    copyLocalToCache();
    emit m_q->syncComplete();
}

}

#include "moc_BookmarkSyncManager.cpp"
