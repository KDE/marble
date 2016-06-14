//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#include "StyleBuilder.h"

#include "MarbleDirs.h"
#include "OsmPlacemarkData.h"
#include "GeoDataTypes.h"
#include "GeoDataPlacemark.h"
#include "OsmPresetLibrary.h"

#include <QDebug>

namespace Marble {

class StyleBuilder::Private
{
public:
    static bool s_defaultStyleInitialized;
    static GeoDataStyle::Ptr s_defaultStyle[GeoDataFeature::LastIndex];

    static GeoDataStyle::Ptr createStyle( qreal width, qreal realWidth, const QColor& color,
                                      const QColor& outlineColor, bool fill, bool outline,
                                      Qt::BrushStyle brushStyle, Qt::PenStyle penStyle,
                                      Qt::PenCapStyle capStyle, bool lineBackground,
                                      const QVector< qreal >& dashPattern = QVector< qreal >(),
                                      const QFont& font = QFont(QLatin1String("Arial")), const QColor& fontColor = Qt::black,
                                      const QString& texturePath = QString());
    static GeoDataStyle::Ptr createOsmPOIStyle( const QFont &font, const QString &bitmap,
                                         const QColor &textColor = Qt::black,
                                         const QColor &color = QColor( 0xBE, 0xAD, 0xAD ),
                                         const QColor &outline = QColor( 0xBE, 0xAD, 0xAD ).darker()
                                         );
    static GeoDataStyle::Ptr createHighwayStyle( const QString &bitmap, const QColor& color, const QColor& outlineColor,
                                             const QFont& font = QFont(QLatin1String("Arial")), const QColor& fontColor = Qt::black,
                                             qreal width = 1, qreal realWidth = 0.0,
                                             Qt::PenStyle penStyle = Qt::SolidLine,
                                             Qt::PenCapStyle capStyle = Qt::RoundCap,
                                             bool lineBackground = false);
    static GeoDataStyle::Ptr createWayStyle( const QColor& color, const QColor& outlineColor,
                                         bool fill = true, bool outline = true,
                                         Qt::BrushStyle brushStyle = Qt::SolidPattern,
                                         const QString& texturePath = QString());

