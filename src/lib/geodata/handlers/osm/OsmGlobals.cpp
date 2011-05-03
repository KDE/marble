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

QColor OsmGlobals::buildingColor( 0xBE, 0xAD, 0xAD );
QColor OsmGlobals::backgroundColor( 0xF1, 0xEE, 0xE8 );
QColor OsmGlobals::waterColor( 0xB5, 0xD0, 0xD0 );

QMap< QString, GeoDataFeature::GeoDataVisualCategory > OsmGlobals::visualCategories()
{
    if ( m_visualCategories.count() < 1 )
        setupCategories();

    return m_visualCategories;
}

void OsmGlobals::setupCategories()
{
    m_visualCategories["amenity=restaurant"] = GeoDataFeature::FoodRestaurant;
    m_visualCategories["amenity=fast_food"] = GeoDataFeature::FoodFastFood;
    m_visualCategories["amenity=pub"] = GeoDataFeature::FoodPub;
    m_visualCategories["amenity=bar"] = GeoDataFeature::FoodBar;
    m_visualCategories["amenity=cafe"] = GeoDataFeature::FoodCafe;
    m_visualCategories["amenity=biergarten"] = GeoDataFeature::FoodBiergarten;

    m_visualCategories["amenity=school"] = GeoDataFeature::EducationSchool;
    m_visualCategories["amenity=college"] = GeoDataFeature::EducationCollege;
    m_visualCategories["amenity=library"] = GeoDataFeature::AmenityLibrary;
    m_visualCategories["amenity=university"] = GeoDataFeature::EducationUniversity;

    m_visualCategories["amenity=bus_station"] = GeoDataFeature::TransportBusStation;
    m_visualCategories["amenity=car_sharing"] = GeoDataFeature::TransportCarShare;
    m_visualCategories["amenity=fuel"] = GeoDataFeature::TransportFuel;
    m_visualCategories["amenity=parking"] = GeoDataFeature::TransportParking;

    m_visualCategories["amenity=bank"] = GeoDataFeature::MoneyBank;

    m_visualCategories["amenity=pharmacy"] = GeoDataFeature::HealthPharmacy;
    m_visualCategories["amenity=hospital"] = GeoDataFeature::HealthHospital;
    m_visualCategories["amenity=doctors"] = GeoDataFeature::HealthDoctors;

    m_visualCategories["amenity=cinema"] = GeoDataFeature::TouristCinema;
    m_visualCategories["amenity=theatre"] = GeoDataFeature::TouristTheatre;
    
    m_visualCategories["amenity=place_of_worship"] = GeoDataFeature::ReligionPlaceOfWorship;

    //FIXME: alcohol != beverages
    m_visualCategories["shop=alcohol"] = GeoDataFeature::ShoppingBeverages;
    m_visualCategories["shop=hifi"] = GeoDataFeature::ShoppingHifi;
    m_visualCategories["shop=supermarket"] = GeoDataFeature::ShoppingSupermarket;
    
    m_visualCategories["religion"] = GeoDataFeature::ReligionPlaceOfWorship;
    m_visualCategories["religion=bahai"] = GeoDataFeature::ReligionBahai;
    m_visualCategories["religion=buddhist"] = GeoDataFeature::ReligionBuddhist;
    m_visualCategories["religion=christian"] = GeoDataFeature::ReligionChristian;
    m_visualCategories["religion=hindu"] = GeoDataFeature::ReligionHindu;
    m_visualCategories["religion=jain"] = GeoDataFeature::ReligionJain;
    m_visualCategories["religion=jewish"] = GeoDataFeature::ReligionJewish;
    m_visualCategories["religion=shinto"] = GeoDataFeature::ReligionShinto;
    m_visualCategories["religion=sikh"] = GeoDataFeature::ReligionSikh;
}

}
}

