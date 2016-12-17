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

#include <QBuffer>
#include <QDataStream>
#include <QFile>
#include <QThread>

#include "GeoDataParser.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataData.h"
#include "GeoDataExtendedData.h"
#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataLineStyle.h"
#include "GeoDataPolygon.h"
#include "GeoDataStyle.h"
#include "GeoDataTypes.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "MarbleModel.h"
#include "ParsingRunnerManager.h"

namespace Marble
{

class FileLoaderPrivate
{
public:
    FileLoaderPrivate( FileLoader* parent, const PluginManager *pluginManager, bool recenter,
                       const QString& file, const QString& property, const GeoDataStyle::Ptr &style, DocumentRole role, int renderOrder )
        : q( parent),
          m_runner( pluginManager ),
          m_recenter( recenter ),
          m_filepath ( file ),
          m_property( property ),
          m_style( style ),
          m_documentRole ( role ),
          m_styleMap( new GeoDataStyleMap ),
          m_document( 0 ),
          m_renderOrder( renderOrder )
    {
        if( m_style ) {
            m_styleMap->setId(QStringLiteral("default-map"));
            m_styleMap->insert(QStringLiteral("normal"), QLatin1Char('#') + m_style->id());
        }
    }

    FileLoaderPrivate( FileLoader* parent, const PluginManager *pluginManager,
                       const QString& contents, const QString& file, DocumentRole role )
        : q( parent ),
          m_runner( pluginManager ),
          m_recenter( false ),
          m_filepath ( file ),
          m_contents ( contents ),
          m_documentRole ( role ),
          m_styleMap( 0 ),
          m_document( 0 )
    {
    }

    ~FileLoaderPrivate()
    {
        delete m_styleMap;
    }

    void createFilterProperties( GeoDataContainer *container );
    static int cityPopIdx( qint64 population );
    static int spacePopIdx( qint64 population );
    static int areaPopIdx( qreal area );

    void documentParsed( GeoDataDocument *doc, const QString& error);

