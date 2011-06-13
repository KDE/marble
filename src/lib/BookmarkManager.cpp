//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Gaurav Gupta <1989.gaurav@googlemail.com>
//

#include "BookmarkManager.h"
#include "BookmarkManager_p.h"
#include "GeoDataParser.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include <QtCore/QFile>

namespace Marble
{

BookmarkManagerPrivate::BookmarkManagerPrivate()
        : m_bookmarkDocument( 0 ), m_bookmarkFileRelativePath( "bookmarks/bookmarks.kml" )
{
    // nothing to do
}

BookmarkManagerPrivate::~BookmarkManagerPrivate()
{
    delete m_bookmarkDocument;
}

GeoDataDocument* BookmarkManagerPrivate::createEmptyBookmarkDocument()
{
    GeoDataFolder* folder = new GeoDataFolder;
    folder->setName( QObject::tr( "Default" ) );
    GeoDataDocument* result = new GeoDataDocument;
    result->setDocumentRole( BookmarkDocument );
    result->append( folder );
    return result;
}

GeoDataDocument* BookmarkManagerPrivate::bookmarkDocument()
{
    if ( !m_bookmarkDocument ) {
        m_bookmarkDocument = createEmptyBookmarkDocument();
    }

    Q_ASSERT( m_bookmarkDocument );
    return m_bookmarkDocument;
}

void BookmarkManagerPrivate::resetBookmarks()
{
    delete m_bookmarkDocument;
    m_bookmarkDocument = 0;
    bookmarkDocument();
}

BookmarkManager::BookmarkManager( QObject *parent )
        : QObject( parent ), d( new BookmarkManagerPrivate() )
{
}

BookmarkManager::~BookmarkManager()
{
    delete d;
}

QString BookmarkManager::bookmarkFile() const
{
    return MarbleDirs::path( d->m_bookmarkFileRelativePath );
}

bool BookmarkManager::loadFile( const QString &relativeFilePath )
{
    d->m_bookmarkFileRelativePath = relativeFilePath;
    QString absoluteFilePath = MarbleDirs::path( d->m_bookmarkFileRelativePath );

    mDebug() << "Loading Bookmark File : " << absoluteFilePath;
    if ( ! relativeFilePath.isNull() ) {
        GeoDataParser parser( GeoData_KML );

        QFile file( absoluteFilePath );

        if ( !file.exists() ) {
            mDebug() <<  "Bookmark file" << relativeFilePath << "does not exist in local and system data directory.";
            return false;
        }

        // Open file in read mode
        file.open( QIODevice::ReadOnly );

        bool recover = false;
        if ( !parser.read( &file ) ) {
            mDebug() << "Could not parse file" << absoluteFilePath;
            mDebug() << "This could be caused by a previous broken bookmark file. Trying to recover.";
            /** @todo: Remove this workaround and return false around Marble 1.4 */
            recover = true;
            // return false;
        }

        delete d->m_bookmarkDocument;
        d->m_bookmarkDocument = dynamic_cast<GeoDataDocument*>( parser.releaseDocument() );
        Q_ASSERT( d->m_bookmarkDocument );
        d->m_bookmarkDocument->setDocumentRole( BookmarkDocument );

        foreach( GeoDataFolder* folder, d->m_bookmarkDocument->folderList() )
        {
            foreach( GeoDataPlacemark* placemark, folder->placemarkList() ) {
                placemark->setVisualCategory( GeoDataFeature::Bookmark );
            }
        }

        file.close();

        if ( recover ) {
            updateBookmarkFile();
        }

        emit bookmarksChanged();
        return true;
    }

    return false;
}

void BookmarkManager::addBookmark( const GeoDataPlacemark &placemark, const QString &folderName )
{
    QVector<GeoDataFolder*> bookmarkFolders = folders();
    QVector<GeoDataFolder*>::const_iterator i = bookmarkFolders.constBegin();
    QVector<GeoDataFolder*>::const_iterator end = bookmarkFolders.constEnd();

    for ( ; i != end; ++i ) {
        //Folder found where bookmark should be inserted
        if ( folderName == ( *i )->name() ) {
            GeoDataPlacemark *bookmark = new GeoDataPlacemark( placemark );
            bookmark->setVisualCategory( GeoDataDocument::Bookmark );
            ( *i )->append( bookmark );
            break;
        }
    }

    updateBookmarkFile();
}

QVector<GeoDataFolder*> BookmarkManager::folders() const
{
    return d->bookmarkDocument()->folderList();
}

void BookmarkManager::addNewBookmarkFolder( const QString &folder )
{
    //If name is empty string
    if ( folder.isEmpty() ) {
        mDebug() << "Folder with empty name is not acceptable, please give it another name" ;
        return;
    }

    //If folder with same name already exist
    QVector<GeoDataFolder*> folderList = folders();

    QVector<GeoDataFolder*>::const_iterator i = folderList.constBegin();
    QVector<GeoDataFolder*>::const_iterator end = folderList.constEnd();
    for ( ; i != end; ++i ) {
        if ( folder == ( *i )->name() ) {
            mDebug() << "Folder with same name already exist, please give it another name";
            return;
        }
    }

    GeoDataFolder *bookmarkFolder = new GeoDataFolder();
    bookmarkFolder->setName( folder );

    d->bookmarkDocument()->append( bookmarkFolder );
    updateBookmarkFile();
}

void BookmarkManager::removeAllBookmarks()
{
    d->resetBookmarks();
    updateBookmarkFile();
}

bool BookmarkManager::updateBookmarkFile()
{
    QString absoluteLocalFilePath = MarbleDirs::localPath() + '/' + d->m_bookmarkFileRelativePath ;

    if ( ! d->m_bookmarkFileRelativePath.isNull() ) {
        GeoWriter writer;
        writer.setDocumentType( "http://earth.google.com/kml/2.2" );

        QFile file( absoluteLocalFilePath );

        if ( !file.exists() ) {
            // Extracting directory of file : for bookmarks it will be MarbleDirs::localPath()+/bookmarks/
            QFileInfo fileInfo( absoluteLocalFilePath );
            QString directoryPath = fileInfo.path();

            //Creating all directories, which doesn't exist
            QDir directory(  MarbleDirs::localPath() );
            directory.mkpath( directoryPath );
        }

        file.open( QIODevice::WriteOnly );

        if ( !writer.write( &file,  d->bookmarkDocument() ) ) {
            mDebug() << "Could not write the bookmarks file" << absoluteLocalFilePath;
            file.close();
            return false;
        }
        emit bookmarksChanged();
        file.close();
        return true;
    }
    return false;
}

}

#include "BookmarkManager.moc"
