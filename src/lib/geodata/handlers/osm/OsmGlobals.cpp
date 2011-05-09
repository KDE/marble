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
#include "GeoDataIconStyle.h"
#include "global.h"
#include "MarbleDirs.h"

namespace Marble
{
namespace osm
{
QMap<QString, GeoDataFeature::GeoDataVisualCategory> OsmGlobals::m_visualCategories;
QList<QString> OsmGlobals::m_areaTags;

QColor OsmGlobals::buildingColor( 0xBE, 0xAD, 0xAD );
QColor OsmGlobals::backgroundColor( 0xF1, 0xEE, 0xE8 );

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
    m_visualCategories["highway=footway"]            = GeoDataFeature::HighwayPedestrian;
    m_visualCategories["highway=path"]               = GeoDataFeature::HighwayPath;
    m_visualCategories["highway=track"]              = GeoDataFeature::HighwayTrack;
    m_visualCategories["highway=pedestrian"]         = GeoDataFeature::HighwayPedestrian;
    m_visualCategories["highway=service"]            = GeoDataFeature::HighwayService;
    m_visualCategories["highway=living_street"]      = GeoDataFeature::HighwayRoad;
    m_visualCategories["highway=unclassified"]       = GeoDataFeature::HighwayRoad;
    m_visualCategories["highway=residential"]        = GeoDataFeature::HighwayRoad;
    m_visualCategories["highway=tertiary_link"]      = GeoDataFeature::HighwayTertiary;
    m_visualCategories["highway=tertiary"]           = GeoDataFeature::HighwayTertiary;
    m_visualCategories["highway=secondary_link"]     = GeoDataFeature::HighwaySecondary;
    m_visualCategories["highway=secondary"]          = GeoDataFeature::HighwaySecondary;
    m_visualCategories["highway=primary_link"]       = GeoDataFeature::HighwayPrimary;
    m_visualCategories["highway=primary"]            = GeoDataFeature::HighwayPrimary;
    m_visualCategories["highway=trunk_link"]         = GeoDataFeature::HighwayTrunk;
    m_visualCategories["highway=trunk"]              = GeoDataFeature::HighwayTrunk;
    m_visualCategories["highway=motorway_link"]      = GeoDataFeature::HighwayMotorway;
    m_visualCategories["highway=motorway"]           = GeoDataFeature::HighwayMotorway;
    
    m_visualCategories["waterway=stream"]            = GeoDataFeature::NaturalWater;
    m_visualCategories["waterway=river"]             = GeoDataFeature::NaturalWater;
    m_visualCategories["waterway=riverbank"]         = GeoDataFeature::NaturalWater;
    m_visualCategories["waterway=canal"]             = GeoDataFeature::NaturalWater;
    
    m_visualCategories["natural=wood"]               = GeoDataFeature::NaturalWood;
    
    m_visualCategories["landuse=forest"]             = GeoDataFeature::NaturalWood;
}

void OsmGlobals::setupAreaTags()
{
    m_areaTags.append( "landuse=forest" );
    m_areaTags.append( "natural=wood" );
    m_areaTags.append( "area=yes" );
    m_areaTags.append( "waterway=riverbank" );
    m_areaTags.append( "building=yes" );
    
    qSort( m_areaTags.begin(), m_areaTags.end() );
}

}
}

