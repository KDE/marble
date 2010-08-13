//
// This file is part of the Marble Desktop Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2006-2007 Torsten Rahn <tackat@kde.org>
// Copyright 2007      Inge Wallin  <ingwa@kde.org>
//

#include "PlacemarkManager.h"

#include <QtCore/QBuffer>
#include <QtCore/QByteArray>
#include <QtCore/QDataStream>
#include <QtCore/QDateTime>
#include <QtCore/QDir>
#include <QtCore/QFileInfo>

#include "MarbleDebug.h"
#include "FileManager.h"
#include "KmlFileViewItem.h"
#include "MarbleDirs.h"
#include "MarblePlacemarkModel.h"
#include "MarbleDataFacade.h"

#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataParser.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyleMap.h"


using namespace Marble;

namespace Marble
{
class PlacemarkManagerPrivate
{
    public:
        PlacemarkManagerPrivate( )
        : m_datafacade( 0 )
        {
        }

        MarbleDataFacade* m_datafacade;
        FileManager *m_fileManager;
        QVector<GeoDataPlacemark*> m_placemarkContainer;
        QVector<int> m_sizeForDocument;
};
}

PlacemarkManager::PlacemarkManager( QObject *parent )
    : QObject( parent )
    , d( new PlacemarkManagerPrivate() )
{
}


PlacemarkManager::~PlacemarkManager()
{
    delete d;
}

MarblePlacemarkModel* PlacemarkManager::model() const
{
    return d->m_datafacade->placemarkModel();
}

void PlacemarkManager::setDataFacade( MarbleDataFacade *facade )
{
    d->m_datafacade = facade;
    d->m_datafacade->placemarkModel()->setPlacemarkContainer(&d->m_placemarkContainer);
}

void PlacemarkManager::setFileManager( FileManager *fileManager )
{
    disconnect( this, SLOT(addGeoDataDocument(int)) );
    disconnect( this, SLOT(removeGeoDataDocument(int)) );
    d->m_fileManager = fileManager;
    connect( d->m_fileManager, SIGNAL( fileAdded(int)),
             this,          SLOT(addGeoDataDocument(int)) );
    connect( d->m_fileManager, SIGNAL( fileRemoved(int)),
             this,          SLOT(removeGeoDataDocument(int)) );
}

void PlacemarkManager::addGeoDataDocument( int index )
{
    KmlFileViewItem *file = d->m_fileManager->at(index);
    if (file)
    {
        const GeoDataDocument &document = *file->document();
        QVector<GeoDataPlacemark*> result = recurseContainer(file->document());
        if (!result.isEmpty())
        {
            createFilterProperties( result );
            setupStyle( file->document(), result );
            int start = d->m_placemarkContainer.size();
            d->m_placemarkContainer << result;
            d->m_sizeForDocument.resize(index+1);
            d->m_sizeForDocument[index] = result.size();
            mDebug() << "PlacemarkManager::addGeoDataDocument:"
                    << document.fileName() << " size " << result.size();
            d->m_datafacade->placemarkModel()->addPlacemarks( start, result.size() );
        }

    }
}

void PlacemarkManager::removeGeoDataDocument( int index )
{
    KmlFileViewItem *file = d->m_fileManager->at(index);
    if (file)
    {
        const GeoDataDocument &document = *file->document();
        int start = 0;
        for ( int i = 0; i < index; ++i )
        {
            start += d->m_sizeForDocument[i];
        }
        int size = d->m_sizeForDocument[index];
        d->m_placemarkContainer.remove(start, size);
        if (d->m_sizeForDocument.size() > index)
            d->m_sizeForDocument.remove(index);
        mDebug() << "PlacemarkManager::removeGeoDataDocument:"
                << document.fileName() << " size " << size;
        d->m_datafacade->placemarkModel()->removePlacemarks(
                document.fileName(), start, size );
    }
}

QVector<GeoDataPlacemark*> PlacemarkManager::recurseContainer(GeoDataContainer *container)
{
    QVector<GeoDataPlacemark*> results;

    const QVector<GeoDataFeature*> features = container->featureList();
    QVector<GeoDataFeature*>::const_iterator it = features.constBegin();
    QVector<GeoDataFeature*>::const_iterator end = features.constEnd();

    results += container->placemarkList();
    for (; it != end; ++it) {
        GeoDataFolder *folder = dynamic_cast<GeoDataFolder*>(*it);
        if ( folder ) {
            results += recurseContainer( folder );
        }
    }
    return results;
}

