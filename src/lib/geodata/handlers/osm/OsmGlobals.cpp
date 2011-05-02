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
QMap<QString, GeoDataStyle*> OsmGlobals::m_poiStyles;
QMap<QString, GeoDataFeature::GeoDataVisualCategory> OsmGlobals::m_visualCategories;

QColor OsmGlobals::buildingColor( 0xBE, 0xAD, 0xAD );
QColor OsmGlobals::backgroundColor( 0xF1, 0xEE, 0xE8 );
QColor OsmGlobals::waterColor( 0xB5, 0xD0, 0xD0 );

QMap< QString, GeoDataStyle* > OsmGlobals::poiStyles()
{
    if ( m_poiStyles.count() < 1 )
        setupPOIStyles();

    return m_poiStyles;
}

QMap< QString, GeoDataFeature::GeoDataVisualCategory > OsmGlobals::visualCategories()
{
    if ( m_visualCategories.count() < 1 )
        setupCategories();

    return m_visualCategories;
}

void OsmGlobals::setupPOIStyles()
{
    appendStyle( "amenity=restaurant", "food_restaurant.p.16.png" );
    appendStyle( "amenity=fast_food", "food_fastfood2.p.16.png" );
    appendStyle( "amenity=pub", "food_pub.p.16.png" );
    appendStyle( "amenity=bar", "food_bar.p.16.png" );
    appendStyle( "amenity=cafe", "food_cafe.p.16.png" );
    appendStyle( "amenity=biergarten", "food_biergarten.p.16.png" );

    appendStyle( "amenity=school", "education_school.p.16.png" );
    appendStyle( "amenity=college", "education_college.p.16.png" );
    appendStyle( "amenity=library", "amenity_library.p.16.png" );
    appendStyle( "amenity=university", "education_university.p.16.png" );

    appendStyle( "amenity=bus_station", "transport_bus_station.p.16.png" );
    appendStyle( "amenity=car_sharing", "transport_car_share.p.16.png" );
    appendStyle( "amenity=fuel", "transport_fuel.p.16.png" );
    appendStyle( "amenity=parking", "transport_parking.p.16.png" );

    appendStyle( "amenity=bank", "money_bank2.p.16.png" );

    appendStyle( "amenity=pharmacy", "health_pharmacy.p.16.png" );
    appendStyle( "amenity=hospital", "health_hospital.p.16.png" );
    appendStyle( "amenity=doctors", "health_doctors2.p.16.png" );

    appendStyle( "amenity=cinema", "tourist_cinema.p.16.png" );
    appendStyle( "amenity=theatre", "tourist_theatre.p.16.png" );
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
}

void OsmGlobals::appendStyle( const QString& name, const QString& icon )
{
    GeoDataIconStyle iconStyle;
    iconStyle.setIconPath( MarbleDirs::path( "bitmaps/poi/" + icon ) );
    GeoDataStyle *style = new GeoDataStyle();
    style->setIconStyle( iconStyle );
    m_poiStyles[name] = style;
}

}
}

