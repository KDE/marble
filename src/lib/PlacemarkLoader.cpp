//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//

#include "PlacemarkLoader.h"

#include <QtCore/QBuffer>
#include <QtCore/QDataStream>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QThread>

#include "GeoDataParser.h"
#include "GeoSceneDocument.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "MarbleDirs.h"
#include "MarblePlacemarkModel.h"
#include "PlacemarkContainer.h"


namespace Marble {

PlacemarkLoader::PlacemarkLoader( QObject* parent, const QString& file, bool finalize )
    : QThread( parent ), 
      m_filepath( file ),
      m_contents( QString() ),
      m_finalize( finalize ),
      m_document( 0 ),
      m_container( 0 )
{
}

PlacemarkLoader::PlacemarkLoader( QObject* parent, const QString& contents, const QString& file, bool finalize )
    : QThread( parent ), 
      m_filepath( file ), 
      m_contents( contents ),
      m_finalize( finalize ),
      m_document( 0 ),
      m_container( 0 )
{
}

PlacemarkLoader::~PlacemarkLoader()
{
    delete m_container;
    delete m_document;
}

QString PlacemarkLoader::path() const
{
    return m_filepath;
}

void PlacemarkLoader::run()
{
    if( m_contents.isEmpty() ) {
        QString defaultcachename;
        QString defaultsrcname;
        QString defaulthomecache;

        m_container = new PlacemarkContainer( m_filepath );
    
        if( m_filepath.endsWith(".kml") ) {
            m_filepath.remove(QRegExp("\\.kml$"));
        }
        qDebug() << "starting parser for" << m_filepath;

        QFileInfo fileinfo(m_filepath);
        if ( fileinfo.isAbsolute() ) {
            // We got an _absolute_ path now: e.g. "/patrick.kml"
            defaultcachename = m_filepath + ".cache";
            defaultsrcname   = m_filepath + ".kml";
        }
        else {
            if ( m_filepath.contains( '/' ) ) {
                // _relative_ path: "maps/mars/viking/patrick.kml" 
                defaultcachename = MarbleDirs::path( m_filepath + ".cache" );
                defaultsrcname   = MarbleDirs::path( m_filepath + ".kml");
                defaulthomecache = MarbleDirs::localPath() + m_filepath + ".cache";
            }
            else {
                // _standard_ shared placemarks: "placemarks/patrick.kml"
                defaultcachename = MarbleDirs::path( "placemarks/" + m_filepath + ".cache" );
                defaultsrcname   = MarbleDirs::path( "placemarks/" + m_filepath + ".kml");
                defaulthomecache = MarbleDirs::localPath() + "/placemarks/" + m_filepath + ".cache";
            }
        }

        if ( QFile::exists( defaultcachename ) ) {
            qDebug() << "Loading Default Placemark Cache File:" + defaultcachename;

            bool      cacheoutdated = false;
            QDateTime sourceLastModified;
            QDateTime cacheLastModified;

            if ( QFile::exists( defaultsrcname ) ) {
                sourceLastModified = QFileInfo( defaultsrcname ).lastModified();
                cacheLastModified  = QFileInfo( defaultcachename ).lastModified();

                if ( cacheLastModified < sourceLastModified )
                    cacheoutdated = true;
            }

            bool loadok = false;

            if ( !cacheoutdated ) {
                loadok = loadFile( defaultcachename );
                if ( loadok )
                    emit placemarksLoaded( this, m_container );
            }
            qDebug() << "Loading ended" << loadok;
            if ( loadok ) {
                qDebug() << "placemarksLoaded";
                return;
            }
        }

        qDebug() << "No recent Default Placemark Cache File available for " << m_filepath;

        if ( QFile::exists( defaultsrcname ) ) {

            // Read the KML file.
            importKml( defaultsrcname );

            qDebug() << "ContainerSize for" << m_filepath << ":" << m_container->size();
            // Save the contents in the efficient cache format.
            saveFile( defaulthomecache );

            qDebug() << "placemarksLoaded";

            // ...and finally add it to the PlacemarkContainer
            emit placemarksLoaded( this, m_container );
        }
        else {
            qDebug() << "No Default Placemark Source File for " << m_filepath;
            emit placemarkLoaderFailed( this );
        }
    } else {
        m_container = new PlacemarkContainer( m_filepath );

        // Read the KML Data
        importKmlFromData();

        emit placemarksLoaded( this, m_container );
    }
}

const quint32 MarbleMagicNumber = 0x31415926;

void PlacemarkLoader::importKml( const QString& filename )
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

