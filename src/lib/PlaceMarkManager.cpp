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

#include <QtCore/QDebug>
#include <QtCore/QDataStream>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtXml/QXmlInputSource>
#include <QtXml/QXmlSimpleReader>

#include "xmlhandler.h"


PlaceMarkManager::PlaceMarkManager()
{
    m_placeMarkContainer = new PlaceMarkContainer();

    addPlaceMarkFile( "cityplacemarks" );
    addPlaceMarkFile( "baseplacemarks" );
    addPlaceMarkFile( "elevplacemarks" );
//    addPlaceMarkFile( "airportplacemarks" );
}


PlaceMarkManager::~PlaceMarkManager()
{
    m_placeMarkContainer->deleteAll();

#ifdef KML_GSOC
    foreach ( KMLDocument* document, m_documentList ) {
        delete document;
    }
#endif
}


void PlaceMarkManager::addPlaceMarkFile( const QString& filepath )
{
#ifdef KML_GSOC
    /*
     * Simply call loadKml, should use cache in feature
     */
    loadKml( filepath );
#else
    QString  defaultcachename;
    QString  defaultsrcname;
    QString  defaulthomecache;

    if ( !filepath.contains( "\\" && !filepath.contains( '/' ) ) ) {
        defaultcachename = MarbleDirs::path( "placemarks/" + filepath + ".cache" );
        defaultsrcname   = MarbleDirs::path( "placemarks/" + filepath + ".kml");
        defaulthomecache = MarbleDirs::localPath() + "/placemarks/" + filepath + ".cache";
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
#ifdef KML_GSOC
    if ( QFile::exists( filename ) ) {
        QFile sourceFile( filename );

        if ( sourceFile.open( QIODevice::ReadOnly ) ) {
            /*
             * Create KMLDocument and set it's name like input filename
             */
            KMLDocument* document = new KMLDocument();
            document->setName( QFileInfo( sourceFile ).fileName() );
            document->load( sourceFile );

            if ( ! m_documentList.isEmpty() ) {
                const KMLDocument& lastLoadedDocument = *m_documentList.last();
                document->setId( lastLoadedDocument.id() + 1 );
            }

            qDebug("KML document loaded. Name: %s", document->name().toAscii().data());
            m_documentList.append( document );
            sourceFile.close();

            /*
             * Pack document to it's own cache file
             * and update cache index
             */
            cacheDocument( *document );
            updateCacheIndex();
        }
    }

#else
    // This still is buggy and needs a lot of work as does the concept
    // as a whole ...

    // PlaceMarkContainer* tmp = m_placeMarkContainer;
    m_placeMarkContainer -> clear();
    // tmp -> deleteAll();
    // delete tmp;

    importKml( filename, m_placeMarkContainer );
#endif
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
    if ( QDir( MarbleDirs::localPath() + "/placemarks/" ).exists() == false )
        ( QDir::root() ).mkpath( MarbleDirs::localPath() + "/placemarks/" );

    QFile  file( filename );
    file.open(QIODevice::WriteOnly);
    QDataStream  out(&file);

    // Write a header with a "magic number" and a version
    // out << (quint32)0xA0B0C0D0;
    out << (quint32)0x31415926;
    out << (qint32)005;

    out.setVersion(QDataStream::Qt_4_0);

    double  lon;
    double  lat;

    PlaceMarkContainer::const_iterator  it;

    for ( it = placeMarkContainer->constBegin();
          it != placeMarkContainer->constEnd();
          it++ )
    {
        out << (*it) -> name();
        (*it) -> coordinate(lon, lat);

        out << lon << lat;
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
    double   lon;
    double   lat;
    QString  tmpstr;
    qint32   a;

    PlaceMark  *mark;
    while ( !in.atEnd() ) {
        mark = new PlaceMark();

        in >> tmpstr;
        mark -> setName( tmpstr );
        in >> lon >> lat;
        mark -> setCoordinate(lon, lat);
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

#ifdef KML_GSOC
void PlaceMarkManager::updateCacheIndex()
{
    QString cacheIndexFileName = QString( "%1/placemarks/kmldocument-cache.index" ).arg( MarbleDirs::localPath() );

    QFile indexFile( cacheIndexFileName );
    if ( indexFile.open( QIODevice::WriteOnly ) ) {
        QDataStream stream( &indexFile );

        /*
         * TODO: Put some unique header here
         */

        for ( QList <KMLDocument*>::const_iterator iterator = m_documentList.constBegin();
            iterator != m_documentList.constEnd();
            iterator++ )
        {
            const KMLDocument& document = * ( *iterator );

            QString cacheFileName = QString( "%1.%2.cache" ).arg( document.id() ).arg( document.name() );
            stream << cacheFileName;
        }

        indexFile.close();
        qDebug( "Create index cache file: %s", cacheIndexFileName.toAscii().data () );
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
        QDataStream stream ( &cacheFile );
        document.pack( stream );
        cacheFile.close();
        qDebug( "Saved kml document to cache: %s", path.toAscii().data());
    }
    else {
        qDebug( "Unable to cache kml document to: %s", path.toAscii().data());
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
