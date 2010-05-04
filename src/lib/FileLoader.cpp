//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2008      Patrick Spendrin <ps_ml@gmx.de>
//

#include "FileLoader.h"

#include <QtCore/QBuffer>
#include <QtCore/QDataStream>
#include <QtCore/QDateTime>
#include <QtCore/QFile>
#include <QtCore/QThread>

#include "GeoDataParser.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"

namespace Marble
{

FileLoader::FileLoader( QObject* parent, const QString& file )
    : QThread( parent ),
      m_filepath( file ),
      m_contents( QString() ),
      m_document( 0 )
{
}

FileLoader::FileLoader( QObject* parent, const QString& contents, const QString& file )
    : QThread( parent ),
      m_filepath( file ),
      m_contents( contents ),
      m_document( 0 )
{
}

FileLoader::~FileLoader()
{
    delete m_document;
}

QString FileLoader::path() const
{
    return m_filepath;
}

void FileLoader::run()
{
    if ( m_contents.isEmpty() ) {
        QString defaultcachename;
        QString defaultsrcname;

        mDebug() << "starting parser for" << m_filepath;

        QFileInfo fileinfo( m_filepath );
        QString path = fileinfo.path();
        if ( path == "." ) path.clear();
        QString name = fileinfo.completeBaseName();
        QString suffix = fileinfo.suffix();

        if ( fileinfo.isAbsolute() ) {
            // We got an _absolute_ path now: e.g. "/patrick.kml"
            // defaultcachename = path + '/' + name + ".cache";
            defaultsrcname   = path + '/' + name + '.' + suffix;
        }
        else {
            if ( m_filepath.contains( '/' ) ) {
                // _relative_ path: "maps/mars/viking/patrick.kml"
                // defaultcachename = MarbleDirs::path( path + '/' + name + ".cache" );
                defaultsrcname   = MarbleDirs::path( path + '/' + name + '.' + suffix );
            }
            else {
                // _standard_ shared placemarks: "placemarks/patrick.kml"
                defaultcachename = MarbleDirs::path( "placemarks/" + path + name + ".cache" );
                defaultsrcname   = MarbleDirs::path( "placemarks/" + path + name + '.' + suffix );
            }
        }

        if ( QFile::exists( defaultcachename ) ) {
            mDebug() << "Loading Cache File:" + defaultcachename;

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
                    emit newGeoDataDocumentAdded( m_document );
            }
            mDebug() << "Loading ended" << loadok;
            if ( loadok ) {
                mDebug() << "placemarksLoaded";
            }
        }
        else {
            if ( QFile::exists( defaultsrcname ) ) {
                // Read the KML file.
                importKml( defaultsrcname );
                if ( !defaultcachename.isEmpty() ) {
                    saveFile( defaultcachename );
                }
            }
            else {
                mDebug() << "No Default Placemark Source File for " << defaultsrcname;
            }
        }
    } else {
        // Read the KML Data
        importKmlFromData();
    }

    emit loaderFinished( this );
}

const quint32 MarbleMagicNumber = 0x31415926;

void FileLoader::importKml( const QString& filename )
{
    GeoDataParser parser( GeoData_UNKNOWN );

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

    file.close();

    mDebug() << "newGeoDataDocumentAdded" << m_filepath;

    emit newGeoDataDocumentAdded( m_document );
}

void FileLoader::importKmlFromData()
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

    buffer.close();

    mDebug() << "newGeoDataDocumentAdded" << m_filepath;

    emit newGeoDataDocumentAdded( m_document );
}

bool FileLoader::loadFile( const QString &filename )
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

        m_document->append( mark );
    }

    m_document->setVisible( false );
    return true;
}

void FileLoader::saveFile( const QString& filename )
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

    savePlacemarks(out, m_document);
}

void FileLoader::savePlacemarks(QDataStream &out, const GeoDataContainer *container)
{
    qreal lon;
    qreal lat;
    qreal alt;

    const QVector<GeoDataPlacemark> placemarks = container->placemarks();
    QVector<GeoDataPlacemark>::const_iterator it = placemarks.constBegin();
    QVector<GeoDataPlacemark>::const_iterator const end = placemarks.constEnd();
    for (; it != end; ++it ) {
        out << it->name();
        it->coordinate( lon, lat, alt );

        // Use double to provide a single cache file format across architectures
        out << (double)(lon) << (double)(lat) << (double)(alt);
        out << QString( it->role() );
        out << QString( it->description() );
        out << QString( it->countryCode() );
        out << (double) it->area();
        out << (qint64) it->population();
    }

    const QVector<GeoDataFolder> folders = container->folders();
    QVector<GeoDataFolder>::const_iterator cont = folders.constBegin();
    QVector<GeoDataFolder>::const_iterator endcont = folders.constEnd();
    for (; cont != endcont; ++cont ) {
        if (GeoDataFolderId == cont->featureId() ) {
            const GeoDataContainer *subcontainer = & (*cont);
            savePlacemarks(out, subcontainer);
        }
    }
}

#include "FileLoader.moc"
} // namespace Marble
