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
#include <QtXml/QXmlInputSource>
#include <QtXml/QXmlSimpleReader>

#include "KmlFileViewItem.h"
#include "FileViewModel.h"
#include "MarbleDirs.h"
#include "MarblePlacemarkModel.h"
#include "MarbleGeometryModel.h"
#include "PlacemarkContainer.h"
#include "PlacemarkLoader.h"

#include "GeoDataDocument.h"
#include "GeoDataParser.h"
#include "GeoDataPlacemark.h"


using namespace Marble;

namespace Marble {
class PlacemarkManagerPrivate
{
    public:
        PlacemarkManagerPrivate( )
        : m_model( 0 )
        , m_geomodel( 0 )
        , m_fileViewModel( new FileViewModel() )
        , m_finalized( true )
        , m_target( QString() )
        {
        };

        MarblePlacemarkModel* m_model;
        MarbleGeometryModel* m_geomodel;
        QList<PlacemarkLoader*> m_loaderList;
        FileViewModel* m_fileViewModel;
        QStringList m_pathList;

        bool m_finalized;
        QString m_target;
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

    delete d->m_model;
    delete d->m_fileViewModel;
    delete d;
    /* do not delete the d->m_geomodel here
     * it is not this models property
     */
}

MarblePlacemarkModel* PlacemarkManager::model() const
{
    return d->m_model;
}

FileViewModel* PlacemarkManager::fileViewModel() const
{
    return d->m_fileViewModel;
}

MarbleGeometryModel* PlacemarkManager::geomodel() const
{
    return d->m_geomodel;
}

void PlacemarkManager::setGeoModel( MarbleGeometryModel * model )
{
    d->m_geomodel = model;
}

void PlacemarkManager::setPlacemarkModel( MarblePlacemarkModel *model )
{
    d->m_model = model;
}

void PlacemarkManager::clearPlacemarks()
{
    d->m_model->clearPlacemarks();
}

QStringList PlacemarkManager::containers() const
{
    return fileViewModel()->containers() + d->m_pathList;
}

QString PlacemarkManager::toRegularName( QString name )
{
    return name.remove(".kml").remove(".cache");
}

void PlacemarkManager::addPlacemarkFile( const QString& filepath, bool finalized )
{
    if( ! containers().contains( toRegularName( filepath ) ) ) {
        qDebug() << "adding container:" << toRegularName( filepath ) << finalized;
        PlacemarkLoader* loader = new PlacemarkLoader( this, filepath, finalized );
        connect (   loader, SIGNAL( placemarksLoaded( PlacemarkLoader*, PlacemarkContainer * ) ), 
                    this, SLOT( loadPlacemarkContainer( PlacemarkLoader*, PlacemarkContainer * ) ) );
        connect (   loader, SIGNAL( placemarkLoaderFailed( PlacemarkLoader* ) ), 
                    this, SLOT( cleanupLoader( PlacemarkLoader* ) ) );
        connect (   loader, SIGNAL( newGeoDataDocumentAdded( GeoDataDocument* ) ), 
                    this, SLOT( addGeoDataDocument( GeoDataDocument* ) ) );
        d->m_loaderList.append( loader );
        d->m_pathList.append( toRegularName( filepath ) );
        loader->start();
    }
}

void PlacemarkManager::addGeoDataDocument( GeoDataDocument* document )
{
    AbstractFileViewItem* item = new KmlFileViewItem( *this, *document );

    d->m_fileViewModel->append( item );

    // now get the document that will be preserved throughout the life time
    GeoDataDocument* doc = dynamic_cast<KmlFileViewItem*>(item)->document();
    // remove the hashes in front of the styles.
    QVector<GeoDataFeature>::Iterator end = doc->end();
    QVector<GeoDataFeature>::Iterator itr = doc->begin();
    for ( ; itr != end; ++itr ) {
        // use *itr (or itr.value()) here
        QString styleUrl = itr->styleUrl().remove('#');
        itr->setStyle( &doc->style( styleUrl ) );
    }

    // do not set this file if it only contains points
    if( doc->isVisible() && d->m_geomodel )
        d->m_geomodel->setGeoDataRoot( doc );
    emit geoDataDocumentAdded( *doc );
}

void PlacemarkManager::addPlacemarkData( const QString& data, const QString& key )
{
    loadKmlFromData( data, key, false );
}

void PlacemarkManager::removePlacemarkKey( const QString& key )
{
    QString nkey = key;
    qDebug() << "trying to remove file:" << key;
    for( int i = 0; i < d->m_fileViewModel->rowCount(); ++i )
    {
        if( toRegularName( nkey ) == toRegularName( d->m_fileViewModel->data(d->m_fileViewModel->index(i, 0)).toString() ) ) {
            d->m_fileViewModel->remove(d->m_fileViewModel->index(i, 0));
            break;
        }
    };
}

void PlacemarkManager::cleanupLoader( PlacemarkLoader* loader )
{
    d->m_loaderList.removeAll( loader );
    if ( loader->isFinished() ) {
         d->m_pathList.removeAll( loader->path() );
         delete loader;
    }
}

void PlacemarkManager::loadPlacemarkContainer( PlacemarkLoader* loader, PlacemarkContainer * container )
{
    qDebug() << "Containername:" << container->name() << "to be finalized:" << (d->m_loaderList.size() == 1) << d->m_loaderList.size();
    d->m_loaderList.removeAll( loader );
    if ( container )
    { 
        d->m_model->addPlacemarks( *container, false, d->m_finalized && d->m_loaderList.isEmpty() );
    }

    if( d->m_loaderList.isEmpty() ) {
        emit finalize();
    }

    if ( loader->isFinished() ) {
         d->m_pathList.removeAll( loader->path() );
         delete loader;
    }
}

void PlacemarkManager::loadKml( const QString& filename, bool clearPrevious )
{
    Q_UNUSED( clearPrevious )

    addPlacemarkFile( filename, true );
}

void PlacemarkManager::loadKmlFromData( const QString& data, const QString& key, bool finalize )
{
    Q_UNUSED( finalize )

    Q_ASSERT( d->m_model != 0 && "You have called loadKmlFromData before creating a model!" );

    PlacemarkContainer container;

    d->m_finalized = true;
    qDebug() << "adding container:" << key;
    PlacemarkLoader* loader = new PlacemarkLoader( this, data, key );
    connect (   loader, SIGNAL( placemarksLoaded( PlacemarkLoader*, PlacemarkContainer * ) ), 
                this, SLOT( loadPlacemarkContainer( PlacemarkLoader*, PlacemarkContainer * ) ) );
    connect (   loader, SIGNAL( placemarkLoaderFailed( PlacemarkLoader* ) ), 
                this, SLOT( cleanupLoader( PlacemarkLoader* ) ) );
    connect (   loader, SIGNAL( newGeoDataDocumentAdded( GeoDataDocument* ) ), 
                this, SLOT( addGeoDataDocument( GeoDataDocument* ) ) );
    d->m_loaderList.append( loader );
    loader->start();
}

#include "PlacemarkManager.moc"
