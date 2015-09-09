//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
//


#include "GeoDataFeature.h"
#include "GeoDataFeature_p.h"

#include <QDataStream>
#include <QSize>
#include <QPixmap>

#include "MarbleDirs.h"
#include "MarbleDebug.h"

#include "GeoDataStyle.h"
#include "GeoDataStyleMap.h"

#include "GeoDataContainer.h"
#include "GeoDataDocument.h"
#include "GeoDataFolder.h"
#include "GeoDataPlacemark.h"
#include "GeoDataRegion.h"
#include "GeoDataCamera.h"

namespace Marble
{
QFont GeoDataFeaturePrivate::s_defaultFont = QFont("Sans Serif");
QColor GeoDataFeaturePrivate::s_defaultLabelColor = QColor( Qt::black );

bool GeoDataFeaturePrivate::s_defaultStyleInitialized = false;
GeoDataStyle* GeoDataFeaturePrivate::s_defaultStyle[GeoDataFeature::LastIndex];

GeoDataFeature::GeoDataFeature()
    : d( new GeoDataFeaturePrivate() )
{
    d->ref.ref();
}

GeoDataFeature::GeoDataFeature( const GeoDataFeature& other )
    : GeoDataObject(),
      d( other.d )
{
    d->ref.ref();
}

GeoDataFeature::GeoDataFeature( const QString& name )
    : d( new GeoDataFeaturePrivate() )
{
    d->ref.ref();
    d->m_name = name;
}

GeoDataFeature::GeoDataFeature( GeoDataFeaturePrivate *priv ) 
    : d( priv )
{
    d->ref.ref();
}
    
GeoDataFeature::~GeoDataFeature()
{
    if (!d->ref.deref()) {
        delete d;
    }
}

GeoDataFeaturePrivate* GeoDataFeature::p() const
{
    return static_cast<GeoDataFeaturePrivate*>(d);
}

GeoDataFeature& GeoDataFeature::operator=( const GeoDataFeature& other )
{
    if (!d->ref.deref()) {
        delete d;
    }

    d = other.d;
    d->ref.ref();
    
    return *this;
}

bool GeoDataFeature::equals( const GeoDataFeature &other ) const
{
    if ( !GeoDataObject::equals(other) ||
         p()->m_name != other.p()->m_name ||
         p()->m_snippet != other.p()->m_snippet ||
         p()->m_description != other.p()->m_description ||
         p()->m_descriptionCDATA != other.p()->m_descriptionCDATA ||
         p()->m_address != other.p()->m_address ||
         p()->m_phoneNumber != other.p()->m_phoneNumber ||
         p()->m_styleUrl != other.p()->m_styleUrl ||
         p()->m_popularity != other.p()->m_popularity ||
         p()->m_zoomLevel != other.p()->m_zoomLevel ||
         p()->m_visible != other.p()->m_visible ||
         p()->m_role != other.p()->m_role ||
         p()->m_extendedData != other.p()->m_extendedData ||
         p()->m_timeSpan != other.p()->m_timeSpan ||
         p()->m_timeStamp != other.p()->m_timeStamp ||
         p()->m_region != other.p()->m_region ||
         *style() != *other.style() ) {
        return false;
    }

    if ( (!p()->m_styleMap && other.p()->m_styleMap) ||
         (p()->m_styleMap && !other.p()->m_styleMap) ) {
        return false;
    }

    if ( (p()->m_styleMap && other.p()->m_styleMap) &&
         (*p()->m_styleMap != *other.p()->m_styleMap) ) {
        return false;
    }

    if ( !p()->m_abstractView && !other.p()->m_abstractView ) {
        return true;
    } else if ( (!p()->m_abstractView && other.p()->m_abstractView) ||
                (p()->m_abstractView && !other.p()->m_abstractView) ) {
        return false;
    }

    if ( p()->m_abstractView->nodeType() != other.p()->m_abstractView->nodeType() ) {
        return false;
    }

    if ( p()->m_abstractView->nodeType() == GeoDataTypes::GeoDataCameraType ) {
        GeoDataCamera *thisCam = dynamic_cast<GeoDataCamera*>( p()->m_abstractView );
        GeoDataCamera *otherCam = dynamic_cast<GeoDataCamera*>( other.p()->m_abstractView );
        Q_ASSERT(thisCam && otherCam);

        if ( *thisCam != *otherCam ) {
            return false;
        }
    } else if ( p()->m_abstractView->nodeType() == GeoDataTypes::GeoDataLookAtType ) {
        GeoDataLookAt *thisLookAt = dynamic_cast<GeoDataLookAt*>( p()->m_abstractView );
        GeoDataLookAt *otherLookAt = dynamic_cast<GeoDataLookAt*>( other.p()->m_abstractView );
        Q_ASSERT(thisLookAt && otherLookAt);

        if ( *thisLookAt != *otherLookAt ) {
            return false;
        }
    }

    return true;
}

const char* GeoDataFeature::nodeType() const
{
    return p()->nodeType();
}

EnumFeatureId GeoDataFeature::featureId() const
{
    return d->featureId();
}

void GeoDataFeaturePrivate::initializeDefaultStyles()
{
    // We need to do this similar to the way KCmdLineOptions works in
    // the future: Having a PlacemarkStyleProperty properties[] would
    // help here greatly.

    if ( s_defaultStyleInitialized ) {
        return;
    }
    s_defaultStyleInitialized = true;

    QString defaultFamily = s_defaultFont.family();

#ifdef Q_OS_MACX
    int defaultSize = 10;
#else
    int defaultSize = 8;
#endif

    s_defaultStyle[GeoDataFeature::None]
        = new GeoDataStyle( QString(),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::Default]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/default_location.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::Unknown]
        = new GeoDataStyle( QString(),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::SmallCity]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/city_4_white.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::SmallCountyCapital]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/city_4_yellow.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::SmallStateCapital]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/city_4_orange.png" ),
              QFont( defaultFamily, defaultSize, 50, true  ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::SmallNationCapital]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/city_4_red.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::MediumCity]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/city_3_white.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::MediumCountyCapital]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/city_3_yellow.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::MediumStateCapital]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/city_3_orange.png" ),
              QFont( defaultFamily, defaultSize, 50, true  ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::MediumNationCapital]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/city_3_red.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::BigCity]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/city_2_white.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::BigCountyCapital]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/city_2_yellow.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::BigStateCapital]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/city_2_orange.png" ),
              QFont( defaultFamily, defaultSize, 50, true  ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::BigNationCapital]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/city_2_red.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::LargeCity]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/city_1_white.png" ),
              QFont( defaultFamily, defaultSize, 75, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::LargeCountyCapital]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/city_1_yellow.png" ),
              QFont( defaultFamily, defaultSize, 75, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::LargeStateCapital]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/city_1_orange.png" ),
              QFont( defaultFamily, defaultSize, 75, true  ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::LargeNationCapital]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/city_1_red.png" ),
              QFont( defaultFamily, defaultSize, 75, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::Nation]
        = new GeoDataStyle( QString(),
              QFont( defaultFamily, (int)(defaultSize * 1.2 ), 75, false ), QColor( "#404040" ) );
    // Align area labels centered
    s_defaultStyle[GeoDataFeature::Nation]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    s_defaultStyle[GeoDataFeature::Mountain]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/mountain_1.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::Volcano]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/volcano_1.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::Mons]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/mountain_1.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::Valley]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/valley.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::Continent]
        = new GeoDataStyle( QString(),
              QFont( defaultFamily, (int)(defaultSize * 1.7 ), 50, false ), QColor( "#bf0303" ) );
    // Align area labels centered
    s_defaultStyle[GeoDataFeature::Continent]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    s_defaultStyle[GeoDataFeature::Ocean]
        = new GeoDataStyle( QString(),
              QFont( defaultFamily, (int)(defaultSize * 1.7 ), 50, true ), QColor( "#2c72c7" ) );
    // Align area labels centered
    s_defaultStyle[GeoDataFeature::Ocean]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    s_defaultStyle[GeoDataFeature::OtherTerrain]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/other.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::Crater]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/crater.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::Mare]
        = new GeoDataStyle( QString(),
              QFont( defaultFamily, (int)(defaultSize * 1.7 ), 50, false ), QColor( "#bf0303" ) );
    // Align area labels centered
    s_defaultStyle[GeoDataFeature::Mare]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    s_defaultStyle[GeoDataFeature::GeographicPole]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/pole_1.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::MagneticPole]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/pole_2.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::ShipWreck]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/shipwreck.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.8 ), 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::AirPort]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/airport.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::Observatory]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/observatory.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::Wikipedia]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/wikipedia.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::OsmSite]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/osm.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::Coordinate]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/coordinate.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );


    s_defaultStyle[GeoDataFeature::MannedLandingSite]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/manned_landing.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::RoboticRover]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/robotic_rover.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::UnmannedSoftLandingSite]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/unmanned_soft_landing.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::UnmannedHardLandingSite]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/unmanned_hard_landing.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::Folder]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/folder.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultStyle[GeoDataFeature::Bookmark]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/bookmark.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    QFont const osmFont( defaultFamily, defaultSize, 50, false );
    s_defaultStyle[GeoDataFeature::AccomodationCamping]      = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "accommodation_camping.p.16" );
    s_defaultStyle[GeoDataFeature::AccomodationHostel]       = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "accommodation_hostel.p.16" );
    s_defaultStyle[GeoDataFeature::AccomodationHotel]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "accommodation_hotel2.p.16" );
    s_defaultStyle[GeoDataFeature::AccomodationMotel]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "accommodation_motel.p.16" );
    s_defaultStyle[GeoDataFeature::AccomodationYouthHostel]  = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "accommodation_youth_hostel.p.16" );
    s_defaultStyle[GeoDataFeature::AmenityLibrary]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity_library.p.16" );
    s_defaultStyle[GeoDataFeature::EducationCollege]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "education_college.p.16" );
    s_defaultStyle[GeoDataFeature::EducationSchool]          = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "education_school.p.16" );
    s_defaultStyle[GeoDataFeature::EducationUniversity]      = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "education_university.p.16" );
    s_defaultStyle[GeoDataFeature::FoodBar]                  = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "food_bar.p.16" );
    s_defaultStyle[GeoDataFeature::FoodBiergarten]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "food_biergarten.p.16" );
    s_defaultStyle[GeoDataFeature::FoodCafe]                 = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "food_cafe.p.16" );
    s_defaultStyle[GeoDataFeature::FoodFastFood]             = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "food_fastfood2.p.16" );
    s_defaultStyle[GeoDataFeature::FoodPub]                  = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "food_pub.p.16" );
    s_defaultStyle[GeoDataFeature::FoodRestaurant]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "food_restaurant.p.16" );
    s_defaultStyle[GeoDataFeature::HealthDoctors]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "health_doctors2.p.16" );
    s_defaultStyle[GeoDataFeature::HealthHospital]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "health_hospital.p.16" );
    s_defaultStyle[GeoDataFeature::HealthPharmacy]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "health_pharmacy.p.16" );
    s_defaultStyle[GeoDataFeature::MoneyBank]                = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "money_bank2.p.16" );
    s_defaultStyle[GeoDataFeature::ShoppingBeverages]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shopping_alcohol.p.16" );
    s_defaultStyle[GeoDataFeature::ShoppingHifi]             = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shopping_hifi.p.16" );
    s_defaultStyle[GeoDataFeature::ShoppingSupermarket]      = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shopping_supermarket.p.16" );
    s_defaultStyle[GeoDataFeature::TouristAttraction]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "tourist_attraction.p.16" );
    s_defaultStyle[GeoDataFeature::TouristCastle]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "tourist_castle2.p.16" );
    s_defaultStyle[GeoDataFeature::TouristCinema]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "tourist_cinema.p.16" );
    s_defaultStyle[GeoDataFeature::TouristMonument]          = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "tourist_monument.p.16" );
    s_defaultStyle[GeoDataFeature::TouristMuseum]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "tourist_museum.p.16" );
    s_defaultStyle[GeoDataFeature::TouristRuin]              = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "tourist_ruin.p.16" );
    s_defaultStyle[GeoDataFeature::TouristTheatre]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "tourist_theatre.p.16" );
    s_defaultStyle[GeoDataFeature::TouristThemePark]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "tourist_theme_park.p.16" );
    s_defaultStyle[GeoDataFeature::TouristViewPoint]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "tourist_view_point.p.16" );
    s_defaultStyle[GeoDataFeature::TouristZoo]               = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "tourist_zoo.p.16" );
    s_defaultStyle[GeoDataFeature::TransportAerodrome]       = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transport_aerodrome.p.16" );
    s_defaultStyle[GeoDataFeature::TransportAirportTerminal] = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transport_airport_terminal.p.16" );
    s_defaultStyle[GeoDataFeature::TransportBusStation]      = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transport_bus_station.p.16" );
    s_defaultStyle[GeoDataFeature::TransportBusStop]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transport_bus_stop.p.16" );
    s_defaultStyle[GeoDataFeature::TransportCarShare]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transport_car_share.p.16" );
    s_defaultStyle[GeoDataFeature::TransportFuel]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transport_fuel.p.16" );
    s_defaultStyle[GeoDataFeature::TransportParking]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transport_parking.p.16", "#F6EEB6", QColor( "#F6EEB6" ).darker() );
    s_defaultStyle[GeoDataFeature::TransportTrainStation]    = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transport_train_station.p.16" );
    s_defaultStyle[GeoDataFeature::TransportTramStop]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transport_tram_stop.p.16" );
    s_defaultStyle[GeoDataFeature::TransportRentalBicycle]   = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transport_rental_bicycle.p.16" );
    s_defaultStyle[GeoDataFeature::TransportRentalCar]       = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transport_rental_car.p.16" );
    s_defaultStyle[GeoDataFeature::TransportTaxiRank]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transport_taxi_rank.p.16" );
    s_defaultStyle[GeoDataFeature::ReligionPlaceOfWorship]   = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "place_of_worship_unknown3.p.16" );
    s_defaultStyle[GeoDataFeature::ReligionBahai]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "place_of_worship_bahai3.p.16" );
    s_defaultStyle[GeoDataFeature::ReligionBuddhist]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "place_of_worship_buddhist3.p.16" );
    s_defaultStyle[GeoDataFeature::ReligionChristian]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "place_of_worship_christian3.p.16" );
    s_defaultStyle[GeoDataFeature::ReligionHindu]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "place_of_worship_hindu3.p.16" );
    s_defaultStyle[GeoDataFeature::ReligionJain]             = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "place_of_worship_jain3.p.16" );
    s_defaultStyle[GeoDataFeature::ReligionJewish]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "place_of_worship_jewish3.p.16" );
    s_defaultStyle[GeoDataFeature::ReligionShinto]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "place_of_worship_shinto3.p.16" );
    s_defaultStyle[GeoDataFeature::ReligionSikh]             = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "place_of_worship_sikh3.p.16" );
    
    s_defaultStyle[GeoDataFeature::HighwaySteps]             = GeoDataFeaturePrivate::createStyle( 1, 5, "#F98072", "#F98072", true, true,
                                                                                   Qt::SolidPattern, Qt::CustomDashLine, Qt::FlatCap,
                                                                                   false, QVector< qreal >() << 0.2 << 0.2 );

    s_defaultStyle[GeoDataFeature::HighwayUnknown]           = GeoDataFeaturePrivate::createHighwayStyle( "highway_unknown", "#808080", QColor("#808080").darker(150));
    s_defaultStyle[GeoDataFeature::HighwayPath]              = GeoDataFeaturePrivate::createHighwayStyle( "highway_path", "#ECECEC", "#F49B8C", QFont("Arial"), "000000", 1.0, 2, Qt::DotLine, Qt::RoundCap, true);
    s_defaultStyle[GeoDataFeature::HighwayTrack]             = GeoDataFeaturePrivate::createHighwayStyle( "highway_track", "#ECECEC", "#A27F38", QFont("Arial"), "000000", 1.0, 2, Qt::DashDotLine, Qt::RoundCap, true );
    s_defaultStyle[GeoDataFeature::HighwayPedestrian]        = GeoDataFeaturePrivate::createHighwayStyle( "highway_pedestrian", "#EBEBEB", QColor("#EBEBEB").darker(150), QFont("Arial"), "000000", 1.5, 9);
    s_defaultStyle[GeoDataFeature::HighwayFootway]           = GeoDataFeaturePrivate::createHighwayStyle( "highway_footway", "#ECECEC", "#F49B8C", QFont("Arial"), "000000", 1.0, 2, Qt::DotLine, Qt::RoundCap, true);
    s_defaultStyle[GeoDataFeature::HighwayCycleway]          = GeoDataFeaturePrivate::createHighwayStyle( "highway_cycleway", "#ECECEC", "#4B5CAD", QFont("Arial"), "000000", 1.0, 2, Qt::DotLine, Qt::RoundCap, true);
    s_defaultStyle[GeoDataFeature::HighwayService]           = GeoDataFeaturePrivate::createHighwayStyle( "highway_service", "#FEFEFE", QColor("#FEFEFE").darker(150), QFont("Arial"), "000000", 1.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayLivingStreet]      = GeoDataFeaturePrivate::createHighwayStyle( "highway_living_street", "#CCCCCC", QColor("#CCCCCC").darker(150), QFont("Arial"), "000000", 1.5, 6 );
    s_defaultStyle[GeoDataFeature::HighwayRoad]              = GeoDataFeaturePrivate::createHighwayStyle( "highway_road", "#DCDCDC", QColor("#DCDCDC").darker(150), QFont("Arial"), "000000", 1.5, 6 );
    s_defaultStyle[GeoDataFeature::HighwayUnclassified]      = GeoDataFeaturePrivate::createHighwayStyle( "highway_unclassified", "#FEFEFE", QColor("#FEFEFE").darker(150), QFont("Arial"), "000000", 1.5, 6 );
    s_defaultStyle[GeoDataFeature::HighwayTertiary]          = GeoDataFeaturePrivate::createHighwayStyle( "highway_tertiary", "#F8F8BA", QColor("#F8F8BA").darker(150), QFont("Arial"), "000000", 2.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayTertiaryLink]      = GeoDataFeaturePrivate::createHighwayStyle( "highway_tertiary", "#F8F8BA", QColor("#F8F8BA").darker(150), QFont("Arial"), "000000", 2.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwaySecondary]         = GeoDataFeaturePrivate::createHighwayStyle( "highway_secondary", "#F8D6AA", QColor("#F8D6AA").darker(150), QFont("Arial"), "000000", 2.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwaySecondaryLink]     = GeoDataFeaturePrivate::createHighwayStyle( "highway_secondary", "#F8D6AA", QColor("#F8D6AA").darker(150), QFont("Arial"), "000000", 2.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayPrimary]           = GeoDataFeaturePrivate::createHighwayStyle( "highway_primary", "#EB989A", QColor("#EB989A").darker(150), QFont("Arial"), "000000", 3.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayPrimaryLink]       = GeoDataFeaturePrivate::createHighwayStyle( "highway_primary", "#EB989A", QColor("#EB989A").darker(150), QFont("Arial"), "000000", 2.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayTrunk]             = GeoDataFeaturePrivate::createHighwayStyle( "highway_trunk", "#94D494", QColor("#94D494").darker(150), QFont("Arial"), "000000", 3.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayTrunkLink]         = GeoDataFeaturePrivate::createHighwayStyle( "highway_trunk", "#94D494", QColor("#94D494").darker(150), QFont("Arial"), "000000", 3.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayMotorway]          = GeoDataFeaturePrivate::createHighwayStyle( "highway_motorway", "#809BC0", QColor("#809BC0").darker(150), QFont("Arial"), "000000", 3.0, 10 );
    s_defaultStyle[GeoDataFeature::HighwayMotorwayLink]      = GeoDataFeaturePrivate::createHighwayStyle( "highway_motorway", "#809BC0", QColor("#809BC0").darker(150), QFont("Arial"), "000000", 3.0, 10 );

    s_defaultStyle[GeoDataFeature::NaturalWater]             = GeoDataFeaturePrivate::createStyle( 2, 10, "#B5D0D0", "#B5D0D0", true, true,
                                                                                                   Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false, QVector< qreal >(),
                                                                                                   QFont("Arial"), QColor("#B5D0D0").darker(150));
    s_defaultStyle[GeoDataFeature::NaturalWood]              = GeoDataFeaturePrivate::createWayStyle( "#8DC46C", "#8DC46C", true, true, Qt::SolidPattern, "bitmaps/osmcarto/patterns/forest.png" );
    s_defaultStyle[GeoDataFeature::NaturalBeach]             = GeoDataFeaturePrivate::createWayStyle( "#FFF1BA", "#FFF1BA", true, true, Qt::SolidPattern, "bitmaps/osmcarto/patterns/beach.png" );

    s_defaultStyle[GeoDataFeature::LeisurePark]              = GeoDataFeaturePrivate::createWayStyle( "#CDF6CA", "#CDF6CA" );
    s_defaultStyle[GeoDataFeature::LeisurePlayground]        = GeoDataFeaturePrivate::createWayStyle( "#CCFFF1", "#BDFFED" );

    s_defaultStyle[GeoDataFeature::LanduseAllotments]        = GeoDataFeaturePrivate::createWayStyle( "#E4C6AA", "#E4C6AA", true, true, Qt::SolidPattern, "bitmaps/osmcarto/patterns/allotments.png" );
    s_defaultStyle[GeoDataFeature::LanduseBasin]             = GeoDataFeaturePrivate::createWayStyle( QColor(0xB5, 0xD0, 0xD0, 0x80 ), QColor( 0xB5, 0xD0, 0xD0 ) );
    s_defaultStyle[GeoDataFeature::LanduseCemetery]          = GeoDataFeaturePrivate::createWayStyle( "#A9C9AE", "#A9C9AE" );
    s_defaultStyle[GeoDataFeature::LanduseCommercial]        = GeoDataFeaturePrivate::createWayStyle( Qt::transparent, Qt::transparent, false, false );
    s_defaultStyle[GeoDataFeature::LanduseConstruction]      = GeoDataFeaturePrivate::createWayStyle( Qt::transparent, Qt::transparent, false, false );
    s_defaultStyle[GeoDataFeature::LanduseFarmland]          = GeoDataFeaturePrivate::createWayStyle( Qt::transparent, Qt::transparent, false, false );
    s_defaultStyle[GeoDataFeature::LanduseFarmyard]          = GeoDataFeaturePrivate::createWayStyle( Qt::transparent, Qt::transparent, false, false );
    s_defaultStyle[GeoDataFeature::LanduseGarages]           = GeoDataFeaturePrivate::createWayStyle( "#E0DDCD", "#E0DDCD" );
    s_defaultStyle[GeoDataFeature::LanduseGrass]             = GeoDataFeaturePrivate::createWayStyle( "#A8C8A5", "#A8C8A5" );
    s_defaultStyle[GeoDataFeature::LanduseIndustrial]        = GeoDataFeaturePrivate::createWayStyle( "#DED0D5", "#DED0D5" );
    s_defaultStyle[GeoDataFeature::LanduseLandfill]          = GeoDataFeaturePrivate::createWayStyle( Qt::transparent, Qt::transparent, false, false );
    s_defaultStyle[GeoDataFeature::LanduseMeadow]            = GeoDataFeaturePrivate::createWayStyle( Qt::transparent, Qt::transparent, false, false );
    s_defaultStyle[GeoDataFeature::LanduseMilitary]          = GeoDataFeaturePrivate::createWayStyle( "#F3D8D2", "#F3D8D2", true, true, Qt::BDiagPattern, "bitmaps/osmcarto/patterns/military_red_hatch.png" );
    s_defaultStyle[GeoDataFeature::LanduseQuarry]            = GeoDataFeaturePrivate::createWayStyle( "#C4C2C2", "#C4C2C2", true, true, Qt::SolidPattern, "bitmaps/osmcarto/patterns/quarry.png" );
    s_defaultStyle[GeoDataFeature::LanduseRailway]           = GeoDataFeaturePrivate::createWayStyle( "#DED0D5", "#DED0D5" );
    s_defaultStyle[GeoDataFeature::LanduseReservoir]         = GeoDataFeaturePrivate::createWayStyle( "#B5D0D0", "#B5D0D0" );
    s_defaultStyle[GeoDataFeature::LanduseResidential]       = GeoDataFeaturePrivate::createWayStyle( "#DCDCDC", "#DCDCDC" );
    s_defaultStyle[GeoDataFeature::LanduseRetail]            = GeoDataFeaturePrivate::createWayStyle( Qt::transparent, Qt::transparent, false, false );

    s_defaultStyle[GeoDataFeature::RailwayRail]              = GeoDataFeaturePrivate::createStyle( 2, 5, "#706E70", "#EEEEEE", true, true, Qt::SolidPattern, Qt::CustomDashLine, Qt::FlatCap, true,
                                                                                                   QVector< qreal >() << 2 << 3 );
    s_defaultStyle[GeoDataFeature::RailwayTram]              = GeoDataFeaturePrivate::createStyle( 1, 4, "#706E70", "#706E70", true, true, Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false );
    s_defaultStyle[GeoDataFeature::RailwayLightRail]         = GeoDataFeaturePrivate::createWayStyle( Qt::transparent, Qt::transparent, false, false );
    s_defaultStyle[GeoDataFeature::RailwayAbandoned]         = GeoDataFeaturePrivate::createStyle( 2, 5, Qt::transparent, "#706E70", false, false, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false );
    s_defaultStyle[GeoDataFeature::RailwaySubway]            = GeoDataFeaturePrivate::createWayStyle( Qt::transparent, Qt::transparent, false, false );
    s_defaultStyle[GeoDataFeature::RailwayPreserved]         = GeoDataFeaturePrivate::createStyle( 2, 5, "#EEEEEE", "#706E70", true, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, true );
    s_defaultStyle[GeoDataFeature::RailwayMiniature]         = GeoDataFeaturePrivate::createWayStyle( Qt::transparent, Qt::transparent, false, false );
    s_defaultStyle[GeoDataFeature::RailwayConstruction]      = GeoDataFeaturePrivate::createStyle( 2, 5, "#EEEEEE", "#706E70", true, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, true );
    s_defaultStyle[GeoDataFeature::RailwayMonorail]          = GeoDataFeaturePrivate::createWayStyle( Qt::transparent, Qt::transparent, false, false );
    s_defaultStyle[GeoDataFeature::RailwayFunicular]         = GeoDataFeaturePrivate::createWayStyle( Qt::transparent, Qt::transparent, false, false );

    s_defaultStyle[GeoDataFeature::Building]                 = GeoDataFeaturePrivate::createStyle( 1, 0, QColor( 0xBE, 0xAD, 0xAD ), QColor( 0xBE, 0xAD, 0xAD ).darker(),
                                                                                   true, true, Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false );

    s_defaultStyle[GeoDataFeature::Satellite]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/satellite.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultFont = QFont("Sans Serif");

    QFont tmp;


    // Fonts for areas ...
    tmp = s_defaultStyle[GeoDataFeature::Continent]->labelStyle().font();
    tmp.setLetterSpacing( QFont::AbsoluteSpacing, 2 );
    tmp.setCapitalization( QFont::SmallCaps );
    tmp.setBold( true );
    s_defaultStyle[GeoDataFeature::Continent]->labelStyle().setFont( tmp );

    // Fonts for areas ...
    tmp = s_defaultStyle[GeoDataFeature::Mare]->labelStyle().font();
    tmp.setLetterSpacing( QFont::AbsoluteSpacing, 2 );
    tmp.setCapitalization( QFont::SmallCaps );
    tmp.setBold( true );
    s_defaultStyle[GeoDataFeature::Mare]->labelStyle().setFont( tmp );

    // Now we need to underline the capitals ...

    tmp = s_defaultStyle[GeoDataFeature::SmallNationCapital]->labelStyle().font();
    tmp.setUnderline( true );
    s_defaultStyle[GeoDataFeature::SmallNationCapital]->labelStyle().setFont( tmp );

    tmp = s_defaultStyle[GeoDataFeature::MediumNationCapital]->labelStyle().font();
    tmp.setUnderline( true );
    s_defaultStyle[GeoDataFeature::MediumNationCapital]->labelStyle().setFont( tmp );

    tmp = s_defaultStyle[GeoDataFeature::BigNationCapital]->labelStyle().font();
    tmp.setUnderline( true );
    s_defaultStyle[GeoDataFeature::BigNationCapital]->labelStyle().setFont( tmp );

    tmp = s_defaultStyle[GeoDataFeature::LargeNationCapital]->labelStyle().font();
    tmp.setUnderline( true );
    s_defaultStyle[GeoDataFeature::LargeNationCapital]->labelStyle().setFont( tmp );
}

