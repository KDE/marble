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
QFont GeoDataFeaturePrivate::s_defaultFont = QFont(QStringLiteral("Sans Serif"));
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

    s_defaultStyle[GeoDataFeature::PlaceCity] = new GeoDataStyle( QString(), QFont( defaultFamily, (int)(defaultSize * 2.0 ), 75, false ), QColor( "#202020" ) );
    s_defaultStyle[GeoDataFeature::PlaceCity]->labelStyle().setAlignment( GeoDataLabelStyle::Center );
    s_defaultStyle[GeoDataFeature::PlaceSuburb] = new GeoDataStyle( QString(), QFont( defaultFamily, (int)(defaultSize * 2.0 ), 75, false ), QColor( "#707070" ) );
    s_defaultStyle[GeoDataFeature::PlaceSuburb]->labelStyle().setAlignment( GeoDataLabelStyle::Center );
    s_defaultStyle[GeoDataFeature::PlaceHamlet] = new GeoDataStyle( QString(), QFont( defaultFamily, (int)(defaultSize * 2.0 ), 75, false ), QColor( "#707070" ) );
    s_defaultStyle[GeoDataFeature::PlaceHamlet]->labelStyle().setAlignment( GeoDataLabelStyle::Center );
    s_defaultStyle[GeoDataFeature::PlaceLocality] = new GeoDataStyle( QString(), QFont( defaultFamily, (int)(defaultSize * 2.0 ), 75, false ), QColor( "#707070" ) );
    s_defaultStyle[GeoDataFeature::PlaceLocality]->labelStyle().setAlignment( GeoDataLabelStyle::Center );
    s_defaultStyle[GeoDataFeature::PlaceTown] = new GeoDataStyle( QString(), QFont( defaultFamily, (int)(defaultSize * 2.0 ), 75, false ), QColor( "#404040" ) );
    s_defaultStyle[GeoDataFeature::PlaceTown]->labelStyle().setAlignment( GeoDataLabelStyle::Center );
    s_defaultStyle[GeoDataFeature::PlaceVillage] = new GeoDataStyle( QString(), QFont( defaultFamily, (int)(defaultSize * 2.0 ), 75, false ), QColor( "#505050" ) );
    s_defaultStyle[GeoDataFeature::PlaceVillage]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

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

    QColor const shopColor("#ac39ac");
    QColor const transportationColor("#0066ff");
    QColor const amenityColor("#734a08");
    QColor const healthColor("#da0092");
    QColor const airTransportColor("#8461C4");
    QColor const educationalAreasAndHospital("#f0f0d8");
    QColor const buildingColor("#beadad");
    QColor const waterColor("#b5d0d0");
    // Allows to visualize multiple repaints of buildings
