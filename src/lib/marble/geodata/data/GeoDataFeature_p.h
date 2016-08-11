//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//

#ifndef MARBLE_GEODATAFEATUREPRIVATE_H
#define MARBLE_GEODATAFEATUREPRIVATE_H

#include <QString>
#include <QAtomicInt>

#include "GeoDataExtendedData.h"
#include "GeoDataAbstractView.h"
#include "GeoDataFeature.h"
#include "GeoDataRegion.h"
#include "GeoDataTimeStamp.h"
#include "GeoDataTimeSpan.h"
#include "GeoDataTypes.h"
#include "GeoDataStyle.h"
#include "GeoDataSnippet.h"
#include "MarbleDirs.h"

namespace Marble
{

class GeoDataFeaturePrivate
{
    Q_DECLARE_TR_FUNCTIONS(GeoDataFeature)
  public:
    GeoDataFeaturePrivate() :
        m_name(),
        m_snippet(),
        m_description(),
        m_descriptionCDATA(),
        m_address(),
        m_phoneNumber(),
        m_styleUrl(),
        m_abstractView( 0 ),
        m_popularity( 0 ),
        m_zoomLevel( 1 ),
        m_visible( true ),
        m_visualCategory( GeoDataFeature::Default ),
        m_role(" "),
        m_style( 0 ),
        m_styleMap( 0 ),
        m_extendedData(),
        m_timeSpan(),
        m_timeStamp(),
        m_region(),
        ref( 0 )
    {
    }

    GeoDataFeaturePrivate( const GeoDataFeaturePrivate& other ) :
        m_name( other.m_name ),
        m_snippet( other.m_snippet ),
        m_description( other.m_description ),
        m_descriptionCDATA( other.m_descriptionCDATA),
        m_address( other.m_address ),
        m_phoneNumber( other.m_phoneNumber ),
        m_styleUrl( other.m_styleUrl ),
        m_abstractView( other.m_abstractView ),
        m_popularity( other.m_popularity ),
        m_zoomLevel( other.m_zoomLevel ),
        m_visible( other.m_visible ),
        m_visualCategory( other.m_visualCategory ),
        m_role( other.m_role ),
        m_style( other.m_style ),               //FIXME: both style and stylemap need to be reworked internally!!!!
        m_styleMap( other.m_styleMap ),
        m_extendedData( other.m_extendedData ),
        m_timeSpan( other.m_timeSpan ),
        m_timeStamp( other.m_timeStamp ),
        m_region( other.m_region ),
        ref( 0 )
    {
    }

    GeoDataFeaturePrivate& operator=( const GeoDataFeaturePrivate& other )
    {
        m_name = other.m_name;
        m_snippet = other.m_snippet;
        m_description = other.m_description;
        m_descriptionCDATA = other.m_descriptionCDATA;
        m_address = other.m_address;
        m_phoneNumber = other.m_phoneNumber;
        m_styleUrl = other.m_styleUrl;
        m_abstractView = other.m_abstractView;
        m_popularity = other.m_popularity;
        m_zoomLevel = other.m_zoomLevel;
        m_visible = other.m_visible;
        m_role = other.m_role;
        m_style = other.m_style;
        m_styleMap = other.m_styleMap;
        m_timeSpan = other.m_timeSpan;
        m_timeStamp = other.m_timeStamp;
        m_visualCategory = other.m_visualCategory;
        m_extendedData = other.m_extendedData;
        m_region = other.m_region;
        return *this;
    }
    
    virtual GeoDataFeaturePrivate* copy()
    { 
        GeoDataFeaturePrivate* copy = new GeoDataFeaturePrivate;
        *copy = *this;
        return copy;
    }

    virtual EnumFeatureId featureId() const
    {
        return InvalidFeatureId;
    }

    virtual ~GeoDataFeaturePrivate()
    {
    }

    virtual const char* nodeType() const
    {
        return GeoDataTypes::GeoDataFeatureType;
    }

    QString categoryName() const
    {
        switch (m_visualCategory) {
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
        case GeoDataFeature::AmenityKindergarten: return tr("Kindergarten");
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
        case GeoDataFeature::AmenityShelter: return tr("Shelter");
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
        case GeoDataFeature::TransportAirportGate: return tr("Airport Gate");
        case GeoDataFeature::TransportAirportRunway: return tr("Airport Runway");
        case GeoDataFeature::TransportAirportTaxiway: return tr("Airport Taxiway");
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
        case GeoDataFeature::LeisureMarina: return tr("Marina");
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
        case GeoDataFeature::Bathymetry: return tr("Bathymetry");
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


    QString             m_name;         // Name of the feature. Is shown on screen
    GeoDataSnippet      m_snippet;      // Snippet of the feature.
    QString             m_description;  // A longer textual description
    bool                m_descriptionCDATA; // True if description should be considered CDATA
    QString             m_address;      // The address.  Optional
    QString             m_phoneNumber;  // Phone         Optional
    QString             m_styleUrl;     // styleUrl     Url#tag to a document wide style
    GeoDataAbstractView* m_abstractView; // AbstractView  Optional
    qint64              m_popularity;   // Population/Area/Altitude depending on placemark(!)
    int                 m_zoomLevel;    // Zoom Level of the feature

    bool        m_visible;      // True if this feature should be shown.
    GeoDataFeature::GeoDataVisualCategory  m_visualCategory; // the visual category


    QString       m_role;

    GeoDataStyle::Ptr m_style;
    const GeoDataStyleMap* m_styleMap;

    GeoDataExtendedData m_extendedData;

    GeoDataTimeSpan  m_timeSpan;
    GeoDataTimeStamp m_timeStamp;

    GeoDataRegion m_region;
    
    QAtomicInt  ref;

    // Static members
    static const QSharedPointer<const GeoDataStyle> s_defaultStyle;
};

} // namespace Marble

#endif