    FileLoader *q;
    ParsingRunnerManager m_runner;
    bool m_recenter;
    QString m_filepath;
    QString m_contents;
    QString m_property;
    GeoDataStyle::Ptr m_style;
    DocumentRole m_documentRole;
    GeoDataStyleMap* m_styleMap;
    GeoDataDocument *m_document;
    QString m_error;
    int m_renderOrder;
};

FileLoader::FileLoader( QObject* parent, const PluginManager *pluginManager, bool recenter, const QString& file,
                        const QString& property, const GeoDataStyle::Ptr &style, DocumentRole role, int renderOrder )
    : QThread( parent ),
      d( new FileLoaderPrivate( this, pluginManager, recenter, file, property, style, role, renderOrder ) )
{
}

FileLoader::FileLoader( QObject* parent, const PluginManager *pluginManager,
                        const QString& contents, const QString& file, DocumentRole role )
    : QThread( parent ),
      d( new FileLoaderPrivate( this, pluginManager, contents, file, role ) )
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

QString FileLoader::error() const
{
    return d->m_error;
}

void FileLoader::run()
{
    if ( d->m_contents.isEmpty() ) {
        QString defaultSourceName;

        mDebug() << "starting parser for" << d->m_filepath;

        QFileInfo fileinfo( d->m_filepath );
        QString path = fileinfo.path();
        if (path == QLatin1String(".")) path.clear();
        QString name = fileinfo.completeBaseName();
        QString suffix = fileinfo.suffix();

        // determine source, cache names
        if ( fileinfo.isAbsolute() ) {
            // We got an _absolute_ path now: e.g. "/patrick.kml"
            defaultSourceName = path + QLatin1Char('/') + name + QLatin1Char('.') + suffix;
        }
        else if ( d->m_filepath.contains( '/' ) ) {
            // _relative_ path: "maps/mars/viking/patrick.kml"
            defaultSourceName = MarbleDirs::path(path + QLatin1Char('/') + name + QLatin1Char('.') + suffix);
            if ( !QFile::exists( defaultSourceName ) ) {
                defaultSourceName = MarbleDirs::path(path + QLatin1Char('/') + name + QLatin1String(".cache"));
            }
        }
        else {
            // _standard_ shared placemarks: "placemarks/patrick.kml"
            defaultSourceName = MarbleDirs::path(QLatin1String("placemarks/") + path + name + QLatin1Char('.') + suffix);
            if ( !QFile::exists( defaultSourceName ) ) {
                defaultSourceName = MarbleDirs::path(QLatin1String("placemarks/") + path + name + QLatin1String(".cache"));
            }
        }

        if ( QFile::exists( defaultSourceName ) ) {
            mDebug() << "No recent Default Placemark Cache File available!";

            // use runners: pnt, gpx, osm
            connect( &d->m_runner, SIGNAL(parsingFinished(GeoDataDocument*,QString)),
                    this, SLOT(documentParsed(GeoDataDocument*,QString)) );
            d->m_runner.parseFile( defaultSourceName, d->m_documentRole );
        }
        else {
            mDebug() << "No Default Placemark Source File for " << name;
        }
    // content is not empty, we load from data
    } else {
        // Read the KML Data
        GeoDataParser parser( GeoData_KML );

        QByteArray ba( d->m_contents.toUtf8() );
        QBuffer buffer( &ba );
        buffer.open( QIODevice::ReadOnly );

        if ( !parser.read( &buffer ) ) {
            qWarning( "Could not import kml buffer!" );
            emit loaderFinished( this );
            return;
        }

        GeoDocument* document = parser.releaseDocument();
        Q_ASSERT( document );

        d->m_document = static_cast<GeoDataDocument*>( document );
        d->m_document->setProperty( d->m_property );
        d->m_document->setDocumentRole( d->m_documentRole );
        d->createFilterProperties( d->m_document );
        buffer.close();

        mDebug() << "newGeoDataDocumentAdded" << d->m_filepath;

        emit newGeoDataDocumentAdded( d->m_document );
        emit loaderFinished( this );
    }

}

bool FileLoader::recenter() const
{
    return d->m_recenter;
}

void FileLoaderPrivate::documentParsed( GeoDataDocument* doc, const QString& error )
{
    m_error = error;
    if ( doc ) {
        m_document = doc;
        doc->setProperty( m_property );
        if( m_style ) {
            doc->addStyleMap( *m_styleMap );
            doc->addStyle( m_style );
        }

        if (m_renderOrder != 0) {
            foreach (GeoDataPlacemark* placemark, doc->placemarkList()) {
                if (placemark->geometry() && placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType) {
                    GeoDataPolygon *polygon = static_cast<GeoDataPolygon*>(placemark->geometry());
                    polygon->setRenderOrder(m_renderOrder);
                }
            }
        }

        createFilterProperties( doc );
        emit q->newGeoDataDocumentAdded( m_document );
    }
    emit q->loaderFinished( q );
}

void FileLoaderPrivate::createFilterProperties( GeoDataContainer *container )
{
    const QString styleUrl = QLatin1Char('#') + m_styleMap->id();

    QVector<GeoDataFeature*>::Iterator i = container->begin();
    QVector<GeoDataFeature*>::Iterator const end = container->end();
    for (; i != end; ++i ) {
        if ( (*i)->nodeType() == GeoDataTypes::GeoDataFolderType
             || (*i)->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
            GeoDataContainer *child = static_cast<GeoDataContainer*>( *i );
            createFilterProperties( child );
        } else if (    (*i)->nodeType() == GeoDataTypes::GeoDataTourType
                    || (*i)->nodeType() == GeoDataTypes::GeoDataGroundOverlayType
                    || (*i)->nodeType() == GeoDataTypes::GeoDataPhotoOverlayType
                    || (*i)->nodeType() == GeoDataTypes::GeoDataScreenOverlayType ) {
            /** @todo: How to handle this ? */
        } else if ( (*i)->nodeType() == GeoDataTypes::GeoDataPlacemarkType ) {
            Q_ASSERT( dynamic_cast<GeoDataPlacemark*>( *i ) );

            GeoDataPlacemark* placemark = static_cast<GeoDataPlacemark*>( *i );
            const QString placemarkRole = placemark->role();
            Q_ASSERT( placemark->geometry() );

            bool hasPopularity = false;

            if ( placemark->geometry()->nodeType() != GeoDataTypes::GeoDataTrackType &&
                placemark->geometry()->nodeType() != GeoDataTypes::GeoDataPointType
                 && m_documentRole == MapDocument
                 && m_style ) {
                placemark->setStyleUrl(styleUrl);
            }

            // Mountain (H), Volcano (V), Shipwreck (W)
            if (placemarkRole == QLatin1String("H") ||
                placemarkRole == QLatin1String("V") ||
                placemarkRole == QLatin1String("W"))
            {
                qreal altitude = placemark->coordinate().altitude();
                if ( altitude != 0.0 )
                {
                    hasPopularity = true;
                    placemark->setPopularity( (qint64)(altitude * 1000.0) );
                    placemark->setZoomLevel( cityPopIdx( qAbs( (qint64)(altitude * 1000.0) ) ) );
                }
            }
            // Continent (K), Ocean (O), Nation (S)
            else if (placemarkRole == QLatin1String("K") ||
                     placemarkRole == QLatin1String("O") ||
                     placemarkRole == QLatin1String("S"))
            {
                qreal area = placemark->area();
                if ( area >= 0.0 )
                {
                    hasPopularity = true;
                    //                mDebug() << placemark->name() << " " << (qint64)(area);
                    placemark->setPopularity( (qint64)(area * 100) );
                    placemark->setZoomLevel( areaPopIdx( area ) );
                }
            }
            // Pole (P)
            else if (placemarkRole == QLatin1String("P") )
            {
                placemark->setPopularity( 1000000000 );
                placemark->setZoomLevel( 1 );
            }
            // Magnetic Pole (M)
            else if (placemarkRole == QLatin1String("M"))
            {
                placemark->setPopularity( 10000000 );
                placemark->setZoomLevel( 3 );
            }
            // MannedLandingSite (h)
            else if (placemarkRole == QLatin1String("h"))
            {
                placemark->setPopularity( 1000000000 );
                placemark->setZoomLevel( 1 );
            }
            // RoboticRover (r)
            else if (placemarkRole == QLatin1String("r"))
            {
                placemark->setPopularity( 10000000 );
                placemark->setZoomLevel( 2 );
            }
            // UnmannedSoftLandingSite (u)
            else if (placemarkRole == QLatin1String("u"))
            {
                placemark->setPopularity( 1000000 );
                placemark->setZoomLevel( 3 );
            }
            // UnmannedSoftLandingSite (i)
            else if (placemarkRole == QLatin1String("i"))
            {
                placemark->setPopularity( 1000000 );
                placemark->setZoomLevel( 3 );
            }
            // Space Terrain: Craters, Maria, Montes, Valleys, etc.
            else if (placemarkRole == QLatin1String("m") ||
                     placemarkRole == QLatin1String("v") ||
                     placemarkRole == QLatin1String("o") ||
                     placemarkRole == QLatin1String("c") ||
                     placemarkRole == QLatin1String("a"))
            {
                qint64 diameter = placemark->population();
                if ( diameter >= 0 )
                {
                    hasPopularity = true;
                    placemark->setPopularity( diameter );
                    if (placemarkRole == QLatin1String("c")) {
                        placemark->setZoomLevel( spacePopIdx( diameter ) );
                        if (placemark->name() == QLatin1String("Tycho") ||
                            placemark->name() == QLatin1String("Copernicus")) {
                            placemark->setZoomLevel( 1 );
                        }
                    }
                    else {
                        placemark->setZoomLevel( spacePopIdx( diameter ) );
                    }

                    if (placemarkRole == QLatin1String("a") && diameter == 0) {
                        placemark->setPopularity( 1000000000 );
                        placemark->setZoomLevel( 1 );
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
                    placemark->setZoomLevel( cityPopIdx( population ) );
                }
            }

            //  Then we set the visual category:

            if (placemarkRole == QLatin1String("H"))      placemark->setVisualCategory( GeoDataPlacemark::Mountain );
            else if (placemarkRole == QLatin1String("V")) placemark->setVisualCategory( GeoDataPlacemark::Volcano );

            else if (placemarkRole == QLatin1String("m")) placemark->setVisualCategory( GeoDataPlacemark::Mons );
            else if (placemarkRole == QLatin1String("v")) placemark->setVisualCategory( GeoDataPlacemark::Valley );
            else if (placemarkRole == QLatin1String("o")) placemark->setVisualCategory( GeoDataPlacemark::OtherTerrain );
            else if (placemarkRole == QLatin1String("c")) placemark->setVisualCategory( GeoDataPlacemark::Crater );
            else if (placemarkRole == QLatin1String("a")) placemark->setVisualCategory( GeoDataPlacemark::Mare );

            else if (placemarkRole == QLatin1String("P")) placemark->setVisualCategory( GeoDataPlacemark::GeographicPole );
            else if (placemarkRole == QLatin1String("M")) placemark->setVisualCategory( GeoDataPlacemark::MagneticPole );
            else if (placemarkRole == QLatin1String("W")) placemark->setVisualCategory( GeoDataPlacemark::ShipWreck );
            else if (placemarkRole == QLatin1String("F")) placemark->setVisualCategory( GeoDataPlacemark::AirPort );
            else if (placemarkRole == QLatin1String("A")) placemark->setVisualCategory( GeoDataPlacemark::Observatory );
            else if (placemarkRole == QLatin1String("K")) placemark->setVisualCategory( GeoDataPlacemark::Continent );
            else if (placemarkRole == QLatin1String("O")) placemark->setVisualCategory( GeoDataPlacemark::Ocean );
            else if (placemarkRole == QLatin1String("S")) placemark->setVisualCategory( GeoDataPlacemark::Nation );
            else if (placemarkRole == QLatin1String("PPL") ||
                     placemarkRole == QLatin1String("PPLF") ||
                     placemarkRole == QLatin1String("PPLG") ||
                     placemarkRole == QLatin1String("PPLL") ||
                     placemarkRole == QLatin1String("PPLQ") ||
                     placemarkRole == QLatin1String("PPLR") ||
                     placemarkRole == QLatin1String("PPLS") ||
                     placemarkRole == QLatin1String("PPLW")) {
                switch (placemark->zoomLevel()) {
                case 3:
                case 4:
                    placemark->setVisualCategory(GeoDataPlacemark::LargeCity);
                    break;
                case 5:
                case 6:
                    placemark->setVisualCategory(GeoDataPlacemark::BigCity);
                    break;
                case 7:
                case 8:
                    placemark->setVisualCategory(GeoDataPlacemark::MediumCity);
                    break;
                default:
                    placemark->setVisualCategory(GeoDataPlacemark::SmallCity);
                    break;
                }
            }
            else if (placemarkRole == QLatin1String("PPLA")) {
                switch (placemark->zoomLevel()) {
                case 3:
                case 4:
                    placemark->setVisualCategory(GeoDataPlacemark::LargeStateCapital);
                    break;
                case 5:
                case 6:
                    placemark->setVisualCategory(GeoDataPlacemark::BigStateCapital);
                    break;
                case 7:
                case 8:
                    placemark->setVisualCategory(GeoDataPlacemark::MediumStateCapital);
                    break;
                default:
                    placemark->setVisualCategory(GeoDataPlacemark::SmallStateCapital);
                    break;
                }
            }
            else if (placemarkRole == QLatin1String("PPLC")) {
                switch (placemark->zoomLevel()) {
                case 3:
                case 4:
                    placemark->setVisualCategory(GeoDataPlacemark::LargeNationCapital);
                    break;
                case 5:
                case 6:
                    placemark->setVisualCategory(GeoDataPlacemark::BigNationCapital);
                    break;
                case 7:
                case 8:
                    placemark->setVisualCategory(GeoDataPlacemark::MediumNationCapital);
                    break;
                default:
                    placemark->setVisualCategory(GeoDataPlacemark::SmallNationCapital);
                    break;
                }
            }
            else if (placemarkRole == QLatin1String("PPLA2") ||
                     placemarkRole == QLatin1String("PPLA3") ||
                     placemarkRole == QLatin1String("PPLA4")) {
                switch (placemark->zoomLevel()) {
                case 3:
                case 4:
                    placemark->setVisualCategory(GeoDataPlacemark::LargeCountyCapital);
                    break;
                case 5:
                case 6:
                    placemark->setVisualCategory(GeoDataPlacemark::BigCountyCapital);
                    break;
                case 7:
                case 8:
                    placemark->setVisualCategory(GeoDataPlacemark::MediumCountyCapital);
                    break;
                default:
                    placemark->setVisualCategory(GeoDataPlacemark::SmallCountyCapital);
                    break;
                }
            }
            else if (placemarkRole == QLatin1String(" ") && !hasPopularity && placemark->visualCategory() == GeoDataPlacemark::Unknown) {
                placemark->setVisualCategory( GeoDataPlacemark::Unknown ); // default location
                placemark->setZoomLevel(0);
            }
            else if (placemarkRole == QLatin1String("h")) {
                placemark->setVisualCategory( GeoDataPlacemark::MannedLandingSite );
            }
            else if (placemarkRole == QLatin1String("r")) {
                placemark->setVisualCategory( GeoDataPlacemark::RoboticRover );
            }
            else if (placemarkRole == QLatin1String("u")) {
                placemark->setVisualCategory( GeoDataPlacemark::UnmannedSoftLandingSite );
            }
            else if (placemarkRole == QLatin1String("i")) {
                placemark->setVisualCategory( GeoDataPlacemark::UnmannedHardLandingSite );
            }

            // At last fine-tune zoomlevel:
            if (!placemark->isVisible()) {
                placemark->setZoomLevel( 18 );
            }
            // Workaround: Emulate missing "setVisible" serialization by allowing for population
            // values smaller than -1 which are considered invisible.
            else if (placemark->population() < -1) {
                placemark->setZoomLevel( 18 );
            }
            else if (placemarkRole == QLatin1String("W")) {
                if (placemark->zoomLevel() < 4) {
                    placemark->setZoomLevel( 4 );
                }
            }
            else if (placemarkRole == QLatin1String("O")) {
                placemark->setZoomLevel( 2 );
            }
            else if (placemarkRole == QLatin1String("K")) {
                placemark->setZoomLevel( 0 );
            }
        } else {
            qWarning() << Q_FUNC_INFO << "Unknown feature" << (*i)->nodeType() << ". Skipping.";
        }
    }
}

int FileLoaderPrivate::cityPopIdx( qint64 population )
{
    int popidx = 3;

    if ( population < 2500 )        popidx=10;
    else if ( population < 5000)    popidx=9;
    else if ( population < 25000)   popidx=8;
    else if ( population < 75000)   popidx=7;
    else if ( population < 250000)  popidx=6;
    else if ( population < 750000)  popidx=5;
    else if ( population < 2500000) popidx=4;

    return popidx;
}

int FileLoaderPrivate::spacePopIdx( qint64 population )
{
    int popidx = 1;

    if ( population < 1000 )        popidx=10;
    else if ( population < 2000)    popidx=9;
    else if ( population < 8000)    popidx=8;
    else if ( population < 20000)   popidx=7;
    else if ( population < 60000)    popidx=6;
    else if ( population < 100000)   popidx=5;
    else if ( population < 200000 )  popidx=4;
    else if ( population < 400000 )  popidx=2;
    else if ( population < 600000 )  popidx=1;

    return popidx;
}

int FileLoaderPrivate::areaPopIdx( qreal area )
{
    int popidx = 1;
    if      ( area <  200000  )      popidx=5;
    else if ( area < 1000000  )      popidx=4;
    else if ( area < 2500000  )      popidx=3;
    else if ( area < 5000000  )      popidx=2;

    return popidx;
}



#include "moc_FileLoader.cpp"
} // namespace Marble