GeoDataStyle* GeoDataFeature::presetStyle( GeoDataVisualCategory category )
{
    return GeoDataFeaturePrivate::s_defaultStyle[ category ];
}

QFont GeoDataFeature::defaultFont()
{
    return GeoDataFeaturePrivate::s_defaultFont;
}

void GeoDataFeature::setDefaultFont( const QFont& font )
{
    GeoDataFeaturePrivate::s_defaultFont = font;
    GeoDataFeaturePrivate::s_defaultStyleInitialized = false;
}

QColor GeoDataFeature::defaultLabelColor()
{
    return GeoDataFeaturePrivate::s_defaultLabelColor;
}

void GeoDataFeature::setDefaultLabelColor( const QColor& color )
{
    GeoDataFeaturePrivate::s_defaultLabelColor = color;
    GeoDataFeaturePrivate::s_defaultStyleInitialized = false;
}

QString GeoDataFeature::name() const
{
    return d->m_name;
}

void GeoDataFeature::setName( const QString &value )
{
    detach();
    d->m_name = value;
}

GeoDataSnippet GeoDataFeature::snippet() const
{
    return d->m_snippet;
}

void GeoDataFeature::setSnippet( const GeoDataSnippet &snippet )
{
    detach();
    d->m_snippet = snippet;
}

