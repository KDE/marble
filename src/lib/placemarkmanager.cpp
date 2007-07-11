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


#include "placemarkmanager.h"

#include <QtCore/QDebug>
#include <QtCore/QDataStream>
#include <QtCore/QDateTime>
#include <QtXml/QXmlInputSource>
#include <QtXml/QXmlSimpleReader>

#include "xmlhandler.h"
#include "kml/KMLDocument.h"


PlaceMarkManager::PlaceMarkManager()
{
    m_placeMarkContainer = new PlaceMarkContainer();

    addPlaceMarkFile( "cityplacemarks" );
    addPlaceMarkFile( "baseplacemarks" );
    addPlaceMarkFile( "elevplacemarks" );
    addPlaceMarkFile( "airportplacemarks" );
}


void PlaceMarkManager::addPlaceMarkFile( const QString& filepath )
{
#ifdef KML_GSOC
    /*
     * Sample code to test parsing of kml document
     */

    if ( QFile::exists( filepath ) ) {
        QFile sourceFile( filepath );

        if ( sourceFile.open( QIODevice::ReadOnly ) ) {
            KMLDocument document;
            document.load( sourceFile );
        }
    }

#else
    QString  defaultcachename;
    QString  defaultsrcname;
    QString  defaulthomecache;

    if ( !filepath.contains( "\\" && !filepath.contains( '/' ) ) ) {
        defaultcachename = KAtlasDirs::path( "placemarks/" + filepath + ".cache" );
        defaultsrcname   = KAtlasDirs::path( "placemarks/" + filepath + ".kml");
        defaulthomecache = KAtlasDirs::localDir() + "/placemarks/" + filepath + ".cache";
    }
    else
        return;

    if ( QFile::exists( defaultcachename ) ) {
        qDebug() << "Loading Default Placemark Cache File:" + defaultcachename;

        bool       cacheoutdated = false;
        QDateTime  sourceLastModified;
        QDateTime  cacheLastModified;

        if ( QFile::exists( defaultsrcname ) ) {
            sourceLastModified = QFileInfo( defaultsrcname ).lastModified();
            cacheLastModified  = QFileInfo( defaultcachename ).lastModified();

            if ( cacheLastModified < sourceLastModified )
                cacheoutdated = true;
        }

        bool  loadok = false;

        if ( cacheoutdated == false )
            loadok = loadFile( defaultcachename, m_placeMarkContainer );

        if ( loadok == true )
            return;
    }

    qDebug( "No recent Default Placemark Cache File available!" );

    if ( QFile::exists( defaultsrcname ) ) {
        PlaceMarkContainer  *importcontainer = new PlaceMarkContainer();

        // Read the KML file.
        importKml( defaultsrcname, importcontainer );

        // Save the contents in the efficient cache format.
        saveFile( defaulthomecache, importcontainer );

        // ...and finally add it to the PlaceMarkContainer
        *m_placeMarkContainer << *importcontainer;
    }
    else {
        qDebug() << "No Default Placemark Source File!";
    }
#endif
}


void PlaceMarkManager::loadKml( const QString& filename )
{
    // This still is buggy and needs a lot of work as does the concept
    // as a whole ...

    // PlaceMarkContainer* tmp = m_placeMarkContainer;
    m_placeMarkContainer -> clear();
    // tmp -> deleteAll();
    // delete tmp;

    importKml( filename, m_placeMarkContainer );
}


void PlaceMarkManager::importKml( const QString& filename,
                                  PlaceMarkContainer* placeMarkContainer )
{

    KAtlasXmlHandler handler( placeMarkContainer );

    QFile file( filename );

    // gzip reader:
#if 0
    QDataStream  dataIn(&file);
    QByteArray   compByteArray;
    dataIn >> compByteArray;
    QByteArray   xmlByteArray = qUncompress( compByteArray );
    QString      xmlString    = QString::fromUtf8( xmlByteArray.data(),
                                                   xmlByteArray.size() );
    QXmlInputSource  source;
    source.setData(xmlString);
#endif

    QXmlInputSource   source( &file );
    QXmlSimpleReader  reader;
    reader.setContentHandler( &handler );
    reader.parse( source );
}


void PlaceMarkManager::saveFile( const QString& filename,
                                 PlaceMarkContainer* placeMarkContainer )
{
    if ( QDir( KAtlasDirs::localDir() + "/placemarks/" ).exists() == false )
        ( QDir::root() ).mkpath( KAtlasDirs::localDir() + "/placemarks/" );

    QFile  file( filename );
    file.open(QIODevice::WriteOnly);
    QDataStream  out(&file);

    // Write a header with a "magic number" and a version
    // out << (quint32)0xA0B0C0D0;
    out << (quint32)0x31415926;
    out << (qint32)005;

    out.setVersion(QDataStream::Qt_4_0);

    double  lng;
    double  lat;

    PlaceMarkContainer::const_iterator  it;

    for ( it = placeMarkContainer->constBegin();
          it != placeMarkContainer->constEnd();
          it++ )
    {
        out << (*it) -> name();
        (*it) -> coordinate(lng, lat);

        out << lng << lat;
        out << QString( (*it) -> role() );
        out << QString( (*it) -> description() );
        out << QString( (*it) -> countryCode() );
        out << (qint32)(*it) -> popidx();
        out << (qint32)(*it) -> symbol();
        out << (qint32)(*it) -> population();
    }
}


bool PlaceMarkManager::loadFile( const QString& filename,
                                 PlaceMarkContainer* placeMarkContainer )
{
    QFile  file( filename );
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);

    // Read and check the header
    quint32  magic;
    in >> magic;
    if (magic != 0x31415926) {
        qDebug( "Bad file format!" );
        return false;
    }

    // Read the version
    qint32  version;
    in >> version;
    if (version < 005) {
        qDebug( "Bad file - too old!" );
        return false;
    }
    /*
      if (version > 002) {
      qDebug( "Bad file - too new!" );
      return;
      }
    */

    in.setVersion(QDataStream::Qt_4_0);

    // Read the data itself
    double   lng;
    double   lat;
    QString  tmpstr;
    qint32   a;

    PlaceMark  *mark;
    while ( !in.atEnd() ) {
        mark = new PlaceMark();

        in >> tmpstr;
        mark -> setName( tmpstr );
        in >> lng >> lat;
        mark -> setCoordinate(lng, lat);
        in >> tmpstr;
        mark -> setRole( tmpstr.at(0) );
        in >> tmpstr;
        mark -> setDescription( tmpstr );
        in >> tmpstr;
        mark -> setCountryCode( tmpstr );
        in >> a;
        mark -> setPopidx( a );
        in >> a;
        mark -> setSymbol( a );
        in >> a;
        mark -> setPopulation( a );

        placeMarkContainer -> append( mark );
    }

    return true;
}
