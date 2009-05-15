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
        PlacemarkManagerPrivate( QObject* parent )
        : m_model( 0 )
        , m_geomodel( new MarbleGeometryModel() )
        , m_fileViewModel( new FileViewModel(parent ) )
        , m_finalized( true )
        , m_target( QString() )
        {
        };

        MarblePlacemarkModel* m_model;
        MarbleGeometryModel* m_geomodel;
        QList<PlacemarkLoader*> m_loaderList;
        FileViewModel* m_fileViewModel;

        bool m_finalized;
        QString m_target;
};
}

PlacemarkManager::PlacemarkManager( QObject *parent )
    : QObject( parent )
    , d( new PlacemarkManagerPrivate( parent ) )
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

void PlacemarkManager::addPlacemarkFile( const QString& filepath, bool finalized )
{
    if( !(d->m_model->containers().contains( filepath ) ) ) {
        qDebug() << "adding container:" << filepath << finalized;
        PlacemarkLoader* loader = new PlacemarkLoader( this, filepath, finalized );
        connect (   loader, SIGNAL( placemarksLoaded( PlacemarkLoader*, PlacemarkContainer * ) ), 
                    this, SLOT( loadPlacemarkContainer( PlacemarkLoader*, PlacemarkContainer * ) ) );
        connect (   loader, SIGNAL( placemarkLoaderFailed( PlacemarkLoader* ) ), 
                    this, SLOT( cleanupLoader( PlacemarkLoader* ) ) );
        connect (   loader, SIGNAL( newGeoDataDocumentAdded( GeoDataDocument* ) ), 
                    this, SIGNAL( geoDataDocumentAdded( GeoDataDocument* ) ) );
        connect (   loader, SIGNAL( newGeoDataDocumentAdded( GeoDataDocument* ) ), 
                    this, SLOT( addGeoDataDocument( GeoDataDocument* ) ) );
        d->m_loaderList.append( loader );
        loader->start();
    }
}

void PlacemarkManager::addGeoDataDocument( GeoDataDocument* document )
{
    AbstractFileViewItem* item = new KmlFileViewItem( *this,
                                                      *document );

    d->m_fileViewModel->append( item );
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
        if(nkey.remove(".kml").remove(".cache") == d->m_fileViewModel->data(d->m_fileViewModel->index(i, 0)).toString().remove(".kml").remove(".cache")) {
            d->m_fileViewModel->remove(d->m_fileViewModel->index(i, 0));
        }
    };
}

void PlacemarkManager::cleanupLoader( PlacemarkLoader* loader )
{
    d->m_loaderList.removeAll( loader );
    if ( loader->isFinished() ) {
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
                this, SIGNAL( geoDataDocumentAdded( GeoDataDocument* ) ) );
    connect (   loader, SIGNAL( newGeoDataDocumentAdded( GeoDataDocument* ) ), 
                this, SLOT( addGeoDataDocument( GeoDataDocument* ) ) );
    d->m_loaderList.append( loader );
    loader->start();
}

#include "PlacemarkManager.moc"