QString GeoDataFeature::address() const
{
    return d->m_address;
}

void GeoDataFeature::setAddress( const QString &value)
{
    detach();
    d->m_address = value;
}

QString GeoDataFeature::phoneNumber() const
{
    return d->m_phoneNumber;
}

void GeoDataFeature::setPhoneNumber( const QString &value)
{
    detach();
    d->m_phoneNumber = value;
}

QString GeoDataFeature::description() const
{
    return d->m_description;
}

void GeoDataFeature::setDescription( const QString &value)
{
    detach();
    d->m_description = value;
}

bool GeoDataFeature::descriptionIsCDATA() const
{
    return d->m_descriptionCDATA;
}

void GeoDataFeature::setDescriptionCDATA( bool cdata )
{
    detach();
    d->m_descriptionCDATA = cdata;
}

const GeoDataAbstractView* GeoDataFeature::abstractView() const
{
    return d->m_abstractView;
}

GeoDataAbstractView *GeoDataFeature::abstractView()
{
    // FIXME: Calling detach() doesn't help at all because the m_abstractView
    // object isn't actually copied in the Private class as well.
    // detach();
    return d->m_abstractView;
}

void GeoDataFeature::setAbstractView( GeoDataAbstractView *abstractView )
{
    detach();
    d->m_abstractView = abstractView;
}

