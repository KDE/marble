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

#include "PlacemarkManager.h"

#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtCore/QDataStream>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include "KmlFileViewItem.h"
#include "FileViewModel.h"
#include "MarbleDirs.h"
#include "MarblePlacemarkModel.h"
#include "MarbleGeometryModel.h"
#include "MarbleDataFacade.h"
#include "PlacemarkLoader.h"

#include "GeoDataDocument.h"
#include "GeoDataParser.h"
#include "GeoDataPlacemark.h"


using namespace Marble;

namespace Marble
{
class PlacemarkManagerPrivate
{
    public:
        PlacemarkManagerPrivate( )
        : m_datafacade( 0 )
        {
        }

        MarbleDataFacade* m_datafacade;
        QList<PlacemarkLoader*> m_loaderList;
        QStringList m_pathList;
        QList < AbstractFileViewItem* > m_fileItemList;
};
}

PlacemarkManager::PlacemarkManager( QObject *parent )
    : QObject( parent )
    , d( new PlacemarkManagerPrivate() )
{
}


PlacemarkManager::~PlacemarkManager()
{
    foreach( PlacemarkLoader *loader, d->m_loaderList ) {
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

MarblePlacemarkModel* PlacemarkManager::model() const
{
    return d->m_datafacade->placemarkModel();
}

void PlacemarkManager::setDataFacade( MarbleDataFacade *facade )
{
    d->m_datafacade = facade;
    d->m_datafacade->fileViewModel()->setPlacemarkManager(this);
}

QStringList PlacemarkManager::containers() const
{
    QStringList retList;
    for( int line = 0; line < d->m_fileItemList.count(); ++line ) {
        retList << d->m_fileItemList.at( line )->name();
    }
    return retList + d->m_pathList;
}

QString PlacemarkManager::toRegularName( QString name )
{
    return name.remove(".kml").remove(".cache");
}

void PlacemarkManager::addPlacemarkFile( const QString& filepath )
{
    if( ! containers().contains( toRegularName( filepath ) ) ) {
        qDebug() << "adding container:" << toRegularName( filepath );
        PlacemarkLoader* loader = new PlacemarkLoader( this, filepath );
        appendLoader( loader );
        d->m_pathList.append( toRegularName( filepath ) );
    }
}

void PlacemarkManager::addGeoDataDocument( GeoDataDocument* document )
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
}

void PlacemarkManager::addPlacemarkData( const QString& data, const QString& key )
{
    Q_ASSERT( d->m_datafacade->placemarkModel() != 0 && "You have called loadKmlFromData before creating a model!" );

    qDebug() << "adding container:" << key;
    PlacemarkLoader* loader = new PlacemarkLoader( this, data, key );
    appendLoader( loader );
}

void PlacemarkManager::removePlacemarkKey( const QString& key )
{
    QString nkey = key;
    qDebug() << "trying to remove file:" << key;
    for( int i = 0; i < d->m_fileItemList.size(); ++i )
    {
        if( toRegularName( nkey ) == toRegularName( d->m_fileItemList.at(i)->name() ) ) {
            closeFile(i);
            break;
        }
    }
}

void PlacemarkManager::addFile ( AbstractFileViewItem * item )
{
    d->m_fileItemList.append( item );
    emit fileAdded(d->m_fileItemList.indexOf( item ) );
}

void PlacemarkManager::saveFile( int index )
{
    if (index < d->m_fileItemList.size() )
    {
        d->m_fileItemList.at( index )->saveFile();
    }
}

void PlacemarkManager::closeFile( int index )
{
    if (index < d->m_fileItemList.size() )
    {
        d->m_fileItemList.at( index )->closeFile( indexStart( index ));
        delete d->m_fileItemList.at( index );
        d->m_fileItemList.removeAt( index );
        emit fileRemoved( index );
    }
}

int PlacemarkManager::size() const
{
    return d->m_fileItemList.size();
}

AbstractFileViewItem * PlacemarkManager::at( int index )
{
    if (index < d->m_fileItemList.size() )
    {
        return d->m_fileItemList.at( index );
    }
    return 0;
}

void PlacemarkManager::appendLoader( PlacemarkLoader *loader )
{
    connect (   loader, SIGNAL( placemarksLoaded( PlacemarkLoader*, PlacemarkContainer * ) ),
                this, SLOT( loadPlacemarkContainer( PlacemarkLoader*, PlacemarkContainer * ) ) );

    connect (   loader, SIGNAL( placemarkLoaderFailed( PlacemarkLoader* ) ),
                this, SLOT( cleanupLoader( PlacemarkLoader* ) ) );

    connect (   loader, SIGNAL( newGeoDataDocumentAdded( GeoDataDocument* ) ),
                this, SLOT( addGeoDataDocument( GeoDataDocument* ) ) );

    d->m_loaderList.append( loader );
    loader->start();
}

void PlacemarkManager::cleanupLoader( PlacemarkLoader* loader )
{
    d->m_loaderList.removeAll( loader );
    if ( loader->isFinished() ) {
         d->m_pathList.removeAll( loader->path() );
         delete loader;
    }
}

void PlacemarkManager::loadPlacemarkContainer( PlacemarkLoader* loader, QVector<Marble::GeoDataPlacemark> * container )
{
    qDebug() << "Containername " << "to be finalized:" << (d->m_loaderList.size() == 1) << d->m_loaderList.size();
    d->m_loaderList.removeAll( loader );
    if ( container )
    { 
        d->m_datafacade->placemarkModel()->addPlacemarks( *container, false, d->m_loaderList.isEmpty() );
    }

    if( d->m_loaderList.isEmpty() ) {
        emit finalize();
    }

    if ( loader->isFinished() ) {
         d->m_pathList.removeAll( loader->path() );
         delete loader;
    }
}

int PlacemarkManager::indexStart( int index )
{
    int start = 0;
    for( int i = 0; i < index; i++ ) {
        start += d->m_fileItemList.at( i )->size();
    }
    return start;
}

#include "PlacemarkManager.moc"
