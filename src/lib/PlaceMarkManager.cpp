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

#include "MarbleDirs.h"
#include "MarblePlacemarkModel.h"
#include "MarbleGeometryModel.h"
#include "PlaceMarkContainer.h"
#include "PlaceMarkLoader.h"

#include "GeoDataDocument.h"
#include "GeoDataParser.h"
#include "GeoDataPlacemark.h"


using namespace Marble;

PlaceMarkManager::PlaceMarkManager( QObject *parent )
    : QObject( parent ),
      m_model( 0 ),
      m_geomodel( new MarbleGeometryModel() ),
      m_target(QString())
{
}


PlaceMarkManager::~PlaceMarkManager()
{
    delete m_model;
    /* do not delete the m_geomodel here
     * it is not this models property
     */
}

MarblePlacemarkModel* PlaceMarkManager::model() const
{
    return m_model;
}

MarbleGeometryModel* PlaceMarkManager::geomodel() const
{
    return m_geomodel;
}

void PlaceMarkManager::setGeoModel( MarbleGeometryModel * model )
{
    m_geomodel = model;
}

void PlaceMarkManager::setPlaceMarkModel( MarblePlacemarkModel *model )
{
    m_model = model;
}

void PlaceMarkManager::clearPlaceMarks()
{
    m_model->clearPlaceMarks();
}

void PlaceMarkManager::addPlaceMarkFile( const QString& filepath, bool finalized )
{
    m_finalized = finalized;
    if( !(m_model->containers().contains( filepath ) ) ) {
        qDebug() << "adding container:" << filepath << finalized;
        PlaceMarkLoader* loader = new PlaceMarkLoader( this, filepath );
        connect (   loader, SIGNAL( placeMarksLoaded( PlaceMarkLoader*, PlaceMarkContainer * ) ), 
                    this, SLOT( loadPlaceMarkContainer( PlaceMarkLoader*, PlaceMarkContainer * ) ) );
        connect (   loader, SIGNAL( placeMarkLoaderFailed( PlaceMarkLoader* ) ), 
                    this, SLOT( cleanupLoader( PlaceMarkLoader* ) ) );
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

void PlaceMarkManager::cleanupLoader( PlaceMarkLoader* loader )
{
    foreach( PlaceMarkLoader *loader, m_loaderList ) {
        if ( loader ) {
            loader->wait();
        }
    }

    m_loaderList.removeAll( loader );
    if ( loader->isFinished() ) {
        delete loader;
        loader = 0;
    }
}

void PlaceMarkManager::loadPlaceMarkContainer( PlaceMarkLoader* loader, PlaceMarkContainer * container )
{
    m_loaderList.removeAll( loader );
    if ( loader->isFinished() ) {
         delete loader;
    }

    if ( container )
    { 
        m_model->addPlaceMarks( *container, false, m_finalized && m_loaderList.isEmpty() );
        if( m_finalized ) 
            emit finalize();
    }
}

void PlaceMarkManager::loadKml( const QString& filename, bool clearPrevious )
{
    addPlaceMarkFile( filename, true );
}

void PlaceMarkManager::loadKmlFromData( const QString& data, const QString& key, bool finalize )
{
    Q_ASSERT( m_model != 0 && "You have called loadKmlFromData before creating a model!" );

    PlaceMarkContainer container;

    m_finalized = true;
    qDebug() << "adding container:" << key;
    PlaceMarkLoader* loader = new PlaceMarkLoader( this, data, key );
    connect (   loader, SIGNAL( placeMarksLoaded( PlaceMarkLoader*, PlaceMarkContainer * ) ), 
                this, SLOT( loadPlaceMarkContainer( PlaceMarkLoader*, PlaceMarkContainer * ) ) );
    connect (   loader, SIGNAL( placeMarkLoaderFailed( PlaceMarkLoader* ) ), 
                this, SLOT( cleanupLoader( PlaceMarkLoader* ) ) );
    connect (   loader, SIGNAL( newGeoDataDocumentAdded( GeoDataDocument* ) ), 
                this, SIGNAL( geoDataDocumentAdded( GeoDataDocument* ) ) );
    m_loaderList.append( loader );
    loader->start();
}

#include "PlaceMarkManager.moc"
