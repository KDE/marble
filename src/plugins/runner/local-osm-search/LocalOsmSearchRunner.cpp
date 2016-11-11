//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011 Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2013      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
//

#include "LocalOsmSearchRunner.h"
#include "DatabaseQuery.h"

#include "MarbleDebug.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"

#include <QString>
#include <QVector>
#include <QUrl>

namespace Marble
{

QMap<OsmPlacemark::OsmCategory, GeoDataPlacemark::GeoDataVisualCategory> LocalOsmSearchRunner::m_categoryMap;

LocalOsmSearchRunner::LocalOsmSearchRunner( const QStringList &databaseFiles, QObject *parent ) :
    SearchRunner( parent ),
    m_database( databaseFiles )
{
    if ( m_categoryMap.isEmpty() ) {
        m_categoryMap[OsmPlacemark::UnknownCategory] = GeoDataPlacemark::OsmSite;
        m_categoryMap[OsmPlacemark::Address] = GeoDataPlacemark::OsmSite;
        m_categoryMap[OsmPlacemark::AccomodationCamping] = GeoDataPlacemark::AccomodationCamping;
        m_categoryMap[OsmPlacemark::AccomodationHostel] = GeoDataPlacemark::AccomodationHostel;
        m_categoryMap[OsmPlacemark::AccomodationHotel] = GeoDataPlacemark::AccomodationHotel;
        m_categoryMap[OsmPlacemark::AccomodationMotel] = GeoDataPlacemark::AccomodationMotel;
        m_categoryMap[OsmPlacemark::AccomodationYouthHostel] = GeoDataPlacemark::AccomodationYouthHostel;
        m_categoryMap[OsmPlacemark::AmenityLibrary] = GeoDataPlacemark::AmenityLibrary;
        m_categoryMap[OsmPlacemark::EducationCollege] = GeoDataPlacemark::EducationCollege;
        m_categoryMap[OsmPlacemark::EducationSchool] = GeoDataPlacemark::EducationSchool;
        m_categoryMap[OsmPlacemark::EducationUniversity] = GeoDataPlacemark::EducationUniversity;
        m_categoryMap[OsmPlacemark::FoodBar] = GeoDataPlacemark::FoodBar;
        m_categoryMap[OsmPlacemark::FoodBiergarten] = GeoDataPlacemark::FoodBiergarten;
        m_categoryMap[OsmPlacemark::FoodCafe] = GeoDataPlacemark::FoodCafe;
        m_categoryMap[OsmPlacemark::FoodFastFood] = GeoDataPlacemark::FoodFastFood;
        m_categoryMap[OsmPlacemark::FoodPub] = GeoDataPlacemark::FoodPub;
        m_categoryMap[OsmPlacemark::FoodRestaurant] = GeoDataPlacemark::FoodRestaurant;
        m_categoryMap[OsmPlacemark::HealthDoctors] = GeoDataPlacemark::HealthDoctors;
        m_categoryMap[OsmPlacemark::HealthHospital] = GeoDataPlacemark::HealthHospital;
        m_categoryMap[OsmPlacemark::HealthPharmacy] = GeoDataPlacemark::HealthPharmacy;
        m_categoryMap[OsmPlacemark::MoneyAtm] = GeoDataPlacemark::MoneyAtm;
        m_categoryMap[OsmPlacemark::MoneyBank] = GeoDataPlacemark::MoneyBank;
        m_categoryMap[OsmPlacemark::ShoppingBeverages] = GeoDataPlacemark::ShopBeverages;
        m_categoryMap[OsmPlacemark::ShoppingHifi] = GeoDataPlacemark::ShopHifi;
        m_categoryMap[OsmPlacemark::ShoppingSupermarket] = GeoDataPlacemark::ShopSupermarket;
        m_categoryMap[OsmPlacemark::TouristAttraction] = GeoDataPlacemark::TouristAttraction;
        m_categoryMap[OsmPlacemark::TouristCastle] = GeoDataPlacemark::TouristCastle;
        m_categoryMap[OsmPlacemark::TouristCinema] = GeoDataPlacemark::TouristCinema;
        m_categoryMap[OsmPlacemark::TouristMonument] = GeoDataPlacemark::TouristMonument;
        m_categoryMap[OsmPlacemark::TouristMuseum] = GeoDataPlacemark::TouristMuseum;
        m_categoryMap[OsmPlacemark::TouristRuin] = GeoDataPlacemark::TouristRuin;
        m_categoryMap[OsmPlacemark::TouristTheatre] = GeoDataPlacemark::TouristTheatre;
        m_categoryMap[OsmPlacemark::TouristThemePark] = GeoDataPlacemark::TouristThemePark;
        m_categoryMap[OsmPlacemark::TouristViewPoint] = GeoDataPlacemark::TouristViewPoint;
        m_categoryMap[OsmPlacemark::TouristZoo] = GeoDataPlacemark::TouristZoo;
        m_categoryMap[OsmPlacemark::TransportAirport] = GeoDataPlacemark::TransportAerodrome;
        m_categoryMap[OsmPlacemark::TransportAirportTerminal] = GeoDataPlacemark::TransportAirportTerminal;
        m_categoryMap[OsmPlacemark::TransportAirportRunway] = GeoDataPlacemark::TransportAirportRunway;
        m_categoryMap[OsmPlacemark::TransportAirportApron] = GeoDataPlacemark::TransportAirportApron;
        m_categoryMap[OsmPlacemark::TransportAirportTaxiway] = GeoDataPlacemark::TransportAirportTaxiway;
        m_categoryMap[OsmPlacemark::TransportBusStation] = GeoDataPlacemark::TransportBusStation;
        m_categoryMap[OsmPlacemark::TransportBusStop] = GeoDataPlacemark::TransportBusStop;
        m_categoryMap[OsmPlacemark::TransportCarShare] = GeoDataPlacemark::TransportCarShare;
        m_categoryMap[OsmPlacemark::TransportFuel] = GeoDataPlacemark::TransportFuel;
        m_categoryMap[OsmPlacemark::TransportParking] = GeoDataPlacemark::TransportParking;
        m_categoryMap[OsmPlacemark::TransportTrainStation] = GeoDataPlacemark::TransportTrainStation;
        m_categoryMap[OsmPlacemark::TransportTramStop] = GeoDataPlacemark::TransportTramStop;
        m_categoryMap[OsmPlacemark::TransportRentalBicycle] = GeoDataPlacemark::TransportRentalBicycle;
        m_categoryMap[OsmPlacemark::TransportRentalCar] = GeoDataPlacemark::TransportRentalCar;
        m_categoryMap[OsmPlacemark::TransportSpeedCamera] = GeoDataPlacemark::OsmSite;
        m_categoryMap[OsmPlacemark::TransportTaxiRank] = GeoDataPlacemark::TransportTaxiRank;
        m_categoryMap[OsmPlacemark::PlacesRegion] = GeoDataPlacemark::OsmSite;
        m_categoryMap[OsmPlacemark::PlacesCounty] = GeoDataPlacemark::OsmSite;
        m_categoryMap[OsmPlacemark::PlacesCity] = GeoDataPlacemark::PlaceCity;
        m_categoryMap[OsmPlacemark::PlacesTown] = GeoDataPlacemark::PlaceTown;
        m_categoryMap[OsmPlacemark::PlacesVillage] = GeoDataPlacemark::PlaceVillage;
        m_categoryMap[OsmPlacemark::PlacesHamlet] = GeoDataPlacemark::PlaceHamlet;
        m_categoryMap[OsmPlacemark::PlacesIsolatedDwelling] = GeoDataPlacemark::OsmSite;
        m_categoryMap[OsmPlacemark::PlacesSuburb] = GeoDataPlacemark::PlaceSuburb;
        m_categoryMap[OsmPlacemark::PlacesLocality] = GeoDataPlacemark::PlaceLocality;
        m_categoryMap[OsmPlacemark::PlacesIsland] = GeoDataPlacemark::OsmSite;
    }
}

LocalOsmSearchRunner::~LocalOsmSearchRunner()
{
}


void LocalOsmSearchRunner::search( const QString &searchTerm, const GeoDataLatLonBox &preferred )
{
    const DatabaseQuery userQuery( model(), searchTerm, preferred );

    QVector<OsmPlacemark> placemarks = m_database.find( userQuery );

    QVector<GeoDataPlacemark*> result;
    foreach( const OsmPlacemark &placemark, placemarks ) {
        GeoDataPlacemark* hit = new GeoDataPlacemark;
        hit->setName( placemark.name() );
        if ( placemark.category() == OsmPlacemark::Address && !placemark.houseNumber().isEmpty() ) {
            hit->setName(hit->name() + QLatin1Char(' ') + placemark.houseNumber());
        }
        if ( !placemark.additionalInformation().isEmpty() ) {
            hit->setName(hit->name() + QLatin1Char('(') + placemark.additionalInformation() + QLatin1Char(')'));
        }
        if ( placemark.category() != OsmPlacemark::UnknownCategory ) {
            hit->setVisualCategory( m_categoryMap[placemark.category()] );
        }
        hit->setGeometry( new GeoDataPoint( placemark.longitude(), placemark.latitude(), 0.0, GeoDataCoordinates::Degree ) );
        result << hit;
    }

    emit searchFinished( result );
}

} // namespace Marble

#include "moc_LocalOsmSearchRunner.cpp"
