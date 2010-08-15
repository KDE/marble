//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2010      Gaurav Gupta <1989.gaurav@googlemail.com>
//

#include "BookmarkManager.h"
#include "GeoDataParser.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoWriter.h"
#include "KmlElementDictionary.h"
#include "MarbleDebug.h"
#include "MarbleDirs.h"
#include <QtCore/QFile>
using namespace Marble;

BookmarkManager::BookmarkManager()
     : d( new BookmarkManagerPrivate() )
{
}

BookmarkManager::~BookmarkManager()
{
    
    delete d->m_bookmarkDocument;   
    delete d;
}

QString BookmarkManager::bookmarkFile() const
{
    return MarbleDirs::path( d->m_bookmarkFileRelativePath );
}

bool BookmarkManager::loadFile( const QString &relativeFilePath)
{

    d->m_bookmarkFileRelativePath = relativeFilePath;
    QString absoluteFilePath = MarbleDirs::path( d->m_bookmarkFileRelativePath ); 

    mDebug() << "Loading Bookmark File : " << absoluteFilePath;
    if ( ! relativeFilePath.isNull() ) {
        GeoDataParser parser( GeoData_KML );

        QFile file( absoluteFilePath );
        
        if ( !file.exists() ) {
            mDebug() <<  " File does not exist in local and system path! ";
            return false;
        }

        // Open file in read mode
        file.open( QIODevice::ReadOnly );

        if ( !parser.read( &file ) ) {
            qWarning( "Could not parse file!" );
            
        }
        
        d->m_bookmarkDocument = dynamic_cast<GeoDataDocument*>(parser.releaseDocument() );
        Q_ASSERT( d->m_bookmarkDocument );

        file.close();
        return true;
    }

    return false;
}

void BookmarkManager::addBookmark( const GeoDataPlacemark &bookmark, const QString &folderName ) 
{
    QVector<GeoDataFolder*> bookmarkFolders = folders();
    QVector<GeoDataFolder*>::const_iterator i = bookmarkFolders.constBegin();
    QVector<GeoDataFolder*>::const_iterator end = bookmarkFolders.constEnd();

    for (; i != end; ++i ) {
        //Folder found where bookmark should be inserted
        if( !folderName.compare( (*i)->name() ) ){
            (*i)->append( new GeoDataPlacemark( bookmark ) );
            break;            
        }
    }

    updateBookmarkFile();
}

QVector<GeoDataFolder*> BookmarkManager::folders() const
{
        return d->m_bookmarkDocument->folderList();
}

void BookmarkManager::addNewBookmarkFolder( const QString &folder)
{
    //If name is empty string
    if( folder == "" )
    {
        mDebug() << "Folder with empty name is not acceptable, please give an another name" ;
        return;
    }
    
    //If folder with same name already exist
    QVector<GeoDataFolder*> folderList = folders();

    QVector<GeoDataFolder*>::const_iterator i = folderList.constBegin();
    QVector<GeoDataFolder*>::const_iterator end = folderList.constEnd();
    for (; i != end; ++i ) {
        if( folder == (*i)->name() )
        {
            mDebug() << "Folder with same name already exist, please give an another name";
            return;
        }
    }


    GeoDataFolder *bookmarkFolder = new GeoDataFolder();
    bookmarkFolder->setName( folder );

    d->m_bookmarkDocument->append( bookmarkFolder );
    updateBookmarkFile();
}

void BookmarkManager::removeAllBookmarks()
{
    d->m_bookmarkDocument->clear();
    
    GeoDataFolder *defaultFolder = new GeoDataFolder();
    defaultFolder->setName( "Default" );
    d->m_bookmarkDocument->append( defaultFolder );
   
     updateBookmarkFile();
}

bool BookmarkManager::updateBookmarkFile() const
{
    QString absoluteLocalFilePath = MarbleDirs::localPath()+ '/'+d->m_bookmarkFileRelativePath ;


    if ( ! d->m_bookmarkFileRelativePath.isNull() ) {
        GeoWriter writer;
        writer.setDocumentType( "http://earth.google.com/kml/2.2" );

        QFile file( absoluteLocalFilePath );
    
        if( !file.exists() )
        {
            mDebug() << " Bookmarks file doesn't exist, creating Bookmarks.kml in MarbleDirs::localPath()/relativeFilePath ";
            
            // Extracting directory of file : for bookmarks it will be MarbleDirs::localPath()+/bookmarks/
            QFileInfo *fileInfo = new QFileInfo( absoluteLocalFilePath );
            QString directoryPath = fileInfo->path();
            delete fileInfo;

            //Creating all directories, which doesn't exist
            QDir *directory = new QDir(  MarbleDirs::localPath() );
            directory->mkpath( directoryPath );
            delete directory;
        }

        file.open( QIODevice::ReadWrite );

        if ( !writer.write( &file, *( d->m_bookmarkDocument )  ) ) {
            qWarning( "Could not write the file." );
            return false;
        }
        return true;
    }
    return false;
    
}

