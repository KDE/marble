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
#include "GeoDataTreeModel.h"
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
    m_treeModel->removeDocument( m_bookmarkDocument );
    delete m_bookmarkDocument;
}

void BookmarkManagerPrivate::resetBookmarkDocument()
{
    if ( m_bookmarkDocument ) {
        m_treeModel->removeDocument( m_bookmarkDocument );
        delete m_bookmarkDocument;
    }

    GeoDataFolder* folder = new GeoDataFolder;
    folder->setName( QObject::tr( "Default" ) );

    m_bookmarkDocument = new GeoDataDocument;
    m_bookmarkDocument->setDocumentRole( BookmarkDocument );
    m_bookmarkDocument->setName( QObject::tr("Bookmarks") );
    m_bookmarkDocument->append( folder );
    m_treeModel->addDocument( m_bookmarkDocument );
}

GeoDataDocument* BookmarkManagerPrivate::bookmarkDocument()
{
    if ( !m_bookmarkDocument ) {
        resetBookmarkDocument();
    }
    return m_bookmarkDocument;
}


BookmarkManager::BookmarkManager( GeoDataTreeModel *treeModel, QObject *parent )
        : QObject( parent ), d( new BookmarkManagerPrivate() )
{
    d->m_treeModel = treeModel;
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
        GeoDataDocument *document = openFile( absoluteFilePath );
        bool recover = false;
        if ( !document ) {
            mDebug() << "Could not parse file" << absoluteFilePath;
            mDebug() << "This could be caused by a previous broken bookmark file. Trying to recover.";
            /** @todo: Remove this workaround and return false around Marble 1.4 */
            recover = true;
            // return false;
        }

        d->m_treeModel->removeDocument( d->m_bookmarkDocument );
        delete d->m_bookmarkDocument;
        d->m_bookmarkDocument = document;
        Q_ASSERT( d->m_bookmarkDocument );

        d->m_treeModel->addDocument( d->m_bookmarkDocument );

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

    GeoDataFolder* folder = 0;
    for ( ; i != end; ++i ) {
        //Folder found where bookmark should be inserted
        if ( folderName == ( *i )->name() ) {
            folder = *i;
            break;
        }
    }

    if ( !folder ) {
        mDebug() << "Creating new folder " << folderName << " to host " << placemark.name();
        GeoDataFolder* folder = new GeoDataFolder;
        folder->setName( folderName );
        d->m_treeModel->addFeature( d->m_bookmarkDocument, folder );
    }

    GeoDataPlacemark *bookmark = new GeoDataPlacemark( placemark );
    bookmark->setVisualCategory( GeoDataDocument::Bookmark );
    d->m_treeModel->addFeature( folder, bookmark );

    updateBookmarkFile();
}

void BookmarkManager::removeBookmark( GeoDataPlacemark *bookmark )
{
    d->m_treeModel->removeFeature( bookmark );
    delete bookmark;
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

    d->m_treeModel->addFeature( d->m_bookmarkDocument, bookmarkFolder );
    updateBookmarkFile();
}

void BookmarkManager::renameBookmarkFolder( GeoDataFolder *folder, const QString &name )
{
    GeoDataContainer *parent = static_cast<GeoDataContainer*>( folder->parent() );
    d->m_treeModel->removeFeature( folder );
    folder->setName( name );
    d->m_treeModel->addFeature( parent, folder );
}

void BookmarkManager::removeBookmarkFolder( GeoDataFolder *folder )
{
    d->m_treeModel->removeFeature( folder );
    delete folder;
}

void BookmarkManager::removeAllBookmarks()
{
    d->resetBookmarkDocument();
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

GeoDataDocument* BookmarkManager::openFile( const QString &fileName ) const
{
    GeoDataParser parser( GeoData_KML );
    QFile file( fileName );

    if ( !file.exists() ) {
        return 0;
    }

    if ( !file.open( QIODevice::ReadOnly ) || !parser.read( &file ) ) {
        mDebug() << "Could not open/parse file" << fileName;
        return 0;
    }

    GeoDataDocument *result = dynamic_cast<GeoDataDocument*>( parser.releaseDocument() );
    if ( !result ) {
        return 0;
    }

    result->setDocumentRole( BookmarkDocument );
    foreach( GeoDataFolder* folder, result->folderList() ) {
        foreach( GeoDataPlacemark* placemark, folder->placemarkList() ) {
            placemark->setVisualCategory( GeoDataFeature::Bookmark );
        }
    }

    return result;
}

}

#include "BookmarkManager.moc"
