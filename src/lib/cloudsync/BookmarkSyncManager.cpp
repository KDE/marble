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
#include "CloudSyncManager.h"
#include "GeoDataCoordinates.h"
#include "OwncloudSyncBackend.h"

#include <QFile>
#include <QBuffer>
#include <QScriptValue>
#include <QScriptEngine>
#include <QTemporaryFile>
#include <QNetworkAccessManager>

namespace Marble {

class BookmarkSyncManager::Private
{
public:
    Private( CloudSyncManager *cloudSyncManager );
    ~Private();

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
};

BookmarkSyncManager::Private::Private( CloudSyncManager *cloudSyncManager ) : m_cloudSyncManager( cloudSyncManager )
{
    m_cachePath = QString( "%0/cloudsync/cache/bookmarks" ).arg( MarbleDirs::localPath() );
    m_localBookmarksPath = QString( "%0/bookmarks/bookmarks.kml" ).arg( MarbleDirs::localPath() );
    m_downloadEndpoint = "bookmarks/kml";
    m_uploadEndpoint = "bookmarks/update";
    m_timestampEndpoint = "bookmarks/timestamp";
}

BookmarkSyncManager::Private::~Private()
{
}

BookmarkSyncManager::BookmarkSyncManager( CloudSyncManager *cloudSyncManager ) : QObject(), d( new Private( cloudSyncManager ) )
{
}

BookmarkSyncManager::~BookmarkSyncManager()
{
    delete d;
}

void BookmarkSyncManager::startBookmarkSync()
{
    connect( this, SIGNAL(timestampDownloaded()),
             this, SLOT(continueSynchronization()) );
    downloadTimestamp();
}

QUrl BookmarkSyncManager::endpointUrl( const QString &endpoint )
{
    return QUrl( QString( "%0/%1" ).arg( d->m_cloudSyncManager->apiUrl().toString() ).arg( endpoint ) );
}

void BookmarkSyncManager::uploadBookmarks()
{
    QByteArray data;
    QByteArray lineBreak = "\r\n";
    QString word = "----MarbleCloudBoundary";
    QString boundary = QString( "--%0" ).arg( word );
    QNetworkRequest request( endpointUrl( d->m_uploadEndpoint ) );
    request.setHeader( QNetworkRequest::ContentTypeHeader, QString( "multipart/form-data; boundary=%0" ).arg( word ) );

    data.append( QString( boundary + lineBreak ).toUtf8() );
    data.append( "Content-Disposition: form-data; name=\"bookmarks\"; filename=\"bookmarks.kml\"" + lineBreak );
    data.append( "Content-Type: application/vnd.google-earth.kml+xml" + lineBreak + lineBreak );

    QFile bookmarksFile( d->m_localBookmarksPath );
    if( !bookmarksFile.open( QFile::ReadOnly ) ) {
        mDebug() << "Failed to open file" << bookmarksFile.fileName()
                 <<  ". It is either missing or not readable.";
        return;
    }

    QByteArray kmlContent = bookmarksFile.readAll();
    data.append( kmlContent + lineBreak + lineBreak );
    data.append( QString( boundary ).toUtf8() );
    bookmarksFile.close();

    d->m_uploadReply = d->m_network.post( request, data );
    connect( d->m_uploadReply, SIGNAL(uploadProgress(qint64,qint64)),
             this, SIGNAL(uploadProgress(qint64,qint64)) );
    connect( d->m_uploadReply, SIGNAL(finished()),
             this, SLOT(completeUpload()) );
}

void BookmarkSyncManager::downloadBookmarks()
{
    QNetworkRequest request( endpointUrl( d->m_downloadEndpoint ) );
    d->m_downloadReply = d->m_network.get( request );
    connect( d->m_downloadReply, SIGNAL(finished()),
             this, SIGNAL(bookmarksDownloaded()) );
    connect( d->m_downloadReply, SIGNAL(downloadProgress(qint64,qint64)),
             this, SIGNAL(downloadProgress(qint64,qint64)) );
}

void BookmarkSyncManager::downloadTimestamp()
{
    d->m_timestampReply = d->m_network.get( QNetworkRequest( endpointUrl( d->m_timestampEndpoint ) ) );
    connect( d->m_timestampReply, SIGNAL(finished()),
             this, SLOT(parseTimestamp()) );
}

bool BookmarkSyncManager::cloudBookmarksModified( const QString &cloudTimestamp )
{
    QStringList entryList = QDir( d->m_cachePath ).entryList(
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

void BookmarkSyncManager::clearCache()
{
    QDir cacheDir( d->m_cachePath );
    QFileInfoList fileInfoList = cacheDir.entryInfoList(
                QStringList() << "*.kml",
                QDir::NoFilter, QDir::Name );
    if( !fileInfoList.isEmpty() ) {
        foreach ( QFileInfo fileInfo, fileInfoList ) {
            QFile file( fileInfo.absoluteFilePath() );
            bool removed = file.remove();
            if( !removed ) {
                mDebug() << "Could not delete" << file.fileName() <<
                         "Make sure you have sufficient permissions.";
            }
        }
    }
}

QString BookmarkSyncManager::lastSyncedKmlPath()
{
    QDir cacheDir( d->m_cachePath );
    QFileInfoList fileInfoList = cacheDir.entryInfoList(
                QStringList() << "*.kml",
                QDir::NoFilter, QDir::Name );
    if( !fileInfoList.isEmpty() ) {
        return fileInfoList.last().absoluteFilePath();
    } else {
        return QString();
    }
}

QList<DiffItem> BookmarkSyncManager::getPlacemarks( GeoDataDocument *document, GeoDataDocument *other, DiffItem::Status diffDirection )
{
    QList<DiffItem> diffItems;
    foreach ( GeoDataFolder *folder, document->folderList() ) {
        QString path = QString( "/%0" ).arg( folder->name() );
        diffItems.append( getPlacemarks( folder, path, other, diffDirection ) );
    }

    return diffItems;
}

QList<DiffItem> BookmarkSyncManager::getPlacemarks( GeoDataFolder *folder, QString &path, GeoDataDocument *other, DiffItem::Status diffDirection )
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

GeoDataPlacemark* BookmarkSyncManager::findPlacemark( GeoDataContainer* container, const GeoDataPlacemark &bookmark ) const
{
    foreach( GeoDataPlacemark* placemark, container->placemarkList() ) {
        if ( EARTH_RADIUS * distanceSphere( placemark->coordinate(), bookmark.coordinate() ) <= 1 ) {
            return placemark;
        }
    }

    foreach( GeoDataFolder* folder, container->folderList() ) {
        GeoDataPlacemark* placemark = findPlacemark( folder, bookmark );
        if ( placemark ) {
            return placemark;
        }
    }

    return 0;
}

void BookmarkSyncManager::determineDiffStatus( DiffItem &item, GeoDataDocument *document )
{
    GeoDataPlacemark *match = findPlacemark( document, item.m_placemarkA );

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

QList<DiffItem> BookmarkSyncManager::diff( QString &sourcePath, QString &destinationPath )
{
    GeoDataParser parserA( GeoData_KML );
    QFile fileA( sourcePath );
    if( !fileA.open( QFile::ReadOnly ) ) {
        mDebug() << "Could not open file " << fileA.fileName();
    }
    parserA.read( &fileA );
    GeoDataDocument *documentA = dynamic_cast<GeoDataDocument*>( parserA.releaseDocument() );

    GeoDataParser parserB( GeoData_KML );
    QFile fileB( destinationPath );
    if( !fileB.open( QFile::ReadOnly ) ) {
        mDebug() << "Could not open file " << fileB.fileName();
    }
    parserB.read( &fileB );
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

    fileA.close();
    fileB.close();

    return diffItems;
}

void BookmarkSyncManager::merge()
{
    foreach( DiffItem itemA, d->m_diffA ) {
        if( itemA.m_action == DiffItem::NoAction ) {
            bool deleted = false;
            bool changed = false;
            DiffItem other;

            foreach( DiffItem itemB, d->m_diffB ) {
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
                d->m_merged.append( other );
            } else if( !deleted ) {
                d->m_merged.append( itemA );
            }
        } else if( itemA.m_action == DiffItem::Created ) {
            d->m_merged.append( itemA );
        } else if( itemA.m_action == DiffItem::Changed || itemA.m_action == DiffItem::Deleted ) {
            bool conflict = false;
            DiffItem other;

            foreach( DiffItem itemB, d->m_diffB ) {
                if( EARTH_RADIUS * distanceSphere( itemA.m_placemarkB.coordinate(), itemB.m_placemarkB.coordinate() ) <= 1 ) {
                    if( ( itemA.m_action == DiffItem::Changed && ( itemB.m_action == DiffItem::Changed || itemB.m_action == DiffItem::Deleted ) )
                            || ( itemA.m_action == DiffItem::Deleted && itemB.m_action == DiffItem::Changed ) ) {
                        conflict = true;
                        other = itemB;
                    }
                }
            }

            if( !conflict && itemA.m_action == DiffItem::Changed ) {
                d->m_merged.append( itemA );
            } else if ( conflict ) {
                d->m_conflictItem = other;
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

                emit mergeConflict( mergeItem );
                return;
            }
        }

        if( !d->m_diffA.isEmpty() ) {
            d->m_diffA.removeFirst();
        }
    }

    foreach( DiffItem itemB, d->m_diffB ) {
        if( itemB.m_action == DiffItem::Created ) {
            d->m_merged.append( itemB );
        }
    }

    completeMerge();
}

GeoDataFolder* BookmarkSyncManager::createFolders( GeoDataContainer *container, QStringList &pathList )
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

GeoDataDocument* BookmarkSyncManager::constructDocument( const QList<DiffItem> &mergedList )
{
    GeoDataDocument *document = new GeoDataDocument();

    foreach( DiffItem item, mergedList ) {
        GeoDataPlacemark *placemark = new GeoDataPlacemark( item.m_placemarkA );
        QStringList splitted = item.m_path.split( "/", QString::SkipEmptyParts );
        GeoDataFolder *folder = createFolders( document, splitted );
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

    merge();
}

void BookmarkSyncManager::saveDownloadedToCache( const QByteArray &kml )
{
    QString localBookmarksDir = d->m_localBookmarksPath;
    QDir().mkdir( localBookmarksDir.remove( "bookmarks.kml" ) );
    QFile bookmarksFile( d->m_localBookmarksPath );
    if( !bookmarksFile.open( QFile::ReadWrite ) ) {
        mDebug() << "Failed to open file" << bookmarksFile.fileName()
                 <<  ". It is either missing or not readable.";
        return;
    }

    bookmarksFile.write( kml );
    bookmarksFile.close();
    copyLocalToCache();
}

void BookmarkSyncManager::parseTimestamp()
{
    QString response = d->m_timestampReply->readAll();
    QScriptEngine engine;
    QScriptValue parsedResponse = engine.evaluate( QString( "(%0)" ).arg( response ) );
    QString timestamp = parsedResponse.property( "data" ).toString();
    d->m_cloudTimestamp = timestamp;
    emit timestampDownloaded();
}
void BookmarkSyncManager::copyLocalToCache()
{
    QDir().mkpath( d->m_cachePath );
    clearCache();

    QFile bookmarksFile( d->m_localBookmarksPath );
    bookmarksFile.copy( QString( "%0/%1.kml" ).arg( d->m_cachePath, d->m_cloudTimestamp ) );
    emit syncComplete();
}

// Bookmark synchronization steps
void BookmarkSyncManager::continueSynchronization()
{
    bool cloudModified = cloudBookmarksModified( d->m_cloudTimestamp );
    if( !cloudModified ) {
        QString lastSyncedPath = lastSyncedKmlPath();
        if( lastSyncedPath == QString() ) {
            uploadBookmarks();
        } else {
            QList<DiffItem> diffList = diff( lastSyncedPath, d->m_localBookmarksPath );
            bool localModified = false;
            foreach( DiffItem item, diffList ) {
                if( item.m_action != DiffItem::NoAction ) {
                    localModified = true;
                }
            }

            if( localModified ) {
                uploadBookmarks();
            }
        }
    } else {
        connect( this, SIGNAL(bookmarksDownloaded()),
                 this, SLOT(completeSynchronization()) );
        downloadBookmarks();
    }
}

void BookmarkSyncManager::completeSynchronization()
{
    QString lastSyncedPath = lastSyncedKmlPath();
    QFile localBookmarksFile( d->m_localBookmarksPath );
    QByteArray result = d->m_downloadReply->readAll();

    if( lastSyncedPath == QString() ) {
        if( localBookmarksFile.exists() ) {
            // Conflict here!
        } else {
            saveDownloadedToCache( result );
        }
    } else {
        QTemporaryFile file;
        file.open();
        file.write( result );
        file.close();

        QString tempName = file.fileName();

        d->m_diffA.clear();
        d->m_diffB.clear();
        d->m_merged.clear();

        d->m_diffA = diff( lastSyncedPath, d->m_localBookmarksPath );
        d->m_diffB = diff( lastSyncedPath, tempName );
        merge();
    }
}

void BookmarkSyncManager::completeMerge()
{
    QFile localBookmarksFile( d->m_localBookmarksPath );
    GeoDataDocument *doc = constructDocument( d->m_merged );
    GeoWriter writer;
    localBookmarksFile.remove();
    localBookmarksFile.open( QFile::ReadWrite );
    writer.write( &localBookmarksFile, doc );
    localBookmarksFile.close();
    uploadBookmarks();
}

void BookmarkSyncManager::completeUpload()
{
    QString response = d->m_uploadReply->readAll();
    QScriptEngine engine;
    QScriptValue parsedResponse = engine.evaluate( QString( "(%0)" ).arg( response ) );
    QString timestamp = parsedResponse.property( "data" ).toString();
    d->m_cloudTimestamp = timestamp;
    copyLocalToCache();
}

}

#include "BookmarkSyncManager.moc"
