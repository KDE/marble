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
#include <QtGui/QMessageBox>

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
    FileManagerPrivate( MarbleModel* model, FileManager* parent )
        : m_model( model ),
          q( parent ),
          m_recenter( false )
    {
    }

    ~FileManagerPrivate()
    {
        foreach ( FileLoader *loader, m_loaderList ) {
            if ( loader ) {
                loader->wait();
            }
        }
    }

    void appendLoader( FileLoader *loader );
    void saveFile( int index );
    void closeFile( int index );
    void addGeoDataDocument( GeoDataDocument *document );
    void cleanupLoader( FileLoader *loader );

    MarbleModel* const m_model;

    FileManager * const q;
    QList<FileLoader*> m_loaderList;
    QList < GeoDataDocument* > m_fileItemList;
    bool m_recenter;
    QTime m_timer;
};
}

FileManager::FileManager( MarbleModel *model, QObject *parent )
    : QObject( parent )
    , d( new FileManagerPrivate( model, this ) )
{
}


FileManager::~FileManager()
{
    delete d;
}

void FileManager::addFile( const QString& filepath, DocumentRole role, bool recenter )
{
    foreach ( const GeoDataDocument *document, d->m_fileItemList ) {
        if ( document->fileName() == filepath )
            return;  // already loaded
    }

    foreach ( const FileLoader *loader, d->m_loaderList ) {
        if ( loader->path() == filepath )
            return;  // currently loading
    }

    mDebug() << "adding container:" << filepath;
    mDebug() << "Starting placemark loading timer";
    d->m_timer.start();
    d->m_recenter = recenter;
    FileLoader* loader = new FileLoader( this, d->m_model, filepath, role );
    d->appendLoader( loader );
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
    d->appendLoader( loader );
}

void FileManagerPrivate::appendLoader( FileLoader *loader )
{
    QObject::connect( loader, SIGNAL( loaderFinished( FileLoader* ) ),
             q, SLOT( cleanupLoader( FileLoader* ) ) );

    QObject::connect( loader, SIGNAL( newGeoDataDocumentAdded( GeoDataDocument* ) ),
             q, SLOT( addGeoDataDocument( GeoDataDocument* ) ) );

    m_loaderList.append( loader );
    loader->start();
}

void FileManager::removeFile( const QString& key )
{
    foreach ( FileLoader *loader, d->m_loaderList ) {
        if ( loader->path() == key ) {
            disconnect( loader, 0, this, 0 );
            loader->wait();
            d->m_loaderList.removeAll( loader );
            delete loader->document();
            return;
        }
    }

    for ( int i = 0; i < d->m_fileItemList.size(); ++i ) {
        if ( key == d->m_fileItemList.at(i)->fileName() ) {
            d->closeFile( i );
            return;
        }
    }

    mDebug() << "could not identify " << key;
}

void FileManagerPrivate::saveFile( int index )
{
    Q_UNUSED(index)
}

void FileManagerPrivate::closeFile( int index )
{
    mDebug() << "FileManager::closeFile " << m_fileItemList.at( index )->fileName();
    if ( index < m_fileItemList.size() ) {
        m_model->treeModel()->removeDocument( m_fileItemList.at( index ) );
        emit q->fileRemoved( index );
        delete m_fileItemList.at( index );
        m_fileItemList.removeAt( index );
    }
}

void FileManager::saveFile( GeoDataDocument *document )
{
    Q_UNUSED(document)
}

void FileManager::closeFile( GeoDataDocument *document )
{
    for ( int i = 0; i < d->m_fileItemList.size(); ++i ) {
        if ( document == d->m_fileItemList.at(i) ) {
            d->closeFile( i );
            return;
        }
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

void FileManagerPrivate::addGeoDataDocument( GeoDataDocument* document )
{
    if ( document->name().isEmpty() && !document->fileName().isEmpty() )
    {
        QFileInfo file( document->fileName() );
        document->setName( file.baseName() );
    }

    m_fileItemList.append( document );
    m_model->treeModel()->addDocument( document );
    emit q->fileAdded( m_fileItemList.indexOf( document ) );
}

void FileManagerPrivate::cleanupLoader( FileLoader* loader )
{
    GeoDataDocument *doc = loader->document();
    m_loaderList.removeAll( loader );
    if ( loader->isFinished() ) {
        if ( doc && m_recenter ) {
            emit q->centeredDocument( doc->latLonAltBox() );
            m_recenter = false;
        }
        if ( !loader->error().isEmpty() ) {
            QMessageBox errorBox;
            errorBox.setWindowTitle( QObject::tr("File Parsing Error"));
            errorBox.setText( loader->error() );
            errorBox.setIcon( QMessageBox::Warning );
            errorBox.exec();
            qWarning() << "File Parsing error " << loader->error();
        }
        delete loader;
    }
    if ( m_loaderList.isEmpty()  )
    {
        mDebug() << "Finished loading all placemarks " << m_timer.elapsed();
    }
}

#include "FileManager.moc"
