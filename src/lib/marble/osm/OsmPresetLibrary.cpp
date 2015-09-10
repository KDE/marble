//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015    Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

#include "OsmPresetLibrary.h"

#include "GeoDataTypes.h"
#include "GeoDataFeature.h"
#include "GeoDataPlacemark.h"
#include "GeoDataStyle.h"
#include "OsmPlacemarkData.h"

#include <QMap>

namespace Marble
{

QMap<OsmPresetLibrary::OsmTag, GeoDataFeature::GeoDataVisualCategory> OsmPresetLibrary::s_visualCategories;
QList<OsmPresetLibrary::OsmTag> OsmPresetLibrary::s_additionalOsmTags;


void OsmPresetLibrary::initializeOsmVisualCategories()
{
    // Only initialize the map once
    if( !s_visualCategories.isEmpty() ) {
        return;
    }

    s_visualCategories[ OsmTag( "admin_level", "1" ) ]              = GeoDataFeature::OtherTerrain;
    s_visualCategories[ OsmTag( "admin_level", "2" ) ]              = GeoDataFeature::OtherTerrain;
    s_visualCategories[ OsmTag( "admin_level", "3" ) ]              = GeoDataFeature::OtherTerrain;
    s_visualCategories[ OsmTag( "admin_level", "4" ) ]              = GeoDataFeature::OtherTerrain;
    s_visualCategories[ OsmTag( "admin_level", "5" ) ]              = GeoDataFeature::OtherTerrain;
    s_visualCategories[ OsmTag( "admin_level", "6" ) ]              = GeoDataFeature::OtherTerrain;
    s_visualCategories[ OsmTag( "admin_level", "7" ) ]              = GeoDataFeature::OtherTerrain;
    s_visualCategories[ OsmTag( "admin_level", "8" ) ]              = GeoDataFeature::OtherTerrain;

    s_visualCategories[ OsmTag( "amenity", "restaurant" ) ]         = GeoDataFeature::FoodRestaurant;
    s_visualCategories[ OsmTag( "amenity", "fast_food" ) ]          = GeoDataFeature::FoodFastFood;
    s_visualCategories[ OsmTag( "amenity", "pub" ) ]                = GeoDataFeature::FoodPub;
    s_visualCategories[ OsmTag( "amenity", "bar" ) ]                = GeoDataFeature::FoodBar;
    s_visualCategories[ OsmTag( "amenity", "cafe" ) ]               = GeoDataFeature::FoodCafe;
    s_visualCategories[ OsmTag( "amenity", "biergarten" ) ]         = GeoDataFeature::FoodBiergarten;
    s_visualCategories[ OsmTag( "amenity", "school" ) ]             = GeoDataFeature::EducationSchool;
    s_visualCategories[ OsmTag( "amenity", "college" ) ]            = GeoDataFeature::EducationCollege;
    s_visualCategories[ OsmTag( "amenity", "library" ) ]            = GeoDataFeature::AmenityLibrary;
    s_visualCategories[ OsmTag( "amenity", "university" ) ]         = GeoDataFeature::EducationUniversity;
    s_visualCategories[ OsmTag( "amenity", "bus_station" ) ]        = GeoDataFeature::TransportBusStation;
    s_visualCategories[ OsmTag( "amenity", "car_sharing" ) ]        = GeoDataFeature::TransportCarShare;
    s_visualCategories[ OsmTag( "amenity", "fuel" ) ]               = GeoDataFeature::TransportFuel;
    s_visualCategories[ OsmTag( "amenity", "parking" ) ]            = GeoDataFeature::TransportParking;
    s_visualCategories[ OsmTag( "amenity", "atm" ) ]                = GeoDataFeature::MoneyAtm;
    s_visualCategories[ OsmTag( "amenity", "bank" ) ]               = GeoDataFeature::MoneyBank;

    s_visualCategories[ OsmTag( "amenity", "bench" ) ]              = GeoDataFeature::AmenityBench;
    s_visualCategories[ OsmTag( "amenity", "courthouse" ) ]         = GeoDataFeature::AmenityCourtHouse;
    s_visualCategories[ OsmTag( "amenity", "firestation" ) ]        = GeoDataFeature::AmenityFireStation;
    s_visualCategories[ OsmTag( "amenity", "hunting_stand" ) ]      = GeoDataFeature::AmenityHuntingStand;
    s_visualCategories[ OsmTag( "amenity", "police" ) ]             = GeoDataFeature::AmenityPolice;
    s_visualCategories[ OsmTag( "amenity", "post_box" ) ]           = GeoDataFeature::AmenityPostBox;
    s_visualCategories[ OsmTag( "amenity", "post_office" ) ]        = GeoDataFeature::AmenityPostOffice;
    s_visualCategories[ OsmTag( "amenity", "prison" ) ]             = GeoDataFeature::AmenityPrison;
    s_visualCategories[ OsmTag( "amenity", "recycling" ) ]          = GeoDataFeature::AmenityRecycling;
    s_visualCategories[ OsmTag( "amenity", "telephone" ) ]          = GeoDataFeature::AmenityTelephone;
    s_visualCategories[ OsmTag( "amenity", "toilets" ) ]            = GeoDataFeature::AmenityToilets;
    s_visualCategories[ OsmTag( "amenity", "townhall" ) ]           = GeoDataFeature::AmenityTownHall;
    s_visualCategories[ OsmTag( "amenity", "waste_basket" ) ]       = GeoDataFeature::AmenityWasteBasket;

    s_visualCategories[ OsmTag( "amenity", "dentist" ) ]            = GeoDataFeature::HealthDentist;
    s_visualCategories[ OsmTag( "amenity", "pharmacy" ) ]           = GeoDataFeature::HealthPharmacy;
    s_visualCategories[ OsmTag( "amenity", "hospital" ) ]           = GeoDataFeature::HealthHospital;
    s_visualCategories[ OsmTag( "amenity", "doctors" ) ]            = GeoDataFeature::HealthDoctors;
    s_visualCategories[ OsmTag( "amenity", "veterinary" ) ]         = GeoDataFeature::HealthVeterinary;

    s_visualCategories[ OsmTag( "amenity", "cinema" ) ]             = GeoDataFeature::TouristCinema;
    s_visualCategories[ OsmTag( "amenity", "theatre" ) ]            = GeoDataFeature::TouristTheatre;
    s_visualCategories[ OsmTag( "amenity", "place_of_worship" ) ]   = GeoDataFeature::ReligionPlaceOfWorship;

    //FIXME: alcohol != beverages
    s_visualCategories[ OsmTag( "shop", "beverages" ) ]             = GeoDataFeature::ShopBeverages;
    s_visualCategories[ OsmTag( "shop", "hifi" ) ]                  = GeoDataFeature::ShopHifi;
    s_visualCategories[ OsmTag( "shop", "supermarket" ) ]           = GeoDataFeature::ShopSupermarket;
    s_visualCategories[ OsmTag( "shop", "alcohol" ) ]               = GeoDataFeature::ShopAlcohol;
    s_visualCategories[ OsmTag( "shop", "bakery" ) ]                = GeoDataFeature::ShopBakery;
    s_visualCategories[ OsmTag( "shop", "confectionery" ) ]         = GeoDataFeature::ShopConfectionery;
    s_visualCategories[ OsmTag( "shop", "convenience" ) ]           = GeoDataFeature::ShopConvenience;
    s_visualCategories[ OsmTag( "shop", "greengrocer" ) ]           = GeoDataFeature::ShopGreengrocer;
    s_visualCategories[ OsmTag( "shop", "seafood" ) ]               = GeoDataFeature::ShopSeafood;
    s_visualCategories[ OsmTag( "shop", "kiosk" ) ]                 = GeoDataFeature::ShopKiosk;
    s_visualCategories[ OsmTag( "shop", "clothes" ) ]               = GeoDataFeature::ShopClothes;
    s_visualCategories[ OsmTag( "shop", "fashion" ) ]               = GeoDataFeature::ShopFashion;
    s_visualCategories[ OsmTag( "shop", "jewelry" ) ]               = GeoDataFeature::ShopJewelry;
    s_visualCategories[ OsmTag( "shop", "shoes" ) ]                 = GeoDataFeature::ShopShoes;
    s_visualCategories[ OsmTag( "shop", "variety_store" ) ]         = GeoDataFeature::ShopVarietyStore;
    s_visualCategories[ OsmTag( "shop", "beauty" ) ]                = GeoDataFeature::ShopBeauty;
    s_visualCategories[ OsmTag( "shop", "chemist" ) ]               = GeoDataFeature::ShopChemist;
    s_visualCategories[ OsmTag( "shop", "cosmetics" ) ]             = GeoDataFeature::ShopCosmetics;
    s_visualCategories[ OsmTag( "shop", "hairdresser" ) ]           = GeoDataFeature::ShopHairdresser;
    s_visualCategories[ OsmTag( "shop", "optician" ) ]              = GeoDataFeature::ShopOptician;
    s_visualCategories[ OsmTag( "shop", "perfumery" ) ]             = GeoDataFeature::ShopPerfumery;
    s_visualCategories[ OsmTag( "shop", "doityourself" ) ]          = GeoDataFeature::ShopDoitYourself;
    s_visualCategories[ OsmTag( "shop", "florist" ) ]               = GeoDataFeature::ShopFlorist;
    s_visualCategories[ OsmTag( "shop", "hardware" ) ]              = GeoDataFeature::ShopHardware;
    s_visualCategories[ OsmTag( "shop", "furniture" ) ]             = GeoDataFeature::ShopFurniture;
    s_visualCategories[ OsmTag( "shop", "electronics" ) ]           = GeoDataFeature::ShopElectronics;
    s_visualCategories[ OsmTag( "shop", "mobile_phone" ) ]          = GeoDataFeature::ShopMobilePhone;
    s_visualCategories[ OsmTag( "shop", "bicycle" ) ]               = GeoDataFeature::ShopBicycle;
    s_visualCategories[ OsmTag( "shop", "car" ) ]                   = GeoDataFeature::ShopCar;
    s_visualCategories[ OsmTag( "shop", "car_repair" ) ]            = GeoDataFeature::ShopCarRepair;
    s_visualCategories[ OsmTag( "shop", "car_parts" ) ]             = GeoDataFeature::ShopCarParts;
    s_visualCategories[ OsmTag( "shop", "motorcycle" ) ]            = GeoDataFeature::ShopMotorcycle;
    s_visualCategories[ OsmTag( "shop", "musical_instrument" ) ]    = GeoDataFeature::ShopMusicalInstrument;
    s_visualCategories[ OsmTag( "shop", "photo" ) ]                 = GeoDataFeature::ShopPhoto;
    s_visualCategories[ OsmTag( "shop", "books" ) ]                 = GeoDataFeature::ShopBook;
    s_visualCategories[ OsmTag( "shop", "gift" ) ]                  = GeoDataFeature::ShopGift;
    s_visualCategories[ OsmTag( "shop", "stationery" ) ]            = GeoDataFeature::ShopStationery;
    s_visualCategories[ OsmTag( "shop", "laundry" ) ]               = GeoDataFeature::ShopLaundry;
    s_visualCategories[ OsmTag( "shop", "pet" ) ]                   = GeoDataFeature::ShopPet;
    s_visualCategories[ OsmTag( "shop", "toys" ) ]                  = GeoDataFeature::ShopToys;
    s_visualCategories[ OsmTag( "shop", "travel_agency" ) ]         = GeoDataFeature::ShopTravelAgency;


    s_visualCategories[ OsmTag( "religion", "" ) ]                  = GeoDataFeature::ReligionPlaceOfWorship;
    s_visualCategories[ OsmTag( "religion", "bahai" ) ]             = GeoDataFeature::ReligionBahai;
    s_visualCategories[ OsmTag( "religion", "buddhist" ) ]          = GeoDataFeature::ReligionBuddhist;
    s_visualCategories[ OsmTag( "religion", "christian" ) ]         = GeoDataFeature::ReligionChristian;
    s_visualCategories[ OsmTag( "religion", "hindu" ) ]             = GeoDataFeature::ReligionHindu;
    s_visualCategories[ OsmTag( "religion", "jain" ) ]              = GeoDataFeature::ReligionJain;
    s_visualCategories[ OsmTag( "religion", "jewish" ) ]            = GeoDataFeature::ReligionJewish;
    s_visualCategories[ OsmTag( "religion", "shinto" ) ]            = GeoDataFeature::ReligionShinto;
    s_visualCategories[ OsmTag( "religion", "sikh" ) ]              = GeoDataFeature::ReligionSikh;

    s_visualCategories[ OsmTag( "tourism", "attraction" ) ]         = GeoDataFeature::TouristAttraction;
    s_visualCategories[ OsmTag( "tourism", "camp_site" ) ]          = GeoDataFeature::AccomodationCamping;
    s_visualCategories[ OsmTag( "tourism", "hostel" ) ]             = GeoDataFeature::AccomodationHostel;
    s_visualCategories[ OsmTag( "tourism", "hotel" ) ]              = GeoDataFeature::AccomodationHotel;
    s_visualCategories[ OsmTag( "tourism", "motel" ) ]              = GeoDataFeature::AccomodationMotel;
    s_visualCategories[ OsmTag( "tourism", "museum" ) ]             = GeoDataFeature::TouristMuseum;
    s_visualCategories[ OsmTag( "tourism", "theme_park" ) ]         = GeoDataFeature::TouristThemePark;
    s_visualCategories[ OsmTag( "tourism", "viewpoint" ) ]          = GeoDataFeature::TouristViewPoint;
    s_visualCategories[ OsmTag( "tourism", "zoo" ) ]                = GeoDataFeature::TouristZoo;


    s_visualCategories[ OsmTag( "historic", "castle" ) ]            = GeoDataFeature::TouristCastle;
    s_visualCategories[ OsmTag( "historic", "fort" ) ]              = GeoDataFeature::TouristCastle;
    s_visualCategories[ OsmTag( "historic", "monument" ) ]          = GeoDataFeature::TouristMonument;
    s_visualCategories[ OsmTag( "historic", "ruins" ) ]             = GeoDataFeature::TouristRuin;

    s_visualCategories[ OsmTag( "highway", "traffic_signals" ) ]    = GeoDataFeature::HighwayTrafficSignals;

    s_visualCategories[ OsmTag( "highway", "unknown" ) ]            = GeoDataFeature::HighwayUnknown;
    s_visualCategories[ OsmTag( "highway", "steps" ) ]              = GeoDataFeature::HighwaySteps;
    s_visualCategories[ OsmTag( "highway", "footway" ) ]            = GeoDataFeature::HighwayFootway;
    s_visualCategories[ OsmTag( "highway", "path" ) ]               = GeoDataFeature::HighwayPath;
    s_visualCategories[ OsmTag( "highway", "track" ) ]              = GeoDataFeature::HighwayTrack;
    s_visualCategories[ OsmTag( "highway", "pedestrian" ) ]         = GeoDataFeature::HighwayPedestrian;
    s_visualCategories[ OsmTag( "highway", "service" ) ]            = GeoDataFeature::HighwayService;
    s_visualCategories[ OsmTag( "highway", "living_street" ) ]      = GeoDataFeature::HighwayLivingStreet;
    s_visualCategories[ OsmTag( "highway", "unclassified" ) ]       = GeoDataFeature::HighwayUnclassified;
    s_visualCategories[ OsmTag( "highway", "residential" ) ]        = GeoDataFeature::HighwayUnclassified;
    s_visualCategories[ OsmTag( "highway", "road" ) ]               = GeoDataFeature::HighwayRoad;
    s_visualCategories[ OsmTag( "highway", "tertiary_link" ) ]      = GeoDataFeature::HighwayTertiaryLink;
    s_visualCategories[ OsmTag( "highway", "tertiary" ) ]           = GeoDataFeature::HighwayTertiary;
    s_visualCategories[ OsmTag( "highway", "secondary_link" ) ]     = GeoDataFeature::HighwaySecondaryLink;
    s_visualCategories[ OsmTag( "highway", "secondary" ) ]          = GeoDataFeature::HighwaySecondary;
    s_visualCategories[ OsmTag( "highway", "primary_link" ) ]       = GeoDataFeature::HighwayPrimaryLink;
    s_visualCategories[ OsmTag( "highway", "primary" ) ]            = GeoDataFeature::HighwayPrimary;
    s_visualCategories[ OsmTag( "highway", "trunk_link" ) ]         = GeoDataFeature::HighwayTrunkLink;
    s_visualCategories[ OsmTag( "highway", "trunk" ) ]              = GeoDataFeature::HighwayTrunk;
    s_visualCategories[ OsmTag( "highway", "motorway_link" ) ]      = GeoDataFeature::HighwayMotorwayLink;
    s_visualCategories[ OsmTag( "highway", "motorway" ) ]           = GeoDataFeature::HighwayMotorway;

    s_visualCategories[ OsmTag( "natural", "water" ) ]              = GeoDataFeature::NaturalWater;
    s_visualCategories[ OsmTag( "waterway", "stream" ) ]            = GeoDataFeature::NaturalWater;
    s_visualCategories[ OsmTag( "waterway", "river" ) ]             = GeoDataFeature::NaturalWater;
    s_visualCategories[ OsmTag( "waterway", "riverbank" ) ]         = GeoDataFeature::NaturalWater;
    s_visualCategories[ OsmTag( "waterway", "canal" ) ]             = GeoDataFeature::NaturalWater;

    s_visualCategories[ OsmTag( "natural", "wood" ) ]               = GeoDataFeature::NaturalWood;
    s_visualCategories[ OsmTag( "natural", "beach" ) ]              = GeoDataFeature::NaturalBeach;
    s_visualCategories[ OsmTag( "natural", "wetland" ) ]            = GeoDataFeature::NaturalWetland;
    s_visualCategories[ OsmTag( "natural", "glacier" ) ]            = GeoDataFeature::NaturalGlacier;

    s_visualCategories[ OsmTag( "landuse", "forest" ) ]             = GeoDataFeature::NaturalWood;
    s_visualCategories[ OsmTag( "landuse", "allotments" ) ]         = GeoDataFeature::LanduseAllotments;
    s_visualCategories[ OsmTag( "landuse", "basin" ) ]              = GeoDataFeature::LanduseBasin;
    s_visualCategories[ OsmTag( "landuse", "brownfield" ) ]         = GeoDataFeature::LanduseConstruction;
    s_visualCategories[ OsmTag( "landuse", "cemetery" ) ]           = GeoDataFeature::LanduseCemetery;
    s_visualCategories[ OsmTag( "landuse", "commercial" ) ]         = GeoDataFeature::LanduseCommercial;
    s_visualCategories[ OsmTag( "landuse", "construction" ) ]       = GeoDataFeature::LanduseConstruction;
    s_visualCategories[ OsmTag( "landuse", "farm" ) ]               = GeoDataFeature::LanduseFarmland;
    s_visualCategories[ OsmTag( "landuse", "farmland" ) ]           = GeoDataFeature::LanduseFarmland;
    s_visualCategories[ OsmTag( "landuse", "farmyard" ) ]           = GeoDataFeature::LanduseFarmland;
    s_visualCategories[ OsmTag( "landuse", "garages" ) ]            = GeoDataFeature::LanduseGarages;
    s_visualCategories[ OsmTag( "landuse", "greenfield" ) ]         = GeoDataFeature::LanduseConstruction;
    s_visualCategories[ OsmTag( "landuse", "industrial" ) ]         = GeoDataFeature::LanduseIndustrial;
    s_visualCategories[ OsmTag( "landuse", "landfill" ) ]           = GeoDataFeature::LanduseLandfill;
    s_visualCategories[ OsmTag( "landuse", "meadow" ) ]             = GeoDataFeature::LanduseMeadow;
    s_visualCategories[ OsmTag( "landuse", "military" ) ]           = GeoDataFeature::LanduseMilitary;
    s_visualCategories[ OsmTag( "landuse", "orchard" ) ]            = GeoDataFeature::LanduseFarmland;
    s_visualCategories[ OsmTag( "landuse", "quarry" ) ]             = GeoDataFeature::LanduseQuarry;
    s_visualCategories[ OsmTag( "landuse", "railway" ) ]            = GeoDataFeature::LanduseRailway;
    s_visualCategories[ OsmTag( "landuse", "reservoir" ) ]          = GeoDataFeature::LanduseReservoir;
    s_visualCategories[ OsmTag( "landuse", "residential" ) ]        = GeoDataFeature::LanduseResidential;
    s_visualCategories[ OsmTag( "landuse", "retail" ) ]             = GeoDataFeature::LanduseRetail;

    s_visualCategories[ OsmTag( "leisure", "park" ) ]               = GeoDataFeature::LeisurePark;
    s_visualCategories[ OsmTag( "leisure", "pitch" ) ]              = GeoDataFeature::LeisurePark;
    s_visualCategories[ OsmTag( "leisure", "playground" ) ]         = GeoDataFeature::LeisurePlayground;

    s_visualCategories[ OsmTag( "railway", "rail" ) ]               = GeoDataFeature::RailwayRail;
    s_visualCategories[ OsmTag( "railway", "tram" ) ]               = GeoDataFeature::RailwayTram;
    s_visualCategories[ OsmTag( "railway", "light_rail" ) ]         = GeoDataFeature::RailwayLightRail;
    s_visualCategories[ OsmTag( "railway", "preserved" ) ]          = GeoDataFeature::RailwayPreserved;
    s_visualCategories[ OsmTag( "railway", "abandoned" ) ]          = GeoDataFeature::RailwayAbandoned;
    s_visualCategories[ OsmTag( "railway", "disused" ) ]            = GeoDataFeature::RailwayAbandoned;
    s_visualCategories[ OsmTag( "railway", "subway" ) ]             = GeoDataFeature::RailwaySubway;
    s_visualCategories[ OsmTag( "railway", "miniature" ) ]          = GeoDataFeature::RailwayMiniature;
    s_visualCategories[ OsmTag( "railway", "construction" ) ]       = GeoDataFeature::RailwayConstruction;
    s_visualCategories[ OsmTag( "railway", "monorail" ) ]           = GeoDataFeature::RailwayMonorail;
    s_visualCategories[ OsmTag( "railway", "funicular" ) ]          = GeoDataFeature::RailwayFunicular;
    s_visualCategories[ OsmTag( "railway", "station" ) ]            = GeoDataFeature::TransportTrainStation;

    s_visualCategories[ OsmTag( "transport", "aerodrome" ) ]        = GeoDataFeature::TransportAerodrome;
    s_visualCategories[ OsmTag( "transport", "airpor_terminal" ) ]  = GeoDataFeature::TransportAirportTerminal;
    s_visualCategories[ OsmTag( "transport", "bus_station" ) ]      = GeoDataFeature::TransportBusStation;
    s_visualCategories[ OsmTag( "highway", "bus_stop" ) ]           = GeoDataFeature::TransportBusStop;
    s_visualCategories[ OsmTag( "transport", "car_share" ) ]        = GeoDataFeature::TransportCarShare;
    s_visualCategories[ OsmTag( "transport", "fuel" ) ]             = GeoDataFeature::TransportFuel;
    s_visualCategories[ OsmTag( "transport", "parking" ) ]          = GeoDataFeature::TransportParking;
    s_visualCategories[ OsmTag( "transport", "rental_bicycle" ) ]   = GeoDataFeature::TransportRentalBicycle;
    s_visualCategories[ OsmTag( "transport", "rental_car" ) ]       = GeoDataFeature::TransportRentalCar;
    s_visualCategories[ OsmTag( "transport", "taxi_rank" ) ]        = GeoDataFeature::TransportTaxiRank;
    s_visualCategories[ OsmTag( "transport", "train_station" ) ]    = GeoDataFeature::TransportTrainStation;
    s_visualCategories[ OsmTag( "transport", "tram_stop" ) ]        = GeoDataFeature::TransportTramStop;
    s_visualCategories[ OsmTag( "transport", "bus_stop" ) ]           = GeoDataFeature::TransportBusStop;

    s_visualCategories[ OsmTag( "place", "city" ) ]                 = GeoDataFeature::LargeCity;
    s_visualCategories[ OsmTag( "place", "hamlet" ) ]               = GeoDataFeature::SmallCity;
    s_visualCategories[ OsmTag( "place", "locality" ) ]             = GeoDataFeature::SmallCity;
    s_visualCategories[ OsmTag( "place", "town" ) ]                 = GeoDataFeature::BigCity;
    s_visualCategories[ OsmTag( "place", "village" ) ]              = GeoDataFeature::MediumCity;

    // Default for buildings
    foreach(const QString &value, buildingValues() ) {
        s_visualCategories[ OsmTag( "building", value ) ]           = GeoDataFeature::Building;
    }
}

void OsmPresetLibrary::initializeAdditionalOsmTags()
{
    // Only initialize the list once
    if( !s_additionalOsmTags.isEmpty() ) {
        return;
    }

    // Recommended for nodes
    s_additionalOsmTags << OsmTag( "power", "tower" );
    s_additionalOsmTags << OsmTag( "power", "pole" );
    s_additionalOsmTags << OsmTag( "power", "generator" );
    s_additionalOsmTags << OsmTag( "barrier", "fence" );
    s_additionalOsmTags << OsmTag( "barrier", "wall" );
    s_additionalOsmTags << OsmTag( "barrier", "gate" );
    s_additionalOsmTags << OsmTag( "amenity", "toilets" );



    // Recommended for ways
    s_additionalOsmTags << OsmTag( "lanes", "" );
    s_additionalOsmTags << OsmTag( "maxspeed", "" );
    s_additionalOsmTags << OsmTag( "maxheight", "" );
    s_additionalOsmTags << OsmTag( "maxweight", "" );
    s_additionalOsmTags << OsmTag( "oneway", "yes" );
    s_additionalOsmTags << OsmTag( "service", "driveway" );
    s_additionalOsmTags << OsmTag( "service", "parking_aisle" );
    s_additionalOsmTags << OsmTag( "service", "alley" );
    s_additionalOsmTags << OsmTag( "bridge", "yes" );
    s_additionalOsmTags << OsmTag( "tunnel", "yes" );
    s_additionalOsmTags << OsmTag( "abutters", "commercial" );
    s_additionalOsmTags << OsmTag( "abutters", "industrial" );
    s_additionalOsmTags << OsmTag( "abutters", "mixed" );
    s_additionalOsmTags << OsmTag( "abutters", "residential" );



    // Recommended for areas
    s_additionalOsmTags << OsmTag( "surface", "unpaved" );
    s_additionalOsmTags << OsmTag( "surface", "paved" );
    s_additionalOsmTags << OsmTag( "surface", "gravel" );
    s_additionalOsmTags << OsmTag( "surface", "dirt" );
    s_additionalOsmTags << OsmTag( "surface", "grass" );



    // Relations
    s_additionalOsmTags << OsmTag( "type", "route" );
    s_additionalOsmTags << OsmTag( "type", "route_master" );
    s_additionalOsmTags << OsmTag( "type", "public_transport" );
    s_additionalOsmTags << OsmTag( "type", "destination_sign" );
    s_additionalOsmTags << OsmTag( "type", "waterway" );
    s_additionalOsmTags << OsmTag( "type", "enforcement" );



    // Relations: route
    s_additionalOsmTags << OsmTag( "route", "road" );
    s_additionalOsmTags << OsmTag( "route", "bicycle" );
    s_additionalOsmTags << OsmTag( "route", "foot" );
    s_additionalOsmTags << OsmTag( "route", "hiking" );
    s_additionalOsmTags << OsmTag( "route", "bus" );
    s_additionalOsmTags << OsmTag( "route", "trolleybus" );
    s_additionalOsmTags << OsmTag( "route", "ferry" );
    s_additionalOsmTags << OsmTag( "route", "detour" );
    s_additionalOsmTags << OsmTag( "route", "train" );
    s_additionalOsmTags << OsmTag( "route", "tram" );
    s_additionalOsmTags << OsmTag( "route", "mtb" );
    s_additionalOsmTags << OsmTag( "route", "horse" );
    s_additionalOsmTags << OsmTag( "route", "ski" );
    s_additionalOsmTags << OsmTag( "roundtrip", "yes" );
    s_additionalOsmTags << OsmTag( "network", "" );
    s_additionalOsmTags << OsmTag( "ref", "" );
    s_additionalOsmTags << OsmTag( "operator", "" );



    // Relations: route_master
    s_additionalOsmTags << OsmTag( "route_master", "train" );
    s_additionalOsmTags << OsmTag( "route_master", "subway" );
    s_additionalOsmTags << OsmTag( "route_master", "monorail" );
    s_additionalOsmTags << OsmTag( "route_master", "tram" );
    s_additionalOsmTags << OsmTag( "route_master", "bus" );
    s_additionalOsmTags << OsmTag( "route_master", "trolleybus" );
    s_additionalOsmTags << OsmTag( "route_master", "ferry" );
    s_additionalOsmTags << OsmTag( "route_master", "bicycle" );



    // Relations: public_transport
    s_additionalOsmTags << OsmTag( "public_transport", "stop_area" );
    s_additionalOsmTags << OsmTag( "public_transport", "stop_area_group" );



    // Relations: waterway
    s_additionalOsmTags << OsmTag( "waterway", "river" );
    s_additionalOsmTags << OsmTag( "waterway", "stream" );
    s_additionalOsmTags << OsmTag( "waterway", "canal" );
    s_additionalOsmTags << OsmTag( "waterway", "drain" );
    s_additionalOsmTags << OsmTag( "waterway", "ditch" );



    // Relations: enforcement
    s_additionalOsmTags << OsmTag( "enforcement", "maxheight" );
    s_additionalOsmTags << OsmTag( "enforcement", "maxweight" );
    s_additionalOsmTags << OsmTag( "enforcement", "maxspeed" );
    s_additionalOsmTags << OsmTag( "enforcement", "mindistance" );
    s_additionalOsmTags << OsmTag( "enforcement", "traffic_signals" );
    s_additionalOsmTags << OsmTag( "enforcement", "check" );
    s_additionalOsmTags << OsmTag( "enforcement", "access" );
    s_additionalOsmTags << OsmTag( "enforcement", "toll" );



    // Others
    s_additionalOsmTags << OsmTag( "height", "" );
    s_additionalOsmTags << OsmTag( "rooms", "" );
    s_additionalOsmTags << OsmTag( "beds", "" );
    s_additionalOsmTags << OsmTag( "wheelchair", "" );
    s_additionalOsmTags << OsmTag( "website", "" );
    s_additionalOsmTags << OsmTag( "email", "" );
    s_additionalOsmTags << OsmTag( "fee", "" );
    s_additionalOsmTags << OsmTag( "destination", "" );
    s_additionalOsmTags << OsmTag( "drinking_water", "yes" );
    s_additionalOsmTags << OsmTag( "indoor", "yes" );



    // Recommended for all
    s_additionalOsmTags << OsmTag( "addr:street", "" );
    s_additionalOsmTags << OsmTag( "addr:housenumber", "" );
    s_additionalOsmTags << OsmTag( "addr:postcode", "" );
    s_additionalOsmTags << OsmTag( "addr:country", "" );
    s_additionalOsmTags << OsmTag( "access", "private" );
    s_additionalOsmTags << OsmTag( "access", "permissive" );
}

GeoDataStyle *OsmPresetLibrary::presetStyle( const OsmTag &tag )
{
  GeoDataFeature::GeoDataVisualCategory category = OsmPresetLibrary::OsmVisualCategory( tag.first + "=" + tag.second );
  return GeoDataFeature::presetStyle( category );
}

bool OsmPresetLibrary::hasVisualCategory ( const OsmTag &tag )
{

    return s_visualCategories.contains( tag );
}

GeoDataFeature::GeoDataVisualCategory OsmPresetLibrary::OsmVisualCategory( const QString &keyValue )
{
    initializeOsmVisualCategories();
    QStringList tokens = keyValue.split( '=' );

    if ( tokens.size() != 2 ) {
        return GeoDataFeature::None;
    }

    QString key = tokens.at( 0 );
    QString value = tokens.at( 1 );

    return s_visualCategories.value( OsmTag( key, value ) );
}

QMap<OsmPresetLibrary::OsmTag, GeoDataFeature::GeoDataVisualCategory>::const_iterator OsmPresetLibrary::begin()
{
    initializeOsmVisualCategories();
    return s_visualCategories.begin();
}

QMap<OsmPresetLibrary::OsmTag, GeoDataFeature::GeoDataVisualCategory>::const_iterator OsmPresetLibrary::end()
{
    initializeOsmVisualCategories();
    return s_visualCategories.end();
}

QList<OsmPresetLibrary::OsmTag>::const_iterator OsmPresetLibrary::additionalTagsBegin()
{
    initializeAdditionalOsmTags();
    return s_additionalOsmTags.begin();
}

QList<OsmPresetLibrary::OsmTag>::const_iterator OsmPresetLibrary::additionalTagsEnd()
{
    initializeAdditionalOsmTags();
    return s_additionalOsmTags.end();
}

QStringList OsmPresetLibrary::buildingValues()
{
    // from https://taginfo.openstreetmap.org/keys/building#values
    static QStringList osmBuildingValues = QStringList()
        << "yes" << "house" << "residential" << "garage" << "apartments"
        << "hut" << "industrial" << "detached" << "roof" << "garages"
        << "commercial" << "terrace" << "shed" << "school" << "retail"
        << "farm_auxiliary" << "church" << "greenhouse" << "barn"
        << "service" << "manufacture" << "construction" << "cabin"
        << "farm" << "warehouse" << "House" << "office"
        << "civic" << "Residential" << "hangar" << "public" << "university"
        << "hospital" << "chapel" << "hotel" << "train_station" << "dormitory"
        << "kindergarten" << "stable" << "storage_tank" << "shop" << "college"
        << "supermarket" << "factory" << "bungalow" << "tower" << "silo"
        << "storage" << "station" << "education" << "carport";
    return osmBuildingValues;
}

}

