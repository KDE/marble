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

void PlaceMarkManager::loadStandardPlaceMarks(  const QString& target  )
{
    if ( target != m_target )
    {
        clearPlaceMarks();

        if ( target == "earth" ){
            addPlaceMarkFile( "cityplacemarks" );
            addPlaceMarkFile( "baseplacemarks" );
            addPlaceMarkFile( "elevplacemarks" );
            addPlaceMarkFile( "otherplacemarks" );
            addPlaceMarkFile( "boundaryplacemarks" );
        }
        if ( target == "moon" ){
            addPlaceMarkFile( "moonterrain" );
            addPlaceMarkFile( "moonlandingsites" );
        }

        m_target = target;
    }
}

void PlaceMarkManager::addPlaceMarkFile( const QString& filepath, bool finalize )
{
    m_finalized = finalize;
    if( !(m_model->containers().contains( filepath ) ) ) {
        qDebug() << "adding container:" << filepath << finalize;
        PlaceMarkLoader* loader = new PlaceMarkLoader( this, filepath );
        connect (   loader, SIGNAL( placeMarksLoaded( PlaceMarkLoader*, PlaceMarkContainer * ) ), 
                    this, SLOT( loadPlaceMarkContainer( PlaceMarkLoader*, PlaceMarkContainer * ) ) );
        connect (   loader, SIGNAL( placeMarkLoaderFailed( PlaceMarkLoader* ) ), 
                    this, SLOT( cleanupLoader( PlaceMarkLoader* ) ) );
        m_loaderList.append( loader );
        loader->start();
    }
}

void PlaceMarkManager::cleanupLoader( PlaceMarkLoader* loader )
{
    m_loaderList.removeAll( loader );
    if ( loader->isFinished() ) {
         delete loader;
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
    }
}

void PlaceMarkManager::loadKml( const QString& filename, bool clearPrevious )
{
    Q_ASSERT( m_model != 0 && "You have called loadKml before creating a model!" );

    PlaceMarkContainer container;
    importKml( filename, &container );

    m_model->addPlaceMarks( container, clearPrevious );
    qDebug() << "loaded placemarks in " << m_geomodel->geoDataRoot();
    emit geoDataDocumentLoaded( *(m_geomodel->geoDataRoot()) );
}

void PlaceMarkManager::loadKmlFromData( const QString& data, bool clearPrevious )
{
    Q_ASSERT( m_model != 0 && "You have called loadKmlFromData before creating a model!" );

    PlaceMarkContainer container;
    importKmlFromData( data, &container );

    m_model->addPlaceMarks( container, clearPrevious );
}

void PlaceMarkManager::importKml( const QString& filename,
                                  PlaceMarkContainer* placeMarkContainer )
{
    GeoDataParser parser( GeoData_KML );

    QFile file( filename );
    if ( !file.exists() ) {
        qWarning( "File does not exist!" );
        return;
    }

    // Open file in right mode
    file.open( QIODevice::ReadOnly );

    if ( !parser.read( &file ) ) {
        qWarning( "Could not parse file!" );
        return;
    }
    GeoDocument* document = parser.releaseDocument();
    Q_ASSERT( document );

    GeoDataDocument *dataDocument = static_cast<GeoDataDocument*>( document );
    *placeMarkContainer = PlaceMarkContainer( dataDocument->placemarks(), 
                                              QFileInfo( filename ).baseName() );

    // This sets and initializes the geomodel
    m_geomodel->setGeoDataRoot( dataDocument );
}

void PlaceMarkManager::importKmlFromData( const QString& data,
                                         PlaceMarkContainer* placeMarkContainer )
{
    GeoDataParser parser( GeoData_KML );

    QByteArray ba( data.toUtf8() );
    QBuffer buffer( &ba );
    buffer.open( QIODevice::ReadOnly );

    if ( !parser.read( &buffer ) ) {
        qWarning( "Could not parse data!" );
        return;
    }
    GeoDocument* document = parser.releaseDocument();
    Q_ASSERT( document );

    GeoDataDocument *dataDocument = static_cast<GeoDataDocument*>( document );
    // we might have to suppress caching for this part
    *placeMarkContainer = PlaceMarkContainer( dataDocument->placemarks(),
                                              QString("DataImport") );
    // This sets and initializes the geomodel
    m_geomodel->setGeoDataRoot( dataDocument );
}

static const quint32 MarbleMagicNumber = 0x31415926;

void PlaceMarkManager::saveFile( const QString& filename,
                                 PlaceMarkContainer* placeMarkContainer )
{
    if ( QDir( MarbleDirs::localPath() + "/placemarks/" ).exists() == false )
        ( QDir::root() ).mkpath( MarbleDirs::localPath() + "/placemarks/" );

    QFile file( filename );
    file.open( QIODevice::WriteOnly );
    QDataStream out( &file );

    // Write a header with a "magic number" and a version
    // out << (quint32)0xA0B0C0D0;
    out << (quint32)MarbleMagicNumber;
    out << (qint32)014;

    out.setVersion( QDataStream::Qt_4_2 );

    qreal lon;
    qreal lat;
    qreal alt;

    PlaceMarkContainer::const_iterator it = placeMarkContainer->constBegin();
    PlaceMarkContainer::const_iterator const end = placeMarkContainer->constEnd();
    for (; it != end; ++it )
    {
        out << (*it)->name();
        (*it)->coordinate( lon, lat, alt );

        out << lon << lat << alt;
        out << QString( (*it)->role() );
        out << QString( (*it)->description() );
        out << QString( (*it)->countryCode() );
        out << (qreal)(*it)->area();
        out << (qint64)(*it)->population();
    }
}


bool PlaceMarkManager::loadFile( const QString& filename,
                                 PlaceMarkContainer* placeMarkContainer )
{
    QFile file( filename );
    file.open( QIODevice::ReadOnly );
    QDataStream in( &file );

    // Read and check the header
    quint32 magic;
    in >> magic;
    if ( magic != MarbleMagicNumber ) {
        qDebug( "Bad file format!" );
        return false;
    }

    // Read the version
    qint32 version;
    in >> version;
    if ( version < 014 ) {
        qDebug( "Bad file - too old!" );
        return false;
    }
    /*
      if (version > 002) {
      qDebug( "Bad file - too new!" );
      return;
      }
    */

    in.setVersion( QDataStream::Qt_4_2 );

    // Read the data itself
    qreal   lon;
    qreal   lat;
    qreal   alt;
    qreal   area;

    QString  tmpstr;
    qint64   tmpint64;

    QString testo;

    GeoDataPlacemark  *mark;
    while ( !in.atEnd() ) {
        mark = new GeoDataPlacemark;

        in >> tmpstr;
        mark->setName( tmpstr );
        testo = tmpstr;
        in >> lon >> lat >> alt;
        mark->setCoordinate( lon, lat, alt );
        in >> tmpstr;
        mark->setRole( tmpstr.at(0) );
        in >> tmpstr;
        mark->setDescription( tmpstr );
        in >> tmpstr;
        mark->setCountryCode( tmpstr );
        in >> area;
        mark->setArea( area );
        in >> tmpint64;
        mark->setPopulation( tmpint64 );

        placeMarkContainer->append( mark );
    }

    return true;
}

#include "PlaceMarkManager.moc"
