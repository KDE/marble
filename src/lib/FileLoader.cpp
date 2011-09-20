//
// This file is part of the Marble Virtual Globe.
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
#include "GeoDataData.h"
#include "GeoDataExtendedData.h"
#include "GeoDataStyleMap.h"
#include "GeoDataTypes.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "MarbleRunnerManager.h"

namespace Marble
{

class FileLoaderPrivate
{
public:
    FileLoaderPrivate( FileLoader* parent, MarbleModel *model,
                       const QString& file, DocumentRole role )
        : q( parent),
          m_runner( new MarbleRunnerManager( model->pluginManager(), q ) ),
          m_filepath ( file ),
          m_documentRole ( role ),
          m_document( 0 )
    {
        m_runner->setModel( model );
    };

    FileLoaderPrivate( FileLoader* parent, MarbleModel *model,
                       const QString& contents, const QString& file, DocumentRole role )
        : q( parent ),
          m_runner( new MarbleRunnerManager( model->pluginManager(), q ) ),
          m_filepath ( file ),
          m_contents ( contents ),
          m_documentRole ( role ),
          m_document( 0 )
    {
        m_runner->setModel( model );
    };

    ~FileLoaderPrivate()
    {
        delete m_runner;
    }

    void importKml( const QString& filename );
    void importKmlFromData();

    void saveFile(const QString& filename );
    void savePlacemarks(QDataStream &out, const GeoDataContainer *container);

    void createFilterProperties( GeoDataContainer *container );
    int cityPopIdx( qint64 population ) const;
    int spacePopIdx( qint64 population ) const;
    int areaPopIdx( qreal area ) const;

    void documentParsed( GeoDataDocument *);

    FileLoader *q;
    MarbleRunnerManager *m_runner;
    QString m_filepath;
    QString m_contents;
    DocumentRole m_documentRole;
    GeoDataDocument *m_document;

};

FileLoader::FileLoader( QObject* parent, MarbleModel *model,
                       const QString& file, DocumentRole role = UnknownDocument )
    : QThread( parent ),
      d( new FileLoaderPrivate( this, model, file, role ) )
{
}

FileLoader::FileLoader( QObject* parent, MarbleModel *model,
                        const QString& contents, const QString& file, DocumentRole role = UnknownDocument)
    : QThread( parent ),
      d( new FileLoaderPrivate( this, model, contents, file, role ) )
{
}

FileLoader::~FileLoader()
{
    delete d;
}

QString FileLoader::path() const
{
    return d->m_filepath;
}

GeoDataDocument* FileLoader::document()
{
    return d->m_document;
}

void FileLoader::run()
{
    if ( d->m_contents.isEmpty() ) {
        QString defaultCacheName;
        QString defaultSourceName;

        mDebug() << "starting parser for" << d->m_filepath;

        QFileInfo fileinfo( d->m_filepath );
        QString path = fileinfo.path();
        if ( path == "." ) path.clear();
        QString name = fileinfo.completeBaseName();
        QString suffix = fileinfo.suffix();

        // determine source, cache names
        if ( fileinfo.isAbsolute() ) {
            // We got an _absolute_ path now: e.g. "/patrick.kml"
            // defaultCacheName = path + '/' + name + ".cache";
            defaultSourceName   = path + '/' + name + '.' + suffix;
        }
        else {
            if ( d->m_filepath.contains( '/' ) ) {
                // _relative_ path: "maps/mars/viking/patrick.kml"
                // defaultCacheName = MarbleDirs::path( path + '/' + name + ".cache" );
                defaultSourceName   = MarbleDirs::path( path + '/' + name + '.' + suffix );
            }
            else {
                // _standard_ shared placemarks: "placemarks/patrick.kml"
                defaultCacheName = MarbleDirs::path( "placemarks/" + path + name + ".cache" );
		if (defaultCacheName.isEmpty()) {
			defaultCacheName = MarbleDirs::localPath() + "/placemarks/" + path + name + ".cache";
		}
		defaultSourceName   = MarbleDirs::path( "placemarks/" + path + name + '.' + suffix );
            }
        }

        // if cache file more recent that source file, load cache file
        if ( QFile::exists( defaultCacheName ) ) {
            mDebug() << "Loading Cache File:" + defaultCacheName;

            QDateTime sourceLastModified;
            QDateTime cacheLastModified;

            if ( QFile::exists( defaultSourceName ) ) {
                sourceLastModified = QFileInfo( defaultSourceName ).lastModified();
            }
            cacheLastModified  = QFileInfo( defaultCacheName ).lastModified();

            if ( sourceLastModified < cacheLastModified ) {
                connect( d->m_runner, SIGNAL( parsingFinished(GeoDataDocument*) ),
                        this, SLOT( documentParsed(GeoDataDocument*) ) );
                d->m_runner->parseFile( defaultCacheName, d->m_documentRole );
            }
        }
        // we load source file, multiple cases
        else {
            mDebug() << "No recent Default Placemark Cache File available!";
            if ( QFile::exists( defaultSourceName ) ) {
                if( suffix.compare( "kml", Qt::CaseInsensitive ) == 0 ) {
                    // Read the KML file.
                    d->importKml( defaultSourceName );

                    if (!defaultCacheName.isEmpty() ) {
                        d->saveFile(defaultCacheName);
                    }
                    emit loaderFinished( this );
                }
                // use runners: pnt, gpx, osm
                else {
                    connect( d->m_runner, SIGNAL( parsingFinished(GeoDataDocument*) ),
                              this, SLOT( documentParsed(GeoDataDocument*) ) );
                    d->m_runner->parseFile( d->m_filepath, d->m_documentRole );
                }
            }
            else {
                mDebug() << "No Default Placemark Source File for " << name;
            }
        }
    // content is not empty, we load from data
    } else {
        // Read the KML Data
        d->importKmlFromData();
        emit loaderFinished( this );
    }

}

const quint32 MarbleMagicNumber = 0x31415926;

void FileLoaderPrivate::importKml( const QString& filename )
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
        qWarning( "Could not import kml file!" );
        return;
    }
    GeoDocument* document = parser.releaseDocument();
    Q_ASSERT( document );

    m_document = static_cast<GeoDataDocument*>( document );
    m_document->setDocumentRole( m_documentRole );
    m_document->setFileName( m_filepath );
    file.close();
    createFilterProperties( m_document );
    mDebug() << "newGeoDataDocumentAdded" << m_filepath;

    emit q->newGeoDataDocumentAdded( m_document );
}