QString GeoDataFeature::styleUrl() const
{
    return d->m_styleUrl;
}

void GeoDataFeature::setStyleUrl( const QString &value )
{
    detach();

    d->m_styleUrl = value;

    if ( value.isEmpty() ) {
        d->m_style = nullptr;
        return;
    }

    QString styleUrl = value;
    styleUrl.remove('#');
    GeoDataObject *object = parent();
    bool found = false;
    while ( object && !found ) {
        if( object->nodeType() == GeoDataTypes::GeoDataDocumentType ) {
            GeoDataDocument *doc = static_cast<GeoDataDocument*> ( object );
            GeoDataStyleMap &styleMap = doc->styleMap( styleUrl );
            if( !styleMap.value( QString( "normal" ) ).isEmpty() ) {
                styleUrl = styleMap.value( QString( "normal" ) );
                styleUrl.remove('#');
            }
            // Not calling setStyle here because we don't want
            // re-parenting of the style
            d->m_style = &doc->style( styleUrl );
            found = true;
        }
        object = object->parent();
    }
}

bool GeoDataFeature::isVisible() const
{
    return d->m_visible;
}

void GeoDataFeature::setVisible( bool value )
{
    detach();
    d->m_visible = value;
}

bool GeoDataFeature::isGloballyVisible() const
{
    if ( parent() == 0 ) {
        return d->m_visible;
    }
    GeoDataContainer *container = static_cast<GeoDataContainer*>( parent() );
    return d->m_visible && container->isGloballyVisible();
}


