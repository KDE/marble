//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <earthwings@gentoo.org>

#include "LocalOsmSearchRunner.h"

#include "OsmDatabase.h"
#include "MarbleAbstractRunner.h"
#include "MarbleDebug.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"

#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QUrl>

namespace Marble
{

QMap<OsmPlacemark::OsmCategory, GeoDataFeature::GeoDataVisualCategory> LocalOsmSearchRunner::m_categoryMap;

LocalOsmSearchRunner::LocalOsmSearchRunner( OsmDatabase *database, QObject *parent ) :
    MarbleAbstractRunner( parent ),
    m_database( database )
{
    if ( m_categoryMap.isEmpty() ) {
        m_categoryMap[OsmPlacemark::AccomodationCamping] = GeoDataFeature::AccomodationCamping;
        m_categoryMap[OsmPlacemark::AccomodationHostel] = GeoDataFeature::AccomodationHostel;
        m_categoryMap[OsmPlacemark::AccomodationHotel] = GeoDataFeature::AccomodationHotel;
        m_categoryMap[OsmPlacemark::AccomodationMotel] = GeoDataFeature::AccomodationMotel;
        m_categoryMap[OsmPlacemark::AccomodationYouthHostel] = GeoDataFeature::AccomodationYouthHostel;
        m_categoryMap[OsmPlacemark::AmenityLibrary] = GeoDataFeature::AmenityLibrary;
        m_categoryMap[OsmPlacemark::EducationCollege] = GeoDataFeature::EducationCollege;
        m_categoryMap[OsmPlacemark::EducationSchool] = GeoDataFeature::EducationSchool;
        m_categoryMap[OsmPlacemark::EducationUniversity] = GeoDataFeature::EducationUniversity;
        m_categoryMap[OsmPlacemark::FoodBar] = GeoDataFeature::FoodBar;
        m_categoryMap[OsmPlacemark::FoodBiergarten] = GeoDataFeature::FoodBiergarten;
        m_categoryMap[OsmPlacemark::FoodCafe] = GeoDataFeature::FoodCafe;
        m_categoryMap[OsmPlacemark::FoodFastFood] = GeoDataFeature::FoodFastFood;
        m_categoryMap[OsmPlacemark::FoodPub] = GeoDataFeature::FoodPub;
        m_categoryMap[OsmPlacemark::FoodRestaurant] = GeoDataFeature::FoodRestaurant;
        m_categoryMap[OsmPlacemark::HealthDoctors] = GeoDataFeature::HealthDoctors;
        m_categoryMap[OsmPlacemark::HealthHospital] = GeoDataFeature::HealthHospital;
        m_categoryMap[OsmPlacemark::HealthPharmacy] = GeoDataFeature::HealthPharmacy;
        m_categoryMap[OsmPlacemark::MoneyBank] = GeoDataFeature::MoneyBank;
        m_categoryMap[OsmPlacemark::ShoppingBeverages] = GeoDataFeature::ShoppingBeverages;
        m_categoryMap[OsmPlacemark::ShoppingHifi] = GeoDataFeature::ShoppingHifi;
        m_categoryMap[OsmPlacemark::ShoppingSupermarket] = GeoDataFeature::ShoppingSupermarket;
        m_categoryMap[OsmPlacemark::TouristAttraction] = GeoDataFeature::TouristAttraction;
        m_categoryMap[OsmPlacemark::TouristCastle] = GeoDataFeature::TouristCastle;
        m_categoryMap[OsmPlacemark::TouristCinema] = GeoDataFeature::TouristCinema;
        m_categoryMap[OsmPlacemark::TouristMonument] = GeoDataFeature::TouristMonument;
        m_categoryMap[OsmPlacemark::TouristMuseum] = GeoDataFeature::TouristMuseum;
        m_categoryMap[OsmPlacemark::TouristRuin] = GeoDataFeature::TouristRuin;
        m_categoryMap[OsmPlacemark::TouristTheatre] = GeoDataFeature::TouristTheatre;
        m_categoryMap[OsmPlacemark::TouristThemePark] = GeoDataFeature::TouristThemePark;
        m_categoryMap[OsmPlacemark::TouristViewPoint] = GeoDataFeature::TouristViewPoint;
        m_categoryMap[OsmPlacemark::TouristZoo] = GeoDataFeature::TouristZoo;
        m_categoryMap[OsmPlacemark::TransportAerodrome] = GeoDataFeature::TransportAerodrome;
        m_categoryMap[OsmPlacemark::TransportAirportTerminal] = GeoDataFeature::TransportAirportTerminal;
        m_categoryMap[OsmPlacemark::TransportBusStation] = GeoDataFeature::TransportBusStation;
        m_categoryMap[OsmPlacemark::TransportBusStop] = GeoDataFeature::TransportBusStop;
        m_categoryMap[OsmPlacemark::TransportCarShare] = GeoDataFeature::TransportCarShare;
        m_categoryMap[OsmPlacemark::TransportFuel] = GeoDataFeature::TransportFuel;
        m_categoryMap[OsmPlacemark::TransportParking] = GeoDataFeature::TransportParking;    }
}

LocalOsmSearchRunner::~LocalOsmSearchRunner()
{
}

GeoDataFeature::GeoDataVisualCategory LocalOsmSearchRunner::category() const
{
    return GeoDataFeature::Coordinate;
}


void LocalOsmSearchRunner::search( const QString &searchTerm )
{
    QList<OsmPlacemark> placemarks = m_database->find( searchTerm );

    QVector<GeoDataPlacemark*> result;
    foreach( const OsmPlacemark &placemark, placemarks ) {
        GeoDataPlacemark* hit = new GeoDataPlacemark;
        hit->setName( placemark.name() );
        if ( !placemark.houseNumber().isEmpty() ) {
            hit->setName( hit->name() + " " + placemark.houseNumber() );
        }
        if ( !placemark.regionName().isEmpty() ) {
            hit->setName( hit->name() + ", " + placemark.regionName() );
        }
        if ( placemark.category() != OsmPlacemark::UnknownCategory ) {
            hit->setVisualCategory( m_categoryMap[placemark.category()] );
        }
        GeoDataCoordinates coordinate( placemark.longitude(), placemark.latitude(), 0.0, GeoDataCoordinates::Degree );
        hit->setCoordinate( coordinate );
        result << hit;
    }

    emit searchFinished( result );
}

} // namespace Marble

#include "LocalOsmSearchRunner.moc"
