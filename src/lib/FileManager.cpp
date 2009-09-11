//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>"
// Copyright 2007      Inge Wallin  <ingwa@kde.org>"
//

#include "FileManager.h"

#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include "FileLoader.h"
#include "FileViewModel.h"
#include "KmlFileViewItem.h"
#include "MarbleDataFacade.h"
#include "MarbleGeometryModel.h"

#include "GeoDataDocument.h"


using namespace Marble;

namespace Marble
{
class FileManagerPrivate
{
    public:
        FileManagerPrivate( )
        : m_datafacade( 0 )
        {
        }

        MarbleDataFacade* m_datafacade;
        QList<FileLoader*> m_loaderList;
        QStringList m_pathList;
        QList < AbstractFileViewItem* > m_fileItemList;
};
}

FileManager::FileManager( QObject *parent )
    : QObject( parent )
    , d( new FileManagerPrivate() )
{
}


FileManager::~FileManager()
{
    foreach( FileLoader *loader, d->m_loaderList ) {
        if ( loader ) {
            loader->wait();
        }
    }

    foreach( AbstractFileViewItem *file, d->m_fileItemList)
    {
        delete file;
    }

    delete d;
}

void FileManager::setDataFacade( MarbleDataFacade *facade )
{
    d->m_datafacade = facade;
    d->m_datafacade->fileViewModel()->setFileManager(this);
}

MarbleDataFacade *FileManager::dataFacade()
{
    return d->m_datafacade;
}

QStringList FileManager::containers() const
{
    QStringList retList;
    for( int line = 0; line < d->m_fileItemList.count(); ++line ) {
        retList << d->m_fileItemList.at( line )->name();
    }
    return retList + d->m_pathList;
}

QString FileManager::toRegularName( QString name )
{
    return name.remove(".kml").remove(".cache");
}

void FileManager::addFile( const QString& filepath )
{
    if( ! containers().contains( toRegularName( filepath ) ) ) {
        qDebug() << "adding container:" << toRegularName( filepath );
        FileLoader* loader = new FileLoader( this, filepath );
        appendLoader( loader );
        d->m_pathList.append( toRegularName( filepath ) );
    }
}

void FileManager::addData( const QString &name, const QString &data )
{
    FileLoader* loader = new FileLoader( this, data, name );
    appendLoader( loader );
}

void FileManager::appendLoader( FileLoader *loader )
{
    connect (   loader, SIGNAL( fileLoaderFailed( FileLoader* ) ),
                this, SLOT( cleanupLoader( FileLoader* ) ) );

    connect (   loader, SIGNAL( newGeoDataDocumentAdded( GeoDataDocument* ) ),
                this, SLOT( addGeoDataDocument( GeoDataDocument* ) ) );

    d->m_loaderList.append( loader );
    loader->start();
}

void FileManager::removeFile( const QString& key )
{
    for( int i = 0; i < d->m_fileItemList.size(); ++i )
    {
        if( toRegularName( key ) == toRegularName( d->m_fileItemList.at(i)->name() ) ) {
            closeFile(i);
            break;
        }
    }
}

void FileManager::addFile ( AbstractFileViewItem * item )
{
    qDebug() << "FileManager::addFile";
    d->m_fileItemList.append( item );
    emit fileAdded(d->m_fileItemList.indexOf( item ) );
}

void FileManager::saveFile( int index )
{
    if (index < d->m_fileItemList.size() )
    {
        d->m_fileItemList.at( index )->saveFile();
    }
}

void FileManager::closeFile( int index )
{
    qDebug() << "FileManager::closeFile";
    if (index < d->m_fileItemList.size() )
    {
        d->m_fileItemList.at( index )->closeFile( indexStart( index ));
        delete d->m_fileItemList.at( index );
        d->m_fileItemList.removeAt( index );
        emit fileRemoved( index );
    }
}

int FileManager::size() const
{
    return d->m_fileItemList.size();
}

AbstractFileViewItem * FileManager::at( int index )
{
    if (index < d->m_fileItemList.size() )
    {
        return d->m_fileItemList.at( index );
    }
    return 0;
}

void FileManager::addGeoDataDocument( GeoDataDocument* document )
{
    KmlFileViewItem* item = new KmlFileViewItem( *this, *document );
    addFile( item );

    // now get the document that will be preserved throughout the life time
    GeoDataDocument* doc = item->document();
    // remove the hashes in front of the styles.
    QVector<GeoDataFeature>::Iterator end = doc->end();
    QVector<GeoDataFeature>::Iterator itr = doc->begin();
    for ( ; itr != end; ++itr ) {
        // use *itr (or itr.value()) here
        QString styleUrl = itr->styleUrl().remove('#');
        itr->setStyle( &doc->style( styleUrl ) );
    }

    // do not set this file if it only contains points
    if( doc->isVisible() && d->m_datafacade->geometryModel() )
        d->m_datafacade->geometryModel()->setGeoDataRoot( doc );
    emit geoDataDocumentAdded( *doc );

    if( d->m_loaderList.isEmpty() ) {
        emit finalize();
    }
}

void FileManager::cleanupLoader( FileLoader* loader )
{
    d->m_loaderList.removeAll( loader );
    if ( loader->isFinished() ) {
         d->m_pathList.removeAll( loader->path() );
         delete loader;
    }
}

int FileManager::indexStart( int index )
{
    int start = 0;
    for( int i = 0; i < index; i++ ) {
        start += d->m_fileItemList.at( i )->size();
    }
    return start;
}

#include "FileManager.moc"
