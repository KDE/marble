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

#include "PlaceMarkManager.h"

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
#include "PlaceMarkContainer.h"
#include "PlaceMarkLoader.h"

#include "GeoDataDocument.h"
#include "GeoDataParser.h"
#include "GeoDataPlacemark.h"


using namespace Marble;

namespace Marble {
class PlaceMarkManagerPrivate
{
    public:
        PlaceMarkManagerPrivate( QObject* parent )
        : m_model( 0 )
        , m_geomodel( new MarbleGeometryModel() )
        , m_target( QString() )
        , m_finalized( true )
        , m_fileViewModel( new FileViewModel(parent ) )
        {
        };

        MarblePlacemarkModel* m_model;
        MarbleGeometryModel* m_geomodel;
        QList<PlaceMarkLoader*> m_loaderList;
        FileViewModel* m_fileViewModel;

        bool m_finalized;
        QString m_target;
        PlaceMarkManager* q_ptr;
};
}

PlaceMarkManager::PlaceMarkManager( QObject *parent )
    : QObject( parent )
    , d(new PlaceMarkManagerPrivate( parent ) )
{
    
}


PlaceMarkManager::~PlaceMarkManager()
{
    foreach( PlaceMarkLoader *loader, d->m_loaderList ) {
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

MarblePlacemarkModel* PlaceMarkManager::model() const
{
    return d->m_model;
}

FileViewModel* PlaceMarkManager::fileViewModel() const
{
    return d->m_fileViewModel;
}

MarbleGeometryModel* PlaceMarkManager::geomodel() const
{
    return d->m_geomodel;
}

void PlaceMarkManager::setGeoModel( MarbleGeometryModel * model )
{
    d->m_geomodel = model;
}

void PlaceMarkManager::setPlaceMarkModel( MarblePlacemarkModel *model )
{
    d->m_model = model;
}

void PlaceMarkManager::clearPlaceMarks()
{
    d->m_model->clearPlaceMarks();
}

void PlaceMarkManager::addPlaceMarkFile( const QString& filepath, bool finalized )
{
    if( !(d->m_model->containers().contains( filepath ) ) ) {
        qDebug() << "adding container:" << filepath << finalized;
        PlaceMarkLoader* loader = new PlaceMarkLoader( this, filepath, finalized );
        connect (   loader, SIGNAL( placeMarksLoaded( PlaceMarkLoader*, PlaceMarkContainer * ) ), 
                    this, SLOT( loadPlaceMarkContainer( PlaceMarkLoader*, PlaceMarkContainer * ) ) );
        connect (   loader, SIGNAL( placeMarkLoaderFailed( PlaceMarkLoader* ) ), 
                    this, SLOT( cleanupLoader( PlaceMarkLoader* ) ) );
        connect (   loader, SIGNAL( newGeoDataDocumentAdded( GeoDataDocument* ) ), 
                    this, SIGNAL( geoDataDocumentAdded( GeoDataDocument* ) ) );
        connect (   loader, SIGNAL( newGeoDataDocumentAdded( GeoDataDocument* ) ), 
                    this, SLOT( addGeoDataDocument( GeoDataDocument* ) ) );
        d->m_loaderList.append( loader );
        loader->start();
    }
}

void PlaceMarkManager::addGeoDataDocument( GeoDataDocument* document )
{
    AbstractFileViewItem* item = new KmlFileViewItem( *this,
                                                      *document );

    d->m_fileViewModel->append( item );
}

void PlaceMarkManager::addPlaceMarkData( const QString& data, const QString& key )
{
    loadKmlFromData( data, key, false );
}

void PlaceMarkManager::removePlaceMarkKey( const QString& key )
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

void PlaceMarkManager::cleanupLoader( PlaceMarkLoader* loader )
{
    d->m_loaderList.removeAll( loader );
    if ( loader->isFinished() ) {
         delete loader;
    }
}

void PlaceMarkManager::loadPlaceMarkContainer( PlaceMarkLoader* loader, PlaceMarkContainer * container )
{
    qDebug() << "Containername:" << container->name() << "to be finalized:" << loader->finalize() << d->m_loaderList.size();
    d->m_loaderList.removeAll( loader );
    if ( container )
    { 
        d->m_model->addPlaceMarks( *container, false, d->m_finalized && d->m_loaderList.isEmpty() );
    }

    if( 0 == d->m_loaderList.size() ) 
        emit finalize();
    if ( loader->isFinished() ) {
         delete loader;
    }
}

void PlaceMarkManager::loadKml( const QString& filename, bool clearPrevious )
{
    addPlaceMarkFile( filename, true );
}

void PlaceMarkManager::loadKmlFromData( const QString& data, const QString& key, bool finalize )
{
    Q_ASSERT( d->m_model != 0 && "You have called loadKmlFromData before creating a model!" );

    PlaceMarkContainer container;

    d->m_finalized = true;
    qDebug() << "adding container:" << key;
    PlaceMarkLoader* loader = new PlaceMarkLoader( this, data, key );
    connect (   loader, SIGNAL( placeMarksLoaded( PlaceMarkLoader*, PlaceMarkContainer * ) ), 
                this, SLOT( loadPlaceMarkContainer( PlaceMarkLoader*, PlaceMarkContainer * ) ) );
    connect (   loader, SIGNAL( placeMarkLoaderFailed( PlaceMarkLoader* ) ), 
                this, SLOT( cleanupLoader( PlaceMarkLoader* ) ) );
    connect (   loader, SIGNAL( newGeoDataDocumentAdded( GeoDataDocument* ) ), 
                this, SIGNAL( geoDataDocumentAdded( GeoDataDocument* ) ) );
    connect (   loader, SIGNAL( newGeoDataDocumentAdded( GeoDataDocument* ) ), 
                this, SLOT( addGeoDataDocument( GeoDataDocument* ) ) );
    d->m_loaderList.append( loader );
    loader->start();
}

#include "PlaceMarkManager.moc"