void FileLoaderPrivate::importKmlFromData()
{
    GeoDataParser parser( GeoData_KML );

    QByteArray ba( m_contents.toUtf8() );
    QBuffer buffer( &ba );
    buffer.open( QIODevice::ReadOnly );

    if ( !parser.read( &buffer ) ) {
        qWarning( "Could not import kml buffer!" );
        return;
    }
    GeoDocument* document = parser.releaseDocument();
    Q_ASSERT( document );

    m_document = static_cast<GeoDataDocument*>( document );
    m_document->setDocumentRole( m_documentRole );
    m_document->setFileName( m_filepath );
    createFilterProperties( m_document );
    buffer.close();

    mDebug() << "newGeoDataDocumentAdded" << m_filepath;

    emit q->newGeoDataDocumentAdded( m_document );
}

void FileLoaderPrivate::saveFile( const QString& filename )
{

    if ( !QDir( MarbleDirs::localPath() + "/placemarks/" ).exists() )
        ( QDir::root() ).mkpath( MarbleDirs::localPath() + "/placemarks/" );
   
    mDebug() << "Creating cache at " << filename ;

    QFile file( filename );
    file.open( QIODevice::WriteOnly );
    QDataStream out( &file );

    // Write a header with a "magic number" and a version
    // out << (quint32)0xA0B0C0D0;
    out << (quint32)MarbleMagicNumber;
    out << (qint32)015;

    out.setVersion( QDataStream::Qt_4_2 );

    savePlacemarks(out, m_document);
}

