//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//

#include <OsmNode.h>

#include "osm/OsmObjectManager.h"
#include <GeoDataPlacemark.h>
#include <GeoDataStyle.h>
#include <GeoDataIconStyle.h>
#include <GeoDataDocument.h>
#include <MarbleDirs.h>
#include <StyleBuilder.h>

#include <QXmlStreamAttributes>

namespace Marble {

QHash<GeoDataPlacemark::GeoDataVisualCategory, qint64> OsmNode::m_popularities;
QHash<GeoDataPlacemark::GeoDataVisualCategory, int> OsmNode::m_zoomLevels;

void OsmNode::parseCoordinates(const QXmlStreamAttributes &attributes)
{
    qreal const lon = attributes.value(QLatin1String("lon")).toDouble();
    qreal const lat = attributes.value(QLatin1String("lat")).toDouble();
    setCoordinates(GeoDataCoordinates(lon, lat, 0, GeoDataCoordinates::Degree));
}

void OsmNode::setCoordinates(const GeoDataCoordinates &coordinates)
{
    m_coordinates = coordinates;
}

void OsmNode::create(GeoDataDocument *document) const
{
    GeoDataPlacemark::GeoDataVisualCategory const category = StyleBuilder::determineVisualCategory(m_osmData);

    if (category == GeoDataPlacemark::None) {
        return;
    }

    GeoDataPlacemark* placemark = new GeoDataPlacemark;
    placemark->setOsmData(m_osmData);
    auto coordinates = m_coordinates;
    coordinates.setAltitude(m_osmData.tagValue("ele").toDouble());
    placemark->setCoordinate(coordinates);

    QHash<QString, QString>::const_iterator tagIter;
    if ((category == GeoDataPlacemark::TransportCarShare || category == GeoDataPlacemark::MoneyAtm)
            && (tagIter = m_osmData.findTag(QStringLiteral("operator"))) != m_osmData.tagsEnd()) {
        placemark->setName(tagIter.value());
    } else {
        placemark->setName(m_osmData.tagValue(QStringLiteral("name")));
    }
    if (placemark->name().isEmpty()) {
        placemark->setName(m_osmData.tagValue(QStringLiteral("ref")));
    }
    placemark->setVisualCategory(category);
    placemark->setStyle( GeoDataStyle::Ptr() );
    placemark->setZoomLevel(zoomLevelFor(category));
    placemark->setPopularity(popularityFor(category));

    if (category >= GeoDataPlacemark::PlaceCity && category <= GeoDataPlacemark::PlaceVillageCapital) {
        int const population = m_osmData.tagValue(QStringLiteral("population")).toInt();
        placemark->setPopulation(qMax(0, population));
        if (population > 0) {
            placemark->setZoomLevel(populationIndex(population));
            placemark->setPopularity(population);
        }
    }

    if (m_osmData.containsTagKey(QLatin1String("marbleZoomLevel"))) {
        int const zoomLevel = m_osmData.tagValue(QLatin1String("marbleZoomLevel")).toInt();
        placemark->setZoomLevel(zoomLevel);
    }

    OsmObjectManager::registerId(m_osmData.id());
    document->append(placemark);
}

int OsmNode::populationIndex(qint64 population) const
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

int OsmNode::zoomLevelFor(GeoDataPlacemark::GeoDataVisualCategory category)
{
    int const defaultValue = 18;
    if (m_zoomLevels.isEmpty()) {
        m_zoomLevels[GeoDataPlacemark::PlaceCityCapital] = 9;
        m_zoomLevels[GeoDataPlacemark::PlaceCity] = 9;

        m_zoomLevels[GeoDataPlacemark::PlaceTownCapital] = 11;
        m_zoomLevels[GeoDataPlacemark::PlaceTown] = 11;
        m_zoomLevels[GeoDataPlacemark::NaturalPeak] = 11;

        m_zoomLevels[GeoDataPlacemark::PlaceSuburb] = 13;
        m_zoomLevels[GeoDataPlacemark::PlaceVillageCapital] = 13;
        m_zoomLevels[GeoDataPlacemark::PlaceVillage] = 13;

        m_zoomLevels[GeoDataPlacemark::PlaceHamlet] = 15;
        m_zoomLevels[GeoDataPlacemark::HealthHospital] = 15;
        m_zoomLevels[GeoDataPlacemark::PlaceLocality] = 15;

        m_zoomLevels[GeoDataPlacemark::AmenityBench] = 19;
        m_zoomLevels[GeoDataPlacemark::AmenityWasteBasket] = 19;
        m_zoomLevels[GeoDataPlacemark::PowerTower] = 19;
    }

    return m_zoomLevels.value(category, defaultValue);
}

qint64 OsmNode::popularityFor(GeoDataPlacemark::GeoDataVisualCategory category)
{
    qint64 const defaultValue = 100;
    if (m_popularities.isEmpty()) {
        QVector<GeoDataPlacemark::GeoDataVisualCategory> popularities;
        popularities << GeoDataPlacemark::PlaceCityCapital;
        popularities << GeoDataPlacemark::PlaceTownCapital;
        popularities << GeoDataPlacemark::PlaceCity;
        popularities << GeoDataPlacemark::PlaceTown;
        popularities << GeoDataPlacemark::PlaceSuburb;
        popularities << GeoDataPlacemark::PlaceVillageCapital;
        popularities << GeoDataPlacemark::PlaceVillage;
        popularities << GeoDataPlacemark::PlaceHamlet;
        popularities << GeoDataPlacemark::PlaceLocality;

        popularities << GeoDataPlacemark::AmenityEmergencyPhone;
        popularities << GeoDataPlacemark::HealthHospital;
        popularities << GeoDataPlacemark::AmenityToilets;
        popularities << GeoDataPlacemark::MoneyAtm;

        popularities << GeoDataPlacemark::NaturalPeak;

        popularities << GeoDataPlacemark::AccomodationHotel;
        popularities << GeoDataPlacemark::AccomodationMotel;
        popularities << GeoDataPlacemark::AccomodationGuestHouse;
        popularities << GeoDataPlacemark::AccomodationYouthHostel;
        popularities << GeoDataPlacemark::AccomodationHostel;
        popularities << GeoDataPlacemark::AccomodationCamping;

        popularities << GeoDataPlacemark::HealthDentist;
        popularities << GeoDataPlacemark::HealthDoctors;
        popularities << GeoDataPlacemark::HealthPharmacy;
        popularities << GeoDataPlacemark::HealthVeterinary;

        popularities << GeoDataPlacemark::AmenityLibrary;
        popularities << GeoDataPlacemark::EducationCollege;
        popularities << GeoDataPlacemark::EducationSchool;
        popularities << GeoDataPlacemark::EducationUniversity;

        popularities << GeoDataPlacemark::FoodBar;
        popularities << GeoDataPlacemark::FoodBiergarten;
        popularities << GeoDataPlacemark::FoodCafe;
        popularities << GeoDataPlacemark::FoodFastFood;
        popularities << GeoDataPlacemark::FoodPub;
        popularities << GeoDataPlacemark::FoodRestaurant;

        popularities << GeoDataPlacemark::MoneyBank;

        popularities << GeoDataPlacemark::AmenityArchaeologicalSite;
        popularities << GeoDataPlacemark::AmenityEmbassy;
        popularities << GeoDataPlacemark::AmenityWaterPark;
        popularities << GeoDataPlacemark::AmenityCommunityCentre;
        popularities << GeoDataPlacemark::AmenityFountain;
        popularities << GeoDataPlacemark::AmenityNightClub;
        popularities << GeoDataPlacemark::AmenityCourtHouse;
        popularities << GeoDataPlacemark::AmenityFireStation;
        popularities << GeoDataPlacemark::AmenityHuntingStand;
        popularities << GeoDataPlacemark::AmenityPolice;
        popularities << GeoDataPlacemark::AmenityPostBox;
        popularities << GeoDataPlacemark::AmenityPostOffice;
        popularities << GeoDataPlacemark::AmenityPrison;
        popularities << GeoDataPlacemark::AmenityRecycling;
        popularities << GeoDataPlacemark::AmenityTelephone;
        popularities << GeoDataPlacemark::AmenityTownHall;
        popularities << GeoDataPlacemark::AmenityDrinkingWater;
        popularities << GeoDataPlacemark::AmenityGraveyard;

        popularities << GeoDataPlacemark::ManmadeBridge;
        popularities << GeoDataPlacemark::ManmadeLighthouse;
        popularities << GeoDataPlacemark::ManmadePier;
        popularities << GeoDataPlacemark::ManmadeWaterTower;
        popularities << GeoDataPlacemark::ManmadeWindMill;
        popularities << GeoDataPlacemark::TouristAttraction;
        popularities << GeoDataPlacemark::TouristCastle;
        popularities << GeoDataPlacemark::TouristCinema;
        popularities << GeoDataPlacemark::TouristInformation;
        popularities << GeoDataPlacemark::TouristMonument;
        popularities << GeoDataPlacemark::TouristMuseum;
        popularities << GeoDataPlacemark::TouristRuin;
        popularities << GeoDataPlacemark::TouristTheatre;
        popularities << GeoDataPlacemark::TouristThemePark;
        popularities << GeoDataPlacemark::TouristViewPoint;
        popularities << GeoDataPlacemark::TouristZoo;
        popularities << GeoDataPlacemark::TouristAlpineHut;
        popularities << GeoDataPlacemark::TransportAerodrome;
        popularities << GeoDataPlacemark::TransportHelipad;
        popularities << GeoDataPlacemark::TransportAirportTerminal;
        popularities << GeoDataPlacemark::TransportBusStation;
        popularities << GeoDataPlacemark::TransportBusStop;
        popularities << GeoDataPlacemark::TransportCarShare;
        popularities << GeoDataPlacemark::TransportFuel;
        popularities << GeoDataPlacemark::TransportParking;
        popularities << GeoDataPlacemark::TransportParkingSpace;
        popularities << GeoDataPlacemark::TransportPlatform;
        popularities << GeoDataPlacemark::TransportRentalBicycle;
        popularities << GeoDataPlacemark::TransportRentalCar;
        popularities << GeoDataPlacemark::TransportTaxiRank;
        popularities << GeoDataPlacemark::TransportTrainStation;
        popularities << GeoDataPlacemark::TransportTramStop;
        popularities << GeoDataPlacemark::TransportBicycleParking;
        popularities << GeoDataPlacemark::TransportMotorcycleParking;
        popularities << GeoDataPlacemark::TransportSubwayEntrance;

        popularities << GeoDataPlacemark::ShopBeverages;
        popularities << GeoDataPlacemark::ShopHifi;
        popularities << GeoDataPlacemark::ShopSupermarket;
        popularities << GeoDataPlacemark::ShopAlcohol;
        popularities << GeoDataPlacemark::ShopBakery;
        popularities << GeoDataPlacemark::ShopButcher;
        popularities << GeoDataPlacemark::ShopConfectionery;
        popularities << GeoDataPlacemark::ShopConvenience;
        popularities << GeoDataPlacemark::ShopGreengrocer;
        popularities << GeoDataPlacemark::ShopSeafood;
        popularities << GeoDataPlacemark::ShopDepartmentStore;
        popularities << GeoDataPlacemark::ShopKiosk;
        popularities << GeoDataPlacemark::ShopBag;
        popularities << GeoDataPlacemark::ShopClothes;
        popularities << GeoDataPlacemark::ShopFashion;
        popularities << GeoDataPlacemark::ShopJewelry;
        popularities << GeoDataPlacemark::ShopShoes;
        popularities << GeoDataPlacemark::ShopVarietyStore;
        popularities << GeoDataPlacemark::ShopBeauty;
        popularities << GeoDataPlacemark::ShopChemist;
        popularities << GeoDataPlacemark::ShopCosmetics;
        popularities << GeoDataPlacemark::ShopHairdresser;
        popularities << GeoDataPlacemark::ShopOptician;
        popularities << GeoDataPlacemark::ShopPerfumery;
        popularities << GeoDataPlacemark::ShopDoitYourself;
        popularities << GeoDataPlacemark::ShopFlorist;
        popularities << GeoDataPlacemark::ShopHardware;
        popularities << GeoDataPlacemark::ShopFurniture;
        popularities << GeoDataPlacemark::ShopElectronics;
        popularities << GeoDataPlacemark::ShopMobilePhone;
        popularities << GeoDataPlacemark::ShopBicycle;
        popularities << GeoDataPlacemark::ShopCar;
        popularities << GeoDataPlacemark::ShopCarRepair;
        popularities << GeoDataPlacemark::ShopCarParts;
        popularities << GeoDataPlacemark::ShopMotorcycle;
        popularities << GeoDataPlacemark::ShopOutdoor;
        popularities << GeoDataPlacemark::ShopMusicalInstrument;
        popularities << GeoDataPlacemark::ShopPhoto;
        popularities << GeoDataPlacemark::ShopBook;
        popularities << GeoDataPlacemark::ShopGift;
        popularities << GeoDataPlacemark::ShopStationery;
        popularities << GeoDataPlacemark::ShopLaundry;
        popularities << GeoDataPlacemark::ShopPet;
        popularities << GeoDataPlacemark::ShopToys;
        popularities << GeoDataPlacemark::ShopTravelAgency;
        popularities << GeoDataPlacemark::Shop;

        popularities << GeoDataPlacemark::LeisureGolfCourse;
        popularities << GeoDataPlacemark::LeisurePark;
        popularities << GeoDataPlacemark::LeisurePlayground;
        popularities << GeoDataPlacemark::LeisurePitch;
        popularities << GeoDataPlacemark::LeisureSportsCentre;
        popularities << GeoDataPlacemark::LeisureStadium;
        popularities << GeoDataPlacemark::LeisureTrack;
        popularities << GeoDataPlacemark::LeisureSwimmingPool;

        popularities << GeoDataPlacemark::HighwayTrafficSignals;
        popularities << GeoDataPlacemark::BarrierGate;
        popularities << GeoDataPlacemark::BarrierLiftGate;
        popularities << GeoDataPlacemark::AmenityBench;
        popularities << GeoDataPlacemark::NaturalTree;
        popularities << GeoDataPlacemark::AmenityWasteBasket;
        popularities << GeoDataPlacemark::PowerTower;

        int const offset = 10;
        int value = defaultValue + offset * popularities.size();
        for (auto popularity: popularities) {
            m_popularities[popularity] = value;
            value -= offset;
        }
    }

    return m_popularities.value(category, defaultValue);
}

const GeoDataCoordinates &OsmNode::coordinates() const
{
    return m_coordinates;
}

OsmPlacemarkData &OsmNode::osmData()
{
    return m_osmData;
}

const OsmPlacemarkData &OsmNode::osmData() const
{
    return m_osmData;
}

}
