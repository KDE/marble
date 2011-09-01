//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "OsmGlobals.h"
#include "GeoDataStyle.h"
#include "GeoDataPlacemark.h"
#include "GeoDataDocument.h"
#include "GeoDataIconStyle.h"
#include "global.h"
#include "MarbleDirs.h"

namespace Marble
{
namespace osm
{
QMap<QString, GeoDataFeature::GeoDataVisualCategory> OsmGlobals::m_visualCategories;
QList<QString> OsmGlobals::m_areaTags;

QColor OsmGlobals::backgroundColor( 0xF1, 0xEE, 0xE8 );
QList<GeoDataPlacemark*> OsmGlobals::dummyPlacemarks;

QMap< QString, GeoDataFeature::GeoDataVisualCategory > OsmGlobals::visualCategories()
{
    if ( m_visualCategories.count() < 1 )
        setupCategories();

    return m_visualCategories;
}

bool OsmGlobals::tagNeedArea(const QString& keyValue)
{
    if ( m_areaTags.count() < 1 )
        setupAreaTags();
    
    return qBinaryFind( m_areaTags.constBegin(), m_areaTags.constEnd(), keyValue ) != m_areaTags.constEnd();
}


void OsmGlobals::setupCategories()
{
    m_visualCategories["amenity=restaurant"]         = GeoDataFeature::FoodRestaurant;
    m_visualCategories["amenity=fast_food"]          = GeoDataFeature::FoodFastFood;
    m_visualCategories["amenity=pub"]                = GeoDataFeature::FoodPub;
    m_visualCategories["amenity=bar"]                = GeoDataFeature::FoodBar;
    m_visualCategories["amenity=cafe"]               = GeoDataFeature::FoodCafe;
    m_visualCategories["amenity=biergarten"]         = GeoDataFeature::FoodBiergarten;
    m_visualCategories["amenity=school"]             = GeoDataFeature::EducationSchool;
    m_visualCategories["amenity=college"]            = GeoDataFeature::EducationCollege;
    m_visualCategories["amenity=library"]            = GeoDataFeature::AmenityLibrary;
    m_visualCategories["amenity=university"]         = GeoDataFeature::EducationUniversity;
    m_visualCategories["amenity=bus_station"]        = GeoDataFeature::TransportBusStation;
    m_visualCategories["amenity=car_sharing"]        = GeoDataFeature::TransportCarShare;
    m_visualCategories["amenity=fuel"]               = GeoDataFeature::TransportFuel;
    m_visualCategories["amenity=parking"]            = GeoDataFeature::TransportParking;
    m_visualCategories["amenity=bank"]               = GeoDataFeature::MoneyBank;
    m_visualCategories["amenity=pharmacy"]           = GeoDataFeature::HealthPharmacy;
    m_visualCategories["amenity=hospital"]           = GeoDataFeature::HealthHospital;
    m_visualCategories["amenity=doctors"]            = GeoDataFeature::HealthDoctors;
    m_visualCategories["amenity=cinema"]             = GeoDataFeature::TouristCinema;
    m_visualCategories["amenity=theatre"]            = GeoDataFeature::TouristTheatre;
    m_visualCategories["amenity=place_of_worship"]   = GeoDataFeature::ReligionPlaceOfWorship;

    //FIXME: alcohol != beverages
    m_visualCategories["shop=alcohol"]               = GeoDataFeature::ShoppingBeverages;
    m_visualCategories["shop=hifi"]                  = GeoDataFeature::ShoppingHifi;
    m_visualCategories["shop=supermarket"]           = GeoDataFeature::ShoppingSupermarket;
    
    m_visualCategories["religion"]                   = GeoDataFeature::ReligionPlaceOfWorship;
    m_visualCategories["religion=bahai"]             = GeoDataFeature::ReligionBahai;
    m_visualCategories["religion=buddhist"]          = GeoDataFeature::ReligionBuddhist;
    m_visualCategories["religion=christian"]         = GeoDataFeature::ReligionChristian;
    m_visualCategories["religion=hindu"]             = GeoDataFeature::ReligionHindu;
    m_visualCategories["religion=jain"]              = GeoDataFeature::ReligionJain;
    m_visualCategories["religion=jewish"]            = GeoDataFeature::ReligionJewish;
    m_visualCategories["religion=shinto"]            = GeoDataFeature::ReligionShinto;
    m_visualCategories["religion=sikh"]              = GeoDataFeature::ReligionSikh;
    
    m_visualCategories["tourism=attraction"]         = GeoDataFeature::TouristAttraction;
    m_visualCategories["tourism=camp_site"]          = GeoDataFeature::AccomodationCamping;
    m_visualCategories["tourism=hostel"]             = GeoDataFeature::AccomodationHostel;
    m_visualCategories["tourism=hotel"]              = GeoDataFeature::AccomodationHotel;
    m_visualCategories["tourism=motel"]              = GeoDataFeature::AccomodationMotel;
    m_visualCategories["tourism=museum"]             = GeoDataFeature::TouristMuseum;
    m_visualCategories["tourism=theme_park"]         = GeoDataFeature::TouristThemePark;
    m_visualCategories["tourism=viewpoint"]          = GeoDataFeature::TouristViewPoint;
    m_visualCategories["tourism=zoo"]                = GeoDataFeature::TouristZoo;
    
    m_visualCategories["historic=castle"]            = GeoDataFeature::TouristCastle;
    m_visualCategories["historic=fort"]              = GeoDataFeature::TouristCastle;
    m_visualCategories["historic=monument"]          = GeoDataFeature::TouristMonument;
    m_visualCategories["historic=ruins"]             = GeoDataFeature::TouristRuin;
    
    m_visualCategories["highway"]                    = GeoDataFeature::HighwayUnknown;  
    m_visualCategories["highway=steps"]              = GeoDataFeature::HighwaySteps;
    m_visualCategories["highway=footway"]            = GeoDataFeature::HighwayPedestrian;
    m_visualCategories["highway=path"]               = GeoDataFeature::HighwayPath;
    m_visualCategories["highway=track"]              = GeoDataFeature::HighwayTrack;
    m_visualCategories["highway=pedestrian"]         = GeoDataFeature::HighwayPedestrian;
    m_visualCategories["highway=service"]            = GeoDataFeature::HighwayService;
    m_visualCategories["highway=living_street"]      = GeoDataFeature::HighwayRoad;
    m_visualCategories["highway=unclassified"]       = GeoDataFeature::HighwayRoad;
    m_visualCategories["highway=residential"]        = GeoDataFeature::HighwayRoad;
    m_visualCategories["highway=tertiary_link"]      = GeoDataFeature::HighwayTertiaryLink;
    m_visualCategories["highway=tertiary"]           = GeoDataFeature::HighwayTertiary;
    m_visualCategories["highway=secondary_link"]     = GeoDataFeature::HighwaySecondaryLink;
    m_visualCategories["highway=secondary"]          = GeoDataFeature::HighwaySecondary;
    m_visualCategories["highway=primary_link"]       = GeoDataFeature::HighwayPrimaryLink;
    m_visualCategories["highway=primary"]            = GeoDataFeature::HighwayPrimary;
    m_visualCategories["highway=trunk_link"]         = GeoDataFeature::HighwayTrunkLink;
    m_visualCategories["highway=trunk"]              = GeoDataFeature::HighwayTrunk;
    m_visualCategories["highway=motorway_link"]      = GeoDataFeature::HighwayMotorwayLink;
    m_visualCategories["highway=motorway"]           = GeoDataFeature::HighwayMotorway;
    
    m_visualCategories["waterway=stream"]            = GeoDataFeature::NaturalWater;
    m_visualCategories["waterway=river"]             = GeoDataFeature::NaturalWater;
    m_visualCategories["waterway=riverbank"]         = GeoDataFeature::NaturalWater;
    m_visualCategories["waterway=canal"]             = GeoDataFeature::NaturalWater;
    
    m_visualCategories["natural=wood"]               = GeoDataFeature::NaturalWood;
  
    m_visualCategories["landuse=forest"]             = GeoDataFeature::NaturalWood;
    m_visualCategories["landuse=allotments"]         = GeoDataFeature::LanduseAllotments;
    m_visualCategories["landuse=basin"]              = GeoDataFeature::LanduseBasin;
    m_visualCategories["landuse=brownfield"]         = GeoDataFeature::LanduseConstruction;
    m_visualCategories["landuse=cemetery"]           = GeoDataFeature::LanduseCemetery;
    m_visualCategories["landuse=commercial"]         = GeoDataFeature::LanduseCommercial;
    m_visualCategories["landuse=construction"]       = GeoDataFeature::LanduseConstruction;
    m_visualCategories["landuse=farm"]               = GeoDataFeature::LanduseFarmland;
    m_visualCategories["landuse=farmland"]           = GeoDataFeature::LanduseFarmland;
    m_visualCategories["landuse=farmyard"]           = GeoDataFeature::LanduseFarmyard;
    m_visualCategories["landuse=garages"]            = GeoDataFeature::LanduseGarages;
    m_visualCategories["landuse=greenfield"]         = GeoDataFeature::LanduseConstruction;
    m_visualCategories["landuse=industrial"]         = GeoDataFeature::LanduseIndustrial;
    m_visualCategories["landuse=landfill"]           = GeoDataFeature::LanduseLandfill;
    m_visualCategories["landuse=meadow"]             = GeoDataFeature::LanduseMeadow;
    m_visualCategories["landuse=military"]           = GeoDataFeature::LanduseMilitary;
    m_visualCategories["landuse=orchard"]            = GeoDataFeature::LanduseFarmland;
    m_visualCategories["landuse=quarry"]             = GeoDataFeature::LanduseQuarry;
    m_visualCategories["landuse=railway"]            = GeoDataFeature::LanduseRailway;
    m_visualCategories["landuse=reservoir"]          = GeoDataFeature::LanduseReservoir;
    m_visualCategories["landuse=residential"]        = GeoDataFeature::LanduseResidential;
    m_visualCategories["landuse=retail"]             = GeoDataFeature::LanduseRetail;
    
    m_visualCategories["leisure=park"]               = GeoDataFeature::LeisurePark;
    
    m_visualCategories["railway=rail"]               = GeoDataFeature::RailwayRail;
    m_visualCategories["railway=tram"]               = GeoDataFeature::RailwayTram;
    m_visualCategories["railway=light_rail"]         = GeoDataFeature::RailwayLightRail;
    m_visualCategories["railway=abandoned"]          = GeoDataFeature::RailwayAbandoned;
    m_visualCategories["railway=disused"]            = GeoDataFeature::RailwayAbandoned;
    m_visualCategories["railway=subway"]             = GeoDataFeature::RailwaySubway;
    m_visualCategories["railway=miniature"]          = GeoDataFeature::RailwayMiniature;
    m_visualCategories["railway=construction"]       = GeoDataFeature::RailwayConstruction;
    m_visualCategories["railway=monorail"]           = GeoDataFeature::RailwayMonorail;
    m_visualCategories["railway=funicular"]          = GeoDataFeature::RailwayFunicular;
}

void OsmGlobals::setupAreaTags()
{
    m_areaTags.append( "landuse=forest" );
    m_areaTags.append( "natural=wood" );
    m_areaTags.append( "area=yes" );
    m_areaTags.append( "waterway=riverbank" );
    m_areaTags.append( "building=yes" );
    m_areaTags.append( "amenity=parking" );
    m_areaTags.append( "leisure=park" );
    
    m_areaTags.append( "landuse=allotments" );
    m_areaTags.append( "landuse=basin" );
    m_areaTags.append( "landuse=brownfield" );
    m_areaTags.append( "landuse=cemetery" );
    m_areaTags.append( "landuse=commercial" );
    m_areaTags.append( "landuse=construction" );
    m_areaTags.append( "landuse=farm" );
    m_areaTags.append( "landuse=farmland" );
    m_areaTags.append( "landuse=farmyard" );
    m_areaTags.append( "landuse=garages" );
    m_areaTags.append( "landuse=greenfield" );
    m_areaTags.append( "landuse=industrial" );
    m_areaTags.append( "landuse=landfill" );
    m_areaTags.append( "landuse=meadow" );
    m_areaTags.append( "landuse=military" );
    m_areaTags.append( "landuse=orchard" );
    m_areaTags.append( "landuse=quarry" );
    m_areaTags.append( "landuse=railway" );
    m_areaTags.append( "landuse=reservoir" );
    m_areaTags.append( "landuse=residential" );
    m_areaTags.append( "landuse=retail" );
    
    qSort( m_areaTags.begin(), m_areaTags.end() );
}

void OsmGlobals::addDummyPlacemark( GeoDataPlacemark* placemark )
{
    dummyPlacemarks << placemark;
}

void OsmGlobals::cleanUpDummyPlacemarks()
{
    foreach( GeoDataFeature* placemark, dummyPlacemarks )
    {
        delete placemark;
    }
    dummyPlacemarks.clear();
}

}
}