void FileLoaderPrivate::savePlacemarks(QDataStream &out, const GeoDataContainer *container)
{

    qreal lon;
    qreal lat;
    qreal alt;

    const QVector<GeoDataPlacemark*> placemarks = container->placemarkList();
    QVector<GeoDataPlacemark*>::const_iterator it = placemarks.constBegin();
    QVector<GeoDataPlacemark*>::const_iterator const end = placemarks.constEnd();
    for (; it != end; ++it ) {
        out << (*it)->name();
        (*it)->coordinate( lon, lat, alt );

        // Use double to provide a single cache file format across architectures
        out << (double)(lon) << (double)(lat) << (double)(alt);
        out << QString( (*it)->role() );
        out << QString( (*it)->description() );
        out << QString( (*it)->countryCode() );
        out << QString( (*it)->state() );
        out << (double) (*it)->area();
        out << (qint64) (*it)->population();
        out << ( qint16 ) ( (*it)->extendedData().value("gmt").value().toInt() );
        out << ( qint8 ) ( (*it)->extendedData().value("dst").value().toInt() );
    }

    const QVector<GeoDataFolder*> folders = container->folderList();
    QVector<GeoDataFolder*>::const_iterator cont = folders.constBegin();
    QVector<GeoDataFolder*>::const_iterator endcont = folders.constEnd();
    for (; cont != endcont; ++cont ) {
            savePlacemarks(out, *cont);
    }
}

void FileLoaderPrivate::documentParsed( GeoDataDocument* doc )
{
    m_document = doc;
    doc->setFileName( m_filepath );
    createFilterProperties( doc );
    emit q->newGeoDataDocumentAdded( doc );
    emit q->loaderFinished( q );
}

