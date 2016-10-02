//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Gaurav Gupta <1989.gaurav@googlemail.com>
// Copyright 2012      Thibaut Gridel <tgridel@free.fr>
//

#include "BookmarkManager.h"
#include "BookmarkManager_p.h"
#include "GeoDataParser.h"
#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTreeModel.h"
#include "GeoDataDocumentWriter.h"
#include "GeoDataIconStyle.h"
#include "KmlElementDictionary.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include "StyleBuilder.h"
#include <QFile>

namespace Marble
{

BookmarkManagerPrivate::BookmarkManagerPrivate( GeoDataTreeModel *treeModel ) :
    m_treeModel( treeModel ),
    m_bookmarkDocument( 0 ),
    m_bookmarkFileRelativePath( "bookmarks/bookmarks.kml" ),
    m_styleBuilder(nullptr)
{
    resetBookmarkDocument();
}

BookmarkManagerPrivate::~BookmarkManagerPrivate()
{
    Q_ASSERT( m_bookmarkDocument && "BookmarkManagerPrivate::m_bookmarkDocument is 0. Please report a Marble bug at http://bugs.kde.org" );
    if ( m_bookmarkDocument )
    {
        m_treeModel->removeDocument( m_bookmarkDocument );
    }
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

void BookmarkManagerPrivate::setVisualCategory( GeoDataContainer *container ) {
    foreach( GeoDataFolder* folder, container->folderList() ) {
        setVisualCategory( folder );
    }
    foreach( GeoDataPlacemark* placemark, container->placemarkList() ) {
        placemark->setVisualCategory(GeoDataPlacemark::Bookmark);
        placemark->setZoomLevel( 1 );
    }

}

BookmarkManager::BookmarkManager( GeoDataTreeModel *treeModel, QObject *parent ) :
    QObject( parent ),
    d( new BookmarkManagerPrivate( treeModel ) )
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
    QString absoluteFilePath = bookmarkFile();

    mDebug() << Q_FUNC_INFO << "Loading Bookmark File:" << absoluteFilePath;

    if (absoluteFilePath.isEmpty())
        return false;

    if ( relativeFilePath.isNull() )
        return false;

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

    if ( recover ) {
        d->resetBookmarkDocument();
        updateBookmarkFile();
    } else {
        Q_ASSERT( d->m_bookmarkDocument && "d->m_bookmarkDocument is 0 but must not be. Please report a bug at http://bugs.kde.org" );
        d->m_treeModel->addDocument( d->m_bookmarkDocument );
    }
    ensureDefaultFolder();

    emit bookmarksChanged();
    return true;
}


void BookmarkManager::addBookmark( GeoDataContainer *container, const GeoDataPlacemark &placemark )
{
    GeoDataPlacemark *bookmark = new GeoDataPlacemark( placemark );
    bookmark->setVisualCategory(GeoDataPlacemark::Bookmark);
    bookmark->setZoomLevel( 1 );
    if (bookmark->name().isEmpty()) {
        bookmark->setName(bookmark->coordinate().toString(GeoDataCoordinates::Decimal).trimmed());
    }
    if (d->m_styleBuilder && bookmark->style()->iconStyle().iconPath().isEmpty()) {
        StyleParameters style;
        style.placemark = bookmark;
        bookmark->setStyle(GeoDataStyle::Ptr(new GeoDataStyle(*d->m_styleBuilder->createStyle(style))));
    }
    d->m_treeModel->addFeature( container, bookmark );

    updateBookmarkFile();
}

void BookmarkManager::updateBookmark( GeoDataPlacemark *bookmark )
{
    d->m_treeModel->updateFeature( bookmark );
}

void BookmarkManager::removeBookmark( GeoDataPlacemark *bookmark )
{
    d->m_treeModel->removeFeature( bookmark );
    delete bookmark;
    updateBookmarkFile();
}

GeoDataDocument * BookmarkManager::document()
{
    return d->m_bookmarkDocument;
}

const GeoDataDocument * BookmarkManager::document() const
{
    return d->m_bookmarkDocument;
}

bool BookmarkManager::showBookmarks() const
{
    return d->m_bookmarkDocument->isVisible();
}

void BookmarkManager::setShowBookmarks( bool visible )
{
    d->m_bookmarkDocument->setVisible( visible );
    d->m_treeModel->updateFeature( d->m_bookmarkDocument );
}

QVector<GeoDataFolder*> BookmarkManager::folders() const
{
    return d->m_bookmarkDocument->folderList();
}

void BookmarkManager::addNewBookmarkFolder( GeoDataContainer *container, const QString &name )
{
    //If name is empty string
    if ( name.isEmpty() ) {
        mDebug() << "Folder with empty name is not acceptable, please give it another name" ;
        return;
    }

    //If folder with same name already exist
    QVector<GeoDataFolder*> folderList = container->folderList();

    QVector<GeoDataFolder*>::const_iterator i = folderList.constBegin();
    QVector<GeoDataFolder*>::const_iterator end = folderList.constEnd();
    for ( ; i != end; ++i ) {
        if ( name == ( *i )->name() ) {
            mDebug() << "Folder with same name already exist, please give it another name";
            return;
        }
    }

    GeoDataFolder *bookmarkFolder = new GeoDataFolder();
    bookmarkFolder->setName( name );

    d->m_treeModel->addFeature( container, bookmarkFolder );
    updateBookmarkFile();
}

void BookmarkManager::renameBookmarkFolder( GeoDataFolder *folder, const QString &name )
{
    folder->setName( name );
    d->m_treeModel->updateFeature( folder );
}

void BookmarkManager::removeBookmarkFolder( GeoDataFolder *folder )
{
    d->m_treeModel->removeFeature( folder );
    delete folder;
}

void BookmarkManager::ensureDefaultFolder()
{
    if ( d->m_bookmarkDocument->size() == 0 ) {
        addNewBookmarkFolder( d->m_bookmarkDocument, "Default" );
    }
}

void BookmarkManager::removeAllBookmarks()
{
    d->resetBookmarkDocument();
    updateBookmarkFile();
}

void BookmarkManager::setStyleBuilder(const StyleBuilder *styleBuilder)
{
    d->m_styleBuilder = styleBuilder;
}

bool BookmarkManager::updateBookmarkFile()
{
    const QString absoluteLocalFilePath = MarbleDirs::localPath() + QLatin1Char('/') + d->m_bookmarkFileRelativePath;

    if ( ! d->m_bookmarkFileRelativePath.isNull() ) {
        QFile file( absoluteLocalFilePath );
        if ( !file.exists() ) {
            // Extracting directory of file : for bookmarks it will be MarbleDirs::localPath()+/bookmarks/
            QFileInfo fileInfo( absoluteLocalFilePath );
            QString directoryPath = fileInfo.path();

            //Creating all directories, which doesn't exist
            QDir directory(  MarbleDirs::localPath() );
            directory.mkpath( directoryPath );
        }

        if (!GeoDataDocumentWriter::write(absoluteLocalFilePath, *d->m_bookmarkDocument)) {
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

GeoDataDocument* BookmarkManager::openFile( const QString &fileName )
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
        BookmarkManagerPrivate::setVisualCategory( folder );
    }

    return result;
}

}

#include "moc_BookmarkManager.cpp"
