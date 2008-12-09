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
#ifdef KML_GSOC
    foreach ( KMLDocument* document, m_documentList ) {
        delete document;
    }
#else
    delete m_model;
    /* do not delete the m_geomodel here
     * it is not this models property
     */
#endif
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

void PlaceMarkManager::addPlaceMarkFile( const QString& filepath )
{
#ifdef KML_GSOC
    /*
     * Simply call loadKml, should use cache in feature
     */
    loadKml( filepath );
#else
    PlaceMarkLoader* loader = new PlaceMarkLoader( this, m_model, filepath );
    m_loaderList.append( loader );
    loader->start();
#endif
}


void PlaceMarkManager::loadKml( const QString& filename, bool clearPrevious )
{
#ifdef KML_GSOC
    if ( QFile::exists( filename ) ) {
        QFile sourceFile( filename );

        if ( sourceFile.open( QIODevice::ReadOnly ) ) {
            /*
             * Create KMLDocument and set it's name like input filename
             */
            KMLDocument* document = new KMLDocument;
            document->setName( QFileInfo( sourceFile ).fileName() );

            QTime t;
            t.start();
            document->load( sourceFile );
            qDebug( "KML document loaded. Name: %s. Time: %d", 
                    document->name().toAscii().data(), t.elapsed() );
            t.start();

            if ( ! m_documentList.isEmpty() ) {
                const KMLDocument& lastLoadedDocument = *m_documentList.last();
                document->setId( lastLoadedDocument.id() + 1 );
            }

            m_documentList.append( document );
            sourceFile.close();

            /*
             * Pack document to it's own cache file
             * and update cache index
             */
            cacheDocument( *document );
            updateCacheIndex();

            emit geoDataDocumentLoaded( *document );
        }
    }
#else
    Q_ASSERT( m_model != 0 && "You have called loadKml before creating a model!" );

    PlaceMarkContainer container;
    importKml( filename, &container );

    m_model->addPlaceMarks( container, clearPrevious );
    qDebug() << "loaded placemarks in " << m_geomodel->geoDataRoot();
    emit geoDataDocumentLoaded( *(m_geomodel->geoDataRoot()) );
#endif
}

void PlaceMarkManager::loadKmlFromData( const QString& data, bool clearPrevious )
{
    Q_ASSERT( m_model != 0 && "You have called loadKmlFromData before creating a model!" );

    PlaceMarkContainer container;
    importKmlFromData( data, &container );

    m_model->addPlaceMarks( container, clearPrevious );
}


#ifdef KML_GSOC
const QList < KMLFolder* >& PlaceMarkManager::getFolderList() const
{
    return ( QList < KMLFolder*>& ) m_documentList;
}
#endif

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

#ifdef KML_GSOC
void PlaceMarkManager::updateCacheIndex()
{
    QString cacheIndexFileName = QString( "%1/placemarks/kmldocument-cache.index" )
        .arg( MarbleDirs::localPath() );

    QFile indexFile( cacheIndexFileName );
    if ( indexFile.open( QIODevice::WriteOnly ) ) {
        QDataStream stream( &indexFile );

        /*
         * TODO: Put some unique header here
         */

        for ( QList <KMLDocument*>::const_iterator iterator = m_documentList.constBegin();
            iterator != m_documentList.constEnd();
            ++iterator )
        {
            const KMLDocument& document = **iterator;

            QString cacheFileName = QString( "%1.%2.cache" )
                .arg( document.id() ).arg( document.name() );
            stream << cacheFileName;
        }

        indexFile.close();
        qDebug( "Create index cache file: %s", cacheIndexFileName.toAscii().data() );
    }
    else {
        qDebug( "Unable to create index file: %s", cacheIndexFileName.toAscii().data() );
    }
}

void PlaceMarkManager::cacheDocument( const KMLDocument& document )
{
    QString path = QString( "%1/placemarks/%2.%3.cache" );
    path = path.arg( MarbleDirs::localPath() );
    path = path.arg( document.id() );
    path = path.arg( document.name() );

    QFile cacheFile( path );
    if ( cacheFile.open( QIODevice::WriteOnly ) ) {
        QDataStream stream( &cacheFile );
        document.pack( stream );
        cacheFile.close();
        qDebug( "Saved kml document to cache: %s", path.toAscii().data() );
    }
    else {
        qDebug( "Unable to cache kml document to: %s", path.toAscii().data() );
    }
}

void PlaceMarkManager::loadDocumentFromCache ( QString &path, KMLDocument& document )
{
    if ( QFile::exists( path ) ) {
        QFile cacheFile( path );
        if ( cacheFile.open( QIODevice::ReadOnly ) ) {
            QDataStream stream( &cacheFile );
            document.unpack( stream );
            cacheFile.close();
            qDebug( "Loaded document from cache '%s'", path.toAscii().data() );
        }
        else {
            qDebug( "Unable to open cache file: %s", path.toAscii().data() );
        }
    }
    else {
        qDebug( "Cache file '%s' not exists!", path.toAscii().data() );
    }
}

#endif

#include "PlaceMarkManager.moc"