const GeoDataTimeSpan &GeoDataFeature::timeSpan() const
{
    return d->m_timeSpan;
}

GeoDataTimeSpan &GeoDataFeature::timeSpan()
{
    detach();
    return d->m_timeSpan;
}

void GeoDataFeature::setTimeSpan( const GeoDataTimeSpan &timeSpan )
{
    detach();
    d->m_timeSpan = timeSpan;
}

const GeoDataTimeStamp &GeoDataFeature::timeStamp() const
{
    return d->m_timeStamp;
}

GeoDataTimeStamp &GeoDataFeature::timeStamp()
{
    detach();
    return d->m_timeStamp;
}

void GeoDataFeature::setTimeStamp( const GeoDataTimeStamp &timeStamp )
{
    detach();
    d->m_timeStamp = timeStamp;
}

const GeoDataStyle* GeoDataFeature::style() const
{
    if ( d->m_style != 0 ) {
        return d->m_style;
    } else
    {
        if ( GeoDataFeaturePrivate::s_defaultStyleInitialized == false )
            GeoDataFeaturePrivate::initializeDefaultStyles();

        if ( d->m_visualCategory != None
             && GeoDataFeaturePrivate::s_defaultStyle[ d->m_visualCategory] )
        {
            return GeoDataFeaturePrivate::s_defaultStyle[ d->m_visualCategory ];
        }
        else
        {
            return GeoDataFeaturePrivate::s_defaultStyle[ GeoDataFeature::Default];
        }
    }
}

