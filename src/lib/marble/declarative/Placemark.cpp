//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Dennis Nienhüser <nienhueser@kde.org>
//

#include "Placemark.h"

#ifdef HAVE_QT5_POSITIONING
#include <GeoDataExtendedData.h>
#include <QGeoAddress>
#endif // HAVE_QT5_POSITIONING

#include <osm/OsmPlacemarkData.h>

namespace Marble {

Placemark::Placemark(QObject *parent ) :
    QObject( parent )
{
    // nothing to do
}

void Placemark::setGeoDataPlacemark( const Marble::GeoDataPlacemark &placemark )
{
    m_placemark = placemark;
    m_address = QString();
    m_description = QString();
    m_website = QString();
    m_wikipedia = QString();
    m_fuelDetails = QString();
    m_openingHours = QString();
    emit coordinatesChanged();
    emit nameChanged();
    emit descriptionChanged();
    emit addressChanged();
    emit websiteChanged();
    emit wikipediaChanged();
    emit openingHoursChanged();
    if (m_placemark.visualCategory() == GeoDataFeature::TransportFuel) {
        emit fuelDetailsChanged();
    }
}

Marble::GeoDataPlacemark & Placemark::placemark()
{
    return m_placemark;
}

QString Placemark::name() const
{
    return m_placemark.name();
}

QString Placemark::description() const
{
    if (m_description.isEmpty()) {
        m_description = m_placemark.description();

        if (m_description.isEmpty()) {
            auto const category = m_placemark.visualCategory();
            m_description = categoryName(category);
            if (category >= GeoDataFeature::FoodBar && category <= GeoDataFeature::FoodRestaurant) {
                addTagValue("brand");
                addTagValue("cuisine");
                addTagDescription(m_description, "self_service", "yes", "Self Service");
                addTagDescription(m_description, "takeaway", "yes", "Take Away");
                addTagDescription(m_description, "outdoor_seating", "yes", "Outdoor Seating");
                addTagDescription(m_description, "ice_cream", "yes", "Ice Cream");
                addTagDescription(m_description, "smoking", "dedicated", "Smoking (dedicated)");
                addTagDescription(m_description, "smoking", "yes", "Smoking allowed");
                addTagDescription(m_description, "smoking", "separated", "Smoking (separated)");
                addTagDescription(m_description, "smoking", "isolated", "Smoking (isolated)");
                addTagDescription(m_description, "smoking", "no", "No smoking");
                addTagDescription(m_description, "smoking", "outside", "Smoking (outside)");
                addTagDescription(m_description, "smoking:outside", "yes", "Smoking (outside)");
                addTagDescription(m_description, "smoking:outside", "separated", "Smoking (outside separated)");
                addTagDescription(m_description, "smoking:outside", "no", "No smoking outside");
            } else if (category >= GeoDataFeature::ShopBeverages && category <= GeoDataFeature::Shop) {
                addTagValue("operator");
            } else if (category == GeoDataFeature::TransportBusStop) {
                addTagValue("network");
                addTagValue("operator");
                addTagValue("ref");
            } else if (category == GeoDataFeature::TransportCarShare) {
                addTagValue("network");
                addTagValue("operator");
            } else if (category == GeoDataFeature::TransportFuel) {
                addTagValue("brand");
                addTagValue("operator");
            } else if (category == GeoDataFeature::NaturalTree) {
                addTagValue("species:en");
                addTagValue("genus:en");
                addTagValue("leaf_type");
            }
        }
    }

    return m_description;
}

QString Placemark::address() const
{
    if (m_address.isEmpty()) {
        m_address = addressFromExtendedData();
        if (m_address.isEmpty()) {
            m_address = addressFromOsmData();
        }
        if (m_address.isEmpty()) {
            m_address = m_placemark.address();
        }
    }

    return m_address;
}

QString Placemark::fuelDetails() const
{
    if (m_fuelDetails.isEmpty() && m_placemark.visualCategory() == GeoDataFeature::TransportFuel) {
        addTagDescription(m_fuelDetails, "fuel:diesel", "yes", tr("Diesel"));
        addTagDescription(m_fuelDetails, "fuel:octane_91", "yes", tr("Octane 91"));
        addTagDescription(m_fuelDetails, "fuel:octane_95", "yes", tr("Octane 95"));
        addTagDescription(m_fuelDetails, "fuel:octane_98", "yes", tr("Octane 98"));
        addTagDescription(m_fuelDetails, "fuel:e10", "yes", tr("E10"));
        addTagDescription(m_fuelDetails, "fuel:lpg", "yes", tr("LPG"));

    }
    return m_fuelDetails;
}

QString Placemark::website() const
{
    if (!m_website.isEmpty()) {
        return m_website;
    }
    foreach(const QString &tag, QStringList() << "website" << "contact:website" << "facebook" << "contact:facebook" << "url") {
        m_website = m_placemark.osmData().tagValue(tag);
        if (!m_website.isEmpty()) {
            return m_website;
        }
    }

    return m_website;
}

QString Placemark::wikipedia() const
{
    if (!m_wikipedia.isEmpty()) {
        return m_wikipedia;
    }

    m_wikipedia = m_placemark.osmData().tagValue("wikipedia");
    return m_wikipedia;
}

QString Placemark::openingHours() const
{
    if (!m_openingHours.isEmpty()) {
        return m_openingHours;
    }

    m_openingHours = m_placemark.osmData().tagValue("opening_hours");
    return m_openingHours;
}

QString Placemark::coordinates() const
{
    return m_placemark.coordinate().toString(GeoDataCoordinates::Decimal);
}

void Placemark::setName(const QString & name)
{
    if (m_placemark.name() == name) {
        return;
    }

    m_placemark.setName(name);
    emit nameChanged();
}

QString Placemark::categoryName(GeoDataFeature::GeoDataVisualCategory category) const
{
    switch (category) {
    case GeoDataFeature::SmallCity: return tr("City");
    case GeoDataFeature::SmallCountyCapital: return tr("County Capital");
    case GeoDataFeature::SmallStateCapital: return tr("State Capital");
    case GeoDataFeature::SmallNationCapital: return tr("Nation Capital");
    case GeoDataFeature::MediumCity: return tr("City");
    case GeoDataFeature::MediumCountyCapital: return tr("County Capital");
    case GeoDataFeature::MediumStateCapital: return tr("State Capital");
    case GeoDataFeature::MediumNationCapital: return tr("Nation Capital");
    case GeoDataFeature::BigCity: return tr("City");
    case GeoDataFeature::BigCountyCapital: return tr("County Capital");
    case GeoDataFeature::BigStateCapital: return tr("State Capital");
    case GeoDataFeature::BigNationCapital: return tr("Nation Capital");
    case GeoDataFeature::LargeCity: return tr("City");
    case GeoDataFeature::LargeCountyCapital: return tr("County Capital");
    case GeoDataFeature::LargeStateCapital: return tr("State Capital");
    case GeoDataFeature::LargeNationCapital: return tr("Nation Capital");
    case GeoDataFeature::Nation: return tr("Nation");
    case GeoDataFeature::PlaceCity: return tr("City");
    case GeoDataFeature::PlaceSuburb: return tr("Suburb");
    case GeoDataFeature::PlaceHamlet: return tr("Hamlet");
    case GeoDataFeature::PlaceLocality: return tr("Locality");
    case GeoDataFeature::PlaceTown: return tr("Town");
    case GeoDataFeature::PlaceVillage: return tr("Village");
    case GeoDataFeature::Mountain: return tr("Mountain");
    case GeoDataFeature::Volcano: return tr("Volcano");
    case GeoDataFeature::Continent: return tr("Continent");
    case GeoDataFeature::Ocean: return tr("Ocean");
    case GeoDataFeature::GeographicPole: return tr("Geographic Pole");
    case GeoDataFeature::MagneticPole: return tr("Magnetic Pole");
    case GeoDataFeature::ShipWreck: return tr("Ship Wreck");
    case GeoDataFeature::AirPort: return tr("Air Port");
    case GeoDataFeature::Observatory: return tr("Observatory");
    case GeoDataFeature::MilitaryDangerArea: return tr("Military Danger Area");
    case GeoDataFeature::OsmSite: return tr("OSM Site");
    case GeoDataFeature::Coordinate: return tr("Coordinate");
    case GeoDataFeature::Folder: return tr("Folder");
    case GeoDataFeature::Bookmark: return tr("Bookmark");
    case GeoDataFeature::NaturalWater: return tr("Water");
    case GeoDataFeature::NaturalReef: return tr("Reef");
    case GeoDataFeature::NaturalWood: return tr("Wood");
    case GeoDataFeature::NaturalBeach: return tr("Beach");
    case GeoDataFeature::NaturalWetland: return tr("Wetland");
    case GeoDataFeature::NaturalGlacier: return tr("Glacier");
    case GeoDataFeature::NaturalIceShelf: return tr("Ice Shelf");
    case GeoDataFeature::NaturalScrub: return tr("Scrub");
    case GeoDataFeature::NaturalCliff: return tr("Cliff");
    case GeoDataFeature::NaturalHeath: return tr("Heath");
    case GeoDataFeature::HighwayTrafficSignals: return tr("Traffic Signals");
    case GeoDataFeature::HighwaySteps: return tr("Steps");
    case GeoDataFeature::HighwayUnknown: return tr("Unknown Road");
    case GeoDataFeature::HighwayPath: return tr("Path");
    case GeoDataFeature::HighwayFootway: return tr("Footway");
    case GeoDataFeature::HighwayTrack: return tr("Track");
    case GeoDataFeature::HighwayPedestrian: return tr("Footway");
    case GeoDataFeature::HighwayCycleway: return tr("Cycleway");
    case GeoDataFeature::HighwayService: return tr("Service Road");
    case GeoDataFeature::HighwayRoad: return tr("Road");
    case GeoDataFeature::HighwayResidential: return tr("Residential Road");
    case GeoDataFeature::HighwayLivingStreet: return tr("Living Street");
    case GeoDataFeature::HighwayUnclassified: return tr("Unclassified Road");
    case GeoDataFeature::HighwayTertiaryLink: return tr("Tertiary Link Road");
    case GeoDataFeature::HighwayTertiary: return tr("Tertiary Road");
    case GeoDataFeature::HighwaySecondaryLink: return tr("Secondary Link Road");
    case GeoDataFeature::HighwaySecondary: return tr("Secondary Road");
    case GeoDataFeature::HighwayPrimaryLink: return tr("Primary Link Road");
    case GeoDataFeature::HighwayPrimary: return tr("Primary Road");
    case GeoDataFeature::HighwayTrunkLink: return tr("Trunk Link Road");
    case GeoDataFeature::HighwayTrunk: return tr("Trunk Road");
    case GeoDataFeature::HighwayMotorwayLink: return tr("Motorway Link Road");
    case GeoDataFeature::HighwayMotorway: return tr("Motorway");
    case GeoDataFeature::Building: return tr("Building");
    case GeoDataFeature::AccomodationCamping: return tr("Camping");
    case GeoDataFeature::AccomodationHostel: return tr("Hostel");
    case GeoDataFeature::AccomodationHotel: return tr("Hotel");
    case GeoDataFeature::AccomodationMotel: return tr("Motel");
    case GeoDataFeature::AccomodationYouthHostel: return tr("Youth Hostel");
    case GeoDataFeature::AccomodationGuestHouse: return tr("Guest House");
    case GeoDataFeature::AmenityLibrary: return tr("Library");
    case GeoDataFeature::EducationCollege: return tr("College");
    case GeoDataFeature::EducationSchool: return tr("School");
    case GeoDataFeature::EducationUniversity: return tr("University");
    case GeoDataFeature::FoodBar: return tr("Bar");
    case GeoDataFeature::FoodBiergarten: return tr("Biergarten");
    case GeoDataFeature::FoodCafe: return tr("Cafe");
    case GeoDataFeature::FoodFastFood: return tr("Fast Food");
    case GeoDataFeature::FoodPub: return tr("Pub");
    case GeoDataFeature::FoodRestaurant: return tr("Restaurant");
    case GeoDataFeature::HealthDentist: return tr("Dentist");
    case GeoDataFeature::HealthDoctors: return tr("Doctors");
    case GeoDataFeature::HealthHospital: return tr("Hospital");
    case GeoDataFeature::HealthPharmacy: return tr("Pharmacy");
    case GeoDataFeature::HealthVeterinary: return tr("Veterinary");
    case GeoDataFeature::MoneyAtm: return tr("ATM");
    case GeoDataFeature::MoneyBank: return tr("Bank");
    case GeoDataFeature::AmenityArchaeologicalSite: return tr("Archaeological Site");
    case GeoDataFeature::AmenityEmbassy: return tr("Embassy");
    case GeoDataFeature::AmenityEmergencyPhone: return tr("Emergency Phone");
    case GeoDataFeature::AmenityWaterPark: return tr("Water Park");
    case GeoDataFeature::AmenityCommunityCentre: return tr("Community Centre");
    case GeoDataFeature::AmenityFountain: return tr("Fountain");
    case GeoDataFeature::AmenityNightClub: return tr("Night Club");
    case GeoDataFeature::AmenityBench: return tr("Bench");
    case GeoDataFeature::AmenityCourtHouse: return tr("Court House");
    case GeoDataFeature::AmenityFireStation: return tr("Fire Station");
    case GeoDataFeature::AmenityHuntingStand: return tr("Hunting Stand");
    case GeoDataFeature::AmenityPolice: return tr("Police");
    case GeoDataFeature::AmenityPostBox: return tr("Post Box");
    case GeoDataFeature::AmenityPostOffice: return tr("Post Office");
    case GeoDataFeature::AmenityPrison: return tr("Prison");
    case GeoDataFeature::AmenityRecycling: return tr("Recycling");
    case GeoDataFeature::AmenityTelephone: return tr("Telephone");
    case GeoDataFeature::AmenityToilets: return tr("Toilets");
    case GeoDataFeature::AmenityTownHall: return tr("Town Hall");
    case GeoDataFeature::AmenityWasteBasket: return tr("Waste Basket");
    case GeoDataFeature::AmenityDrinkingWater: return tr("Drinking Water");
    case GeoDataFeature::AmenityGraveyard: return tr("Graveyard");
    case GeoDataFeature::BarrierCityWall: return tr("City Wall");
    case GeoDataFeature::BarrierGate: return tr("Gate");
    case GeoDataFeature::BarrierLiftGate: return tr("Lift Gate");
    case GeoDataFeature::BarrierWall: return tr("Wall");
    case GeoDataFeature::NaturalPeak: return tr("Peak");
    case GeoDataFeature::NaturalTree: return tr("Tree");
    case GeoDataFeature::ShopBeverages: return tr("Beverages");
    case GeoDataFeature::ShopHifi: return tr("Hifi");
    case GeoDataFeature::ShopSupermarket: return tr("Supermarket");
    case GeoDataFeature::ShopAlcohol: return tr("Alcohol");
    case GeoDataFeature::ShopBakery: return tr("Bakery");
    case GeoDataFeature::ShopButcher: return tr("Butcher");
    case GeoDataFeature::ShopConfectionery: return tr("Confectionery");
    case GeoDataFeature::ShopConvenience: return tr("Convenience Shop");
    case GeoDataFeature::ShopGreengrocer: return tr("Greengrocer");
    case GeoDataFeature::ShopSeafood: return tr("Seafood");
    case GeoDataFeature::ShopDepartmentStore: return tr("Department Store");
    case GeoDataFeature::ShopKiosk: return tr("Kiosk");
    case GeoDataFeature::ShopBag: return tr("Bag");
    case GeoDataFeature::ShopClothes: return tr("Clothes");
    case GeoDataFeature::ShopFashion: return tr("Fashion");
    case GeoDataFeature::ShopJewelry: return tr("Jewelry");
    case GeoDataFeature::ShopShoes: return tr("Shoes");
    case GeoDataFeature::ShopVarietyStore: return tr("Variety Store");
    case GeoDataFeature::ShopBeauty: return tr("Beauty");
    case GeoDataFeature::ShopChemist: return tr("Chemist");
    case GeoDataFeature::ShopCosmetics: return tr("Cosmetics");
    case GeoDataFeature::ShopHairdresser: return tr("Hairdresser");
    case GeoDataFeature::ShopOptician: return tr("Optician");
    case GeoDataFeature::ShopPerfumery: return tr("Perfumery");
    case GeoDataFeature::ShopDoitYourself: return tr("Doit Yourself");
    case GeoDataFeature::ShopFlorist: return tr("Florist");
    case GeoDataFeature::ShopHardware: return tr("Hardware");
    case GeoDataFeature::ShopFurniture: return tr("Furniture");
    case GeoDataFeature::ShopElectronics: return tr("Electronics");
    case GeoDataFeature::ShopMobilePhone: return tr("Mobile Phone");
    case GeoDataFeature::ShopBicycle: return tr("Bicycle");
    case GeoDataFeature::ShopCar: return tr("Car");
    case GeoDataFeature::ShopCarRepair: return tr("Car Repair");
    case GeoDataFeature::ShopCarParts: return tr("Car Parts");
    case GeoDataFeature::ShopMotorcycle: return tr("Motorcycle");
    case GeoDataFeature::ShopOutdoor: return tr("Outdoor");
    case GeoDataFeature::ShopMusicalInstrument: return tr("Musical Instrument");
    case GeoDataFeature::ShopPhoto: return tr("Photo");
    case GeoDataFeature::ShopBook: return tr("Book");
    case GeoDataFeature::ShopGift: return tr("Gift");
    case GeoDataFeature::ShopStationery: return tr("Stationery");
    case GeoDataFeature::ShopLaundry: return tr("Laundry");
    case GeoDataFeature::ShopPet: return tr("Pet");
    case GeoDataFeature::ShopToys: return tr("Toys");
    case GeoDataFeature::ShopTravelAgency: return tr("Travel Agency");
    case GeoDataFeature::Shop: return tr("Shop");
    case GeoDataFeature::ManmadeBridge: return tr("Bridge");
    case GeoDataFeature::ManmadeLighthouse: return tr("Lighthouse");
    case GeoDataFeature::ManmadePier: return tr("Pier");
    case GeoDataFeature::ManmadeWaterTower: return tr("Water Tower");
    case GeoDataFeature::ManmadeWindMill: return tr("Wind Mill");
    case GeoDataFeature::TouristAttraction: return tr("Tourist Attraction");
    case GeoDataFeature::TouristCastle: return tr("Castle");
    case GeoDataFeature::TouristCinema: return tr("Cinema");
    case GeoDataFeature::TouristInformation: return tr("Information");
    case GeoDataFeature::TouristMonument: return tr("Monument");
    case GeoDataFeature::TouristMuseum: return tr("Museum");
    case GeoDataFeature::TouristRuin: return tr("Ruin");
    case GeoDataFeature::TouristTheatre: return tr("Theatre");
    case GeoDataFeature::TouristThemePark: return tr("Theme Park");
    case GeoDataFeature::TouristViewPoint: return tr("View Point");
    case GeoDataFeature::TouristZoo: return tr("Zoo");
    case GeoDataFeature::TouristAlpineHut: return tr("Alpine Hut");
    case GeoDataFeature::TransportAerodrome: return tr("Aerodrome");
    case GeoDataFeature::TransportHelipad: return tr("Helipad");
    case GeoDataFeature::TransportAirportTerminal: return tr("Airport Terminal");
    case GeoDataFeature::TransportBusStation: return tr("Bus Station");
    case GeoDataFeature::TransportBusStop: return tr("Bus Stop");
    case GeoDataFeature::TransportCarShare: return tr("Car Sharing");
    case GeoDataFeature::TransportFuel: return tr("Gas Station");
    case GeoDataFeature::TransportParking: return tr("Parking");
    case GeoDataFeature::TransportParkingSpace: return tr("Parking Space");
    case GeoDataFeature::TransportPlatform: return tr("Platform");
    case GeoDataFeature::TransportRentalBicycle: return tr("Rental Bicycle");
    case GeoDataFeature::TransportRentalCar: return tr("Rental Car");
    case GeoDataFeature::TransportTaxiRank: return tr("Taxi Rank");
    case GeoDataFeature::TransportTrainStation: return tr("Train Station");
    case GeoDataFeature::TransportTramStop: return tr("Tram Stop");
    case GeoDataFeature::TransportBicycleParking: return tr("Bicycle Parking");
    case GeoDataFeature::TransportMotorcycleParking: return tr("Motorcycle Parking");
    case GeoDataFeature::TransportSubwayEntrance: return tr("Subway Entrance");
    case GeoDataFeature::ReligionPlaceOfWorship: return tr("Place Of Worship");
    case GeoDataFeature::ReligionBahai: return tr("Bahai");
    case GeoDataFeature::ReligionBuddhist: return tr("Buddhist");
    case GeoDataFeature::ReligionChristian: return tr("Christian");
    case GeoDataFeature::ReligionMuslim: return tr("Muslim");
    case GeoDataFeature::ReligionHindu: return tr("Hindu");
    case GeoDataFeature::ReligionJain: return tr("Jain");
    case GeoDataFeature::ReligionJewish: return tr("Jewish");
    case GeoDataFeature::ReligionShinto: return tr("Shinto");
    case GeoDataFeature::ReligionSikh: return tr("Sikh");
    case GeoDataFeature::LeisureGolfCourse: return tr("Golf Course");
    case GeoDataFeature::LeisurePark: return tr("Park");
    case GeoDataFeature::LeisurePlayground: return tr("Playground");
    case GeoDataFeature::LeisurePitch: return tr("Pitch");
    case GeoDataFeature::LeisureSportsCentre: return tr("Sports Centre");
    case GeoDataFeature::LeisureStadium: return tr("Stadium");
    case GeoDataFeature::LeisureTrack: return tr("Track");
    case GeoDataFeature::LeisureSwimmingPool: return tr("Swimming Pool");
    case GeoDataFeature::LanduseAllotments: return tr("Allotments");
    case GeoDataFeature::LanduseBasin: return tr("Basin");
    case GeoDataFeature::LanduseCemetery: return tr("Cemetery");
    case GeoDataFeature::LanduseCommercial: return tr("Commercial");
    case GeoDataFeature::LanduseConstruction: return tr("Construction");
    case GeoDataFeature::LanduseFarmland: return tr("Farmland");
    case GeoDataFeature::LanduseFarmyard: return tr("Farmyard");
    case GeoDataFeature::LanduseGarages: return tr("Garages");
    case GeoDataFeature::LanduseGrass: return tr("Grass");
    case GeoDataFeature::LanduseIndustrial: return tr("Industrial");
    case GeoDataFeature::LanduseLandfill: return tr("Landfill");
    case GeoDataFeature::LanduseMeadow: return tr("Meadow");
    case GeoDataFeature::LanduseMilitary: return tr("Military");
    case GeoDataFeature::LanduseQuarry: return tr("Quarry");
    case GeoDataFeature::LanduseRailway: return tr("Railway");
    case GeoDataFeature::LanduseReservoir: return tr("Reservoir");
    case GeoDataFeature::LanduseResidential: return tr("Residential");
    case GeoDataFeature::LanduseRetail: return tr("Retail");
    case GeoDataFeature::LanduseOrchard: return tr("Orchard");
    case GeoDataFeature::LanduseVineyard: return tr("Vineyard");
    case GeoDataFeature::RailwayRail: return tr("Rail");
    case GeoDataFeature::RailwayNarrowGauge: return tr("Narrow Gauge");
    case GeoDataFeature::RailwayTram: return tr("Tram");
    case GeoDataFeature::RailwayLightRail: return tr("Light Rail");
    case GeoDataFeature::RailwayAbandoned: return tr("Abandoned Railway");
    case GeoDataFeature::RailwaySubway: return tr("Subway");
    case GeoDataFeature::RailwayPreserved: return tr("Preserved Railway");
    case GeoDataFeature::RailwayMiniature: return tr("Miniature Railway");
    case GeoDataFeature::RailwayConstruction: return tr("Railway Construction");
    case GeoDataFeature::RailwayMonorail: return tr("Monorail");
    case GeoDataFeature::RailwayFunicular: return tr("Funicular Railway");
    case GeoDataFeature::PowerTower: return tr("Power Tower");
    case GeoDataFeature::Satellite: return tr("Satellite");
    case GeoDataFeature::AdminLevel1: return tr("Admin Boundary (Level 1)");
    case GeoDataFeature::AdminLevel2: return tr("Admin Boundary (Level 2)");
    case GeoDataFeature::AdminLevel3: return tr("Admin Boundary (Level 3)");
    case GeoDataFeature::AdminLevel4: return tr("Admin Boundary (Level 4)");
    case GeoDataFeature::AdminLevel5: return tr("Admin Boundary (Level 5)");
    case GeoDataFeature::AdminLevel6: return tr("Admin Boundary (Level 6)");
    case GeoDataFeature::AdminLevel7: return tr("Admin Boundary (Level 7)");
    case GeoDataFeature::AdminLevel8: return tr("Admin Boundary (Level 8)");
    case GeoDataFeature::AdminLevel9: return tr("Admin Boundary (Level 9)");
    case GeoDataFeature::AdminLevel10: return tr("Admin Boundary (Level 10)");
    case GeoDataFeature::AdminLevel11: return tr("Admin Boundary (Level 11)");
    case GeoDataFeature::BoundaryMaritime: return tr("Boundary (Maritime)");
    case GeoDataFeature::Landmass: return tr("Land Mass");
    case GeoDataFeature::UrbanArea: return tr("Urban Area");
    case GeoDataFeature::InternationalDateLine: return tr("International Date Line");
    case GeoDataFeature::Valley: return tr("Valley");
    case GeoDataFeature::OtherTerrain: return tr("Terrain");
    case GeoDataFeature::Crater: return tr("Crater");
    case GeoDataFeature::Mare: return tr("Sea");
    case GeoDataFeature::MannedLandingSite: return tr("Manned Landing Site");
    case GeoDataFeature::RoboticRover: return tr("Robotic Rover");
    case GeoDataFeature::UnmannedSoftLandingSite: return tr("Unmanned Soft Landing Site");
    case GeoDataFeature::UnmannedHardLandingSite: return tr("Unmanned Hard Landing Site");
    case GeoDataFeature::Mons: return tr("Mountain");
    case GeoDataFeature::Default:
    case GeoDataFeature::Unknown:
    case GeoDataFeature::None:
    case GeoDataFeature::LastIndex: return QString();
    }

    return QString();
}

void Placemark::addTagValue(const QString &key) const
{
    auto const & osmData = m_placemark.osmData();
    QString value = osmData.tagValue(key);
    if (!value.isEmpty()) {
        m_description += " - " + value.replace(';', ", ");
    }
}

void Placemark::addTagDescription(QString &target, const QString &key, const QString &value, const QString &description) const
{
    auto const & osmData = m_placemark.osmData();
    if (osmData.containsTag(key, value)) {
        if (!target.isEmpty()) {
            target += " · ";
        }
        target += description;
    }
}

QString Placemark::addressFromExtendedData() const
{
#ifdef HAVE_QT5_POSITIONING
        QGeoAddress address;
        Marble::GeoDataExtendedData data = m_placemark.extendedData();
        address.setCountry(data.value("country").value().toString());
        address.setState(data.value("state").value().toString());
        address.setCounty(data.value("county").value().toString());

        QString city = data.value("city").value().toString();
        if (city.isEmpty()) {
            city = data.value("town").value().toString();
        }
        if (city.isEmpty()) {
            city = data.value("village").value().toString();
        }
        if (city.isEmpty()) {
            city = data.value("hamlet").value().toString();
        }
        address.setCity(city);
        address.setDistrict(data.value("district").value().toString());
        address.setPostalCode(data.value("postcode").value().toString());

        if (data.value("class").value().toString() != "highway") {
            // Do not set the street for streets itself -- the placemark will have the street name included already

            // Unfortunately QGeoAddress cannot handle house number / street name ordering via its API,
            // so we have to fall back to our own translations
            QString const street = data.value("road").value().toString();
            QString const houseNumber = data.value("house_number").value().toString();
            address.setStreet(formatStreet(street, houseNumber));
        }

        // @todo FIXME Unfortunately QGeoAddress docs claim it wants a three-letter country code that neither OSM nor QLocale provide
        // address.setCountryCode(QLocale::system().name());
        // address.setCountryCode(data.value("country_code").value().toString());

        return address.text().replace("<br/>", ", ");
#else
    return QString();
#endif
}

QString Placemark::addressFromOsmData() const
{
#ifdef HAVE_QT5_POSITIONING
    QGeoAddress address;
    OsmPlacemarkData const data = m_placemark.osmData();
    address.setCountry(data.tagValue("addr:country"));
    address.setState(data.tagValue("addr:state"));
    address.setCity(data.tagValue("addr:city"));
    address.setDistrict(data.tagValue("district"));
    address.setPostalCode(data.tagValue("addr:postcode"));
    QString const street = data.tagValue("addr:street");
    QString const houseNumber = data.tagValue("addr:housenumber");
    address.setStreet(formatStreet(street, houseNumber));
    return address.text().replace("<br/>", ", ");
#else
    return QString();
#endif
}

QString Placemark::formatStreet(const QString &street, const QString &houseNumber) const
{
    return houseNumber.isEmpty() ? street : tr("%1 %2",
        "House number (first argument) and street name (second argument) in an address").arg(houseNumber).arg(street).trimmed();
}

}

#include "moc_Placemark.cpp"
