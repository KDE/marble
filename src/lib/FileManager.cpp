//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#include "FileManager.h"

#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtCore/QTime>

#include "FileLoader.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "GeoDataTreeModel.h"

#include "GeoDataDocument.h"
#include "GeoDataLatLonAltBox.h"


using namespace Marble;

namespace Marble
{
class FileManagerPrivate
{
public:
    FileManagerPrivate( MarbleModel* model )
        : m_model( model ),
          m_recenter( false ),
        m_t ( 0 )
    {
    };

    ~FileManagerPrivate()
    {
        foreach ( FileLoader *loader, m_loaderList ) {
            if ( loader ) {
                loader->wait();
            }
        }
    };

    MarbleModel* const m_model;
    QList<FileLoader*> m_loaderList;
    QStringList m_pathList;
    QList < GeoDataDocument* > m_fileItemList;
    bool m_recenter;
    QTime *m_t;
};
}

FileManager::FileManager( MarbleModel *model, QObject *parent )
    : QObject( parent )
    , d( new FileManagerPrivate( model ) )
{
}


FileManager::~FileManager()
{
    delete d;
}

QStringList FileManager::containers() const
{
    QStringList retList;
    for ( int line = 0; line < d->m_fileItemList.count(); ++line ) {
        retList << d->m_fileItemList.at( line )->fileName();
    }
    return retList + d->m_pathList;
}

void FileManager::addFile( const QString& filepath, DocumentRole role, bool recenter )
{
    if ( !containers().contains( filepath ) ) {
        mDebug() << "adding container:" << filepath;
        if (d->m_t == 0) {
            mDebug() << "Starting placemark loading timer";
            d->m_t = new QTime();
            d->m_t->start();
        }
        d->m_recenter = recenter;
        FileLoader* loader = new FileLoader( this, d->m_model, filepath, role );
        appendLoader( loader );
        d->m_pathList.append( filepath );
    }
}

void FileManager::addFile( const QStringList& filepaths, DocumentRole role )
{
    foreach(const QString& file, filepaths) {
        addFile( file, role );
    }
}

void FileManager::addData( const QString &name, const QString &data, DocumentRole role )
{
    FileLoader* loader = new FileLoader( this, d->m_model, data, name, role );
    appendLoader( loader );
}

void FileManager::appendLoader( FileLoader *loader )
{
    connect( loader, SIGNAL( loaderFinished( FileLoader* ) ),
             this, SLOT( cleanupLoader( FileLoader* ) ) );

    connect( loader, SIGNAL( newGeoDataDocumentAdded( GeoDataDocument* ) ),
             this, SLOT( addGeoDataDocument( GeoDataDocument* ) ) );

    d->m_loaderList.append( loader );
    loader->start();
}

void FileManager::removeFile( const QString& key )
{
    for ( int i = 0; i < d->m_fileItemList.size(); ++i ) {
        if ( key == d->m_fileItemList.at(i)->fileName() ) {
            closeFile( i );
            return;
        }
    }
    mDebug() << "could not identify " << key;
}

void FileManager::saveFile( int index )
{
    Q_UNUSED(index)
}

void FileManager::closeFile( int index )
{
    mDebug() << "FileManager::closeFile " << d->m_fileItemList.at( index )->fileName();
    if ( index < d->m_fileItemList.size() ) {
        d->m_model->treeModel()->removeDocument( d->m_fileItemList.at( index ) );
        emit fileRemoved( index );
        delete d->m_fileItemList.at( index );
        d->m_fileItemList.removeAt( index );
    }
}

int FileManager::size() const
{
    return d->m_fileItemList.size();
}

GeoDataDocument * FileManager::at( int index )
{
    if ( index < d->m_fileItemList.size() ) {
        return d->m_fileItemList.at( index );
    }
    return 0;
}

void FileManager::addGeoDataDocument( GeoDataDocument* document )
{
    if ( document->name().isEmpty() && !document->fileName().isEmpty() )
    {
        QFileInfo file( document->fileName() );
        document->setName( file.baseName() );
    }

    d->m_fileItemList.append( document );
    d->m_model->treeModel()->addDocument( document );
    emit fileAdded( d->m_fileItemList.indexOf( document ) );
}

void FileManager::cleanupLoader( FileLoader* loader )
{
    GeoDataDocument *doc = loader->document();
    d->m_loaderList.removeAll( loader );
    if ( loader->isFinished() ) {
        if ( doc && d->m_recenter ) {
            emit centeredDocument( doc->latLonAltBox() );
            d->m_recenter = false;
        }
        d->m_pathList.removeAll( loader->path() );
        delete loader;
    }
    if ( d->m_loaderList.isEmpty()  )
    {
        qDebug() << "Finished loading all placemarks " << d->m_t->elapsed();
        delete d->m_t;
        d->m_t = 0;
    }
}

#include "FileManager.moc"