    void initializeDefaultStyles();
};

GeoDataStyle::Ptr StyleBuilder::Private::s_defaultStyle[GeoDataFeature::LastIndex];
bool StyleBuilder::Private::s_defaultStyleInitialized = false;

GeoDataStyle::Ptr StyleBuilder::Private::createOsmPOIStyle( const QFont &font, const QString &bitmap,
                                     const QColor &textColor, const QColor &color, const QColor &outline)
{
    GeoDataStyle::Ptr style =  createStyle(1, 0, color, outline, true, true, Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false);
    QString const imagePath = MarbleDirs::path( "bitmaps/osmcarto/symbols/48/" + bitmap + ".png" );
    style->setIconStyle( GeoDataIconStyle( imagePath) );
    style->iconStyle().setScale(0.67);
    style->setLabelStyle( GeoDataLabelStyle( font, textColor ) );
    style->labelStyle().setAlignment(GeoDataLabelStyle::Center);
    return style;
}

GeoDataStyle::Ptr StyleBuilder::Private::createHighwayStyle( const QString &bitmap, const QColor& color, const QColor& outlineColor,
                                         const QFont& font, const QColor& fontColor, qreal width, qreal realWidth, Qt::PenStyle penStyle,
                                                             Qt::PenCapStyle capStyle, bool lineBackground)
{
    GeoDataStyle::Ptr style = createStyle( width, realWidth, color, outlineColor, true, true,
                                       Qt::SolidPattern, penStyle, capStyle, lineBackground, QVector< qreal >(),
                                       font, fontColor );
    if( !bitmap.isEmpty() ) {
        style->setIconStyle( GeoDataIconStyle( MarbleDirs::path( "bitmaps/" + bitmap + ".png" ) ) );
    }
    return style;
}

GeoDataStyle::Ptr StyleBuilder::Private::createWayStyle( const QColor& color, const QColor& outlineColor,
                                     bool fill, bool outline, Qt::BrushStyle brushStyle, const QString& texturePath)
{
    return createStyle( 1, 0, color, outlineColor, fill, outline, brushStyle, Qt::SolidLine, Qt::RoundCap, false, QVector< qreal >(), QFont("Arial"), Qt::black, texturePath );
}

GeoDataStyle::Ptr StyleBuilder::Private::createStyle( qreal width, qreal realWidth, const QColor& color,
                                  const QColor& outlineColor, bool fill, bool outline, Qt::BrushStyle brushStyle, Qt::PenStyle penStyle,
                                  Qt::PenCapStyle capStyle, bool lineBackground, const QVector< qreal >& dashPattern,
                                  const QFont& font, const QColor& fontColor, const QString& texturePath)
{
    GeoDataStyle *style = new GeoDataStyle;
    GeoDataLineStyle lineStyle( outlineColor );
    lineStyle.setCapStyle( capStyle );
    lineStyle.setPenStyle( penStyle );
    lineStyle.setWidth( width );
    lineStyle.setPhysicalWidth( realWidth );
    lineStyle.setBackground( lineBackground );
    lineStyle.setDashPattern( dashPattern );
    GeoDataPolyStyle polyStyle( color );
    polyStyle.setOutline( outline );
    polyStyle.setFill( fill );
    polyStyle.setBrushStyle( brushStyle );
    polyStyle.setTexturePath( texturePath );
    GeoDataLabelStyle labelStyle(font, fontColor);
    style->setLineStyle( lineStyle );
    style->setPolyStyle( polyStyle );
    style->setLabelStyle( labelStyle );
    return GeoDataStyle::Ptr(style);
}

void StyleBuilder::Private::initializeDefaultStyles()
{
    // We need to do this similar to the way KCmdLineOptions works in
    // the future: Having a PlacemarkStyleProperty properties[] would
    // help here greatly.

    if ( s_defaultStyleInitialized ) {
        return;
    }
    s_defaultStyleInitialized = true;

    QString defaultFamily = GeoDataFeature::defaultFont().family();

#ifdef Q_OS_MACX
    int defaultSize = 10;
#else
    int defaultSize = 8;
#endif

    QColor const defaultLabelColor = GeoDataFeature::defaultLabelColor();

    s_defaultStyle[GeoDataFeature::None]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::Default]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/default_location.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::Unknown]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::SmallCity]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_4_white.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::SmallCountyCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_4_yellow.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::SmallStateCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_4_orange.png" ),
              QFont( defaultFamily, defaultSize, 50, true  ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::SmallNationCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_4_red.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::MediumCity]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_3_white.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::MediumCountyCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_3_yellow.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::MediumStateCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_3_orange.png" ),
              QFont( defaultFamily, defaultSize, 50, true  ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::MediumNationCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_3_red.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::BigCity]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_2_white.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::BigCountyCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_2_yellow.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::BigStateCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_2_orange.png" ),
              QFont( defaultFamily, defaultSize, 50, true  ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::BigNationCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_2_red.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::LargeCity]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_1_white.png" ),
              QFont( defaultFamily, defaultSize, 75, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::LargeCountyCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_1_yellow.png" ),
              QFont( defaultFamily, defaultSize, 75, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::LargeStateCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_1_orange.png" ),
              QFont( defaultFamily, defaultSize, 75, true  ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::LargeNationCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_1_red.png" ),
              QFont( defaultFamily, defaultSize, 75, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::Nation]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, (int)(defaultSize * 1.2 ), 75, false ), QColor( "#404040" ) ));
    // Align area labels centered
    s_defaultStyle[GeoDataFeature::Nation]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    s_defaultStyle[GeoDataFeature::PlaceCity] = GeoDataStyle::Ptr(new GeoDataStyle( QString(), QFont( defaultFamily, (int)(defaultSize * 2.0 ), 75, false ), QColor( "#202020" ) ));
    s_defaultStyle[GeoDataFeature::PlaceCity]->labelStyle().setAlignment( GeoDataLabelStyle::Center );
    s_defaultStyle[GeoDataFeature::PlaceSuburb] = GeoDataStyle::Ptr(new GeoDataStyle( QString(), QFont( defaultFamily, (int)(defaultSize * 2.0 ), 75, false ), QColor( "#707070" ) ));
    s_defaultStyle[GeoDataFeature::PlaceSuburb]->labelStyle().setAlignment( GeoDataLabelStyle::Center );
    s_defaultStyle[GeoDataFeature::PlaceHamlet] = GeoDataStyle::Ptr(new GeoDataStyle( QString(), QFont( defaultFamily, (int)(defaultSize * 2.0 ), 75, false ), QColor( "#707070" ) ));
    s_defaultStyle[GeoDataFeature::PlaceHamlet]->labelStyle().setAlignment( GeoDataLabelStyle::Center );
    s_defaultStyle[GeoDataFeature::PlaceLocality] = GeoDataStyle::Ptr(new GeoDataStyle( QString(), QFont( defaultFamily, (int)(defaultSize * 2.0 ), 75, false ), QColor( "#707070" ) ));
    s_defaultStyle[GeoDataFeature::PlaceLocality]->labelStyle().setAlignment( GeoDataLabelStyle::Center );
    s_defaultStyle[GeoDataFeature::PlaceTown] = GeoDataStyle::Ptr(new GeoDataStyle( QString(), QFont( defaultFamily, (int)(defaultSize * 2.0 ), 75, false ), QColor( "#404040" ) ));
    s_defaultStyle[GeoDataFeature::PlaceTown]->labelStyle().setAlignment( GeoDataLabelStyle::Center );
    s_defaultStyle[GeoDataFeature::PlaceVillage] = GeoDataStyle::Ptr(new GeoDataStyle( QString(), QFont( defaultFamily, (int)(defaultSize * 2.0 ), 75, false ), QColor( "#505050" ) ));
    s_defaultStyle[GeoDataFeature::PlaceVillage]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    s_defaultStyle[GeoDataFeature::Mountain]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/mountain_1.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::Volcano]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/volcano_1.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::Mons]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/mountain_1.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::Valley]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/valley.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::Continent]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, (int)(defaultSize * 1.7 ), 50, false ), QColor( "#bf0303" ) ));
    // Align area labels centered
    s_defaultStyle[GeoDataFeature::Continent]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    s_defaultStyle[GeoDataFeature::Ocean]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, (int)(defaultSize * 1.7 ), 50, true ), QColor( "#2c72c7" ) ));
    // Align area labels centered
    s_defaultStyle[GeoDataFeature::Ocean]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    s_defaultStyle[GeoDataFeature::OtherTerrain]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/other.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::Crater]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/crater.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::Mare]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, (int)(defaultSize * 1.7 ), 50, false ), QColor( "#bf0303" ) ));
    // Align area labels centered
    s_defaultStyle[GeoDataFeature::Mare]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    s_defaultStyle[GeoDataFeature::GeographicPole]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/pole_1.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::MagneticPole]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/pole_2.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::ShipWreck]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/shipwreck.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.8 ), 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::AirPort]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/airport.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::Observatory]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/observatory.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::Wikipedia]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/wikipedia.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::OsmSite]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/osm.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::Coordinate]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/coordinate.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));


    s_defaultStyle[GeoDataFeature::MannedLandingSite]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/manned_landing.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::RoboticRover]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/robotic_rover.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::UnmannedSoftLandingSite]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/unmanned_soft_landing.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::UnmannedHardLandingSite]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/unmanned_hard_landing.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::Folder]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/folder.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    s_defaultStyle[GeoDataFeature::Bookmark]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/bookmark.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

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
    s_defaultStyle[GeoDataFeature::AccomodationCamping]      = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/camping.16", transportationColor );
    s_defaultStyle[GeoDataFeature::AccomodationHostel]       = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/hostel.16", transportationColor );
    s_defaultStyle[GeoDataFeature::AccomodationHotel]        = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/hotel.16", transportationColor );
    s_defaultStyle[GeoDataFeature::AccomodationMotel]        = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/motel.16", transportationColor );
    s_defaultStyle[GeoDataFeature::AccomodationYouthHostel]  = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/hostel.16", transportationColor );
    s_defaultStyle[GeoDataFeature::AccomodationGuestHouse]   = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/guest_house.16", transportationColor );
    s_defaultStyle[GeoDataFeature::AmenityLibrary]           = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/library.20", amenityColor );
    s_defaultStyle[GeoDataFeature::EducationCollege]         = StyleBuilder::Private::createOsmPOIStyle( osmFont, QString(), amenityColor, educationalAreasAndHospital, amenityColor );
    s_defaultStyle[GeoDataFeature::EducationSchool]          = StyleBuilder::Private::createOsmPOIStyle( osmFont, QString(), amenityColor, educationalAreasAndHospital, amenityColor );
    s_defaultStyle[GeoDataFeature::EducationUniversity]      = StyleBuilder::Private::createOsmPOIStyle( osmFont, QString(), amenityColor, educationalAreasAndHospital, amenityColor );
    s_defaultStyle[GeoDataFeature::FoodBar]                  = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/bar.16", amenityColor );
    s_defaultStyle[GeoDataFeature::FoodBiergarten]           = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/biergarten.16", amenityColor );
    s_defaultStyle[GeoDataFeature::FoodCafe]                 = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/cafe.16", amenityColor );
    s_defaultStyle[GeoDataFeature::FoodFastFood]             = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/fast_food.16", amenityColor );
    s_defaultStyle[GeoDataFeature::FoodPub]                  = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/pub.16", amenityColor );
    s_defaultStyle[GeoDataFeature::FoodRestaurant]           = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/restaurant.16", amenityColor );

    s_defaultStyle[GeoDataFeature::HealthDentist]            = StyleBuilder::Private::createOsmPOIStyle( osmFont, "health/dentist.16", healthColor );
    s_defaultStyle[GeoDataFeature::HealthDoctors]            = StyleBuilder::Private::createOsmPOIStyle( osmFont, "health/doctors.16", healthColor );
    s_defaultStyle[GeoDataFeature::HealthHospital]           = StyleBuilder::Private::createOsmPOIStyle( osmFont, "health/hospital.16", healthColor, educationalAreasAndHospital, amenityColor );
    s_defaultStyle[GeoDataFeature::HealthPharmacy]           = StyleBuilder::Private::createOsmPOIStyle( osmFont, "health/pharmacy.16", healthColor );
    s_defaultStyle[GeoDataFeature::HealthVeterinary]         = StyleBuilder::Private::createOsmPOIStyle( osmFont, "health/veterinary-14", healthColor );

    s_defaultStyle[GeoDataFeature::MoneyAtm]                 = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/atm.16", amenityColor );
    s_defaultStyle[GeoDataFeature::MoneyBank]                = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/bank.16", amenityColor );

    s_defaultStyle[GeoDataFeature::AmenityArchaeologicalSite] = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/archaeological_site.16", amenityColor, Qt::transparent );
    s_defaultStyle[GeoDataFeature::AmenityEmbassy]           = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/embassy.16", transportationColor );
    s_defaultStyle[GeoDataFeature::AmenityEmergencyPhone]    = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/emergency_phone.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityWaterPark]         = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/water_park.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityCommunityCentre]   = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/community_centre-14", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityFountain]          = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/fountain-14", QColor("#7989de"), waterColor, waterColor.darker(150) );
    s_defaultStyle[GeoDataFeature::AmenityNightClub]         = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/nightclub.18", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityBench]             = StyleBuilder::Private::createOsmPOIStyle( osmFont, "individual/bench.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityCourtHouse]        = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/courthouse-16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityFireStation]       = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/firestation.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityHuntingStand]      = StyleBuilder::Private::createOsmPOIStyle( osmFont, "manmade/hunting-stand.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityPolice]            = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/police.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityPostBox]           = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/post_box-12", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityPostOffice]        = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/post_office-14", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityPrison]            = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/prison.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityRecycling]         = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/recycling.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityTelephone]         = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/telephone.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityToilets]           = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/toilets.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityTownHall]          = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/town_hall.16", amenityColor );
    s_defaultStyle[GeoDataFeature::AmenityWasteBasket]       = StyleBuilder::Private::createOsmPOIStyle( osmFont, "individual/waste_basket.10", amenityColor );

    s_defaultStyle[GeoDataFeature::AmenityDrinkingWater]     = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/drinking_water.16", amenityColor );

    s_defaultStyle[GeoDataFeature::NaturalPeak]              = StyleBuilder::Private::createOsmPOIStyle( osmFont, "individual/peak", amenityColor );
    s_defaultStyle[GeoDataFeature::NaturalPeak]->iconStyle().setScale(0.33);
    s_defaultStyle[GeoDataFeature::NaturalTree]              = StyleBuilder::Private::createOsmPOIStyle( osmFont, "individual/tree-29", amenityColor ); // tree-16 provides the official icon

    s_defaultStyle[GeoDataFeature::ShopBeverages]            = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/beverages-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopHifi]                 = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/hifi-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopSupermarket]          = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_supermarket", shopColor );
    s_defaultStyle[GeoDataFeature::ShopAlcohol]              = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_alcohol.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopBakery]               = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_bakery.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopButcher]              = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/butcher-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopConfectionery]        = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/confectionery.14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopConvenience]          = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_convenience", shopColor );
    s_defaultStyle[GeoDataFeature::ShopGreengrocer]          = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/greengrocer-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopSeafood]              = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/seafood-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopDepartmentStore]      = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/department_store-16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopKiosk]                = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/kiosk-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopBag]                  = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/bag-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopClothes]              = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_clothes.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopFashion]              = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_clothes.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopJewelry]              = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_jewelry.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopShoes]                = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_shoes.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopVarietyStore]         = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/variety_store-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopBeauty]               = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/beauty-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopChemist]              = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/chemist-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopCosmetics]            = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/perfumery-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopHairdresser]          = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_hairdresser.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopOptician]             = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_optician.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopPerfumery]            = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/perfumery-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopDoitYourself]         = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_diy.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopFlorist]              = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/florist.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopHardware]             = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_diy.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopFurniture]            = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_furniture.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopElectronics]          = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_electronics.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopMobilePhone]          = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_mobile_phone.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopBicycle]              = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_bicycle.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopCar]                  = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_car", shopColor );
    s_defaultStyle[GeoDataFeature::ShopCarRepair]            = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shopping_car_repair.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopCarParts]             = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/car_parts-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopMotorcycle]           = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/motorcycle-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopOutdoor]              = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/outdoor-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopMusicalInstrument]    = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/musical_instrument-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopPhoto]                = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/photo-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopBook]                 = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_books.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopGift]                 = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_gift.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopStationery]           = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/stationery-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopLaundry]              = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/laundry-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopPet]                  = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop_pet.16", shopColor );
    s_defaultStyle[GeoDataFeature::ShopToys]                 = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/toys-14", shopColor );
    s_defaultStyle[GeoDataFeature::ShopTravelAgency]         = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/travel_agency-14", shopColor );
    s_defaultStyle[GeoDataFeature::Shop]                     = StyleBuilder::Private::createOsmPOIStyle( osmFont, "shop/shop-14", shopColor );

    s_defaultStyle[GeoDataFeature::ManmadeBridge]            = StyleBuilder::Private::createWayStyle( QColor("#b8b8b8"), QColor("transparent"), true, true );
    s_defaultStyle[GeoDataFeature::ManmadeLighthouse]        = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/lighthouse.16", transportationColor, "#f2efe9", QColor( "#f2efe9" ).darker() );
    s_defaultStyle[GeoDataFeature::ManmadePier]              = StyleBuilder::Private::createStyle(0.0, 3.0, "#f2efe9", "#f2efe9", true, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false );
    s_defaultStyle[GeoDataFeature::ManmadeWaterTower]        = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/water_tower.16", amenityColor );
    s_defaultStyle[GeoDataFeature::ManmadeWindMill]          = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/windmill.16", amenityColor );

    s_defaultStyle[GeoDataFeature::TouristAttraction]        = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/tourist_memorial.16", amenityColor );
    s_defaultStyle[GeoDataFeature::TouristCastle]            = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/cinema.16", amenityColor );
    s_defaultStyle[GeoDataFeature::TouristCinema]            = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/cinema.16", amenityColor );
    s_defaultStyle[GeoDataFeature::TouristInformation]       = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/information.16", amenityColor );
    s_defaultStyle[GeoDataFeature::TouristMonument]          = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/monument.16", amenityColor );
    s_defaultStyle[GeoDataFeature::TouristMuseum]            = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/museum.16", amenityColor );
    s_defaultStyle[GeoDataFeature::TouristRuin]              = StyleBuilder::Private::createOsmPOIStyle( osmFont, QString(), amenityColor );
    s_defaultStyle[GeoDataFeature::TouristTheatre]           = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/theatre.16", amenityColor );
    s_defaultStyle[GeoDataFeature::TouristThemePark]         = StyleBuilder::Private::createOsmPOIStyle( osmFont, QString(), amenityColor );
    s_defaultStyle[GeoDataFeature::TouristViewPoint]         = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/viewpoint.16", amenityColor );
    s_defaultStyle[GeoDataFeature::TouristZoo]               = StyleBuilder::Private::createOsmPOIStyle( osmFont, QString(), amenityColor, Qt::transparent );
    s_defaultStyle[GeoDataFeature::TouristAlpineHut]         = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/alpinehut.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportAerodrome]       = StyleBuilder::Private::createOsmPOIStyle( osmFont, "airtransport/aerodrome", airTransportColor );
    s_defaultStyle[GeoDataFeature::TransportHelipad]         = StyleBuilder::Private::createOsmPOIStyle( osmFont, "airtransport/helipad", airTransportColor );
    s_defaultStyle[GeoDataFeature::TransportAirportTerminal] = StyleBuilder::Private::createOsmPOIStyle( osmFont, QString(), airTransportColor );
    s_defaultStyle[GeoDataFeature::TransportBusStation]      = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/bus_station.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportBusStop]         = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/bus_stop.12", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportCarShare]        = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/car_share.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportFuel]            = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/fuel.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportParking]         = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/parking", transportationColor, "#F6EEB6", QColor( "#F6EEB6" ).darker() );
    s_defaultStyle[GeoDataFeature::TransportParkingSpace]    = StyleBuilder::Private::createWayStyle( "#F6EEB6", QColor( "#F6EEB6" ).darker(), true, true );
    s_defaultStyle[GeoDataFeature::TransportPlatform]        = StyleBuilder::Private::createWayStyle( "#bbbbbb", Qt::transparent, true, false );
    s_defaultStyle[GeoDataFeature::TransportTrainStation]    = StyleBuilder::Private::createOsmPOIStyle( osmFont, "individual/railway_station", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportTramStop]        = StyleBuilder::Private::createOsmPOIStyle( osmFont, QString(), transportationColor );
    s_defaultStyle[GeoDataFeature::TransportRentalBicycle]   = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/rental_bicycle.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportRentalCar]       = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/rental_car.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportTaxiRank]        = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/taxi.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportBicycleParking]  = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/bicycle_parking.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportMotorcycleParking] = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/motorcycle_parking.16", transportationColor );
    s_defaultStyle[GeoDataFeature::TransportSubwayEntrance]  = StyleBuilder::Private::createOsmPOIStyle( osmFont, "transportation/subway_entrance", transportationColor );
    s_defaultStyle[GeoDataFeature::ReligionPlaceOfWorship]   = StyleBuilder::Private::createOsmPOIStyle( osmFont, QString() /* "black/place_of_worship.16" */ );
    s_defaultStyle[GeoDataFeature::ReligionBahai]            = StyleBuilder::Private::createOsmPOIStyle( osmFont, QString() );
    s_defaultStyle[GeoDataFeature::ReligionBuddhist]         = StyleBuilder::Private::createOsmPOIStyle( osmFont, "black/buddhist.16" );
    s_defaultStyle[GeoDataFeature::ReligionChristian]        = StyleBuilder::Private::createOsmPOIStyle( osmFont, "black/christian.16" );
    s_defaultStyle[GeoDataFeature::ReligionMuslim]           = StyleBuilder::Private::createOsmPOIStyle( osmFont, "black/muslim.16" );
    s_defaultStyle[GeoDataFeature::ReligionHindu]            = StyleBuilder::Private::createOsmPOIStyle( osmFont, "black/hinduist.16" );
    s_defaultStyle[GeoDataFeature::ReligionJain]             = StyleBuilder::Private::createOsmPOIStyle( osmFont, QString() );
    s_defaultStyle[GeoDataFeature::ReligionJewish]           = StyleBuilder::Private::createOsmPOIStyle( osmFont, "black/jewish.16" );
    s_defaultStyle[GeoDataFeature::ReligionShinto]           = StyleBuilder::Private::createOsmPOIStyle( osmFont, "black/shintoist.16" );
    s_defaultStyle[GeoDataFeature::ReligionSikh]             = StyleBuilder::Private::createOsmPOIStyle( osmFont, "black/sikhist.16" );

    s_defaultStyle[GeoDataFeature::HighwayTrafficSignals]    = StyleBuilder::Private::createOsmPOIStyle( osmFont, "individual/traffic_light" );

    s_defaultStyle[GeoDataFeature::PowerTower]               = StyleBuilder::Private::createOsmPOIStyle( osmFont, "individual/power_tower", QColor( "#888888" ) );

    s_defaultStyle[GeoDataFeature::BarrierCityWall]          = StyleBuilder::Private::createStyle( 6.0, 3.0, "#787878", "transparent", true, false, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector< qreal >(), QFont(), "transparent" );
    s_defaultStyle[GeoDataFeature::BarrierGate]              = StyleBuilder::Private::createOsmPOIStyle( osmFont, "individual/gate" );
    s_defaultStyle[GeoDataFeature::BarrierLiftGate]          = StyleBuilder::Private::createOsmPOIStyle( osmFont, "individual/liftgate" );
    s_defaultStyle[GeoDataFeature::BarrierWall]              = StyleBuilder::Private::createWayStyle( "#444444", Qt::transparent, true, false );

    s_defaultStyle[GeoDataFeature::HighwaySteps]             = StyleBuilder::Private::createStyle(0.0, 2.0, "#fa8072", QColor(Qt::white), true, true, Qt::SolidPattern, Qt::CustomDashLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3 );
    s_defaultStyle[GeoDataFeature::HighwayUnknown]           = StyleBuilder::Private::createHighwayStyle( "highway_unknown", "#ddd", "#bbb", QFont(QStringLiteral("Arial")), "000000", 0.0, 1.0);
    s_defaultStyle[GeoDataFeature::HighwayPath]              = StyleBuilder::Private::createHighwayStyle( "highway_path", "#dddde8", "#999", QFont(QStringLiteral("Arial")), "000000", 0.0, 1.0, Qt::DotLine, Qt::SquareCap, true);
    s_defaultStyle[GeoDataFeature::HighwayTrack]             = StyleBuilder::Private::createHighwayStyle( "highway_track", "#996600", QColor(Qt::white), QFont(QStringLiteral("Arial")), "000000", 1.0, 1.0, Qt::DashLine, Qt::SquareCap, true );
    s_defaultStyle[GeoDataFeature::HighwayPedestrian]        = StyleBuilder::Private::createHighwayStyle( "highway_pedestrian", "#dddde8", "#999", QFont(QStringLiteral("Arial")), "000000", 0.0, 2.0);
    s_defaultStyle[GeoDataFeature::HighwayFootway]           = StyleBuilder::Private::createHighwayStyle( "highway_footway", "#fa8072", QColor(Qt::white), QFont(QStringLiteral("Arial")), "000000", 0.0, 2.0, Qt::DotLine, Qt::SquareCap, true);
    s_defaultStyle[GeoDataFeature::HighwayCycleway]          = StyleBuilder::Private::createHighwayStyle( "highway_cycleway", QColor(Qt::blue), QColor(Qt::white), QFont(QStringLiteral("Arial")), "000000", 0.0, 2.0, Qt::DotLine, Qt::SquareCap, true);
    s_defaultStyle[GeoDataFeature::HighwayService]           = StyleBuilder::Private::createHighwayStyle( "highway_service", "#ffffff", "#bbb", QFont(QStringLiteral("Arial")), "000000", 1.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayResidential]       = StyleBuilder::Private::createHighwayStyle( "highway_residential", "#ffffff", "#bbb", QFont(QStringLiteral("Arial")), "000000", 3, 6 );
    s_defaultStyle[GeoDataFeature::HighwayLivingStreet]      = StyleBuilder::Private::createHighwayStyle( "highway_living_street", "#ffffff", "#bbb", QFont(QStringLiteral("Arial")), "000000", 3, 6 );
    s_defaultStyle[GeoDataFeature::HighwayRoad]              = StyleBuilder::Private::createHighwayStyle( "highway_road", "#ddd", "#bbb", QFont(QStringLiteral("Arial")), "000000", 3, 6 );
    s_defaultStyle[GeoDataFeature::HighwayUnclassified]      = StyleBuilder::Private::createHighwayStyle( "highway_unclassified", "#ffffff", "#bbb", QFont(QStringLiteral("Arial")), "000000", 3, 6 );
    s_defaultStyle[GeoDataFeature::HighwayTertiary]          = StyleBuilder::Private::createHighwayStyle( "highway_tertiary", "#ffffff", "#8f8f8f", QFont(QStringLiteral("Arial")), "000000", 6.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayTertiaryLink]      = StyleBuilder::Private::createHighwayStyle( "highway_tertiary", "#ffffff", "#8f8f8f", QFont(QStringLiteral("Arial")), "000000", 6.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwaySecondary]         = StyleBuilder::Private::createHighwayStyle( "highway_secondary", "#f7fabf", "#707d05", QFont(QStringLiteral("Arial")), "000000", 6.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwaySecondaryLink]     = StyleBuilder::Private::createHighwayStyle( "highway_secondary", "#f7fabf", "#707d05", QFont(QStringLiteral("Arial")), "000000", 6.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayPrimary]           = StyleBuilder::Private::createHighwayStyle( "highway_primary", "#fcd6a4", "#a06b00", QFont(QStringLiteral("Arial")), "000000", 9.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayPrimaryLink]       = StyleBuilder::Private::createHighwayStyle( "highway_primary", "#fcd6a4", "#a06b00", QFont(QStringLiteral("Arial")), "000000", 6.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayTrunk]             = StyleBuilder::Private::createHighwayStyle( "highway_trunk", "#f9b29c", "#c84e2f", QFont(QStringLiteral("Arial")), "000000", 9.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayTrunkLink]         = StyleBuilder::Private::createHighwayStyle( "highway_trunk", "#f9b29c", "#c84e2f", QFont(QStringLiteral("Arial")), "000000", 9.0, 6 );
    s_defaultStyle[GeoDataFeature::HighwayMotorway]          = StyleBuilder::Private::createHighwayStyle( "highway_motorway", "#e892a2", "#dc2a67", QFont(QStringLiteral("Arial")), "000000", 9.0, 10 );
    s_defaultStyle[GeoDataFeature::HighwayMotorwayLink]      = StyleBuilder::Private::createHighwayStyle( "highway_motorway", "#e892a2", "#dc2a67", QFont(QStringLiteral("Arial")), "000000", 9.0, 10 );

    s_defaultStyle[GeoDataFeature::NaturalWater]             = StyleBuilder::Private::createStyle( 4, 0, waterColor, waterColor, true, true,
                                                                                                   Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false, QVector< qreal >(),
                                                                                                   QFont(QStringLiteral("Arial")), waterColor.darker(150));

    s_defaultStyle[GeoDataFeature::NaturalReef]              = StyleBuilder::Private::createStyle( 5.5, 0, "#36677c", "#36677c", true, true,
                                                                                                   Qt::Dense7Pattern, Qt::DotLine, Qt::RoundCap, false, QVector< qreal >(),
                                                                                                   QFont(QStringLiteral("Arial")), waterColor.darker(150));

    s_defaultStyle[GeoDataFeature::AmenityGraveyard]         = StyleBuilder::Private::createWayStyle( "#AACBAF", "#AACBAF", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_generic.png") );

    s_defaultStyle[GeoDataFeature::NaturalWood]              = StyleBuilder::Private::createWayStyle( "#8DC46C", "#8DC46C", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/forest.png") );
    s_defaultStyle[GeoDataFeature::NaturalBeach]             = StyleBuilder::Private::createWayStyle( "#FFF1BA", "#FFF1BA", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/beach.png") );
    s_defaultStyle[GeoDataFeature::NaturalWetland]           = StyleBuilder::Private::createWayStyle( "#DDECEC", "#DDECEC", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/wetland.png") );
    s_defaultStyle[GeoDataFeature::NaturalGlacier]           = StyleBuilder::Private::createWayStyle( "#DDECEC", "#DDECEC", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/glacier.png") );
    s_defaultStyle[GeoDataFeature::NaturalScrub]             = StyleBuilder::Private::createWayStyle( "#B5E3B5", "#B5E3B5", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/scrub.png") );
    s_defaultStyle[GeoDataFeature::NaturalCliff]             = StyleBuilder::Private::createWayStyle( Qt::transparent, Qt::transparent, true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/cliff2.png") );
    s_defaultStyle[GeoDataFeature::NaturalHeath]             = StyleBuilder::Private::createWayStyle( "#d6d99f", QColor("#d6d99f").darker(150), true, false );

    s_defaultStyle[GeoDataFeature::LeisureGolfCourse]        = StyleBuilder::Private::createWayStyle( QColor("#b5e3b5"), QColor("#b5e3b5").darker(150), true, true );
    s_defaultStyle[GeoDataFeature::LeisurePark]              = StyleBuilder::Private::createWayStyle( QColor("#c8facc"), QColor("#c8facc").darker(150), true, true );
    s_defaultStyle[GeoDataFeature::LeisurePlayground]        = StyleBuilder::Private::createOsmPOIStyle( osmFont, "amenity/playground.16", amenityColor, "#CCFFF1", "#BDFFED" );
    s_defaultStyle[GeoDataFeature::LeisurePitch]             = StyleBuilder::Private::createWayStyle( "#8ad3af", QColor("#8ad3af").darker(150), true, true );
    s_defaultStyle[GeoDataFeature::LeisureSportsCentre]      = StyleBuilder::Private::createWayStyle( "#33cc99", QColor("#33cc99").darker(150), true, true );
    s_defaultStyle[GeoDataFeature::LeisureStadium]           = StyleBuilder::Private::createWayStyle( "#33cc99", QColor("#33cc99").darker(150), true, true );
    s_defaultStyle[GeoDataFeature::LeisureTrack]             = StyleBuilder::Private::createWayStyle( "#74dcba", QColor("#74dcba").darker(150), true, true );
    s_defaultStyle[GeoDataFeature::LeisureSwimmingPool]      = StyleBuilder::Private::createWayStyle( waterColor, waterColor.darker(150), true, true );

    s_defaultStyle[GeoDataFeature::LanduseAllotments]        = StyleBuilder::Private::createWayStyle( "#E4C6AA", "#E4C6AA", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/allotments.png") );
    s_defaultStyle[GeoDataFeature::LanduseBasin]             = StyleBuilder::Private::createWayStyle( QColor(0xB5, 0xD0, 0xD0, 0x80 ), QColor( 0xB5, 0xD0, 0xD0 ) );
    s_defaultStyle[GeoDataFeature::LanduseCemetery]          = StyleBuilder::Private::createWayStyle( "#AACBAF", "#AACBAF", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_generic.png") );
    s_defaultStyle[GeoDataFeature::LanduseCommercial]        = StyleBuilder::Private::createWayStyle( "#F2DAD9", "#D1B2B0", true, true );
    s_defaultStyle[GeoDataFeature::LanduseConstruction]      = StyleBuilder::Private::createWayStyle( "#b6b592", "#b6b592", true, true );
    s_defaultStyle[GeoDataFeature::LanduseFarmland]          = StyleBuilder::Private::createWayStyle( "#EDDDC9", "#C8B69E", true, true );
    s_defaultStyle[GeoDataFeature::LanduseFarmyard]          = StyleBuilder::Private::createWayStyle( "#EFD6B5", "#D1B48C", true, true );
    s_defaultStyle[GeoDataFeature::LanduseGarages]           = StyleBuilder::Private::createWayStyle( "#E0DDCD", "#E0DDCD", true, true );
    s_defaultStyle[GeoDataFeature::LanduseGrass]             = StyleBuilder::Private::createWayStyle( "#A8C8A5", "#A8C8A5", true, true );
    s_defaultStyle[GeoDataFeature::LanduseIndustrial]        = StyleBuilder::Private::createWayStyle( "#DED0D5", "#DED0D5", true, true );
    s_defaultStyle[GeoDataFeature::LanduseLandfill]          = StyleBuilder::Private::createWayStyle( "#b6b592", "#b6b592", true, true );
    s_defaultStyle[GeoDataFeature::LanduseMeadow]            = StyleBuilder::Private::createWayStyle( "#cdebb0", "#cdebb0", true, true );
    s_defaultStyle[GeoDataFeature::LanduseMilitary]          = StyleBuilder::Private::createWayStyle( "#F3D8D2", "#F3D8D2", true, true, Qt::BDiagPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/military_red_hatch.png") );
    s_defaultStyle[GeoDataFeature::LanduseQuarry]            = StyleBuilder::Private::createWayStyle( "#C4C2C2", "#C4C2C2", true, true, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/quarry.png") );
    s_defaultStyle[GeoDataFeature::LanduseRailway]           = StyleBuilder::Private::createWayStyle( "#DED0D5", "#DED0D5", true, true );
    s_defaultStyle[GeoDataFeature::LanduseReservoir]         = StyleBuilder::Private::createWayStyle( waterColor, waterColor, true, true );
    s_defaultStyle[GeoDataFeature::LanduseResidential]       = StyleBuilder::Private::createWayStyle( "#DCDCDC", "#DCDCDC", true, true );
    s_defaultStyle[GeoDataFeature::LanduseRetail]            = StyleBuilder::Private::createWayStyle( "#FFD6D1", "#D99C95", true, true );
    s_defaultStyle[GeoDataFeature::LanduseOrchard]           = StyleBuilder::Private::createWayStyle( "#AEDFA3", "#AEDFA3", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/orchard.png") );
    s_defaultStyle[GeoDataFeature::LanduseVineyard]          = StyleBuilder::Private::createWayStyle( "#AEDFA3", "#AEDFA3", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/vineyard.png") );

    s_defaultStyle[GeoDataFeature::MilitaryDangerArea]       = StyleBuilder::Private::createWayStyle( "#FFC0CB", "#FFC0CB", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/danger.png") );

    s_defaultStyle[GeoDataFeature::RailwayRail]              = StyleBuilder::Private::createStyle( 2.0, 1.435, "#706E70", "#EEEEEE", true, true, Qt::SolidPattern, Qt::CustomDashLine, Qt::FlatCap, true, QVector< qreal >() << 2 << 3 );
    s_defaultStyle[GeoDataFeature::RailwayNarrowGauge]       = StyleBuilder::Private::createStyle( 2.0, 1.0, "#706E70", "#EEEEEE", true, true, Qt::SolidPattern, Qt::CustomDashLine, Qt::FlatCap, true, QVector< qreal >() << 2 << 3 );
    // FIXME: the tram is currently being rendered as a polygon.
    s_defaultStyle[GeoDataFeature::RailwayTram]              = StyleBuilder::Private::createStyle( 2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false );
    s_defaultStyle[GeoDataFeature::RailwayLightRail]         = StyleBuilder::Private::createStyle( 2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false );
    s_defaultStyle[GeoDataFeature::RailwayAbandoned]         = StyleBuilder::Private::createStyle( 2.0, 1.435, Qt::transparent, "#706E70", false, false, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false );
    s_defaultStyle[GeoDataFeature::RailwaySubway]            = StyleBuilder::Private::createStyle( 2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false );
    s_defaultStyle[GeoDataFeature::RailwayPreserved]         = StyleBuilder::Private::createStyle( 2.0, 1.435, "#EEEEEE", "#706E70", true, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, true );
    s_defaultStyle[GeoDataFeature::RailwayMiniature]         = StyleBuilder::Private::createStyle( 2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false );
    s_defaultStyle[GeoDataFeature::RailwayConstruction]      = StyleBuilder::Private::createStyle( 2.0, 1.435, "#EEEEEE", "#706E70", true, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, true );
    s_defaultStyle[GeoDataFeature::RailwayMonorail]          = StyleBuilder::Private::createStyle( 2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false );
    s_defaultStyle[GeoDataFeature::RailwayFunicular]         = StyleBuilder::Private::createStyle( 2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false );

    s_defaultStyle[GeoDataFeature::Building]                 = StyleBuilder::Private::createStyle( 1, 0, buildingColor, buildingColor.darker(),
                                                                                   true, true, Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false );

    s_defaultStyle[GeoDataFeature::Landmass]                 = StyleBuilder::Private::createWayStyle( "#F1EEE8", "#F1EEE8", true, true );

    s_defaultStyle[GeoDataFeature::AdminLevel1]              = StyleBuilder::Private::createStyle(0.0, 0.0, "#DF9CCF", "#DF9CCF", true, true, Qt::SolidPattern, Qt::CustomDashLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3 );
    s_defaultStyle[GeoDataFeature::AdminLevel2]              = StyleBuilder::Private::createStyle(2.0, 0.0, "#DF9CCF", "#DF9CCF", true, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3 );
    s_defaultStyle[GeoDataFeature::AdminLevel3]              = StyleBuilder::Private::createStyle(1.8, 0.0, "#DF9CCF", "#DF9CCF", true, true, Qt::SolidPattern, Qt::DashLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3 );
    s_defaultStyle[GeoDataFeature::AdminLevel4]              = StyleBuilder::Private::createStyle(1.5, 0.0, "#DF9CCF", "#DF9CCF", true, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3 );
    s_defaultStyle[GeoDataFeature::AdminLevel5]              = StyleBuilder::Private::createStyle(1.25, 0.0, "#DF9CCF", "#DF9CCF", true, true, Qt::SolidPattern, Qt::DashDotDotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3 );
    s_defaultStyle[GeoDataFeature::AdminLevel6]              = StyleBuilder::Private::createStyle(1, 0.0, "#DF9CCF", "#DF9CCF", true, true, Qt::SolidPattern, Qt::DashDotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3 );
    s_defaultStyle[GeoDataFeature::AdminLevel7]              = StyleBuilder::Private::createStyle(1, 0.0, "#DF9CCF", "#DF9CCF", true, true, Qt::SolidPattern, Qt::DashLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3 );
    s_defaultStyle[GeoDataFeature::AdminLevel8]              = StyleBuilder::Private::createStyle(1, 0.0, "#DF9CCF", "#DF9CCF", true, true, Qt::SolidPattern, Qt::DashLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3 );
    s_defaultStyle[GeoDataFeature::AdminLevel9]              = StyleBuilder::Private::createStyle(1.5, 0.0, "#DF9CCF", "#DF9CCF", true, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3 );
    s_defaultStyle[GeoDataFeature::AdminLevel10]             = StyleBuilder::Private::createStyle(1.5, 0.0, "#DF9CCF", "#DF9CCF", true, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3 );
    s_defaultStyle[GeoDataFeature::AdminLevel11]             = StyleBuilder::Private::createStyle(1.5, 0.0, "#DF9CCF", "#DF9CCF", true, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3 );

    s_defaultStyle[GeoDataFeature::Satellite]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/satellite.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

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


StyleBuilder::StyleBuilder() :
    d(new Private)
{
    // nothing to do
}

StyleBuilder::~StyleBuilder()
{
    delete d;
}

GeoDataStyle::ConstPtr StyleBuilder::createStyle(const StyleParameters &parameters) const
{
    if (!parameters.feature) {
        Q_ASSERT(false && "Must not pass a null feature to StyleBuilder::createStyle");
        return GeoDataStyle::Ptr();
    }

    if (parameters.feature->customStyle()) {
        return parameters.feature->customStyle();
    }

    auto const visualCategory = parameters.feature->visualCategory();
    GeoDataStyle::ConstPtr style = presetStyle(visualCategory);
    if (parameters.feature->nodeType() != GeoDataTypes::GeoDataPlacemarkType) {
        return style;
    }

    GeoDataPlacemark const * placemark = static_cast<GeoDataPlacemark const *>(parameters.feature);
    OsmPlacemarkData const & osmData = placemark->osmData();
    if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPointType) {
        if (visualCategory == GeoDataFeature::NaturalTree) {
            GeoDataCoordinates const coordinates = placemark->coordinate();
            qreal const lat = coordinates.latitude(GeoDataCoordinates::Degree);
            if (qAbs(lat) > 15) {
                /** @todo Should maybe auto-adjust to MarbleClock at some point */
                int const month = QDate::currentDate().month();
                QString season;
                bool const southernHemisphere = lat < 0;
                if (southernHemisphere) {
                    if (month >= 3 && month <= 5) {
                        season = "autumn";
                    } else if (month >= 6 && month <= 8) {
                        season = "winter";
                    }
                } else {
                    if (month >= 9 && month <= 11) {
                        season = "autumn";
                    } else if (month == 12 || month == 1 || month == 2) {
                        season = "winter";
                    }
                }

                if (!season.isEmpty()) {
                    GeoDataIconStyle iconStyle = style->iconStyle();
                    QString const bitmap = QString("bitmaps/osmcarto/symbols/48/individual/tree-29-%1.png").arg(season);
                    iconStyle.setIconPath(MarbleDirs::path(bitmap));

                    GeoDataStyle::Ptr newStyle(new GeoDataStyle(*style));
                    newStyle->setIconStyle(iconStyle);
                    style = newStyle;
                }
            }
        }
    } else if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLinearRingType) {
        if(visualCategory == GeoDataFeature::AmenityGraveyard || visualCategory == GeoDataFeature::LanduseCemetery) {
            bool adjustStyle = true;
            GeoDataPolyStyle polyStyle = style->polyStyle();
            if( osmData.containsTag("religion","jewish") ){
                polyStyle.setTexturePath(MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_jewish.png"));
            } else if( osmData.containsTag("religion","christian") ){
                polyStyle.setTexturePath(MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_christian.png"));
            } else if( osmData.containsTag("religion","INT-generic") ){
                polyStyle.setTexturePath(MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_generic.png"));
            } else {
                adjustStyle = false;
            }
            if (adjustStyle) {
                GeoDataStyle::Ptr newStyle(new GeoDataStyle(*style));
                newStyle->setPolyStyle(polyStyle);
                style = newStyle;
            }
        }

        QList<GeoDataFeature::GeoDataVisualCategory> categories = OsmPresetLibrary::visualCategories(osmData);
        foreach(GeoDataFeature::GeoDataVisualCategory category, categories) {
            const GeoDataStyle::ConstPtr categoryStyle = GeoDataFeature::presetStyle(category);
            if (!categoryStyle->iconStyle().iconPath().isEmpty()) {
                GeoDataStyle::Ptr newStyle(new GeoDataStyle(*style));
                newStyle->setIconStyle(categoryStyle->iconStyle());
                style = newStyle;
            }
        }
    } else if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType) {
        GeoDataPolyStyle polyStyle = style->polyStyle();
        GeoDataLineStyle lineStyle = style->lineStyle();
        lineStyle.setCosmeticOutline(true);

        if (visualCategory >= GeoDataFeature::HighwayService &&
                visualCategory <= GeoDataFeature::HighwayMotorway) {

            if (parameters.tileLevel >= 0 && parameters.tileLevel <= 7) {
                /** @todo: Dummy implementation for dynamic style changes based on tile level, replace with sane values */
                lineStyle.setPhysicalWidth(0.0);
                lineStyle.setWidth(3.0);
            } else if (parameters.tileLevel >= 0 && parameters.tileLevel <= 9) {
                /** @todo: Dummy implementation for dynamic style changes based on tile level, replace with sane values */
                lineStyle.setPhysicalWidth(0.0);
                lineStyle.setWidth(4.0);
            } else {
                bool const isOneWay = osmData.containsTag("oneway", "yes") || osmData.containsTag("oneway", "-1");
                int const lanes = isOneWay ? 1 : 2; // also for motorway which implicitly is one way, but has two lanes and each direction has its own highway
                double const laneWidth = 3.0;
                double const margins = visualCategory == GeoDataFeature::HighwayMotorway ? 2.0 : (isOneWay ? 1.0 : 0.0);
                double const physicalWidth = margins + lanes * laneWidth;
                lineStyle.setPhysicalWidth(physicalWidth);
            }

            QString const accessValue = osmData.tagValue("access");
            if (accessValue == "private" || accessValue == "no" || accessValue == "agricultural" || accessValue == "delivery" || accessValue == "forestry") {
                QColor polyColor = polyStyle.color();
                qreal hue, sat, val;
                polyColor.getHsvF(&hue, &sat, &val);
                polyColor.setHsvF(0.98, qMin(1.0, 0.2 + sat), val);
                polyStyle.setColor(polyColor);
                lineStyle.setColor(lineStyle.color().darker(150));
            }

            if (osmData.containsTag("tunnel", "yes") ) {
                QColor polyColor = polyStyle.color();
                qreal hue, sat, val;
                polyColor.getHsvF(&hue, &sat, &val);
                polyColor.setHsvF(hue, 0.25 * sat, 0.95 * val);
                polyStyle.setColor(polyColor);
                lineStyle.setColor(lineStyle.color().lighter(115));
            }

        } else if (visualCategory == GeoDataFeature::NaturalWater) {
            QString const widthValue = osmData.tagValue("width").replace(" meters", QString()).replace(" m", QString());
            bool ok;
            float const width = widthValue.toFloat(&ok);
            lineStyle.setPhysicalWidth(ok ? qBound(0.1f, width, 200.0f) : 0.0f);
        }

        GeoDataStyle::Ptr newStyle(new GeoDataStyle(*style));
        newStyle->setPolyStyle(polyStyle);
        newStyle->setLineStyle(lineStyle);

        bool const hideLabel = visualCategory == GeoDataFeature::HighwayTrack
                || (visualCategory >= GeoDataFeature::RailwayRail && visualCategory <= GeoDataFeature::RailwayFunicular);
        if (hideLabel) {
            newStyle->labelStyle().setColor(QColor(Qt::transparent));
        }

        style = newStyle;
    }

    return style;
}

GeoDataStyle::ConstPtr StyleBuilder::presetStyle(GeoDataFeature::GeoDataVisualCategory visualCategory) const
{
    if (!d->s_defaultStyleInitialized) {
        d->initializeDefaultStyles();
    }

    if (visualCategory != GeoDataFeature::None && d->s_defaultStyle[visualCategory] ) {
        return d->s_defaultStyle[visualCategory];
    } else {
        return d->s_defaultStyle[GeoDataFeature::Default];
    }
}

void StyleBuilder::reset()
{
    d->s_defaultStyleInitialized = false;
}

StyleParameters::StyleParameters(const GeoDataFeature *feature_, int tileLevel_) :
    feature(feature_),
    tileLevel(tileLevel_)
{
    // nothing to do
}

}