const GeoDataStyle *GeoDataFeature::customStyle() const
{
    return d->m_style;
}

void GeoDataFeature::setStyle( GeoDataStyle* style )
{
    detach();
    if (style)
        style->setParent( this );
    d->m_style = style;
}

GeoDataExtendedData& GeoDataFeature::extendedData() const
{
    // FIXME: Should call detach(). Maybe don't return reference.
    return d->m_extendedData;
}

void GeoDataFeature::setExtendedData( const GeoDataExtendedData& extendedData )
{
    detach();
    d->m_extendedData = extendedData;
}

GeoDataRegion& GeoDataFeature::region() const
{
    // FIXME: Should call detach(). Maybe don't return reference.
    return d->m_region;
}

void GeoDataFeature::setRegion( const GeoDataRegion& region )
{
    detach();
    d->m_region = region;
}

GeoDataFeature::GeoDataVisualCategory GeoDataFeature::visualCategory() const
{
    return d->m_visualCategory;
}

void GeoDataFeature::setVisualCategory( GeoDataFeature::GeoDataVisualCategory index )
{
    detach();
    d->m_visualCategory = index;
}

const QString GeoDataFeature::role() const
{
    return d->m_role;
}

void GeoDataFeature::setRole( const QString &role )
{
    detach();
    d->m_role = role;
}