void FileLoaderPrivate::createFilterProperties( GeoDataContainer *container )
{
    QVector<GeoDataFeature*>::Iterator i = container->begin();
    QVector<GeoDataFeature*>::Iterator const end = container->end();
    for (; i != end; ++i ) {
        if ( (*i)->nodeType() == GeoDataTypes::GeoDataFolderType
             || (*i)->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
            GeoDataContainer *child = static_cast<GeoDataContainer*>( *i );
            createFilterProperties( child );
        } else {
            GeoDataPlacemark* placemark = static_cast<GeoDataPlacemark*>( *i );

            bool hasPopularity = false;

            // Mountain (H), Volcano (V), Shipwreck (W)
            if ( placemark->role() == "H" || placemark->role() == "V" || placemark->role() == "W" )
            {
                qreal altitude = placemark->coordinate().altitude();
                if ( altitude != 0.0 )
                {
                    hasPopularity = true;
                    placemark->setPopularity( (qint64)(altitude * 1000.0) );
                    placemark->setPopularityIndex( cityPopIdx( qAbs( (qint64)(altitude * 1000.0) ) ) );
                }
            }
            // Continent (K), Ocean (O), Nation (S)
            else if ( placemark->role() == "K" || placemark->role() == "O" || placemark->role() == "S" )
            {
                qreal area = placemark->area();
                if ( area >= 0.0 )
                {
                    hasPopularity = true;
                    //                mDebug() << placemark->name() << " " << (qint64)(area);
                    placemark->setPopularity( (qint64)(area * 100) );
                    placemark->setPopularityIndex( areaPopIdx( area ) );
                }
            }
            // Pole (P)
            else if ( placemark->role() == "P" )
            {
                placemark->setPopularity( 1000000000 );
                placemark->setPopularityIndex( 18 );
            }
            // Magnetic Pole (M)
            else if ( placemark->role() == "M" )
            {
                placemark->setPopularity( 10000000 );
                placemark->setPopularityIndex( 13 );
            }
            // MannedLandingSite (h)
            else if ( placemark->role() == "h" )
            {
                placemark->setPopularity( 1000000000 );
                placemark->setPopularityIndex( 18 );
            }
            // RoboticRover (r)
            else if ( placemark->role() == "r" )
            {
                placemark->setPopularity( 10000000 );
                placemark->setPopularityIndex( 16 );
            }
            // UnmannedSoftLandingSite (u)
            else if ( placemark->role() == "u" )
            {
                placemark->setPopularity( 1000000 );
                placemark->setPopularityIndex( 14 );
            }
            // UnmannedSoftLandingSite (i)
            else if ( placemark->role() == "i" )
            {
                placemark->setPopularity( 1000000 );
                placemark->setPopularityIndex( 14 );
            }
            // Space Terrain: Craters, Maria, Montes, Valleys, etc.
            else if (    placemark->role() == "m" || placemark->role() == "v"
                         || placemark->role() == "o" || placemark->role() == "c"
                         || placemark->role() == "a" )
            {
                qint64 diameter = placemark->population();
                if ( diameter >= 0 )
                {
                    hasPopularity = true;
                    placemark->setPopularity( diameter );
                    if ( placemark->role() == "c" ) {
                        placemark->setPopularityIndex( spacePopIdx( diameter ) );
                        if ( placemark->name() == "Tycho" || placemark->name() == "Copernicus" ) {
                            placemark->setPopularityIndex( 17 );
                        }
                    }
                    else {
                        placemark->setPopularityIndex( spacePopIdx( diameter ) );
                    }

                    if ( placemark->role() == "a" && diameter == 0 ) {
                        placemark->setPopularity( 1000000000 );
                        placemark->setPopularityIndex( 18 );
                    }
                }
            }
            else
            {
                qint64 population = placemark->population();
                if ( population >= 0 )
                {
                    hasPopularity = true;
                    placemark->setPopularity( population );
                    placemark->setPopularityIndex( cityPopIdx( population ) );
                }
            }

            //  Then we set the visual category:

            if ( placemark->role() == "H" )      placemark->setVisualCategory( GeoDataPlacemark::Mountain );
            else if ( placemark->role() == "V" ) placemark->setVisualCategory( GeoDataPlacemark::Volcano );

            else if ( placemark->role() == "m" ) placemark->setVisualCategory( GeoDataPlacemark::Mons );
            else if ( placemark->role() == "v" ) placemark->setVisualCategory( GeoDataPlacemark::Valley );
            else if ( placemark->role() == "o" ) placemark->setVisualCategory( GeoDataPlacemark::OtherTerrain );
            else if ( placemark->role() == "c" ) placemark->setVisualCategory( GeoDataPlacemark::Crater );
            else if ( placemark->role() == "a" ) placemark->setVisualCategory( GeoDataPlacemark::Mare );

            else if ( placemark->role() == "P" ) placemark->setVisualCategory( GeoDataPlacemark::GeographicPole );
            else if ( placemark->role() == "M" ) placemark->setVisualCategory( GeoDataPlacemark::MagneticPole );
            else if ( placemark->role() == "W" ) placemark->setVisualCategory( GeoDataPlacemark::ShipWreck );
            else if ( placemark->role() == "F" ) placemark->setVisualCategory( GeoDataPlacemark::AirPort );
            else if ( placemark->role() == "A" ) placemark->setVisualCategory( GeoDataPlacemark::Observatory );
            else if ( placemark->role() == "K" ) placemark->setVisualCategory( GeoDataPlacemark::Continent );
            else if ( placemark->role() == "O" ) placemark->setVisualCategory( GeoDataPlacemark::Ocean );
            else if ( placemark->role() == "S" ) placemark->setVisualCategory( GeoDataPlacemark::Nation );
            else
                if (  placemark->role()=="PPL"
                   || placemark->role()=="PPLF"
                   || placemark->role()=="PPLG"
                   || placemark->role()=="PPLL"
                   || placemark->role()=="PPLQ"
                   || placemark->role()=="PPLR"
                   || placemark->role()=="PPLS"
                   || placemark->role()=="PPLW" ) placemark->setVisualCategory(
                        ( ( GeoDataPlacemark::GeoDataVisualCategory )( (int)( GeoDataPlacemark::SmallCity )
                                                                       + ( placemark->popularityIndex() -1 ) / 4 * 4 ) ) );
            else if ( placemark->role() == "PPLA" ) placemark->setVisualCategory(
                    ( ( GeoDataPlacemark::GeoDataVisualCategory )( (int)( GeoDataPlacemark::SmallStateCapital )
                                                                   + ( placemark->popularityIndex() -1 ) / 4 * 4 ) ) );
            else if ( placemark->role()=="PPLC" ) placemark->setVisualCategory(
                    ( ( GeoDataPlacemark::GeoDataVisualCategory )( (int)( GeoDataPlacemark::SmallNationCapital )
                                                                   + ( placemark->popularityIndex() -1 ) / 4 * 4 ) ) );
            else if ( placemark->role()=="PPLA2" || placemark->role()=="PPLA3" ) placemark->setVisualCategory(
                    ( ( GeoDataPlacemark::GeoDataVisualCategory )( (int)( GeoDataPlacemark::SmallCountyCapital )
                                                                   + ( placemark->popularityIndex() -1 ) / 4 * 4 ) ) );
            else if ( placemark->role()==" " && !hasPopularity && placemark->visualCategory() == GeoDataPlacemark::Unknown ) {
                placemark->setVisualCategory( GeoDataPlacemark::Unknown ); // default location
                placemark->setPopularityIndex(0);
            }
            else if ( placemark->role() == "h" ) placemark->setVisualCategory( GeoDataPlacemark::MannedLandingSite );
            else if ( placemark->role() == "r" ) placemark->setVisualCategory( GeoDataPlacemark::RoboticRover );
            else if ( placemark->role() == "u" ) placemark->setVisualCategory( GeoDataPlacemark::UnmannedSoftLandingSite );
            else if ( placemark->role() == "i" ) placemark->setVisualCategory( GeoDataPlacemark::UnmannedHardLandingSite );

            if ( placemark->role() == "W" && placemark->popularityIndex() > 12 )
                placemark->setPopularityIndex( 12 );
            if ( placemark->role() == "O" )
                placemark->setPopularityIndex( 16 );
            if ( placemark->role() == "K" )
                placemark->setPopularityIndex( 19 );
            if ( !placemark->isVisible() ) {
                placemark->setPopularityIndex( -1 );
            }
            // Workaround: Emulate missing "setVisible" serialization by allowing for population
            // values smaller than -1 which are considered invisible.
            if ( placemark->population() < -1 ) {
                placemark->setPopularityIndex( -1 );
            }
        }
    }
}

