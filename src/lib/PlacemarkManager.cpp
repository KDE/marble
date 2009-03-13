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

#include "MarbleDirs.h"
#include "MarblePlacemarkModel.h"
#include "MarbleGeometryModel.h"
#include "PlacemarkContainer.h"
#include "PlacemarkLoader.h"

#include "GeoDataDocument.h"
#include "GeoDataParser.h"
#include "GeoDataPlacemark.h"


using namespace Marble;

PlacemarkManager::PlacemarkManager( QObject *parent )
    : QObject( parent ),
      m_model( 0 ),
      m_geomodel( new MarbleGeometryModel() ),
      m_target(QString())
{
}


PlacemarkManager::~PlacemarkManager()
{
    foreach( PlacemarkLoader *loader, m_loaderList ) {
        if ( loader ) {
            loader->wait();
        }
    }

    delete m_model;
    /* do not delete the m_geomodel here
     * it is not this models property
     */
}

MarblePlacemarkModel* PlacemarkManager::model() const
{
    return m_model;
}

MarbleGeometryModel* PlacemarkManager::geomodel() const
{
    return m_geomodel;
}

void PlacemarkManager::setGeoModel( MarbleGeometryModel * model )
{
    m_geomodel = model;
}

void PlacemarkManager::setPlacemarkModel( MarblePlacemarkModel *model )
{
    m_model = model;
}

void PlacemarkManager::clearPlacemarks()
{
    m_model->clearPlacemarks();
}

void PlacemarkManager::addPlacemarkFile( const QString& filepath, bool finalized )
{
    m_finalized = finalized;
    if( !(m_model->containers().contains( filepath ) ) ) {
        qDebug() << "adding container:" << filepath << finalized;
        PlacemarkLoader* loader = new PlacemarkLoader( this, filepath );
        connect (   loader, SIGNAL( placemarksLoaded( PlacemarkLoader*, PlacemarkContainer * ) ), 
                    this, SLOT( loadPlacemarkContainer( PlacemarkLoader*, PlacemarkContainer * ) ) );
        connect (   loader, SIGNAL( placemarkLoaderFailed( PlacemarkLoader* ) ), 
                    this, SLOT( cleanupLoader( PlacemarkLoader* ) ) );
        connect (   loader, SIGNAL( newGeoDataDocumentAdded( GeoDataDocument* ) ), 
                    this, SIGNAL( geoDataDocumentAdded( GeoDataDocument* ) ) );
        m_loaderList.append( loader );
        loader->start();
    }
    else {
        if( finalized ) 
            emit finalize();
    }
}

void PlacemarkManager::cleanupLoader( PlacemarkLoader* loader )
{
    m_loaderList.removeAll( loader );
    if ( loader->isFinished() ) {
         delete loader;
    }
}

void PlacemarkManager::loadPlacemarkContainer( PlacemarkLoader* loader, PlacemarkContainer * container )
{
    m_loaderList.removeAll( loader );
    if ( loader->isFinished() ) {
         delete loader;
    }

    if ( container )
    { 
        m_model->addPlacemarks( *container, false, m_finalized && m_loaderList.isEmpty() );
        if( m_finalized ) 
            emit finalize();
    }
}

void PlacemarkManager::loadKml( const QString& filename, bool clearPrevious )
{
    addPlacemarkFile( filename, true );
}

void PlacemarkManager::loadKmlFromData( const QString& data, const QString& key, bool finalize )
{
    Q_ASSERT( m_model != 0 && "You have called loadKmlFromData before creating a model!" );

    PlacemarkContainer container;

    m_finalized = true;
    qDebug() << "adding container:" << key;
    PlacemarkLoader* loader = new PlacemarkLoader( this, data, key );
    connect (   loader, SIGNAL( placemarksLoaded( PlacemarkLoader*, PlacemarkContainer * ) ), 
                this, SLOT( loadPlacemarkContainer( PlacemarkLoader*, PlacemarkContainer * ) ) );
    connect (   loader, SIGNAL( placemarkLoaderFailed( PlacemarkLoader* ) ), 
                this, SLOT( cleanupLoader( PlacemarkLoader* ) ) );
    connect (   loader, SIGNAL( newGeoDataDocumentAdded( GeoDataDocument* ) ), 
                this, SIGNAL( geoDataDocumentAdded( GeoDataDocument* ) ) );
    m_loaderList.append( loader );
    loader->start();
}

#include "PlacemarkManager.moc"