    m_document = static_cast<GeoDataDocument*>( document );
    m_document->setFileName( m_filepath );

    m_container = new PlacemarkContainer( m_document->placemarks(), 
                                          m_filepath );

    file.close();

    qDebug() << "newGeoDataDocumentAdded" << m_filepath;

    emit newGeoDataDocumentAdded( m_document );
}

void PlacemarkLoader::importKmlFromData()
{
    GeoDataParser parser( GeoData_KML );

    QByteArray ba( m_contents.toUtf8() );
    QBuffer buffer( &ba );
    buffer.open( QIODevice::ReadOnly );

    if ( !parser.read( &buffer ) ) {
        qWarning( "Could not parse buffer!" );
        return;
    }
    GeoDocument* document = parser.releaseDocument();
    Q_ASSERT( document );

    m_document = static_cast<GeoDataDocument*>( document );
    m_document->setFileName( m_filepath );

    m_container = new PlacemarkContainer( m_document->placemarks(), 
                                          m_filepath );

    buffer.close();

    qDebug() << "newGeoDataDocumentAdded" << m_filepath;
    
    emit newGeoDataDocumentAdded( m_document );
}

void PlacemarkLoader::saveFile( const QString& filename )
{
    if ( !QDir( MarbleDirs::localPath() + "/placemarks/" ).exists() )
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

    PlacemarkContainer::const_iterator it = m_container->constBegin();
    PlacemarkContainer::const_iterator const end = m_container->constEnd();
    for (; it != end; ++it )
    {
        out << (*it).name();
        (it)->coordinate( lon, lat, alt );

        // Use double to provide a single cache file format across architectures
        out << (double)(lon) << (double)(lat) << (double)(alt);
        out << QString( (*it).role() );
        out << QString( (*it).description() );
        out << QString( (*it).countryCode() );
        out << (double)(*it).area();
        out << (qint64)(*it).population();
    }
}

bool PlacemarkLoader::finalize()
{
    return m_finalize;
}

bool PlacemarkLoader::loadFile( const QString& filename )
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
    m_document = new GeoDataDocument();

    m_document->setFileName( m_filepath );

    in.setVersion( QDataStream::Qt_4_2 );

    // Read the data itself
    // Use double to provide a single cache file format across architectures
    double   lon;
    double   lat;
    double   alt;
    double   area;

    QString  tmpstr;
    qint64   tmpint64;

    while ( !in.atEnd() ) {
        GeoDataPlacemark mark;
        in >> tmpstr;
        mark.setName( tmpstr );
        in >> lon >> lat >> alt;
        mark.setCoordinate( (qreal)(lon), (qreal)(lat), (qreal)(alt) );
        in >> tmpstr;
        mark.setRole( tmpstr.at(0) );
        in >> tmpstr;
        mark.setDescription( tmpstr );
        in >> tmpstr;
        mark.setCountryCode( tmpstr );
        in >> area;
        mark.setArea( (qreal)(area) );
        in >> tmpint64;
        mark.setPopulation( tmpint64 );

        m_container->append( mark );
        m_document->append( mark );
    }

    m_document->setVisible( false );
    qDebug() << "newGeoDataDocumentAdded" << m_filepath;
    emit newGeoDataDocumentAdded( m_document );
    return true;
}

#include "PlacemarkLoader.moc"
} // namespace Marble