int FileLoaderPrivate::cityPopIdx( qint64 population ) const
{
    int popidx = 15;

    if ( population < 2500 )        popidx=1;
    else if ( population < 5000)    popidx=2;
    else if ( population < 7500)    popidx=3;
    else if ( population < 10000)   popidx=4;
    else if ( population < 25000)   popidx=5;
    else if ( population < 50000)   popidx=6;
    else if ( population < 75000)   popidx=7;
    else if ( population < 100000)  popidx=8;
    else if ( population < 250000)  popidx=9;
    else if ( population < 500000)  popidx=10;
    else if ( population < 750000)  popidx=11;
    else if ( population < 1000000) popidx=12;
    else if ( population < 2500000) popidx=13;
    else if ( population < 5000000) popidx=14;

    return popidx;
}

int FileLoaderPrivate::spacePopIdx( qint64 population ) const
{
    int popidx = 18;

    if ( population < 1000 )        popidx=1;
    else if ( population < 2000)    popidx=2;
    else if ( population < 4000)    popidx=3;
    else if ( population < 6000)    popidx=4;
    else if ( population < 8000)    popidx=5;
    else if ( population < 10000)   popidx=6;
    else if ( population < 20000)   popidx=7;

    else if ( population < 40000  )  popidx=8;
    else if ( population < 60000)    popidx=9;
    else if ( population < 80000  )  popidx=10;
    else if ( population < 100000)   popidx=11;
    else if ( population < 200000 )  popidx=13;
    else if ( population < 400000 )  popidx=15;
    else if ( population < 600000 )  popidx=17;

    return popidx;
}

int FileLoaderPrivate::areaPopIdx( qreal area ) const
{
    int popidx = 17;
    if      ( area <  200000  )      popidx=11;
    else if ( area <  400000  )      popidx=12;
    else if ( area < 1000000  )      popidx=13;
    else if ( area < 2500000  )      popidx=14;
    else if ( area < 5000000  )      popidx=15;
    else if ( area < 10000000 )      popidx=16;

    return popidx;
}



#include "FileLoader.moc"
} // namespace Marble
