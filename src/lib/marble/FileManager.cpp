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

#include <QFileInfo>
#include <QTime>
#include <QMessageBox>

#include "FileLoader.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "GeoDataTreeModel.h"

#include "GeoDataDocument.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataStyle.h"
#include "GeoDataDocumentWriter.h"
#include <KmlElementDictionary.h>


using namespace Marble;

namespace Marble
{
class FileManagerPrivate
{
public:
    FileManagerPrivate( GeoDataTreeModel *treeModel, const PluginManager *pluginManager, FileManager* parent ) :
        q( parent ),
        m_treeModel( treeModel ),
        m_pluginManager( pluginManager )
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

    FileManager *const q;
    GeoDataTreeModel *const m_treeModel;
    const PluginManager *const m_pluginManager;

    QList<FileLoader*> m_loaderList;
    QHash < QString, GeoDataDocument* > m_fileItemHash;
    GeoDataLatLonBox m_latLonBox;
    QTime m_timer;
};
}

FileManager::FileManager( GeoDataTreeModel *treeModel, const PluginManager *pluginManager, QObject *parent )
    : QObject( parent )
    , d( new FileManagerPrivate( treeModel, pluginManager, this ) )
{
}


FileManager::~FileManager()
{
    delete d;
}

void FileManager::addFile( const QString& filepath, const QString& property, const GeoDataStyle::Ptr &style, DocumentRole role, int renderOrder, bool recenter )
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
    FileLoader* loader = new FileLoader( this, d->m_pluginManager, recenter, filepath, property, style, role, renderOrder );
    d->appendLoader( loader );
}

void FileManager::addData( const QString &name, const QString &data, DocumentRole role )
{
    FileLoader* loader = new FileLoader( this, d->m_pluginManager, data, name, role );
    d->appendLoader( loader );
}

void FileManagerPrivate::appendLoader( FileLoader *loader )
{
    QObject::connect( loader, SIGNAL(loaderFinished(FileLoader*)),
             q, SLOT(cleanupLoader(FileLoader*)) );

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
        m_treeModel->removeDocument( doc );
        emit q->fileRemoved( key );
        delete doc;
        m_fileItemHash.remove( key );
    }
}

void FileManager::saveFile( const QString &fileName, const GeoDataDocument *document )
{
    if (document) {
        GeoDataDocumentWriter::write(fileName, *document, kml::kmlTag_nameSpaceOgc22);
    }
}

void FileManager::closeFile( const GeoDataDocument *document )
{
    QHash < QString, GeoDataDocument* >::iterator itpoint = d->m_fileItemHash.begin();
    QHash < QString, GeoDataDocument* >::iterator const endpoint = d->m_fileItemHash.end();
    for (; itpoint != endpoint; ++itpoint ) {
        if( d->m_fileItemHash.value( itpoint.key() ) == document ) {
            d->closeFile( itpoint.key() );
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

int FileManager::pendingFiles() const
{
    return d->m_loaderList.size();
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
            m_treeModel->addDocument( doc );
            m_fileItemHash.insert( loader->path(), doc );
            emit q->fileAdded( loader->path() );
            if( loader->recenter() ) {
                m_latLonBox |= doc->latLonAltBox();
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

        if ( !m_latLonBox.isEmpty() ) {
            emit q->centeredDocument( m_latLonBox );
        }
        m_latLonBox.clear();
    }
}

#include "moc_FileManager.cpp"
