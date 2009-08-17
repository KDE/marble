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
        : m_datafacade( 0 )
        {
        }

        MarbleDataFacade* m_datafacade;
        QList<PlacemarkLoader*> m_loaderList;
        QStringList m_pathList;
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

    delete d;
}

MarblePlacemarkModel* PlacemarkManager::model() const
{
    return d->m_datafacade->placemarkModel();
}

void PlacemarkManager::setDataFacade( MarbleDataFacade *facade )
{
    d->m_datafacade = facade;
}

QStringList PlacemarkManager::containers() const
{
    return d->m_datafacade->fileViewModel()->containers() + d->m_pathList;
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
    AbstractFileViewItem* item = new KmlFileViewItem( *this, *document );

    d->m_datafacade->fileViewModel()->append( item );

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
    FileViewModel *fileViewModel = d->m_datafacade->fileViewModel();
    qDebug() << "trying to remove file:" << key;
    for( int i = 0; i < fileViewModel->rowCount(); ++i )
    {
        if( toRegularName( nkey ) == toRegularName( fileViewModel->data(fileViewModel->index(i, 0)).toString() ) ) {
            fileViewModel->remove(fileViewModel->index(i, 0));
            break;
        }
    };
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

void PlacemarkManager::loadPlacemarkContainer( PlacemarkLoader* loader, PlacemarkContainer * container )
{
    qDebug() << "Containername:" << container->name() << "to be finalized:" << (d->m_loaderList.size() == 1) << d->m_loaderList.size();
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

#include "PlacemarkManager.moc"
