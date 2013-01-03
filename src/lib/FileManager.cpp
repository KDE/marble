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
#include "GeoDataStyle.h"


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
    void closeFile( const QString &key );
    void cleanupLoader( FileLoader *loader );

    MarbleModel* const m_model;

    FileManager * const q;
    QList<FileLoader*> m_loaderList;
    QHash < QString, GeoDataDocument* > m_fileItemHash;
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

void FileManager::addFile( const QString& filepath, const QString& property, GeoDataStyle* style, DocumentRole role, bool recenter )
{
    if( d->m_fileItemHash.contains( filepath ) ) {
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
    FileLoader* loader = new FileLoader( this, d->m_model, filepath, property, style, role );
    d->appendLoader( loader );
}

void FileManager::addFile( const QStringList& filepaths, const QStringList& propertyList, const QList<GeoDataStyle*>& styles, DocumentRole role )
{
    for (int i = 0 ; i < filepaths.size(); ++i ) {
        addFile( filepaths.at(i), propertyList.at(i), styles.at(i), role );
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

    if( d->m_fileItemHash.contains( key ) ) {
        d->closeFile( key );
    }

    mDebug() << "could not identify " << key;
}

void FileManagerPrivate::closeFile( const QString& key )
{
    mDebug() << "FileManager::closeFile " << key;
    if( m_fileItemHash.contains( key ) ) {
        GeoDataDocument *doc = m_fileItemHash.value( key );
        m_model->treeModel()->removeDocument( doc );
        emit q->fileRemoved( key );
        delete doc;
        m_fileItemHash.remove( key );
    }
}

void FileManager::saveFile( GeoDataDocument *document )
{
    Q_UNUSED(document)
}

void FileManager::closeFile( GeoDataDocument *document )
{
    foreach( QString key, d->m_fileItemHash.keys() ) {
        if( d->m_fileItemHash.value( key ) == document ) {
            d->closeFile( key );
            return;
        }
    }
}

int FileManager::size() const
{
    return d->m_fileItemHash.size();
}

GeoDataDocument * FileManager::at( const QString &key )
{
    if ( d->m_fileItemHash.contains( key ) ) {
        return d->m_fileItemHash.value( key );
    }
    return 0;
}

void FileManagerPrivate::cleanupLoader( FileLoader* loader )
{
    GeoDataDocument *doc = loader->document();
    m_loaderList.removeAll( loader );
    if ( loader->isFinished() ) {
        if ( doc ) {
            if ( doc->name().isEmpty() && !doc->fileName().isEmpty() )
            {
                QFileInfo file( doc->fileName() );
                doc->setName( file.baseName() );
            }
            m_model->treeModel()->addDocument( doc );
            m_fileItemHash.insert( loader->path(), doc );
            emit q->fileAdded( loader->path() );
            if( m_recenter ) {
                emit q->centeredDocument( doc->latLonAltBox() );
                m_recenter = false;
            }
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