const GeoDataStyleMap* GeoDataFeature::styleMap() const
{
    return d->m_styleMap;
}

void GeoDataFeature::setStyleMap( const GeoDataStyleMap* styleMap )
{
    d->m_styleMap = styleMap;
}

int GeoDataFeature::zoomLevel() const
{
    return d->m_zoomLevel;
}

void GeoDataFeature::setZoomLevel( int zoomLevel )
{
    detach();
    d->m_zoomLevel = zoomLevel;
}

qint64 GeoDataFeature::popularity() const
{
    return d->m_popularity;
}

void GeoDataFeature::setPopularity( qint64 popularity )
{
    detach();
    d->m_popularity = popularity;
}

void GeoDataFeature::resetDefaultStyles()
{
    GeoDataFeaturePrivate::s_defaultStyleInitialized = false;
}

void GeoDataFeature::detach()
{
#if QT_VERSION < 0x050000
    if(d->ref == 1)
#else
    if(d->ref.load() == 1)
#endif
        return;

    GeoDataFeaturePrivate* new_d = d->copy();

    if (!d->ref.deref()) {
        delete d;
    }

    d = new_d;
    
    d->ref.ref();
}

void GeoDataFeature::pack( QDataStream& stream ) const
{
    GeoDataObject::pack( stream );

    stream << d->m_name;
    stream << d->m_address;
    stream << d->m_phoneNumber;
    stream << d->m_description;
    stream << d->m_visible;
//    stream << d->m_visualCategory;
    stream << d->m_role;
    stream << d->m_popularity;
    stream << d->m_zoomLevel;
}

void GeoDataFeature::unpack( QDataStream& stream )
{
    detach();
    GeoDataObject::unpack( stream );

    stream >> d->m_name;
    stream >> d->m_address;
    stream >> d->m_phoneNumber;
    stream >> d->m_description;
    stream >> d->m_visible;
//    stream >> (int)d->m_visualCategory;
    stream >> d->m_role;
    stream >> d->m_popularity;
    stream >> d->m_zoomLevel;
}

}