//    QColor const buildingColor(0, 255, 0, 64);

    QFont const osmFont( defaultFamily, 10, 50, false );
    s_defaultStyle[GeoDataFeature::AccomodationCamping]      = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/camping.16", transportationColor );
    s_defaultStyle[GeoDataFeature::AccomodationHostel]       = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/hostel.16", transportationColor );
    s_defaultStyle[GeoDataFeature::AccomodationHotel]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/hotel.16", transportationColor );
    s_defaultStyle[GeoDataFeature::AccomodationMotel]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/motel.16", transportationColor );
    s_defaultStyle[GeoDataFeature::AccomodationYouthHostel]  = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/hostel.16", transportationColor );
    s_defaultStyle[GeoDataFeature::AccomodationGuestHouse]   = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/guest_house.16", transportationColor );
    s_defaultStyle[GeoDataFeature::AmenityLibrary]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/library.20", amenityColor );
    s_defaultStyle[GeoDataFeature::EducationCollege]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, QString(), amenityColor, educationalAreasAndHospital, amenityColor );
    s_defaultStyle[GeoDataFeature::EducationSchool]          = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, QString(), amenityColor, educationalAreasAndHospital, amenityColor );
    s_defaultStyle[GeoDataFeature::EducationUniversity]      = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, QString(), amenityColor, educationalAreasAndHospital, amenityColor );
    s_defaultStyle[GeoDataFeature::FoodBar]                  = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/bar.16", amenityColor );
    s_defaultStyle[GeoDataFeature::FoodBiergarten]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/biergarten.16", amenityColor );
    s_defaultStyle[GeoDataFeature::FoodCafe]                 = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/cafe.16", amenityColor );
    s_defaultStyle[GeoDataFeature::FoodFastFood]             = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/fast_food.16", amenityColor );
    s_defaultStyle[GeoDataFeature::FoodPub]                  = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/pub.16", amenityColor );
    s_defaultStyle[GeoDataFeature::FoodRestaurant]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/restaurant.16", amenityColor );

    s_defaultStyle[GeoDataFeature::HealthDentist]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "health/dentist.16", healthColor );
    s_defaultStyle[GeoDataFeature::HealthDoctors]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "health/doctors.16", healthColor );
    s_defaultStyle[GeoDataFeature::HealthHospital]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "health/hospital.16", healthColor, educationalAreasAndHospital, amenityColor );
    s_defaultStyle[GeoDataFeature::HealthPharmacy]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "health/pharmacy.16", healthColor );
    s_defaultStyle[GeoDataFeature::HealthVeterinary]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "health/veterinary-14", healthColor );

    s_defaultStyle[GeoDataFeature::MoneyAtm]                 = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/atm.16", amenityColor );
    s_defaultStyle[GeoDataFeature::MoneyBank]                = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/bank.16", amenityColor );

    s_defaultStyle[GeoDataFeature::AmenityArchaeologicalSite] = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/archaeological_site.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityEmbassy]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/embassy.16", transportationColor );
    s_defaultStyle[GeoDataFeature::AmenityEmergencyPhone]    = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/emergency_phone.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityWaterPark]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/water_park.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityCommunityCentre]   = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/community_centre-14", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityFountain]          = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/fountain-14", amenityColor, waterColor, waterColor.darker(150) );
    s_defaultStyle[GeoDataFeature::AmenityNightClub]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/nightclub.18", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityBench]             = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "individual/bench.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityCourtHouse]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/courthouse-16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityFireStation]       = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/firestation.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityHuntingStand]      = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "manmade/hunting-stand.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityPolice]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/police.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityPostBox]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/post_box-12", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityPostOffice]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/post_office-14", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityPrison]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/prison.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityRecycling]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/recycling.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityTelephone]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/telephone.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityToilets]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/toilets.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityTownHall]          = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/town_hall.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityWasteBasket]       = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "individual/waste_basket.10", amenityColor );

    s_defaultStyle[GeoDataFeature::AmenityDrinkingWater]     = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/drinking_water.16", amenityColor );

    s_defaultStyle[GeoDataFeature::NaturalPeak]              = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "individual/peak", amenityColor );
    s_defaultStyle[GeoDataFeature::NaturalPeak]->iconStyle().setScale(0.33);
    s_defaultStyle[GeoDataFeature::NaturalTree]              = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "individual/tree-29", amenityColor ); // tree-16 provides the official icon

    s_defaultStyle[GeoDataFeature::ShopBeverages]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/beverages-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopHifi]                 = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/hifi-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopSupermarket]          = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_supermarket", shopColor );
    s_defaultStyle[GeoDataFeature::ShopAlcohol]              = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_alcohol.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopBakery]               = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_bakery.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopButcher]              = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/butcher-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopConfectionery]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/confectionery.14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopConvenience]          = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_convenience", shopColor );
    s_defaultStyle[GeoDataFeature::ShopGreengrocer]          = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/greengrocer-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopSeafood]              = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/seafood-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopDepartmentStore]      = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/department_store-16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopKiosk]                = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/kiosk-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopBag]                  = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/bag-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopClothes]              = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_clothes.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopFashion]              = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_clothes.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopJewelry]              = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_jewelry.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopShoes]                = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_shoes.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopVarietyStore]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/variety_store-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopBeauty]               = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/beauty-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopChemist]              = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/chemist-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopCosmetics]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/perfumery-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopHairdresser]          = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_hairdresser.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopOptician]             = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_optician.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopPerfumery]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/perfumery-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopDoitYourself]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_diy.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopFlorist]              = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/florist.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopHardware]             = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_diy.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopFurniture]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_furniture.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopElectronics]          = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_electronics.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopMobilePhone]          = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_mobile_phone.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopBicycle]              = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_bicycle.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopCar]                  = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_car", shopColor );
    s_defaultStyle[GeoDataFeature::ShopCarRepair]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shopping_car_repair.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopCarParts]             = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/car_parts-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopMotorcycle]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/motorcycle-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopOutdoor]              = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/outdoor-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopMusicalInstrument]    = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/musical_instrument-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopPhoto]                = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/photo-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopBook]                 = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_books.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopGift]                 = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_gift.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopStationery]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/stationery-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopLaundry]              = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/laundry-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopPet]                  = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop_pet.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopToys]                 = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/toys-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopTravelAgency]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/travel_agency-14", shopColor );
    s_defaultStyle[GeoDataFeature::Shop]                     = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "shop/shop-14", shopColor );

    s_defaultStyle[GeoDataFeature::ManmadeBridge]            = GeoDataFeaturePrivate::createWayStyle( QColor("#b8b8b8"), QColor("transparent"), true, true );
    s_defaultStyle[GeoDataFeature::ManmadeLighthouse]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/lighthouse.16", transportationColor, "#f2efe9", QColor( "#f2efe9" ).darker() );
    s_defaultStyle[GeoDataFeature::ManmadePier]              = GeoDataFeaturePrivate::createStyle(0.0, 3.0, "#f2efe9", "#f2efe9", true, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false );
    s_defaultStyle[GeoDataFeature::ManmadeWaterTower]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/water_tower.16", amenityColor );
    s_defaultStyle[GeoDataFeature::ManmadeWindMill]          = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/windmill.16", amenityColor );

    s_defaultStyle[GeoDataFeature::TouristAttraction]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/tourist_memorial.16", amenityColor );
    s_defaultStyle[GeoDataFeature::TouristCastle]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/cinema.16", amenityColor );
    s_defaultStyle[GeoDataFeature::TouristCinema]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/cinema.16", amenityColor );
    s_defaultStyle[GeoDataFeature::TouristInformation]       = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/information.16", amenityColor );
    s_defaultStyle[GeoDataFeature::TouristMonument]          = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/monument.16", amenityColor );
    s_defaultStyle[GeoDataFeature::TouristMuseum]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/museum.16", amenityColor );
    s_defaultStyle[GeoDataFeature::TouristRuin]              = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, QString(), amenityColor );
    s_defaultStyle[GeoDataFeature::TouristTheatre]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/theatre.16", amenityColor );
    s_defaultStyle[GeoDataFeature::TouristThemePark]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, QString(), amenityColor );
    s_defaultStyle[GeoDataFeature::TouristViewPoint]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/viewpoint.16", amenityColor );
    s_defaultStyle[GeoDataFeature::TouristZoo]               = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, QString(), amenityColor, Qt::transparent );
    s_defaultStyle[GeoDataFeature::TouristAlpineHut]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/alpinehut.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportAerodrome]       = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "airtransport/aerodrome", airTransportColor );
    s_defaultStyle[GeoDataFeature::TransportHelipad]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "airtransport/helipad", airTransportColor );
    s_defaultStyle[GeoDataFeature::TransportAirportTerminal] = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, QString(), airTransportColor );
    s_defaultStyle[GeoDataFeature::TransportBusStation]      = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/bus_station.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportBusStop]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/bus_stop.12", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportCarShare]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/car_share.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportFuel]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/fuel.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportParking]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/parking", transportationColor, "#F6EEB6", QColor( "#F6EEB6" ).darker() );
    s_defaultStyle[GeoDataFeature::TransportParkingSpace]    = GeoDataFeaturePrivate::createWayStyle( "#F6EEB6", QColor( "#F6EEB6" ).darker(), true, true );
    s_defaultStyle[GeoDataFeature::TransportPlatform]        = GeoDataFeaturePrivate::createWayStyle( "#bbbbbb", Qt::transparent, true, false );
    s_defaultStyle[GeoDataFeature::TransportTrainStation]    = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "individual/railway_station", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportTramStop]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, QString(), transportationColor );
    s_defaultStyle[GeoDataFeature::TransportRentalBicycle]   = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/rental_bicycle.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportRentalCar]       = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/rental_car.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportTaxiRank]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/taxi.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportBicycleParking]  = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/bicycle_parking.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportMotorcycleParking] = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/motorcycle_parking.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportSubwayEntrance]  = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "transportation/subway_entrance", transportationColor );
    s_defaultStyle[GeoDataFeature::ReligionPlaceOfWorship]   = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, QString() /* "black/place_of_worship.16" */ );
    s_defaultStyle[GeoDataFeature::ReligionBahai]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, QString() );
    s_defaultStyle[GeoDataFeature::ReligionBuddhist]         = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "black/buddhist.16" );
    s_defaultStyle[GeoDataFeature::ReligionChristian]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "black/christian.16" );
    s_defaultStyle[GeoDataFeature::ReligionMuslim]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "black/muslim.16" );
    s_defaultStyle[GeoDataFeature::ReligionHindu]            = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "black/hinduist.16" );
    s_defaultStyle[GeoDataFeature::ReligionJain]             = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, QString() );
    s_defaultStyle[GeoDataFeature::ReligionJewish]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "black/jewish.16" );
    s_defaultStyle[GeoDataFeature::ReligionShinto]           = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "black/shintoist.16" );
    s_defaultStyle[GeoDataFeature::ReligionSikh]             = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "black/sikhist.16" );

    s_defaultStyle[GeoDataFeature::HighwayTrafficSignals]    = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "individual/traffic_light" );

    s_defaultStyle[GeoDataFeature::PowerTower]               = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "individual/power_tower", QColor( "#888888" ) );

    s_defaultStyle[GeoDataFeature::BarrierGate]              = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "individual/gate" );
    s_defaultStyle[GeoDataFeature::BarrierLiftGate]              = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "individual/liftgate" );
    s_defaultStyle[GeoDataFeature::BarrierWall]              = GeoDataFeaturePrivate::createWayStyle( "#444444", Qt::transparent, true, false );

    s_defaultStyle[GeoDataFeature::HighwaySteps]             = GeoDataFeaturePrivate::createStyle(0.0, 2.0, "#F98072", "#F98072", true, true, Qt::SolidPattern, Qt::CustomDashLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3 );
    s_defaultStyle[GeoDataFeature::HighwayUnknown]           = GeoDataFeaturePrivate::createHighwayStyle( "highway_unknown", "#808080", QColor("#808080").darker(150), QFont(QStringLiteral("Arial")), "000000", 0.0, 1.0);
    s_defaultStyle[GeoDataFeature::HighwayPath]              = GeoDataFeaturePrivate::createHighwayStyle( "highway_path", "#ECECEC", "#F49B8C", QFont(QStringLiteral("Arial")), "000000", 0.0, 1.0, Qt::DotLine, Qt::SquareCap, true);
    s_defaultStyle[GeoDataFeature::HighwayTrack]             = GeoDataFeaturePrivate::createHighwayStyle( "highway_track", "#ECECEC", "#A27F38", QFont(QStringLiteral("Arial")), "000000", 1.0, 1.0, Qt::DashLine, Qt::SquareCap, true );
    s_defaultStyle[GeoDataFeature::HighwayPedestrian]        = GeoDataFeaturePrivate::createHighwayStyle( "highway_pedestrian", "#EBEBEB", QColor("#EBEBEB").darker(150), QFont(QStringLiteral("Arial")), "000000", 0.0, 2.0);
    s_defaultStyle[GeoDataFeature::HighwayFootway]           = GeoDataFeaturePrivate::createHighwayStyle( "highway_footway", "#ECECEC", "#F49B8C", QFont(QStringLiteral("Arial")), "000000", 0.0, 2.0, Qt::DotLine, Qt::SquareCap, true);
    s_defaultStyle[GeoDataFeature::HighwayCycleway]          = GeoDataFeaturePrivate::createHighwayStyle( "highway_cycleway", "#ECECEC", "#4B5CAD", QFont(QStringLiteral("Arial")), "000000", 0.0, 2.0, Qt::DotLine, Qt::SquareCap, true);
    s_defaultStyle[GeoDataFeature::HighwayService]           = GeoDataFeaturePrivate::createHighwayStyle( "highway_service", "#FEFEFE", QColor("#FEFEFE").darker(150), QFont(QStringLiteral("Arial")), "000000", 1.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayLivingStreet]      = GeoDataFeaturePrivate::createHighwayStyle( "highway_living_street", "#CCCCCC", QColor("#CCCCCC").darker(150), QFont(QStringLiteral("Arial")), "000000", 3, 6 );
    s_defaultStyle[GeoDataFeature::HighwayRoad]              = GeoDataFeaturePrivate::createHighwayStyle( "highway_road", "#DCDCDC", QColor("#DCDCDC").darker(150), QFont(QStringLiteral("Arial")), "000000", 3, 6 );
    s_defaultStyle[GeoDataFeature::HighwayUnclassified]      = GeoDataFeaturePrivate::createHighwayStyle( "highway_unclassified", "#FEFEFE", QColor("#FEFEFE").darker(150), QFont(QStringLiteral("Arial")), "000000", 3, 6 );
    s_defaultStyle[GeoDataFeature::HighwayTertiary]          = GeoDataFeaturePrivate::createHighwayStyle( "highway_tertiary", "#F8F8BA", QColor("#F8F8BA").darker(150), QFont(QStringLiteral("Arial")), "000000", 6.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayTertiaryLink]      = GeoDataFeaturePrivate::createHighwayStyle( "highway_tertiary", "#F8F8BA", QColor("#F8F8BA").darker(150), QFont(QStringLiteral("Arial")), "000000", 6.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwaySecondary]         = GeoDataFeaturePrivate::createHighwayStyle( "highway_secondary", "#F8D6AA", QColor("#F8D6AA").darker(150), QFont(QStringLiteral("Arial")), "000000", 6.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwaySecondaryLink]     = GeoDataFeaturePrivate::createHighwayStyle( "highway_secondary", "#F8D6AA", QColor("#F8D6AA").darker(150), QFont(QStringLiteral("Arial")), "000000", 6.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayPrimary]           = GeoDataFeaturePrivate::createHighwayStyle( "highway_primary", "#EB989A", QColor("#EB989A").darker(150), QFont(QStringLiteral("Arial")), "000000", 9.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayPrimaryLink]       = GeoDataFeaturePrivate::createHighwayStyle( "highway_primary", "#EB989A", QColor("#EB989A").darker(150), QFont(QStringLiteral("Arial")), "000000", 6.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayTrunk]             = GeoDataFeaturePrivate::createHighwayStyle( "highway_trunk", "#94D494", QColor("#94D494").darker(150), QFont(QStringLiteral("Arial")), "000000", 9.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayTrunkLink]         = GeoDataFeaturePrivate::createHighwayStyle( "highway_trunk", "#94D494", QColor("#94D494").darker(150), QFont(QStringLiteral("Arial")), "000000", 9.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayMotorway]          = GeoDataFeaturePrivate::createHighwayStyle( "highway_motorway", "#809BC0", QColor("#809BC0").darker(150), QFont(QStringLiteral("Arial")), "000000", 9.0, 10 );
    s_defaultStyle[GeoDataFeature::HighwayMotorwayLink]      = GeoDataFeaturePrivate::createHighwayStyle( "highway_motorway", "#809BC0", QColor("#809BC0").darker(150), QFont(QStringLiteral("Arial")), "000000", 9.0, 10 );

    s_defaultStyle[GeoDataFeature::NaturalWater]             = GeoDataFeaturePrivate::createStyle( 4, 0, waterColor, waterColor, true, true,
                                                                                                   Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false, QVector< qreal >(),
                                                                                                   QFont(QStringLiteral("Arial")), waterColor.darker(150));

    s_defaultStyle[GeoDataFeature::AmenityGraveyard]         = GeoDataFeaturePrivate::createWayStyle( "#AACBAF", "#AACBAF", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_generic.png") );

    s_defaultStyle[GeoDataFeature::NaturalWood]              = GeoDataFeaturePrivate::createWayStyle( "#8DC46C", "#8DC46C", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/forest.png") );
    s_defaultStyle[GeoDataFeature::NaturalBeach]             = GeoDataFeaturePrivate::createWayStyle( "#FFF1BA", "#FFF1BA", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/beach.png") );
    s_defaultStyle[GeoDataFeature::NaturalWetland]           = GeoDataFeaturePrivate::createWayStyle( "#DDECEC", "#DDECEC", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/wetland.png") );
    s_defaultStyle[GeoDataFeature::NaturalGlacier]           = GeoDataFeaturePrivate::createWayStyle( "#DDECEC", "#DDECEC", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/glacier.png") );
    s_defaultStyle[GeoDataFeature::NaturalScrub]             = GeoDataFeaturePrivate::createWayStyle( "#B5E3B5", "#B5E3B5", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/scrub.png") );
    s_defaultStyle[GeoDataFeature::NaturalCliff]             = GeoDataFeaturePrivate::createWayStyle( Qt::transparent, Qt::transparent, true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/cliff2.png") );
    s_defaultStyle[GeoDataFeature::NaturalHeath]             = GeoDataFeaturePrivate::createWayStyle( "#d6d99f", QColor("#d6d99f").darker(150), true, false );

    s_defaultStyle[GeoDataFeature::LeisureGolfCourse]        = GeoDataFeaturePrivate::createWayStyle( QColor("#b5e3b5"), QColor("#b5e3b5").darker(150), true, true );
    s_defaultStyle[GeoDataFeature::LeisurePark]              = GeoDataFeaturePrivate::createWayStyle( QColor("#c8facc"), QColor("#c8facc").darker(150), true, true );
    s_defaultStyle[GeoDataFeature::LeisurePlayground]        = GeoDataFeaturePrivate::createOsmPOIStyle( osmFont, "amenity/playground.16", amenityColor, "#CCFFF1", "#BDFFED" );
    s_defaultStyle[GeoDataFeature::LeisurePitch]             = GeoDataFeaturePrivate::createWayStyle( "#8ad3af", QColor("#8ad3af").darker(150), true, true );
    s_defaultStyle[GeoDataFeature::LeisureSportsCentre]      = GeoDataFeaturePrivate::createWayStyle( "#33cc99", QColor("#33cc99").darker(150), true, true );
    s_defaultStyle[GeoDataFeature::LeisureStadium]           = GeoDataFeaturePrivate::createWayStyle( "#33cc99", QColor("#33cc99").darker(150), true, true );
    s_defaultStyle[GeoDataFeature::LeisureTrack]             = GeoDataFeaturePrivate::createWayStyle( "#74dcba", QColor("#74dcba").darker(150), true, true );
    s_defaultStyle[GeoDataFeature::LeisureSwimmingPool]      = GeoDataFeaturePrivate::createWayStyle( waterColor, waterColor.darker(150), true, true );

    s_defaultStyle[GeoDataFeature::LanduseAllotments]        = GeoDataFeaturePrivate::createWayStyle( "#E4C6AA", "#E4C6AA", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/allotments.png") );
    s_defaultStyle[GeoDataFeature::LanduseBasin]             = GeoDataFeaturePrivate::createWayStyle( QColor(0xB5, 0xD0, 0xD0, 0x80 ), QColor( 0xB5, 0xD0, 0xD0 ) );
    s_defaultStyle[GeoDataFeature::LanduseCemetery]          = GeoDataFeaturePrivate::createWayStyle( "#AACBAF", "#AACBAF", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_generic.png") );
    s_defaultStyle[GeoDataFeature::LanduseCommercial]        = GeoDataFeaturePrivate::createWayStyle( "#F2DAD9", "#D1B2B0", true, true );
    s_defaultStyle[GeoDataFeature::LanduseConstruction]      = GeoDataFeaturePrivate::createWayStyle( "#b6b592", "#b6b592", true, true );
    s_defaultStyle[GeoDataFeature::LanduseFarmland]          = GeoDataFeaturePrivate::createWayStyle( "#EDDDC9", "#C8B69E", true, true );
    s_defaultStyle[GeoDataFeature::LanduseFarmyard]          = GeoDataFeaturePrivate::createWayStyle( "#EFD6B5", "#D1B48C", true, true );
    s_defaultStyle[GeoDataFeature::LanduseGarages]           = GeoDataFeaturePrivate::createWayStyle( "#E0DDCD", "#E0DDCD", true, true );
    s_defaultStyle[GeoDataFeature::LanduseGrass]             = GeoDataFeaturePrivate::createWayStyle( "#A8C8A5", "#A8C8A5", true, true );
    s_defaultStyle[GeoDataFeature::LanduseIndustrial]        = GeoDataFeaturePrivate::createWayStyle( "#DED0D5", "#DED0D5", true, true );
    s_defaultStyle[GeoDataFeature::LanduseLandfill]          = GeoDataFeaturePrivate::createWayStyle( "#b6b592", "#b6b592", true, true );
    s_defaultStyle[GeoDataFeature::LanduseMeadow]            = GeoDataFeaturePrivate::createWayStyle( "#cdebb0", "#cdebb0", true, true );
    s_defaultStyle[GeoDataFeature::LanduseMilitary]          = GeoDataFeaturePrivate::createWayStyle( "#F3D8D2", "#F3D8D2", true, true, Qt::BDiagPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/military_red_hatch.png") );
    s_defaultStyle[GeoDataFeature::LanduseQuarry]            = GeoDataFeaturePrivate::createWayStyle( "#C4C2C2", "#C4C2C2", true, true, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/quarry.png") );
    s_defaultStyle[GeoDataFeature::LanduseRailway]           = GeoDataFeaturePrivate::createWayStyle( "#DED0D5", "#DED0D5", true, true );
    s_defaultStyle[GeoDataFeature::LanduseReservoir]         = GeoDataFeaturePrivate::createWayStyle( waterColor, waterColor, true, true );
    s_defaultStyle[GeoDataFeature::LanduseResidential]       = GeoDataFeaturePrivate::createWayStyle( "#DCDCDC", "#DCDCDC", true, true );
    s_defaultStyle[GeoDataFeature::LanduseRetail]            = GeoDataFeaturePrivate::createWayStyle( "#FFD6D1", "#D99C95", true, true );
    s_defaultStyle[GeoDataFeature::LanduseOrchard]           = GeoDataFeaturePrivate::createWayStyle( "#AEDFA3", "#AEDFA3", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/orchard.png") );
    s_defaultStyle[GeoDataFeature::LanduseVineyard]          = GeoDataFeaturePrivate::createWayStyle( "#AEDFA3", "#AEDFA3", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/vineyard.png") );

    s_defaultStyle[GeoDataFeature::MilitaryDangerArea]       = GeoDataFeaturePrivate::createWayStyle( "#FFC0CB", "#FFC0CB", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/danger.png") );

    s_defaultStyle[GeoDataFeature::RailwayRail]              = GeoDataFeaturePrivate::createStyle( 2.0, 1.435, "#706E70", "#EEEEEE", true, true, Qt::SolidPattern, Qt::CustomDashLine, Qt::FlatCap, true, QVector< qreal >() << 2 << 3 );
    s_defaultStyle[GeoDataFeature::RailwayNarrowGauge]       = GeoDataFeaturePrivate::createStyle( 2.0, 1.0, "#706E70", "#EEEEEE", true, true, Qt::SolidPattern, Qt::CustomDashLine, Qt::FlatCap, true, QVector< qreal >() << 2 << 3 );
    // FIXME: the tram is currently being rendered as a polygon.
    s_defaultStyle[GeoDataFeature::RailwayTram]              = GeoDataFeaturePrivate::createStyle( 2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false );
    s_defaultStyle[GeoDataFeature::RailwayLightRail]         = GeoDataFeaturePrivate::createStyle( 2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false );
    s_defaultStyle[GeoDataFeature::RailwayAbandoned]         = GeoDataFeaturePrivate::createStyle( 2.0, 1.435, Qt::transparent, "#706E70", false, false, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false );
    s_defaultStyle[GeoDataFeature::RailwaySubway]            = GeoDataFeaturePrivate::createStyle( 2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false );
    s_defaultStyle[GeoDataFeature::RailwayPreserved]         = GeoDataFeaturePrivate::createStyle( 2.0, 1.435, "#EEEEEE", "#706E70", true, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, true );
    s_defaultStyle[GeoDataFeature::RailwayMiniature]         = GeoDataFeaturePrivate::createStyle( 2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false );
    s_defaultStyle[GeoDataFeature::RailwayConstruction]      = GeoDataFeaturePrivate::createStyle( 2.0, 1.435, "#EEEEEE", "#706E70", true, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, true );
    s_defaultStyle[GeoDataFeature::RailwayMonorail]          = GeoDataFeaturePrivate::createStyle( 2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false );
    s_defaultStyle[GeoDataFeature::RailwayFunicular]         = GeoDataFeaturePrivate::createStyle( 2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false );

    s_defaultStyle[GeoDataFeature::Building]                 = GeoDataFeaturePrivate::createStyle( 1, 0, buildingColor, buildingColor.darker(),
                                                                                   true, true, Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false );

    s_defaultStyle[GeoDataFeature::Satellite]
        = new GeoDataStyle( MarbleDirs::path( "bitmaps/satellite.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), s_defaultLabelColor );

    s_defaultFont = QFont( QStringLiteral("Sans Serif") );

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

const GeoDataStyle *GeoDataFeature::presetStyle( GeoDataVisualCategory category )
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
    if(d->ref.load() == 1) {
        return;
    }

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