void PlacemarkManager::setupStyle( GeoDataDocument *doc, QVector<GeoDataPlacemark*> &container )
{
    // remove the hashes in front of the styles.
    QVector<GeoDataPlacemark*>::Iterator itr = container.begin();
    QVector<GeoDataPlacemark*>::Iterator end = container.end();
    for ( ; itr != end; ++itr ) {
        GeoDataPlacemark *placemark = *itr;
        QString styleUrl = placemark->styleUrl().remove('#');
        if ( ! styleUrl.isEmpty() )
        {
            const GeoDataStyleMap& styleMap = doc->styleMap( styleUrl );
            /// hard coded to use only the "normal" style
            if( !styleMap.value( QString( "normal" ) ).isEmpty() ) {
                styleUrl = styleMap.value( QString( "normal" ) );
            }
            styleUrl.remove( '#' );
            placemark->setStyle( &doc->style( styleUrl ) );
        }
    }
}

void PlacemarkManager::createFilterProperties( QVector<GeoDataPlacemark*> &container )
{

    QVector<GeoDataPlacemark*>::Iterator i = container.begin();
    QVector<GeoDataPlacemark*>::Iterator const end = container.end();
    for (; i != end; ++i ) {
        GeoDataPlacemark& placemark = **i;

        bool hasPopularity = false;

        // Mountain (H), Volcano (V), Shipwreck (W)
        if ( placemark.role() == "H" || placemark.role() == "V" || placemark.role() == "W" )
        {
            qreal altitude = placemark.coordinate().altitude();
            if ( altitude != 0.0 )
            {
                hasPopularity = true;
                placemark.setPopularity( (qint64)(altitude * 1000.0) );
                placemark.setPopularityIndex( cityPopIdx( qAbs( (qint64)(altitude * 1000.0) ) ) );
            }
        }
        // Continent (K), Ocean (O), Nation (S)
        else if ( placemark.role() == "K" || placemark.role() == "O" || placemark.role() == "S" )
        {
            qreal area = placemark.area();
            if ( area >= 0.0 )
            {
                hasPopularity = true;
//                mDebug() << placemark->name() << " " << (qint64)(area);
                placemark.setPopularity( (qint64)(area * 100) );
                placemark.setPopularityIndex( areaPopIdx( area ) );
            }
        }
        // Pole (P)
        else if ( placemark.role() == "P" )
        {
            placemark.setPopularity( 1000000000 );
            placemark.setPopularityIndex( 18 );
        }
        // Magnetic Pole (M)
        else if ( placemark.role() == "M" )
        {
            placemark.setPopularity( 10000000 );
            placemark.setPopularityIndex( 13 );
        }
        // MannedLandingSite (h)
        else if ( placemark.role() == "h" )
        {
            placemark.setPopularity( 1000000000 );
            placemark.setPopularityIndex( 18 );
        }
        // RoboticRover (r)
        else if ( placemark.role() == "r" )
        {
            placemark.setPopularity( 10000000 );
            placemark.setPopularityIndex( 16 );
        }
        // UnmannedSoftLandingSite (u)
        else if ( placemark.role() == "u" )
        {
            placemark.setPopularity( 1000000 );
            placemark.setPopularityIndex( 14 );
        }
        // UnmannedSoftLandingSite (i)
        else if ( placemark.role() == "i" )
        {
            placemark.setPopularity( 1000000 );
            placemark.setPopularityIndex( 14 );
        }
        // Space Terrain: Craters, Maria, Montes, Valleys, etc.
        else if (    placemark.role() == "m" || placemark.role() == "v"
                  || placemark.role() == "o" || placemark.role() == "c"
                  || placemark.role() == "a" )
        {
            qint64 diameter = placemark.population();
            if ( diameter >= 0 )
            {
                hasPopularity = true;
                placemark.setPopularity( diameter );
                if ( placemark.role() == "c" ) {
                    placemark.setPopularityIndex( spacePopIdx( diameter ) );
                    if ( placemark.name() == "Tycho" || placemark.name() == "Copernicus" ) {
                        placemark.setPopularityIndex( 17 );
                    }
                }
                else {
                    placemark.setPopularityIndex( spacePopIdx( diameter ) );
                }

                if ( placemark.role() == "a" && diameter == 0 ) {
                    placemark.setPopularity( 1000000000 );
                    placemark.setPopularityIndex( 18 );
                }
            }
        }
        else
        {
            qint64 population = placemark.population();
            if ( population >= 0 )
            {
                hasPopularity = true;
                placemark.setPopularity( population );
                placemark.setPopularityIndex( cityPopIdx( population ) );
            }
        }

//  Then we set the visual category:

        if ( placemark.role() == "H" )      placemark.setVisualCategory( GeoDataPlacemark::Mountain );
        else if ( placemark.role() == "V" ) placemark.setVisualCategory( GeoDataPlacemark::Volcano );

        else if ( placemark.role() == "m" ) placemark.setVisualCategory( GeoDataPlacemark::Mons );
        else if ( placemark.role() == "v" ) placemark.setVisualCategory( GeoDataPlacemark::Valley );
        else if ( placemark.role() == "o" ) placemark.setVisualCategory( GeoDataPlacemark::OtherTerrain );
        else if ( placemark.role() == "c" ) placemark.setVisualCategory( GeoDataPlacemark::Crater );
        else if ( placemark.role() == "a" ) placemark.setVisualCategory( GeoDataPlacemark::Mare );

        else if ( placemark.role() == "P" ) placemark.setVisualCategory( GeoDataPlacemark::GeographicPole );
        else if ( placemark.role() == "M" ) placemark.setVisualCategory( GeoDataPlacemark::MagneticPole );
        else if ( placemark.role() == "W" ) placemark.setVisualCategory( GeoDataPlacemark::ShipWreck );
        else if ( placemark.role() == "F" ) placemark.setVisualCategory( GeoDataPlacemark::AirPort );
        else if ( placemark.role() == "A" ) placemark.setVisualCategory( GeoDataPlacemark::Observatory );
        else if ( placemark.role() == "K" ) placemark.setVisualCategory( GeoDataPlacemark::Continent );
        else if ( placemark.role() == "O" ) placemark.setVisualCategory( GeoDataPlacemark::Ocean );
        else if ( placemark.role() == "S" ) placemark.setVisualCategory( GeoDataPlacemark::Nation );
        else
        if ( placemark.role()=="PPL" ) placemark.setVisualCategory(
            ( ( GeoDataPlacemark::GeoDataVisualCategory )( (int)( GeoDataPlacemark::SmallCity )
                + ( placemark.popularityIndex() -1 ) / 4 * 4 ) ) );
        else if ( placemark.role() == "PPLA" ) placemark.setVisualCategory(
            ( ( GeoDataPlacemark::GeoDataVisualCategory )( (int)( GeoDataPlacemark::SmallStateCapital )
                + ( placemark.popularityIndex() -1 ) / 4 * 4 ) ) );
        else if ( placemark.role()=="PPLC" ) placemark.setVisualCategory(
            ( ( GeoDataPlacemark::GeoDataVisualCategory )( (int)( GeoDataPlacemark::SmallNationCapital )
                + ( placemark.popularityIndex() -1 ) / 4 * 4 ) ) );
        else if ( placemark.role()=="PPLA2" || placemark.role()=="PPLA3" ) placemark.setVisualCategory(
            ( ( GeoDataPlacemark::GeoDataVisualCategory )( (int)( GeoDataPlacemark::SmallCountyCapital )
                + ( placemark.popularityIndex() -1 ) / 4 * 4 ) ) );
        else if ( placemark.role()==" " && !hasPopularity && placemark.visualCategory() == GeoDataPlacemark::Unknown ) {
            placemark.setVisualCategory( GeoDataPlacemark::Unknown ); // default location
            placemark.setPopularityIndex(0);
        }
        else if ( placemark.role() == "h" ) placemark.setVisualCategory( GeoDataPlacemark::MannedLandingSite );
        else if ( placemark.role() == "r" ) placemark.setVisualCategory( GeoDataPlacemark::RoboticRover );
        else if ( placemark.role() == "u" ) placemark.setVisualCategory( GeoDataPlacemark::UnmannedSoftLandingSite );
        else if ( placemark.role() == "i" ) placemark.setVisualCategory( GeoDataPlacemark::UnmannedHardLandingSite );

        if ( placemark.role() == "W" && placemark.popularityIndex() > 12 )
            placemark.setPopularityIndex( 12 );
        if ( placemark.role() == "O" )
            placemark.setPopularityIndex( 16 );
        if ( placemark.role() == "K" )
            placemark.setPopularityIndex( 19 );
        if ( !placemark.isVisible() ) {
            placemark.setPopularityIndex( -1 );
        }
        // Workaround: Emulate missing "setVisible" serialization by allowing for population
        // values smaller than -1 which are considered invisible.
        if ( placemark.population() < -1 ) {
            placemark.setPopularityIndex( -1 );
        }
    }

}

int PlacemarkManager::cityPopIdx( qint64 population ) const
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

int PlacemarkManager::spacePopIdx( qint64 population ) const
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

int PlacemarkManager::areaPopIdx( qreal area ) const
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

#include "PlacemarkManager.moc"
