//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2007      Murad Tagirov <tmurad@gmail.com>
// Copyright 2009      Patrick Spendrin <ps_ml@gmx.de>
// Copyright 2015      Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#include "StyleBuilder.h"

#include "MarbleDirs.h"
#include "OsmPlacemarkData.h"
#include "GeoDataTypes.h"
#include "GeoDataGeometry.h"
#include "GeoDataPlacemark.h"
#include "GeoDataIconStyle.h"
#include "GeoDataLabelStyle.h"
#include "GeoDataLineStyle.h"
#include "GeoDataPolyStyle.h"

#include <QApplication>
#include <QFont>
#include <QImage>
#include <QDate>
#include <QSet>
#include <QScreen>

namespace Marble {

class StyleBuilder::Private
{
public:
    Private();

    GeoDataStyle::Ptr createStyle(qreal width, qreal realWidth, const QColor& color,
                                      const QColor& outlineColor, bool fill, bool outline,
                                      Qt::BrushStyle brushStyle, Qt::PenStyle penStyle,
                                      Qt::PenCapStyle capStyle, bool lineBackground,
                                      const QVector< qreal >& dashPattern,
                                      const QFont& font, const QColor& fontColor = Qt::black,
                                      const QString& texturePath = QString()) const;
    GeoDataStyle::Ptr createPOIStyle(const QFont &font, const QString &bitmap,
                                         const QColor &textColor = Qt::black,
                                         const QColor &color = QColor( 0xBE, 0xAD, 0xAD ),
                                         const QColor &outline = QColor( 0xBE, 0xAD, 0xAD ).darker(),
                                         bool fill=true, bool renderOutline=true) const;
    GeoDataStyle::Ptr createOsmPOIStyle(const QFont &font, const QString &bitmap,
                                         const QColor &textColor = Qt::black,
                                         const QColor &color = QColor( 0xBE, 0xAD, 0xAD ),
                                         const QColor &outline = QColor(0xBE, 0xAD, 0xAD).darker()) const;
    GeoDataStyle::Ptr createOsmPOIRingStyle(const QFont &font, const QString &bitmap,
                                         const QColor &textColor = Qt::black,
                                         const QColor &color = QColor( 0xBE, 0xAD, 0xAD ),
                                         const QColor &outline = QColor(0xBE, 0xAD, 0xAD).darker()) const;
    GeoDataStyle::Ptr createOsmPOIAreaStyle(const QFont &font, const QString &bitmap,
                                         const QColor &textColor = Qt::black,
                                         const QColor &color = QColor( 0xBE, 0xAD, 0xAD ),
                                         const QColor &outline = QColor(0xBE, 0xAD, 0xAD).darker()) const;
    GeoDataStyle::Ptr createHighwayStyle(const QColor& color, const QColor& outlineColor, const QFont& font,
                                             const QColor& fontColor = Qt::black,
                                             qreal width = 1, qreal realWidth = 0.0,
                                             Qt::PenStyle penStyle = Qt::SolidLine,
                                             Qt::PenCapStyle capStyle = Qt::RoundCap,
                                             bool lineBackground = false) const;
    GeoDataStyle::Ptr createWayStyle(const QColor& color, const QColor& outlineColor,
                                         bool fill = true, bool outline = true,
                                         Qt::BrushStyle brushStyle = Qt::SolidPattern,
                                         const QString& texturePath = QString()) const;

    void initializeDefaultStyles();

    static QString createPaintLayerItem(const QString &itemType, GeoDataFeature::GeoDataVisualCategory visualCategory, const QString &subType = QString());

    static void initializeOsmVisualCategories();

    int m_defaultMinZoomLevels[GeoDataFeature::LastIndex];
    int m_maximumZoomLevel;
    QColor m_defaultLabelColor;
    QFont m_defaultFont;
    GeoDataStyle::Ptr m_defaultStyle[GeoDataFeature::LastIndex];
    bool m_defaultStyleInitialized;

    /**
     * @brief s_visualCategories contains osm tag mappings to GeoDataVisualCategories
     */
    static QHash<OsmTag, GeoDataFeature::GeoDataVisualCategory> s_visualCategories;
};

QHash<StyleBuilder::OsmTag, GeoDataFeature::GeoDataVisualCategory> StyleBuilder::Private::s_visualCategories;

StyleBuilder::Private::Private() :
    m_maximumZoomLevel(15),
    m_defaultLabelColor(Qt::black),
    m_defaultFont(QStringLiteral("Sans Serif")),
    m_defaultStyle(),
    m_defaultStyleInitialized(false)
{
    for ( int i = 0; i < GeoDataFeature::LastIndex; i++ )
        m_defaultMinZoomLevels[i] = m_maximumZoomLevel;

    m_defaultMinZoomLevels[GeoDataFeature::Default]             = 1;
    m_defaultMinZoomLevels[GeoDataFeature::NaturalReef]         = 3;
    m_defaultMinZoomLevels[GeoDataFeature::NaturalWater]        = 3;
    m_defaultMinZoomLevels[GeoDataFeature::NaturalWood]         = 8;
    m_defaultMinZoomLevels[GeoDataFeature::NaturalBeach]        = 10;
    m_defaultMinZoomLevels[GeoDataFeature::NaturalWetland]      = 10;
    m_defaultMinZoomLevels[GeoDataFeature::NaturalGlacier]      = 3;
    m_defaultMinZoomLevels[GeoDataFeature::NaturalIceShelf]     = 3;
    m_defaultMinZoomLevels[GeoDataFeature::NaturalScrub]        = 10;
    m_defaultMinZoomLevels[GeoDataFeature::NaturalCliff]        = 15;
    m_defaultMinZoomLevels[GeoDataFeature::NaturalPeak]         = 11;
    m_defaultMinZoomLevels[GeoDataFeature::BarrierCityWall]     = 15;
    m_defaultMinZoomLevels[GeoDataFeature::Building]            = 15;

    m_defaultMinZoomLevels[GeoDataFeature::ManmadeBridge]       = 15;

        // OpenStreetMap highways
    m_defaultMinZoomLevels[GeoDataFeature::HighwaySteps]        = 15;
    m_defaultMinZoomLevels[GeoDataFeature::HighwayUnknown]      = 13;
    m_defaultMinZoomLevels[GeoDataFeature::HighwayPath]         = 13;
    m_defaultMinZoomLevels[GeoDataFeature::HighwayTrack]        = 13;
    m_defaultMinZoomLevels[GeoDataFeature::HighwayPedestrian]   = 13;
    m_defaultMinZoomLevels[GeoDataFeature::HighwayFootway]      = 13;
    m_defaultMinZoomLevels[GeoDataFeature::HighwayCycleway]     = 13;
    m_defaultMinZoomLevels[GeoDataFeature::HighwayService]      = 13;
    m_defaultMinZoomLevels[GeoDataFeature::HighwayRoad]         = 13;
    m_defaultMinZoomLevels[GeoDataFeature::HighwayTertiaryLink] = 10;
    m_defaultMinZoomLevels[GeoDataFeature::HighwayTertiary]     = 10;
    m_defaultMinZoomLevels[GeoDataFeature::HighwaySecondaryLink]= 10;
    m_defaultMinZoomLevels[GeoDataFeature::HighwaySecondary]    = 9;
    m_defaultMinZoomLevels[GeoDataFeature::HighwayPrimaryLink]  = 10;
    m_defaultMinZoomLevels[GeoDataFeature::HighwayPrimary]      = 8;
    m_defaultMinZoomLevels[GeoDataFeature::HighwayTrunkLink]    = 10;
    m_defaultMinZoomLevels[GeoDataFeature::HighwayTrunk]        = 7;
    m_defaultMinZoomLevels[GeoDataFeature::HighwayMotorwayLink] = 10;
    m_defaultMinZoomLevels[GeoDataFeature::HighwayMotorway]     = 6;
    m_defaultMinZoomLevels[GeoDataFeature::TransportAirportRunway] = 15;
    m_defaultMinZoomLevels[GeoDataFeature::TransportAirportTaxiway] = 15;
    m_defaultMinZoomLevels[GeoDataFeature::TransportAirportApron] = 15;

#if 0 // not needed as long as default min zoom level is 15
    for(int i = GeoDataFeature::AccomodationCamping; i <= GeoDataFeature::ReligionSikh; i++)
        s_defaultMinZoomLevels[i] = 15;
#endif

    m_defaultMinZoomLevels[GeoDataFeature::AmenityGraveyard]    = 14;
    m_defaultMinZoomLevels[GeoDataFeature::AmenityFountain]     = 17;

    m_defaultMinZoomLevels[GeoDataFeature::MilitaryDangerArea]  = 11;

    m_defaultMinZoomLevels[GeoDataFeature::LeisureMarina]       = 13;
    m_defaultMinZoomLevels[GeoDataFeature::LeisurePark]         = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LeisurePlayground]   = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseAllotments]   = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseBasin]        = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseCemetery]     = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseCommercial]   = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseConstruction] = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseFarmland]     = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseFarmyard]     = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseGarages]      = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseGrass]        = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseIndustrial]   = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseLandfill]     = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseMeadow]       = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseMilitary]     = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseQuarry]       = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseRailway]      = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseReservoir]    = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseResidential]  = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseRetail]       = 11;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseOrchard]      = 14;
    m_defaultMinZoomLevels[GeoDataFeature::LanduseVineyard]     = 14;

    m_defaultMinZoomLevels[GeoDataFeature::RailwayRail]         = 6;
    m_defaultMinZoomLevels[GeoDataFeature::RailwayNarrowGauge]  = 6;
    m_defaultMinZoomLevels[GeoDataFeature::RailwayTram]         = 14;
    m_defaultMinZoomLevels[GeoDataFeature::RailwayLightRail]    = 12;
    m_defaultMinZoomLevels[GeoDataFeature::RailwayAbandoned]    = 10;
    m_defaultMinZoomLevels[GeoDataFeature::RailwaySubway]       = 13;
    m_defaultMinZoomLevels[GeoDataFeature::RailwayPreserved]    = 13;
    m_defaultMinZoomLevels[GeoDataFeature::RailwayMiniature]    = 13;
    m_defaultMinZoomLevels[GeoDataFeature::RailwayConstruction] = 10;
    m_defaultMinZoomLevels[GeoDataFeature::RailwayMonorail]     = 12;
    m_defaultMinZoomLevels[GeoDataFeature::RailwayFunicular]    = 13;
    m_defaultMinZoomLevels[GeoDataFeature::TransportPlatform]   = 16;

    m_defaultMinZoomLevels[GeoDataFeature::Satellite]           = 0;

    m_defaultMinZoomLevels[GeoDataFeature::Landmass]            = 0;
    m_defaultMinZoomLevels[GeoDataFeature::UrbanArea]           = 3;
    m_defaultMinZoomLevels[GeoDataFeature::InternationalDateLine]      = 1;
    m_defaultMinZoomLevels[GeoDataFeature::Bathymetry]          = 1;


    m_defaultMinZoomLevels[GeoDataFeature::AdminLevel1]         = 0;
    m_defaultMinZoomLevels[GeoDataFeature::AdminLevel2]         = 1;
    m_defaultMinZoomLevels[GeoDataFeature::AdminLevel3]         = 1;
    m_defaultMinZoomLevels[GeoDataFeature::AdminLevel4]         = 2;
    m_defaultMinZoomLevels[GeoDataFeature::AdminLevel5]         = 4;
    m_defaultMinZoomLevels[GeoDataFeature::AdminLevel6]         = 5;
    m_defaultMinZoomLevels[GeoDataFeature::AdminLevel7]         = 5;
    m_defaultMinZoomLevels[GeoDataFeature::AdminLevel8]         = 7;
    m_defaultMinZoomLevels[GeoDataFeature::AdminLevel9]         = 7;
    m_defaultMinZoomLevels[GeoDataFeature::AdminLevel10]        = 8;
    m_defaultMinZoomLevels[GeoDataFeature::AdminLevel11]        = 8;

    m_defaultMinZoomLevels[GeoDataFeature::BoundaryMaritime]    = 1;

    for (int i = 0; i < GeoDataFeature::LastIndex; ++i) {
        m_maximumZoomLevel = qMax(m_maximumZoomLevel, m_defaultMinZoomLevels[i]);
    }
}

GeoDataStyle::Ptr StyleBuilder::Private::createPOIStyle(const QFont &font, const QString &path,
                                     const QColor &textColor, const QColor &color, const QColor &outline, bool fill, bool renderOutline) const
{
    GeoDataStyle::Ptr style =  createStyle(1, 0, color, outline, fill, renderOutline, Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false, QVector<qreal>(), font);
    style->setIconStyle( GeoDataIconStyle( path) );
    auto const screen = QApplication::screens().first();
    double const physicalSize = 6.0; // mm
    int const pixelSize = qRound(physicalSize * screen->physicalDotsPerInch() / (IN2M * M2MM));
    style->iconStyle().setSize(QSize(pixelSize, pixelSize));
    style->setLabelStyle( GeoDataLabelStyle( font, textColor ) );
    style->labelStyle().setAlignment(GeoDataLabelStyle::Center);
    return style;
}

GeoDataStyle::Ptr StyleBuilder::Private::createOsmPOIStyle( const QFont &font, const QString &imagePath,
                                     const QColor &textColor, const QColor &color, const QColor &outline) const
{
    QString const path = MarbleDirs::path(QLatin1String("svg/osmcarto/svg/") + imagePath + QLatin1String(".svg"));
    return createPOIStyle(font, path, textColor, color, outline, false, false);
}

GeoDataStyle::Ptr StyleBuilder::Private::createOsmPOIRingStyle( const QFont &font, const QString &imagePath,
                                     const QColor &textColor, const QColor &color, const QColor &outline) const
{
    QString const path = MarbleDirs::path(QLatin1String("svg/osmcarto/svg/") + imagePath + QLatin1String(".svg"));
    return createPOIStyle(font, path, textColor, color, outline, false, true);
}

GeoDataStyle::Ptr StyleBuilder::Private::createOsmPOIAreaStyle( const QFont &font, const QString &imagePath,
                                     const QColor &textColor, const QColor &color, const QColor &outline) const
{
    QString const path = MarbleDirs::path(QLatin1String("svg/osmcarto/svg/") + imagePath + QLatin1String(".svg"));
    return createPOIStyle(font, path, textColor, color, outline, true, false);
}


GeoDataStyle::Ptr StyleBuilder::Private::createHighwayStyle( const QColor& color, const QColor& outlineColor, const QFont& font,
                                                             const QColor& fontColor, qreal width, qreal realWidth, Qt::PenStyle penStyle,
                                                             Qt::PenCapStyle capStyle, bool lineBackground) const
{
    return createStyle( width, realWidth, color, outlineColor, true, true, Qt::SolidPattern, penStyle, capStyle, lineBackground, QVector< qreal >(), font, fontColor );
}

GeoDataStyle::Ptr StyleBuilder::Private::createWayStyle( const QColor& color, const QColor& outlineColor,
                                     bool fill, bool outline, Qt::BrushStyle brushStyle, const QString& texturePath) const
{
    return createStyle(1, 0, color, outlineColor, fill, outline, brushStyle, Qt::SolidLine, Qt::RoundCap, false, QVector<qreal>(), m_defaultFont, Qt::black, texturePath);
}

GeoDataStyle::Ptr StyleBuilder::Private::createStyle( qreal width, qreal realWidth, const QColor& color,
                                  const QColor& outlineColor, bool fill, bool outline, Qt::BrushStyle brushStyle, Qt::PenStyle penStyle,
                                  Qt::PenCapStyle capStyle, bool lineBackground, const QVector< qreal >& dashPattern,
                                  const QFont& font, const QColor& fontColor, const QString& texturePath) const
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

    if (m_defaultStyleInitialized) {
        return;
    }

    m_defaultStyleInitialized = true;

    QString defaultFamily = m_defaultFont.family();

#ifdef Q_OS_MACX
    int defaultSize = 10;
#else
    int defaultSize = 8;
#endif

    QColor const defaultLabelColor = m_defaultLabelColor;

    m_defaultStyle[GeoDataFeature::None]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::Default]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/default_location.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::Unknown]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::SmallCity]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_4_white.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::SmallCountyCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_4_yellow.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::SmallStateCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_4_orange.png" ),
              QFont( defaultFamily, defaultSize, 50, true  ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::SmallNationCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_4_red.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::MediumCity]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_3_white.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::MediumCountyCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_3_yellow.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::MediumStateCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_3_orange.png" ),
              QFont( defaultFamily, defaultSize, 50, true  ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::MediumNationCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_3_red.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::BigCity]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_2_white.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::BigCountyCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_2_yellow.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::BigStateCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_2_orange.png" ),
              QFont( defaultFamily, defaultSize, 50, true  ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::BigNationCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_2_red.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::LargeCity]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_1_white.png" ),
              QFont( defaultFamily, defaultSize, 75, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::LargeCountyCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_1_yellow.png" ),
              QFont( defaultFamily, defaultSize, 75, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::LargeStateCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_1_orange.png" ),
              QFont( defaultFamily, defaultSize, 75, true  ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::LargeNationCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_1_red.png" ),
              QFont( defaultFamily, defaultSize, 75, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::Nation]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, (int)(defaultSize * 1.2 ), 75, false ), QColor( "#404040" ) ));
    // Align area labels centered
    m_defaultStyle[GeoDataFeature::Nation]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    QFont osmCityFont = QFont(defaultFamily, (int)(defaultSize * 2.0 ), 75, false);
    m_defaultStyle[GeoDataFeature::PlaceCity] = createOsmPOIStyle(osmCityFont, "place/place-6", QColor( "#202020" ));
    m_defaultStyle[GeoDataFeature::PlaceCityCapital] = createOsmPOIStyle(osmCityFont, "place/place-capital-6", QColor( "#202020" ));
    m_defaultStyle[GeoDataFeature::PlaceSuburb] = createOsmPOIStyle(osmCityFont, QString(), QColor( "#707070" ));
    m_defaultStyle[GeoDataFeature::PlaceHamlet] = createOsmPOIStyle(osmCityFont, QString(), QColor( "#707070" ));
    m_defaultStyle[GeoDataFeature::PlaceLocality] = createOsmPOIStyle(osmCityFont, QString(), QColor( "#707070" ));
    m_defaultStyle[GeoDataFeature::PlaceTown] = createOsmPOIStyle(osmCityFont, "place/place-6", QColor( "#404040" ));
    m_defaultStyle[GeoDataFeature::PlaceTownCapital] = createOsmPOIStyle(osmCityFont, "place/place-capital-6", QColor( "#404040" ));
    m_defaultStyle[GeoDataFeature::PlaceVillage] = createOsmPOIStyle(osmCityFont, "place/place-6", QColor( "#505050" ));
    m_defaultStyle[GeoDataFeature::PlaceVillageCapital] = createOsmPOIStyle(osmCityFont, "place/place-capital-6", QColor( "#505050" ));
    for (int i=GeoDataFeature::PlaceCity; i<=GeoDataFeature::PlaceVillageCapital; ++i) {
        m_defaultStyle[GeoDataFeature::GeoDataVisualCategory(i)]->polyStyle().setFill(false);
        m_defaultStyle[GeoDataFeature::GeoDataVisualCategory(i)]->polyStyle().setOutline(false);
        m_defaultStyle[GeoDataFeature::GeoDataVisualCategory(i)]->labelStyle().setAlignment( GeoDataLabelStyle::Center );
        m_defaultStyle[GeoDataFeature::GeoDataVisualCategory(i)]->iconStyle().setScale(0.25);
    }

    m_defaultStyle[GeoDataFeature::Mountain]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/mountain_1.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::Volcano]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/volcano_1.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::Mons]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/mountain_1.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::Valley]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/valley.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::Continent]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, (int)(defaultSize * 1.7 ), 50, false ), QColor( "#bf0303" ) ));
    // Align area labels centered
    m_defaultStyle[GeoDataFeature::Continent]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    m_defaultStyle[GeoDataFeature::Ocean]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, (int)(defaultSize * 1.7 ), 50, true ), QColor( "#2c72c7" ) ));
    // Align area labels centered
    m_defaultStyle[GeoDataFeature::Ocean]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    m_defaultStyle[GeoDataFeature::OtherTerrain]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/other.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::Crater]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/crater.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::Mare]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, (int)(defaultSize * 1.7 ), 50, false ), QColor( "#bf0303" ) ));
    // Align area labels centered
    m_defaultStyle[GeoDataFeature::Mare]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    m_defaultStyle[GeoDataFeature::GeographicPole]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/pole_1.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::MagneticPole]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/pole_2.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::ShipWreck]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/shipwreck.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.8 ), 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::AirPort]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/airport.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::Observatory]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/observatory.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::OsmSite]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/osm.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::Coordinate]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/coordinate.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));


    m_defaultStyle[GeoDataFeature::MannedLandingSite]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/manned_landing.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::RoboticRover]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/robotic_rover.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::UnmannedSoftLandingSite]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/unmanned_soft_landing.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::UnmannedHardLandingSite]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/unmanned_hard_landing.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::Folder]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/folder.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataFeature::Bookmark]
        = createPOIStyle(QFont( defaultFamily, defaultSize, 50, false ),
                                                MarbleDirs::path("svg/bookmark.svg"), defaultLabelColor );
    m_defaultStyle[GeoDataFeature::Bookmark]->iconStyle().setScale(0.75);

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
    m_defaultStyle[GeoDataFeature::AccomodationCamping]      = createOsmPOIRingStyle(osmFont, "transportation/camping.16", transportationColor);
    m_defaultStyle[GeoDataFeature::AccomodationHostel]       = createOsmPOIStyle(osmFont, "transportation/hostel.16", transportationColor);
    m_defaultStyle[GeoDataFeature::AccomodationHotel]        = createOsmPOIStyle(osmFont, "transportation/hotel.16", transportationColor);
    m_defaultStyle[GeoDataFeature::AccomodationMotel]        = createOsmPOIStyle(osmFont, "transportation/motel.16", transportationColor);
    m_defaultStyle[GeoDataFeature::AccomodationYouthHostel]  = createOsmPOIStyle(osmFont, "transportation/hostel.16", transportationColor);
    m_defaultStyle[GeoDataFeature::AccomodationGuestHouse]   = createOsmPOIStyle(osmFont, "transportation/guest_house.16", transportationColor);
    m_defaultStyle[GeoDataFeature::AmenityLibrary]           = createOsmPOIStyle(osmFont, "amenity/library.20", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityKindergarten]      = createOsmPOIAreaStyle(osmFont, QString(), amenityColor, educationalAreasAndHospital, amenityColor);
    m_defaultStyle[GeoDataFeature::EducationCollege]         = createOsmPOIAreaStyle(osmFont, QString(), amenityColor, educationalAreasAndHospital, amenityColor);
    m_defaultStyle[GeoDataFeature::EducationSchool]          = createOsmPOIAreaStyle(osmFont, QString(), amenityColor, educationalAreasAndHospital, amenityColor);
    m_defaultStyle[GeoDataFeature::EducationUniversity]      = createOsmPOIAreaStyle(osmFont, QString(), amenityColor, educationalAreasAndHospital, amenityColor);
    m_defaultStyle[GeoDataFeature::FoodBar]                  = createOsmPOIStyle(osmFont, "amenity/bar.16", amenityColor);
    m_defaultStyle[GeoDataFeature::FoodBiergarten]           = createOsmPOIStyle(osmFont, "amenity/biergarten.16", amenityColor);
    m_defaultStyle[GeoDataFeature::FoodCafe]                 = createOsmPOIStyle(osmFont, "amenity/cafe.16", amenityColor);
    m_defaultStyle[GeoDataFeature::FoodFastFood]             = createOsmPOIStyle(osmFont, "amenity/fast_food.16", amenityColor);
    m_defaultStyle[GeoDataFeature::FoodPub]                  = createOsmPOIStyle(osmFont, "amenity/pub.16", amenityColor);
    m_defaultStyle[GeoDataFeature::FoodRestaurant]           = createOsmPOIStyle(osmFont, "amenity/restaurant.16", amenityColor);

    m_defaultStyle[GeoDataFeature::HealthDentist]            = createOsmPOIStyle(osmFont, "health/dentist.16", healthColor);
    m_defaultStyle[GeoDataFeature::HealthDoctors]            = createOsmPOIStyle(osmFont, "health/doctors.16", healthColor);
    m_defaultStyle[GeoDataFeature::HealthHospital]           = createOsmPOIStyle(osmFont, "health/hospital.16", healthColor, educationalAreasAndHospital, amenityColor);
    m_defaultStyle[GeoDataFeature::HealthPharmacy]           = createOsmPOIStyle(osmFont, "health/pharmacy.16", healthColor);
    m_defaultStyle[GeoDataFeature::HealthVeterinary]         = createOsmPOIStyle(osmFont, "health/veterinary-14", healthColor);

    m_defaultStyle[GeoDataFeature::MoneyAtm]                 = createOsmPOIStyle(osmFont, "amenity/atm.16", amenityColor);
    m_defaultStyle[GeoDataFeature::MoneyBank]                = createOsmPOIStyle(osmFont, "amenity/bank.16", amenityColor);

    m_defaultStyle[GeoDataFeature::AmenityArchaeologicalSite] = createOsmPOIAreaStyle(osmFont, "amenity/archaeological_site.16", amenityColor, Qt::transparent);
    m_defaultStyle[GeoDataFeature::AmenityEmbassy]           = createOsmPOIStyle(osmFont, "transportation/embassy.16", transportationColor);
    m_defaultStyle[GeoDataFeature::AmenityEmergencyPhone]    = createOsmPOIStyle(osmFont, "amenity/emergency_phone.16", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityWaterPark]         = createOsmPOIStyle(osmFont, "amenity/water_park.16", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityCommunityCentre]   = createOsmPOIStyle(osmFont, "amenity/community_centre-14", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityFountain]          = createOsmPOIStyle(osmFont, "amenity/fountain-14", QColor("#7989de"), waterColor, waterColor.darker(150));
    m_defaultStyle[GeoDataFeature::AmenityNightClub]         = createOsmPOIStyle(osmFont, "amenity/nightclub.18", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityBench]             = createOsmPOIStyle(osmFont, "individual/bench.16", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityCourtHouse]        = createOsmPOIStyle(osmFont, "amenity/courthouse-16", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityFireStation]       = createOsmPOIStyle(osmFont, "amenity/firestation.16", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityHuntingStand]      = createOsmPOIStyle(osmFont, "manmade/hunting-stand.16", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityPolice]            = createOsmPOIStyle(osmFont, "amenity/police.16", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityPostBox]           = createOsmPOIStyle(osmFont, "amenity/post_box-12", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityPostOffice]        = createOsmPOIStyle(osmFont, "amenity/post_office-14", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityPrison]            = createOsmPOIStyle(osmFont, "amenity/prison.16", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityRecycling]         = createOsmPOIStyle(osmFont, "amenity/recycling.16", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityShelter]           = createOsmPOIStyle(osmFont, "amenity/shelter-14", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityTelephone]         = createOsmPOIStyle(osmFont, "amenity/telephone.16", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityToilets]           = createOsmPOIStyle(osmFont, "amenity/toilets.16", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityTownHall]          = createOsmPOIStyle(osmFont, "amenity/town_hall.16", amenityColor);
    m_defaultStyle[GeoDataFeature::AmenityWasteBasket]       = createOsmPOIStyle(osmFont, "individual/waste_basket.10", amenityColor);

    m_defaultStyle[GeoDataFeature::AmenityDrinkingWater]     = createOsmPOIStyle(osmFont, "amenity/drinking_water.16", amenityColor);

    m_defaultStyle[GeoDataFeature::NaturalPeak]              = createOsmPOIStyle(osmFont, "individual/peak", amenityColor);
    m_defaultStyle[GeoDataFeature::NaturalPeak]->iconStyle().setScale(0.33);
    m_defaultStyle[GeoDataFeature::NaturalTree]              = createOsmPOIStyle(osmFont, "individual/tree-29", amenityColor); // tree-16 provides the official icon

    m_defaultStyle[GeoDataFeature::ShopBeverages]            = createOsmPOIStyle(osmFont, "shop/beverages-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopHifi]                 = createOsmPOIStyle(osmFont, "shop/hifi-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopSupermarket]          = createOsmPOIStyle(osmFont, "shop/shop_supermarket", shopColor);
    m_defaultStyle[GeoDataFeature::ShopAlcohol]              = createOsmPOIStyle(osmFont, "shop/shop_alcohol.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopBakery]               = createOsmPOIStyle(osmFont, "shop/shop_bakery.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopButcher]              = createOsmPOIStyle(osmFont, "shop/butcher-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopConfectionery]        = createOsmPOIStyle(osmFont, "shop/confectionery.14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopConvenience]          = createOsmPOIStyle(osmFont, "shop/shop_convenience", shopColor);
    m_defaultStyle[GeoDataFeature::ShopGreengrocer]          = createOsmPOIStyle(osmFont, "shop/greengrocer-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopSeafood]              = createOsmPOIStyle(osmFont, "shop/seafood-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopDepartmentStore]      = createOsmPOIStyle(osmFont, "shop/department_store-16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopKiosk]                = createOsmPOIStyle(osmFont, "shop/kiosk-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopBag]                  = createOsmPOIStyle(osmFont, "shop/bag-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopClothes]              = createOsmPOIStyle(osmFont, "shop/shop_clothes.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopFashion]              = createOsmPOIStyle(osmFont, "shop/shop_clothes.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopJewelry]              = createOsmPOIStyle(osmFont, "shop/shop_jewelry.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopShoes]                = createOsmPOIStyle(osmFont, "shop/shop_shoes.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopVarietyStore]         = createOsmPOIStyle(osmFont, "shop/variety_store-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopBeauty]               = createOsmPOIStyle(osmFont, "shop/beauty-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopChemist]              = createOsmPOIStyle(osmFont, "shop/chemist-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopCosmetics]            = createOsmPOIStyle(osmFont, "shop/perfumery-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopHairdresser]          = createOsmPOIStyle(osmFont, "shop/shop_hairdresser.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopOptician]             = createOsmPOIStyle(osmFont, "shop/shop_optician.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopPerfumery]            = createOsmPOIStyle(osmFont, "shop/perfumery-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopDoitYourself]         = createOsmPOIStyle(osmFont, "shop/shop_diy.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopFlorist]              = createOsmPOIStyle(osmFont, "shop/florist.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopHardware]             = createOsmPOIStyle(osmFont, "shop/shop_diy.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopFurniture]            = createOsmPOIStyle(osmFont, "shop/shop_furniture.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopElectronics]          = createOsmPOIStyle(osmFont, "shop/shop_electronics.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopMobilePhone]          = createOsmPOIStyle(osmFont, "shop/shop_mobile_phone.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopBicycle]              = createOsmPOIStyle(osmFont, "shop/shop_bicycle.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopCar]                  = createOsmPOIStyle(osmFont, "shop/shop_car", shopColor);
    m_defaultStyle[GeoDataFeature::ShopCarRepair]            = createOsmPOIStyle(osmFont, "shop/shopping_car_repair.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopCarParts]             = createOsmPOIStyle(osmFont, "shop/car_parts-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopMotorcycle]           = createOsmPOIStyle(osmFont, "shop/motorcycle-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopOutdoor]              = createOsmPOIStyle(osmFont, "shop/outdoor-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopMusicalInstrument]    = createOsmPOIStyle(osmFont, "shop/musical_instrument-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopPhoto]                = createOsmPOIStyle(osmFont, "shop/photo-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopBook]                 = createOsmPOIStyle(osmFont, "shop/shop_books.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopGift]                 = createOsmPOIStyle(osmFont, "shop/shop_gift.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopStationery]           = createOsmPOIStyle(osmFont, "shop/stationery-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopLaundry]              = createOsmPOIStyle(osmFont, "shop/laundry-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopPet]                  = createOsmPOIStyle(osmFont, "shop/shop_pet.16", shopColor);
    m_defaultStyle[GeoDataFeature::ShopToys]                 = createOsmPOIStyle(osmFont, "shop/toys-14", shopColor);
    m_defaultStyle[GeoDataFeature::ShopTravelAgency]         = createOsmPOIStyle(osmFont, "shop/travel_agency-14", shopColor);
    m_defaultStyle[GeoDataFeature::Shop]                     = createOsmPOIStyle(osmFont, "shop/shop-14", shopColor);

    m_defaultStyle[GeoDataFeature::ManmadeBridge]            = createWayStyle(QColor("#b8b8b8"), Qt::transparent, true, true);
    m_defaultStyle[GeoDataFeature::ManmadeLighthouse]        = createOsmPOIStyle(osmFont, "transportation/lighthouse.16", transportationColor, "#f2efe9", QColor( "#f2efe9" ).darker());
    m_defaultStyle[GeoDataFeature::ManmadePier]              = createStyle(0.0, 3.0, "#f2efe9", "#f2efe9", true, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont);
    m_defaultStyle[GeoDataFeature::ManmadeWaterTower]        = createOsmPOIStyle(osmFont, "amenity/water_tower.16", amenityColor);
    m_defaultStyle[GeoDataFeature::ManmadeWindMill]          = createOsmPOIStyle(osmFont, "amenity/windmill.16", amenityColor);

    m_defaultStyle[GeoDataFeature::TouristAttraction]        = createOsmPOIStyle(osmFont, "amenity/tourist_memorial.16", amenityColor);
    m_defaultStyle[GeoDataFeature::TouristCastle]            = createOsmPOIRingStyle(osmFont, "amenity/cinema.16", amenityColor);
    m_defaultStyle[GeoDataFeature::TouristCinema]            = createOsmPOIStyle(osmFont, "amenity/cinema.16", amenityColor);
    m_defaultStyle[GeoDataFeature::TouristInformation]       = createOsmPOIStyle(osmFont, "amenity/information.16", amenityColor);
    m_defaultStyle[GeoDataFeature::TouristMonument]          = createOsmPOIStyle(osmFont, "amenity/monument.16", amenityColor);
    m_defaultStyle[GeoDataFeature::TouristMuseum]            = createOsmPOIStyle(osmFont, "amenity/museum.16", amenityColor);
    m_defaultStyle[GeoDataFeature::TouristRuin]              = createOsmPOIRingStyle(osmFont, QString(), amenityColor);
    m_defaultStyle[GeoDataFeature::TouristTheatre]           = createOsmPOIStyle(osmFont, "amenity/theatre.16", amenityColor);
    m_defaultStyle[GeoDataFeature::TouristThemePark]         = createOsmPOIStyle(osmFont, QString(), amenityColor);
    m_defaultStyle[GeoDataFeature::TouristViewPoint]         = createOsmPOIStyle(osmFont, "amenity/viewpoint.16", amenityColor);
    m_defaultStyle[GeoDataFeature::TouristZoo]               = createOsmPOIRingStyle(osmFont, QString(), amenityColor, Qt::transparent);
    m_defaultStyle[GeoDataFeature::TouristAlpineHut]         = createOsmPOIStyle(osmFont, "transportation/alpinehut.16", transportationColor);
    m_defaultStyle[GeoDataFeature::TransportAerodrome]       = createOsmPOIStyle(osmFont, "airtransport/aerodrome", airTransportColor);
    m_defaultStyle[GeoDataFeature::TransportHelipad]         = createOsmPOIStyle(osmFont, "airtransport/helipad", airTransportColor);
    m_defaultStyle[GeoDataFeature::TransportAirportTerminal] = createOsmPOIAreaStyle(osmFont, QString(), airTransportColor);
    m_defaultStyle[GeoDataFeature::TransportAirportGate]     = createOsmPOIAreaStyle(osmFont, QString(), airTransportColor);
    m_defaultStyle[GeoDataFeature::TransportBusStation]      = createOsmPOIStyle(osmFont, "transportation/bus_station.16", transportationColor);
    m_defaultStyle[GeoDataFeature::TransportBusStop]         = createOsmPOIStyle(osmFont, "transportation/bus_stop.12", transportationColor);
    m_defaultStyle[GeoDataFeature::TransportCarShare]        = createOsmPOIStyle(osmFont, "transportation/car_share.16", transportationColor);
    m_defaultStyle[GeoDataFeature::TransportFuel]            = createOsmPOIStyle(osmFont, "transportation/fuel.16", transportationColor);
    m_defaultStyle[GeoDataFeature::TransportParking]         = createOsmPOIAreaStyle(osmFont, "transportation/parking", transportationColor, "#F6EEB6", QColor( "#F6EEB6" ).darker());
    m_defaultStyle[GeoDataFeature::TransportParkingSpace]    = createWayStyle("#F6EEB6", QColor( "#F6EEB6" ).darker(), true, true);
    m_defaultStyle[GeoDataFeature::TransportPlatform]        = createWayStyle("#bbbbbb", Qt::transparent, true, false);
    m_defaultStyle[GeoDataFeature::TransportTrainStation]    = createOsmPOIStyle(osmFont, "individual/railway_station", transportationColor);
    m_defaultStyle[GeoDataFeature::TransportTramStop]        = createOsmPOIStyle(osmFont, QString(), transportationColor);
    m_defaultStyle[GeoDataFeature::TransportRentalBicycle]   = createOsmPOIStyle(osmFont, "transportation/rental_bicycle.16", transportationColor);
    m_defaultStyle[GeoDataFeature::TransportRentalCar]       = createOsmPOIStyle(osmFont, "transportation/rental_car.16", transportationColor);
    m_defaultStyle[GeoDataFeature::TransportTaxiRank]        = createOsmPOIStyle(osmFont, "transportation/taxi.16", transportationColor);
    m_defaultStyle[GeoDataFeature::TransportBicycleParking]  = createOsmPOIAreaStyle(osmFont, "transportation/bicycle_parking.16", transportationColor);
    m_defaultStyle[GeoDataFeature::TransportMotorcycleParking] = createOsmPOIAreaStyle(osmFont, "transportation/motorcycle_parking.16", transportationColor);
    m_defaultStyle[GeoDataFeature::TransportSubwayEntrance]  = createOsmPOIStyle(osmFont, "transportation/subway_entrance", transportationColor);
    m_defaultStyle[GeoDataFeature::ReligionPlaceOfWorship]   = createOsmPOIStyle(osmFont, QString() /* "black/place_of_worship.16" */);
    m_defaultStyle[GeoDataFeature::ReligionBahai]            = createOsmPOIStyle(osmFont, QString());
    m_defaultStyle[GeoDataFeature::ReligionBuddhist]         = createOsmPOIStyle(osmFont, "black/buddhist.16");
    m_defaultStyle[GeoDataFeature::ReligionChristian]        = createOsmPOIStyle(osmFont, "black/christian.16");
    m_defaultStyle[GeoDataFeature::ReligionMuslim]           = createOsmPOIStyle(osmFont, "black/muslim.16");
    m_defaultStyle[GeoDataFeature::ReligionHindu]            = createOsmPOIStyle(osmFont, "black/hinduist.16");
    m_defaultStyle[GeoDataFeature::ReligionJain]             = createOsmPOIStyle(osmFont, QString());
    m_defaultStyle[GeoDataFeature::ReligionJewish]           = createOsmPOIStyle(osmFont, "black/jewish.16");
    m_defaultStyle[GeoDataFeature::ReligionShinto]           = createOsmPOIStyle(osmFont, "black/shintoist.16");
    m_defaultStyle[GeoDataFeature::ReligionSikh]             = createOsmPOIStyle(osmFont, "black/sikhist.16");

    m_defaultStyle[GeoDataFeature::HighwayTrafficSignals]    = createOsmPOIStyle(osmFont, "individual/traffic_light");

    m_defaultStyle[GeoDataFeature::PowerTower]               = createOsmPOIStyle(osmFont, "individual/power_tower", QColor( "#888888" ));
    m_defaultStyle[GeoDataFeature::PowerTower]->iconStyle().setScale(0.6);

    m_defaultStyle[GeoDataFeature::BarrierCityWall]          = createStyle(6.0, 3.0, "#787878", Qt::transparent, true, false, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, Qt::transparent);
    m_defaultStyle[GeoDataFeature::BarrierGate]              = createOsmPOIStyle(osmFont, "individual/gate");
    m_defaultStyle[GeoDataFeature::BarrierLiftGate]          = createOsmPOIStyle(osmFont, "individual/liftgate");
    m_defaultStyle[GeoDataFeature::BarrierWall]              = createStyle(2.0, 0.0, "#787878", Qt::transparent, true, false, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, Qt::transparent);

    m_defaultStyle[GeoDataFeature::HighwaySteps]             = createStyle(0.0, 2.0, "#fa8072", QColor(Qt::white), true, true, Qt::SolidPattern, Qt::CustomDashLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataFeature::HighwayUnknown]           = createHighwayStyle("#dddddd", "#bbbbbb", osmFont, "000000", 0, 1);
    m_defaultStyle[GeoDataFeature::HighwayPath]              = createHighwayStyle("#dddde8", "#999999", osmFont, "000000", 0, 1, Qt::DotLine, Qt::SquareCap, true);
    m_defaultStyle[GeoDataFeature::HighwayTrack]             = createHighwayStyle("#996600", QColor(Qt::white), osmFont, "000000", 1, 1, Qt::DashLine, Qt::SquareCap, true);
    m_defaultStyle[GeoDataFeature::HighwayPedestrian]        = createHighwayStyle("#dddde8", "#999999", osmFont, "000000", 0, 2);
    m_defaultStyle[GeoDataFeature::HighwayFootway]           = createHighwayStyle("#fa8072", QColor(Qt::white), osmFont, "000000", 0, 2, Qt::DotLine, Qt::SquareCap, true);
    m_defaultStyle[GeoDataFeature::HighwayCycleway]          = createHighwayStyle(QColor(Qt::blue), QColor(Qt::white), osmFont, "000000", 0, 2, Qt::DotLine, Qt::SquareCap, true);
    m_defaultStyle[GeoDataFeature::HighwayService]           = createHighwayStyle("#ffffff", "#bbbbbb", osmFont, "000000", 1, 6);
    m_defaultStyle[GeoDataFeature::HighwayResidential]       = createHighwayStyle("#ffffff", "#bbbbbb", osmFont, "000000", 3, 6);
    m_defaultStyle[GeoDataFeature::HighwayLivingStreet]      = createHighwayStyle("#ffffff", "#bbbbbb", osmFont, "000000", 3, 6);
    m_defaultStyle[GeoDataFeature::HighwayRoad]              = createHighwayStyle("#dddddd", "#bbbbbb", osmFont, "000000", 3, 6);
    m_defaultStyle[GeoDataFeature::HighwayUnclassified]      = createHighwayStyle("#ffffff", "#bbbbbb", osmFont, "000000", 3, 6);
    m_defaultStyle[GeoDataFeature::HighwayTertiary]          = createHighwayStyle("#ffffff", "#8f8f8f", osmFont, "000000", 6, 6);
    m_defaultStyle[GeoDataFeature::HighwayTertiaryLink]      = createHighwayStyle("#ffffff", "#8f8f8f", osmFont, "000000", 6, 6);
    m_defaultStyle[GeoDataFeature::HighwaySecondary]         = createHighwayStyle("#f7fabf", "#707d05", osmFont, "000000", 6, 6);
    m_defaultStyle[GeoDataFeature::HighwaySecondaryLink]     = createHighwayStyle("#f7fabf", "#707d05", osmFont, "000000", 6, 6);
    m_defaultStyle[GeoDataFeature::HighwayPrimary]           = createHighwayStyle("#fcd6a4", "#a06b00", osmFont, "000000", 9, 6);
    m_defaultStyle[GeoDataFeature::HighwayPrimaryLink]       = createHighwayStyle("#fcd6a4", "#a06b00", osmFont, "000000", 6, 6);
    m_defaultStyle[GeoDataFeature::HighwayTrunk]             = createHighwayStyle("#f9b29c", "#c84e2f", osmFont, "000000", 9, 6);
    m_defaultStyle[GeoDataFeature::HighwayTrunkLink]         = createHighwayStyle("#f9b29c", "#c84e2f", osmFont, "000000", 9, 6);
    m_defaultStyle[GeoDataFeature::HighwayMotorway]          = createHighwayStyle("#e892a2", "#dc2a67", osmFont, "000000", 9, 10);
    m_defaultStyle[GeoDataFeature::HighwayMotorwayLink]      = createHighwayStyle("#e892a2", "#dc2a67", osmFont, "000000", 9, 10);
    m_defaultStyle[GeoDataFeature::TransportAirportRunway]   = createHighwayStyle("#bbbbcc", "#bbbbcc", osmFont, "000000", 0, 1);
    m_defaultStyle[GeoDataFeature::TransportAirportTaxiway]  = createHighwayStyle("#bbbbcc", "#bbbbcc", osmFont, "000000", 0, 1);
    m_defaultStyle[GeoDataFeature::TransportAirportApron]    = createWayStyle("#e9d1ff", Qt::transparent, true, false);

    m_defaultStyle[GeoDataFeature::NaturalWater]             = createStyle(4, 0, waterColor, waterColor, true, true,
                                                                                                   Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false, QVector< qreal >(),
                                                                                                   osmFont, waterColor.darker(150));

    m_defaultStyle[GeoDataFeature::NaturalReef]              = createStyle(5.5, 0, "#36677c", "#36677c", true, true,
                                                                                                   Qt::Dense7Pattern, Qt::DotLine, Qt::RoundCap, false, QVector< qreal >(),
                                                                                                   osmFont, waterColor.darker(150));

    m_defaultStyle[GeoDataFeature::AmenityGraveyard]         = createWayStyle("#AACBAF", "#AACBAF", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_generic.png"));

    m_defaultStyle[GeoDataFeature::NaturalWood]              = createWayStyle("#8DC46C", "#8DC46C", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/forest.png"));
    m_defaultStyle[GeoDataFeature::NaturalBeach]             = createWayStyle("#FFF1BA", "#FFF1BA", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/beach.png"));
    m_defaultStyle[GeoDataFeature::NaturalWetland]           = createWayStyle("#DDECEC", "#DDECEC", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/wetland.png"));
    m_defaultStyle[GeoDataFeature::NaturalGlacier]           = createWayStyle("#DDECEC", "#DDECEC", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/glacier.png"));
    m_defaultStyle[GeoDataFeature::NaturalIceShelf]          = createWayStyle("#8ebebe", "#8ebebe", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/glacier.png"));
    m_defaultStyle[GeoDataFeature::NaturalScrub]             = createWayStyle("#B5E3B5", "#B5E3B5", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/scrub.png"));
    m_defaultStyle[GeoDataFeature::NaturalCliff]             = createWayStyle(Qt::transparent, Qt::transparent, true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/cliff2.png"));
    m_defaultStyle[GeoDataFeature::NaturalHeath]             = createWayStyle("#d6d99f", QColor("#d6d99f").darker(150), true, false);

    m_defaultStyle[GeoDataFeature::LeisureGolfCourse]        = createWayStyle(QColor("#b5e3b5"), QColor("#b5e3b5").darker(150), true, true);
    m_defaultStyle[GeoDataFeature::LeisureMarina]            = createOsmPOIStyle(osmFont, QString(), QColor("#95abd5"), QColor("#aec8d1"), QColor("#95abd5").darker(150));
    m_defaultStyle[GeoDataFeature::LeisurePark]              = createWayStyle(QColor("#c8facc"), QColor("#c8facc").darker(150), true, true);
    m_defaultStyle[GeoDataFeature::LeisurePlayground]        = createOsmPOIAreaStyle(osmFont, "amenity/playground.16", amenityColor, "#CCFFF1", "#BDFFED");
    m_defaultStyle[GeoDataFeature::LeisurePitch]             = createWayStyle("#8ad3af", QColor("#8ad3af").darker(150), true, true);
    m_defaultStyle[GeoDataFeature::LeisureSportsCentre]      = createWayStyle("#33cc99", QColor("#33cc99").darker(150), true, true);
    m_defaultStyle[GeoDataFeature::LeisureStadium]           = createWayStyle("#33cc99", QColor("#33cc99").darker(150), true, true);
    m_defaultStyle[GeoDataFeature::LeisureTrack]             = createWayStyle("#74dcba", QColor("#74dcba").darker(150), true, true);
    m_defaultStyle[GeoDataFeature::LeisureSwimmingPool]      = createWayStyle(waterColor, waterColor.darker(150), true, true);

    m_defaultStyle[GeoDataFeature::LanduseAllotments]        = createWayStyle("#E4C6AA", "#E4C6AA", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/allotments.png"));
    m_defaultStyle[GeoDataFeature::LanduseBasin]             = createWayStyle(QColor(0xB5, 0xD0, 0xD0, 0x80 ), QColor( 0xB5, 0xD0, 0xD0 ));
    m_defaultStyle[GeoDataFeature::LanduseCemetery]          = createWayStyle("#AACBAF", "#AACBAF", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_generic.png"));
    m_defaultStyle[GeoDataFeature::LanduseCommercial]        = createWayStyle("#F2DAD9", "#D1B2B0", true, true);
    m_defaultStyle[GeoDataFeature::LanduseConstruction]      = createWayStyle("#b6b592", "#b6b592", true, true);
    m_defaultStyle[GeoDataFeature::LanduseFarmland]          = createWayStyle("#EDDDC9", "#C8B69E", true, true);
    m_defaultStyle[GeoDataFeature::LanduseFarmyard]          = createWayStyle("#EFD6B5", "#D1B48C", true, true);
    m_defaultStyle[GeoDataFeature::LanduseGarages]           = createWayStyle("#E0DDCD", "#E0DDCD", true, true);
    m_defaultStyle[GeoDataFeature::LanduseGrass]             = createWayStyle("#A8C8A5", "#A8C8A5", true, true);
    m_defaultStyle[GeoDataFeature::LanduseIndustrial]        = createWayStyle("#DED0D5", "#DED0D5", true, true);
    m_defaultStyle[GeoDataFeature::LanduseLandfill]          = createWayStyle("#b6b592", "#b6b592", true, true);
    m_defaultStyle[GeoDataFeature::LanduseMeadow]            = createWayStyle("#cdebb0", "#cdebb0", true, true);
    m_defaultStyle[GeoDataFeature::LanduseMilitary]          = createWayStyle("#F3D8D2", "#F3D8D2", true, true, Qt::BDiagPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/military_red_hatch.png"));
    m_defaultStyle[GeoDataFeature::LanduseQuarry]            = createWayStyle("#C4C2C2", "#C4C2C2", true, true, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/quarry.png"));
    m_defaultStyle[GeoDataFeature::LanduseRailway]           = createWayStyle("#DED0D5", "#DED0D5", true, true);
    m_defaultStyle[GeoDataFeature::LanduseReservoir]         = createWayStyle(waterColor, waterColor, true, true);
    m_defaultStyle[GeoDataFeature::LanduseResidential]       = createWayStyle("#DCDCDC", "#DCDCDC", true, true);
    m_defaultStyle[GeoDataFeature::LanduseRetail]            = createWayStyle("#FFD6D1", "#D99C95", true, true);
    m_defaultStyle[GeoDataFeature::LanduseOrchard]           = createWayStyle("#AEDFA3", "#AEDFA3", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/orchard.png"));
    m_defaultStyle[GeoDataFeature::LanduseVineyard]          = createWayStyle("#AEDFA3", "#AEDFA3", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/vineyard.png"));

    m_defaultStyle[GeoDataFeature::MilitaryDangerArea]       = createWayStyle("#FFC0CB", "#FFC0CB", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/danger.png"));

    m_defaultStyle[GeoDataFeature::RailwayRail]              = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", true, true, Qt::SolidPattern, Qt::CustomDashLine, Qt::FlatCap, true, QVector< qreal >() << 2 << 3, osmFont);
    m_defaultStyle[GeoDataFeature::RailwayNarrowGauge]       = createStyle(2.0, 1.0, "#706E70", "#EEEEEE", true, true, Qt::SolidPattern, Qt::CustomDashLine, Qt::FlatCap, true, QVector< qreal >() << 2 << 3, osmFont);
    // FIXME: the tram is currently being rendered as a polygon.
    m_defaultStyle[GeoDataFeature::RailwayTram]              = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont);
    m_defaultStyle[GeoDataFeature::RailwayLightRail]         = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont);
    m_defaultStyle[GeoDataFeature::RailwayAbandoned]         = createStyle(2.0, 1.435, Qt::transparent, "#706E70", false, false, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false, QVector<qreal>(), osmFont);
    m_defaultStyle[GeoDataFeature::RailwaySubway]            = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont);
    m_defaultStyle[GeoDataFeature::RailwayPreserved]         = createStyle(2.0, 1.435, "#EEEEEE", "#706E70", true, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, true, QVector<qreal>(), osmFont);
    m_defaultStyle[GeoDataFeature::RailwayMiniature]         = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont);
    m_defaultStyle[GeoDataFeature::RailwayConstruction]      = createStyle(2.0, 1.435, "#EEEEEE", "#706E70", true, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, true, QVector<qreal>(), osmFont);
    m_defaultStyle[GeoDataFeature::RailwayMonorail]          = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont);
    m_defaultStyle[GeoDataFeature::RailwayFunicular]         = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont);

    m_defaultStyle[GeoDataFeature::Building]                 = createStyle(1, 0, buildingColor, buildingColor.darker(),
                                                                                   true, true, Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false, QVector<qreal>(), osmFont);

    m_defaultStyle[GeoDataFeature::Landmass]                 = createWayStyle("#F1EEE8", "#F1EEE8", true, true);
    m_defaultStyle[GeoDataFeature::UrbanArea]                = createWayStyle("#E6E3DD", "#E6E3DD", true, true);
    m_defaultStyle[GeoDataFeature::InternationalDateLine]    = createStyle(1.0, 0.0, "#000000", "#000000", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont);
    m_defaultStyle[GeoDataFeature::Bathymetry]               = createWayStyle("#a5c9c9", "#a5c9c9", true, true);

    m_defaultStyle[GeoDataFeature::AdminLevel1]              = createStyle(0.0, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::CustomDashLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataFeature::AdminLevel2]              = createStyle(2.0, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataFeature::AdminLevel3]              = createStyle(1.8, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DashLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataFeature::AdminLevel4]              = createStyle(1.5, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataFeature::AdminLevel5]              = createStyle(1.25, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DashDotDotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataFeature::AdminLevel6]              = createStyle(1, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DashDotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataFeature::AdminLevel7]              = createStyle(1, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DashLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataFeature::AdminLevel8]              = createStyle(1, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DashLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataFeature::AdminLevel9]              = createStyle(1.5, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataFeature::AdminLevel10]             = createStyle(1.5, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataFeature::AdminLevel11]             = createStyle(1.5, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);

    m_defaultStyle[GeoDataFeature::BoundaryMaritime]         = createStyle(2.0, 0.0, "#88b3bf", "#88b3bf", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont);

    m_defaultStyle[GeoDataFeature::Satellite]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/satellite.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    QFont tmp;

    // Fonts for areas ...
    tmp = m_defaultStyle[GeoDataFeature::Continent]->labelStyle().font();
    tmp.setLetterSpacing( QFont::AbsoluteSpacing, 2 );
    tmp.setCapitalization( QFont::AllUppercase );
    tmp.setBold( true );
    m_defaultStyle[GeoDataFeature::Continent]->labelStyle().setFont( tmp );

    // Fonts for areas ...
    tmp = m_defaultStyle[GeoDataFeature::Mare]->labelStyle().font();
    tmp.setLetterSpacing( QFont::AbsoluteSpacing, 2 );
    tmp.setCapitalization( QFont::AllUppercase );
    tmp.setBold( true );
    m_defaultStyle[GeoDataFeature::Mare]->labelStyle().setFont( tmp );

    // Now we need to underline the capitals ...

    tmp = m_defaultStyle[GeoDataFeature::SmallNationCapital]->labelStyle().font();
    tmp.setUnderline( true );
    m_defaultStyle[GeoDataFeature::SmallNationCapital]->labelStyle().setFont( tmp );

    tmp = m_defaultStyle[GeoDataFeature::MediumNationCapital]->labelStyle().font();
    tmp.setUnderline( true );
    m_defaultStyle[GeoDataFeature::MediumNationCapital]->labelStyle().setFont( tmp );

    tmp = m_defaultStyle[GeoDataFeature::BigNationCapital]->labelStyle().font();
    tmp.setUnderline( true );
    m_defaultStyle[GeoDataFeature::BigNationCapital]->labelStyle().setFont( tmp );

    tmp = m_defaultStyle[GeoDataFeature::LargeNationCapital]->labelStyle().font();
    tmp.setUnderline( true );
    m_defaultStyle[GeoDataFeature::LargeNationCapital]->labelStyle().setFont( tmp );
}

QString StyleBuilder::Private::createPaintLayerItem(const QString &itemType, GeoDataFeature::GeoDataVisualCategory visualCategory, const QString &subType)
{
    QString const category = visualCategoryName(visualCategory);
    if (subType.isEmpty()) {
        return itemType + QLatin1Char('/') + category;
    } else {
        return itemType + QLatin1Char('/') + category + QLatin1Char('/') + subType;
    }
}

void StyleBuilder::Private::initializeOsmVisualCategories()
{
    // Only initialize the map once
    if (!s_visualCategories.isEmpty()) {
        return;
    }

    s_visualCategories[OsmTag("admin_level", "1")]              = GeoDataFeature::AdminLevel1;
    s_visualCategories[OsmTag("admin_level", "2")]              = GeoDataFeature::AdminLevel2;
    s_visualCategories[OsmTag("admin_level", "3")]              = GeoDataFeature::AdminLevel3;
    s_visualCategories[OsmTag("admin_level", "4")]              = GeoDataFeature::AdminLevel4;
    s_visualCategories[OsmTag("admin_level", "5")]              = GeoDataFeature::AdminLevel5;
    s_visualCategories[OsmTag("admin_level", "6")]              = GeoDataFeature::AdminLevel6;
    s_visualCategories[OsmTag("admin_level", "7")]              = GeoDataFeature::AdminLevel7;
    s_visualCategories[OsmTag("admin_level", "8")]              = GeoDataFeature::AdminLevel8;
    s_visualCategories[OsmTag("admin_level", "9")]              = GeoDataFeature::AdminLevel9;
    s_visualCategories[OsmTag("admin_level", "10")]             = GeoDataFeature::AdminLevel10;
    s_visualCategories[OsmTag("admin_level", "11")]             = GeoDataFeature::AdminLevel11;

    s_visualCategories[OsmTag("boundary", "maritime")]          = GeoDataFeature::BoundaryMaritime;

    s_visualCategories[OsmTag("amenity", "restaurant")]         = GeoDataFeature::FoodRestaurant;
    s_visualCategories[OsmTag("amenity", "fast_food")]          = GeoDataFeature::FoodFastFood;
    s_visualCategories[OsmTag("amenity", "pub")]                = GeoDataFeature::FoodPub;
    s_visualCategories[OsmTag("amenity", "bar")]                = GeoDataFeature::FoodBar;
    s_visualCategories[OsmTag("amenity", "cafe")]               = GeoDataFeature::FoodCafe;
    s_visualCategories[OsmTag("amenity", "biergarten")]         = GeoDataFeature::FoodBiergarten;
    s_visualCategories[OsmTag("amenity", "school")]             = GeoDataFeature::EducationSchool;
    s_visualCategories[OsmTag("amenity", "college")]            = GeoDataFeature::EducationCollege;
    s_visualCategories[OsmTag("amenity", "library")]            = GeoDataFeature::AmenityLibrary;
    s_visualCategories[OsmTag("amenity", "kindergarten")]       = GeoDataFeature::AmenityKindergarten;
    s_visualCategories[OsmTag("amenity", "childcare")]          = GeoDataFeature::AmenityKindergarten;
    s_visualCategories[OsmTag("amenity", "university")]         = GeoDataFeature::EducationUniversity;
    s_visualCategories[OsmTag("amenity", "bus_station")]        = GeoDataFeature::TransportBusStation;
    s_visualCategories[OsmTag("amenity", "car_sharing")]        = GeoDataFeature::TransportCarShare;
    s_visualCategories[OsmTag("amenity", "fuel")]               = GeoDataFeature::TransportFuel;
    s_visualCategories[OsmTag("amenity", "parking")]            = GeoDataFeature::TransportParking;
    s_visualCategories[OsmTag("amenity", "parking_space")]      = GeoDataFeature::TransportParkingSpace;
    s_visualCategories[OsmTag("amenity", "atm")]                = GeoDataFeature::MoneyAtm;
    s_visualCategories[OsmTag("amenity", "bank")]               = GeoDataFeature::MoneyBank;

    s_visualCategories[OsmTag("historic", "archaeological_site")] = GeoDataFeature::AmenityArchaeologicalSite;
    s_visualCategories[OsmTag("amenity", "embassy")]            = GeoDataFeature::AmenityEmbassy;
    s_visualCategories[OsmTag("emergency", "phone")]            = GeoDataFeature::AmenityEmergencyPhone;
    s_visualCategories[OsmTag("leisure", "water_park")]         = GeoDataFeature::AmenityWaterPark;
    s_visualCategories[OsmTag("amenity", "community_centre")]   = GeoDataFeature::AmenityCommunityCentre;
    s_visualCategories[OsmTag("amenity", "fountain")]           = GeoDataFeature::AmenityFountain;
    s_visualCategories[OsmTag("amenity", "nightclub")]          = GeoDataFeature::AmenityNightClub;
    s_visualCategories[OsmTag("amenity", "bench")]              = GeoDataFeature::AmenityBench;
    s_visualCategories[OsmTag("amenity", "courthouse")]         = GeoDataFeature::AmenityCourtHouse;
    s_visualCategories[OsmTag("amenity", "fire_station")]       = GeoDataFeature::AmenityFireStation;
    s_visualCategories[OsmTag("amenity", "hunting_stand")]      = GeoDataFeature::AmenityHuntingStand;
    s_visualCategories[OsmTag("amenity", "police")]             = GeoDataFeature::AmenityPolice;
    s_visualCategories[OsmTag("amenity", "post_box")]           = GeoDataFeature::AmenityPostBox;
    s_visualCategories[OsmTag("amenity", "post_office")]        = GeoDataFeature::AmenityPostOffice;
    s_visualCategories[OsmTag("amenity", "prison")]             = GeoDataFeature::AmenityPrison;
    s_visualCategories[OsmTag("amenity", "recycling")]          = GeoDataFeature::AmenityRecycling;
    s_visualCategories[OsmTag("amenity", "shelter")]            = GeoDataFeature::AmenityShelter;
    s_visualCategories[OsmTag("amenity", "telephone")]          = GeoDataFeature::AmenityTelephone;
    s_visualCategories[OsmTag("amenity", "toilets")]            = GeoDataFeature::AmenityToilets;
    s_visualCategories[OsmTag("amenity", "townhall")]           = GeoDataFeature::AmenityTownHall;
    s_visualCategories[OsmTag("amenity", "waste_basket")]       = GeoDataFeature::AmenityWasteBasket;
    s_visualCategories[OsmTag("amenity", "drinking_water")]     = GeoDataFeature::AmenityDrinkingWater;
    s_visualCategories[OsmTag("amenity", "graveyard")]          = GeoDataFeature::AmenityGraveyard;

    s_visualCategories[OsmTag("amenity", "dentist")]            = GeoDataFeature::HealthDentist;
    s_visualCategories[OsmTag("amenity", "pharmacy")]           = GeoDataFeature::HealthPharmacy;
    s_visualCategories[OsmTag("amenity", "hospital")]           = GeoDataFeature::HealthHospital;
    s_visualCategories[OsmTag("amenity", "doctors")]            = GeoDataFeature::HealthDoctors;
    s_visualCategories[OsmTag("amenity", "veterinary")]         = GeoDataFeature::HealthVeterinary;

    s_visualCategories[OsmTag("amenity", "cinema")]             = GeoDataFeature::TouristCinema;
    s_visualCategories[OsmTag("tourism", "information")]        = GeoDataFeature::TouristInformation;
    s_visualCategories[OsmTag("amenity", "theatre")]            = GeoDataFeature::TouristTheatre;
    s_visualCategories[OsmTag("amenity", "place_of_worship")]   = GeoDataFeature::ReligionPlaceOfWorship;

    s_visualCategories[OsmTag("natural", "peak")]               = GeoDataFeature::NaturalPeak;
    s_visualCategories[OsmTag("natural", "tree")]               = GeoDataFeature::NaturalTree;

    s_visualCategories[OsmTag("shop", "beverages")]             = GeoDataFeature::ShopBeverages;
    s_visualCategories[OsmTag("shop", "hifi")]                  = GeoDataFeature::ShopHifi;
    s_visualCategories[OsmTag("shop", "supermarket")]           = GeoDataFeature::ShopSupermarket;
    s_visualCategories[OsmTag("shop", "alcohol")]               = GeoDataFeature::ShopAlcohol;
    s_visualCategories[OsmTag("shop", "bakery")]                = GeoDataFeature::ShopBakery;
    s_visualCategories[OsmTag("shop", "butcher")]               = GeoDataFeature::ShopButcher;
    s_visualCategories[OsmTag("shop", "confectionery")]         = GeoDataFeature::ShopConfectionery;
    s_visualCategories[OsmTag("shop", "convenience")]           = GeoDataFeature::ShopConvenience;
    s_visualCategories[OsmTag("shop", "greengrocer")]           = GeoDataFeature::ShopGreengrocer;
    s_visualCategories[OsmTag("shop", "seafood")]               = GeoDataFeature::ShopSeafood;
    s_visualCategories[OsmTag("shop", "department_store")]      = GeoDataFeature::ShopDepartmentStore;
    s_visualCategories[OsmTag("shop", "kiosk")]                 = GeoDataFeature::ShopKiosk;
    s_visualCategories[OsmTag("shop", "bag")]                   = GeoDataFeature::ShopBag;
    s_visualCategories[OsmTag("shop", "clothes")]               = GeoDataFeature::ShopClothes;
    s_visualCategories[OsmTag("shop", "fashion")]               = GeoDataFeature::ShopFashion;
    s_visualCategories[OsmTag("shop", "jewelry")]               = GeoDataFeature::ShopJewelry;
    s_visualCategories[OsmTag("shop", "shoes")]                 = GeoDataFeature::ShopShoes;
    s_visualCategories[OsmTag("shop", "variety_store")]         = GeoDataFeature::ShopVarietyStore;
    s_visualCategories[OsmTag("shop", "beauty")]                = GeoDataFeature::ShopBeauty;
    s_visualCategories[OsmTag("shop", "chemist")]               = GeoDataFeature::ShopChemist;
    s_visualCategories[OsmTag("shop", "cosmetics")]             = GeoDataFeature::ShopCosmetics;
    s_visualCategories[OsmTag("shop", "hairdresser")]           = GeoDataFeature::ShopHairdresser;
    s_visualCategories[OsmTag("shop", "optician")]              = GeoDataFeature::ShopOptician;
    s_visualCategories[OsmTag("shop", "perfumery")]             = GeoDataFeature::ShopPerfumery;
    s_visualCategories[OsmTag("shop", "doityourself")]          = GeoDataFeature::ShopDoitYourself;
    s_visualCategories[OsmTag("shop", "florist")]               = GeoDataFeature::ShopFlorist;
    s_visualCategories[OsmTag("shop", "hardware")]              = GeoDataFeature::ShopHardware;
    s_visualCategories[OsmTag("shop", "furniture")]             = GeoDataFeature::ShopFurniture;
    s_visualCategories[OsmTag("shop", "electronics")]           = GeoDataFeature::ShopElectronics;
    s_visualCategories[OsmTag("shop", "mobile_phone")]          = GeoDataFeature::ShopMobilePhone;
    s_visualCategories[OsmTag("shop", "bicycle")]               = GeoDataFeature::ShopBicycle;
    s_visualCategories[OsmTag("shop", "car")]                   = GeoDataFeature::ShopCar;
    s_visualCategories[OsmTag("shop", "car_repair")]            = GeoDataFeature::ShopCarRepair;
    s_visualCategories[OsmTag("shop", "car_parts")]             = GeoDataFeature::ShopCarParts;
    s_visualCategories[OsmTag("shop", "motorcycle")]            = GeoDataFeature::ShopMotorcycle;
    s_visualCategories[OsmTag("shop", "outdoor")]               = GeoDataFeature::ShopOutdoor;
    s_visualCategories[OsmTag("shop", "musical_instrument")]    = GeoDataFeature::ShopMusicalInstrument;
    s_visualCategories[OsmTag("shop", "photo")]                 = GeoDataFeature::ShopPhoto;
    s_visualCategories[OsmTag("shop", "books")]                 = GeoDataFeature::ShopBook;
    s_visualCategories[OsmTag("shop", "gift")]                  = GeoDataFeature::ShopGift;
    s_visualCategories[OsmTag("shop", "stationery")]            = GeoDataFeature::ShopStationery;
    s_visualCategories[OsmTag("shop", "laundry")]               = GeoDataFeature::ShopLaundry;
    s_visualCategories[OsmTag("shop", "pet")]                   = GeoDataFeature::ShopPet;
    s_visualCategories[OsmTag("shop", "toys")]                  = GeoDataFeature::ShopToys;
    s_visualCategories[OsmTag("shop", "travel_agency")]         = GeoDataFeature::ShopTravelAgency;

    // Default for all other shops
    foreach (const QString &value, shopValues()) {
        s_visualCategories[OsmTag("shop", value)]               = GeoDataFeature::Shop;
    }

    s_visualCategories[OsmTag("man_made", "bridge")]            = GeoDataFeature::ManmadeBridge;
    s_visualCategories[OsmTag("man_made", "lighthouse")]        = GeoDataFeature::ManmadeLighthouse;
    s_visualCategories[OsmTag("man_made", "pier")]              = GeoDataFeature::ManmadePier;
    s_visualCategories[OsmTag("man_made", "water_tower")]       = GeoDataFeature::ManmadeWaterTower;
    s_visualCategories[OsmTag("man_made", "windmill")]          = GeoDataFeature::ManmadeWindMill;

    s_visualCategories[OsmTag("religion", "")]                  = GeoDataFeature::ReligionPlaceOfWorship;
    s_visualCategories[OsmTag("religion", "bahai")]             = GeoDataFeature::ReligionBahai;
    s_visualCategories[OsmTag("religion", "buddhist")]          = GeoDataFeature::ReligionBuddhist;
    s_visualCategories[OsmTag("religion", "christian")]         = GeoDataFeature::ReligionChristian;
    s_visualCategories[OsmTag("religion", "muslim")]            = GeoDataFeature::ReligionMuslim;
    s_visualCategories[OsmTag("religion", "hindu")]             = GeoDataFeature::ReligionHindu;
    s_visualCategories[OsmTag("religion", "jain")]              = GeoDataFeature::ReligionJain;
    s_visualCategories[OsmTag("religion", "jewish")]            = GeoDataFeature::ReligionJewish;
    s_visualCategories[OsmTag("religion", "shinto")]            = GeoDataFeature::ReligionShinto;
    s_visualCategories[OsmTag("religion", "sikh")]              = GeoDataFeature::ReligionSikh;

    s_visualCategories[OsmTag("historic", "memorial")]          = GeoDataFeature::TouristAttraction;
    s_visualCategories[OsmTag("tourism", "attraction")]         = GeoDataFeature::TouristAttraction;
    s_visualCategories[OsmTag("tourism", "camp_site")]          = GeoDataFeature::AccomodationCamping;
    s_visualCategories[OsmTag("tourism", "hostel")]             = GeoDataFeature::AccomodationHostel;
    s_visualCategories[OsmTag("tourism", "hotel")]              = GeoDataFeature::AccomodationHotel;
    s_visualCategories[OsmTag("tourism", "motel")]              = GeoDataFeature::AccomodationMotel;
    s_visualCategories[OsmTag("tourism", "guest_house")]        = GeoDataFeature::AccomodationGuestHouse;
    s_visualCategories[OsmTag("tourism", "museum")]             = GeoDataFeature::TouristMuseum;
    s_visualCategories[OsmTag("tourism", "theme_park")]         = GeoDataFeature::TouristThemePark;
    s_visualCategories[OsmTag("tourism", "viewpoint")]          = GeoDataFeature::TouristViewPoint;
    s_visualCategories[OsmTag("tourism", "zoo")]                = GeoDataFeature::TouristZoo;
    s_visualCategories[OsmTag("tourism", "alpine_hut")]         = GeoDataFeature::TouristAlpineHut;

    s_visualCategories[OsmTag("barrier", "city_wall")]           = GeoDataFeature::BarrierCityWall;
    s_visualCategories[OsmTag("barrier", "gate")]               = GeoDataFeature::BarrierGate;
    s_visualCategories[OsmTag("barrier", "lift_gate")]           = GeoDataFeature::BarrierLiftGate;
    s_visualCategories[OsmTag("barrier", "wall")]               = GeoDataFeature::BarrierWall;

    s_visualCategories[OsmTag("historic", "castle")]            = GeoDataFeature::TouristCastle;
    s_visualCategories[OsmTag("historic", "fort")]              = GeoDataFeature::TouristCastle;
    s_visualCategories[OsmTag("historic", "monument")]          = GeoDataFeature::TouristMonument;
    s_visualCategories[OsmTag("historic", "ruins")]             = GeoDataFeature::TouristRuin;

    s_visualCategories[OsmTag("highway", "traffic_signals")]    = GeoDataFeature::HighwayTrafficSignals;

    s_visualCategories[OsmTag("highway", "unknown")]            = GeoDataFeature::HighwayUnknown;
    s_visualCategories[OsmTag("highway", "steps")]              = GeoDataFeature::HighwaySteps;
    s_visualCategories[OsmTag("highway", "footway")]            = GeoDataFeature::HighwayFootway;
    s_visualCategories[OsmTag("highway", "cycleway")]           = GeoDataFeature::HighwayCycleway;
    s_visualCategories[OsmTag("highway", "path")]               = GeoDataFeature::HighwayPath;
    s_visualCategories[OsmTag("highway", "track")]              = GeoDataFeature::HighwayTrack;
    s_visualCategories[OsmTag("highway", "pedestrian")]         = GeoDataFeature::HighwayPedestrian;
    s_visualCategories[OsmTag("highway", "service")]            = GeoDataFeature::HighwayService;
    s_visualCategories[OsmTag("highway", "living_street")]      = GeoDataFeature::HighwayLivingStreet;
    s_visualCategories[OsmTag("highway", "unclassified")]       = GeoDataFeature::HighwayUnclassified;
    s_visualCategories[OsmTag("highway", "residential")]        = GeoDataFeature::HighwayResidential;
    s_visualCategories[OsmTag("highway", "road")]               = GeoDataFeature::HighwayRoad;
    s_visualCategories[OsmTag("highway", "tertiary_link")]      = GeoDataFeature::HighwayTertiaryLink;
    s_visualCategories[OsmTag("highway", "tertiary")]           = GeoDataFeature::HighwayTertiary;
    s_visualCategories[OsmTag("highway", "secondary_link")]     = GeoDataFeature::HighwaySecondaryLink;
    s_visualCategories[OsmTag("highway", "secondary")]          = GeoDataFeature::HighwaySecondary;
    s_visualCategories[OsmTag("highway", "primary_link")]       = GeoDataFeature::HighwayPrimaryLink;
    s_visualCategories[OsmTag("highway", "primary")]            = GeoDataFeature::HighwayPrimary;
    s_visualCategories[OsmTag("highway", "trunk_link")]         = GeoDataFeature::HighwayTrunkLink;
    s_visualCategories[OsmTag("highway", "trunk")]              = GeoDataFeature::HighwayTrunk;
    s_visualCategories[OsmTag("highway", "motorway_link")]      = GeoDataFeature::HighwayMotorwayLink;
    s_visualCategories[OsmTag("highway", "motorway")]           = GeoDataFeature::HighwayMotorway;

    s_visualCategories[OsmTag("natural", "water")]              = GeoDataFeature::NaturalWater;
    s_visualCategories[OsmTag("natural", "reef")]               = GeoDataFeature::NaturalReef;
    s_visualCategories[OsmTag("natural", "bay")]                = GeoDataFeature::NaturalWater;
    s_visualCategories[OsmTag("natural", "coastline")]          = GeoDataFeature::NaturalWater;
    s_visualCategories[OsmTag("waterway", "stream")]            = GeoDataFeature::NaturalWater;
    s_visualCategories[OsmTag("waterway", "river")]             = GeoDataFeature::NaturalWater;
    s_visualCategories[OsmTag("waterway", "riverbank")]         = GeoDataFeature::NaturalWater;
    s_visualCategories[OsmTag("waterway", "canal")]             = GeoDataFeature::NaturalWater;

    s_visualCategories[OsmTag("natural", "wood")]               = GeoDataFeature::NaturalWood;
    s_visualCategories[OsmTag("natural", "beach")]              = GeoDataFeature::NaturalBeach;
    s_visualCategories[OsmTag("natural", "wetland")]            = GeoDataFeature::NaturalWetland;
    s_visualCategories[OsmTag("natural", "glacier")]            = GeoDataFeature::NaturalGlacier;
    s_visualCategories[OsmTag("glacier:type", "shelf")]         = GeoDataFeature::NaturalIceShelf;
    s_visualCategories[OsmTag("natural", "scrub")]              = GeoDataFeature::NaturalScrub;
    s_visualCategories[OsmTag("natural", "cliff")]              = GeoDataFeature::NaturalCliff;

    s_visualCategories[OsmTag("military", "danger_area")]       = GeoDataFeature::MilitaryDangerArea;

    s_visualCategories[OsmTag("landuse", "forest")]             = GeoDataFeature::NaturalWood;
    s_visualCategories[OsmTag("landuse", "allotments")]         = GeoDataFeature::LanduseAllotments;
    s_visualCategories[OsmTag("landuse", "basin")]              = GeoDataFeature::LanduseBasin;
    s_visualCategories[OsmTag("landuse", "brownfield")]         = GeoDataFeature::LanduseConstruction;
    s_visualCategories[OsmTag("landuse", "cemetery")]           = GeoDataFeature::LanduseCemetery;
    s_visualCategories[OsmTag("landuse", "commercial")]         = GeoDataFeature::LanduseCommercial;
    s_visualCategories[OsmTag("landuse", "construction")]       = GeoDataFeature::LanduseConstruction;
    s_visualCategories[OsmTag("landuse", "farm")]               = GeoDataFeature::LanduseFarmland;
    s_visualCategories[OsmTag("landuse", "farmland")]           = GeoDataFeature::LanduseFarmland;
    s_visualCategories[OsmTag("landuse", "greenhouse_horticulture")] = GeoDataFeature::LanduseFarmland;
    s_visualCategories[OsmTag("landuse", "farmyard")]           = GeoDataFeature::LanduseFarmland;
    s_visualCategories[OsmTag("landuse", "garages")]            = GeoDataFeature::LanduseGarages;
    s_visualCategories[OsmTag("landuse", "greenfield")]         = GeoDataFeature::LanduseConstruction;
    s_visualCategories[OsmTag("landuse", "industrial")]         = GeoDataFeature::LanduseIndustrial;
    s_visualCategories[OsmTag("landuse", "landfill")]           = GeoDataFeature::LanduseLandfill;
    s_visualCategories[OsmTag("landuse", "meadow")]             = GeoDataFeature::LanduseMeadow;
    s_visualCategories[OsmTag("landuse", "military")]           = GeoDataFeature::LanduseMilitary;
    s_visualCategories[OsmTag("landuse", "orchard")]            = GeoDataFeature::LanduseFarmland;
    s_visualCategories[OsmTag("landuse", "quarry")]             = GeoDataFeature::LanduseQuarry;
    s_visualCategories[OsmTag("landuse", "railway")]            = GeoDataFeature::LanduseRailway;
    s_visualCategories[OsmTag("landuse", "recreation_ground")]  = GeoDataFeature::LeisurePark;
    s_visualCategories[OsmTag("landuse", "reservoir")]          = GeoDataFeature::LanduseReservoir;
    s_visualCategories[OsmTag("landuse", "residential")]        = GeoDataFeature::LanduseResidential;
    s_visualCategories[OsmTag("landuse", "retail")]             = GeoDataFeature::LanduseRetail;
    s_visualCategories[OsmTag("landuse", "orchard")]            = GeoDataFeature::LanduseOrchard;
    s_visualCategories[OsmTag("landuse", "vineyard")]           = GeoDataFeature::LanduseVineyard;
    s_visualCategories[OsmTag("landuse", "village_green")]      = GeoDataFeature::LanduseGrass;
    s_visualCategories[OsmTag("landuse", "grass")]              = GeoDataFeature::LanduseGrass;

    s_visualCategories[OsmTag("leisure", "common")]             = GeoDataFeature::LanduseGrass;
    s_visualCategories[OsmTag("leisure", "garden")]             = GeoDataFeature::LanduseGrass;
    s_visualCategories[OsmTag("leisure", "golf_course")]        = GeoDataFeature::LeisureGolfCourse;
    s_visualCategories[OsmTag("leisure", "marina")]             = GeoDataFeature::LeisureMarina;
    s_visualCategories[OsmTag("leisure", "park")]               = GeoDataFeature::LeisurePark;
    s_visualCategories[OsmTag("leisure", "playground")]         = GeoDataFeature::LeisurePlayground;
    s_visualCategories[OsmTag("leisure", "pitch")]              = GeoDataFeature::LeisurePitch;
    s_visualCategories[OsmTag("leisure", "sports_centre")]      = GeoDataFeature::LeisureSportsCentre;
    s_visualCategories[OsmTag("leisure", "stadium")]            = GeoDataFeature::LeisureStadium;
    s_visualCategories[OsmTag("leisure", "track")]              = GeoDataFeature::LeisureTrack;
    s_visualCategories[OsmTag("leisure", "swimming_pool")]      = GeoDataFeature::LeisureSwimmingPool;

    s_visualCategories[OsmTag("railway", "rail")]               = GeoDataFeature::RailwayRail;
    s_visualCategories[OsmTag("railway", "narrow_gauge")]       = GeoDataFeature::RailwayNarrowGauge;
    s_visualCategories[OsmTag("railway", "tram")]               = GeoDataFeature::RailwayTram;
    s_visualCategories[OsmTag("railway", "light_rail")]         = GeoDataFeature::RailwayLightRail;
    s_visualCategories[OsmTag("railway", "preserved")]          = GeoDataFeature::RailwayPreserved;
    s_visualCategories[OsmTag("railway", "abandoned")]          = GeoDataFeature::RailwayAbandoned;
    s_visualCategories[OsmTag("railway", "disused")]            = GeoDataFeature::RailwayAbandoned;
    s_visualCategories[OsmTag("railway", "razed")]              = GeoDataFeature::RailwayAbandoned;
    s_visualCategories[OsmTag("railway", "subway")]             = GeoDataFeature::RailwaySubway;
    s_visualCategories[OsmTag("railway", "miniature")]          = GeoDataFeature::RailwayMiniature;
    s_visualCategories[OsmTag("railway", "construction")]       = GeoDataFeature::RailwayConstruction;
    s_visualCategories[OsmTag("railway", "monorail")]           = GeoDataFeature::RailwayMonorail;
    s_visualCategories[OsmTag("railway", "funicular")]          = GeoDataFeature::RailwayFunicular;
    s_visualCategories[OsmTag("railway", "platform")]           = GeoDataFeature::TransportPlatform;
    s_visualCategories[OsmTag("railway", "station")]            = GeoDataFeature::TransportTrainStation;
    s_visualCategories[OsmTag("railway", "halt")]               = GeoDataFeature::TransportTrainStation;

    s_visualCategories[OsmTag("power", "tower")]                = GeoDataFeature::PowerTower;

    s_visualCategories[OsmTag("aeroway", "aerodrome")]          = GeoDataFeature::TransportAerodrome;
    s_visualCategories[OsmTag("aeroway", "helipad")]            = GeoDataFeature::TransportHelipad;
    s_visualCategories[OsmTag("aeroway", "gate")]               = GeoDataFeature::TransportAirportGate;
    s_visualCategories[OsmTag("aeroway", "runway")]             = GeoDataFeature::TransportAirportRunway;
    s_visualCategories[OsmTag("aeroway", "apron")]              = GeoDataFeature::TransportAirportApron;
    s_visualCategories[OsmTag("aeroway", "taxiway")]            = GeoDataFeature::TransportAirportTaxiway;
    s_visualCategories[OsmTag("transport", "airpor_terminal")]  = GeoDataFeature::TransportAirportTerminal;
    s_visualCategories[OsmTag("transport", "bus_station")]      = GeoDataFeature::TransportBusStation;
    s_visualCategories[OsmTag("highway", "bus_stop")]           = GeoDataFeature::TransportBusStop;
    s_visualCategories[OsmTag("transport", "car_share")]        = GeoDataFeature::TransportCarShare;
    s_visualCategories[OsmTag("transport", "fuel")]             = GeoDataFeature::TransportFuel;
    s_visualCategories[OsmTag("transport", "parking")]          = GeoDataFeature::TransportParking;
    s_visualCategories[OsmTag("public_transport", "platform")]  = GeoDataFeature::TransportPlatform;
    s_visualCategories[OsmTag("amenity", "bicycle_rental")]     = GeoDataFeature::TransportRentalBicycle;
    s_visualCategories[OsmTag("amenity", "car_rental")]         = GeoDataFeature::TransportRentalCar;
    s_visualCategories[OsmTag("amenity", "taxi")]               = GeoDataFeature::TransportTaxiRank;
    s_visualCategories[OsmTag("transport", "train_station")]    = GeoDataFeature::TransportTrainStation;
    s_visualCategories[OsmTag("transport", "tram_stop")]        = GeoDataFeature::TransportTramStop;
    s_visualCategories[OsmTag("transport", "bus_stop")]         = GeoDataFeature::TransportBusStop;
    s_visualCategories[OsmTag("amenity", "bicycle_parking")]    = GeoDataFeature::TransportBicycleParking;
    s_visualCategories[OsmTag("amenity", "motorcycle_parking")] = GeoDataFeature::TransportMotorcycleParking;
    s_visualCategories[OsmTag("railway", "subway_entrance")]    = GeoDataFeature::TransportSubwayEntrance;

    s_visualCategories[OsmTag("place", "city")]                 = GeoDataFeature::PlaceCity;
    s_visualCategories[OsmTag("place", "suburb")]               = GeoDataFeature::PlaceSuburb;
    s_visualCategories[OsmTag("place", "hamlet")]               = GeoDataFeature::PlaceHamlet;
    s_visualCategories[OsmTag("place", "locality")]             = GeoDataFeature::PlaceLocality;
    s_visualCategories[OsmTag("place", "town")]                 = GeoDataFeature::PlaceTown;
    s_visualCategories[OsmTag("place", "village")]              = GeoDataFeature::PlaceVillage;

    //Custom Marble OSM Tags
    s_visualCategories[OsmTag("marble_land", "landmass")]       = GeoDataFeature::Landmass;
    s_visualCategories[OsmTag("settlement", "yes")]             = GeoDataFeature::UrbanArea;
    s_visualCategories[OsmTag("marble_line", "date")]           = GeoDataFeature::InternationalDateLine;
    s_visualCategories[OsmTag("marble:feature", "bathymetry")]  = GeoDataFeature::Bathymetry;

    // Default for buildings
    foreach (const auto &tag, buildingTags()) {
        s_visualCategories[tag]                                 = GeoDataFeature::Building;
    }
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

QFont StyleBuilder::defaultFont() const
{
    return d->m_defaultFont;
}

void StyleBuilder::setDefaultFont( const QFont& font )
{
    d->m_defaultFont = font;
    reset();
}

QColor StyleBuilder::defaultLabelColor() const
{
    return d->m_defaultLabelColor;
}

void StyleBuilder::setDefaultLabelColor( const QColor& color )
{
    d->m_defaultLabelColor = color;
    reset();
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
                    QString const image = QLatin1String("svg/osmcarto/svg/individual/tree-29-") + season + QLatin1String(".svg");
                    iconStyle.setIconPath(MarbleDirs::path(image));

                    GeoDataStyle::Ptr newStyle(new GeoDataStyle(*style));
                    newStyle->setIconStyle(iconStyle);
                    style = newStyle;
                }
            }
        }
    } else if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLinearRingType) {
        bool adjustStyle = false;

        GeoDataPolyStyle polyStyle = style->polyStyle();
        GeoDataLineStyle lineStyle = style->lineStyle();
        if (visualCategory == GeoDataFeature::NaturalWater) {
            if (osmData.containsTag(QStringLiteral("salt"), QStringLiteral("yes"))) {
                polyStyle.setColor("#ffff80");
                lineStyle.setPenStyle(Qt::DashLine);
                lineStyle.setWidth(2);
                adjustStyle = true;
            }
        }
        else if (visualCategory == GeoDataFeature::Bathymetry) {
            auto tagIter = osmData.findTag(QStringLiteral("ele"));
            if (tagIter != osmData.tagsEnd()) {
                const QString& elevation = tagIter.value();
                if (elevation == QLatin1String("4000")) {
                    polyStyle.setColor("#94c2c2");
                    lineStyle.setColor("#94c2c2");
                    adjustStyle = true;
                }
            }
        }
        else if (visualCategory == GeoDataFeature::AmenityGraveyard || visualCategory == GeoDataFeature::LanduseCemetery) {
            auto tagIter = osmData.findTag(QStringLiteral("religion"));
            if (tagIter != osmData.tagsEnd()) {
                const QString& religion = tagIter.value();
                if (religion == QLatin1String("jewish")) {
                    polyStyle.setTexturePath(MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_jewish.png"));
                    adjustStyle = true;
                } else if (religion == QLatin1String("christian")) {
                    polyStyle.setTexturePath(MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_christian.png"));
                    adjustStyle = true;
                } else if (religion == QLatin1String("INT-generic")) {
                    polyStyle.setTexturePath(MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_generic.png"));
                    adjustStyle = true;
                }
            }
        } else if (visualCategory == GeoDataFeature::HighwayPedestrian) {
            polyStyle.setOutline(false);
            adjustStyle = true;
        }

        if (adjustStyle) {
            GeoDataStyle::Ptr newStyle(new GeoDataStyle(*style));
            newStyle->setPolyStyle(polyStyle);
            newStyle->setLineStyle(lineStyle);
            style = newStyle;
        }

        if (style->iconStyle().iconPath().isEmpty()) {
            const GeoDataFeature::GeoDataVisualCategory category = determineVisualCategory(osmData);
            const GeoDataStyle::ConstPtr categoryStyle = presetStyle(category);
            if (category != GeoDataFeature::None && !categoryStyle->iconStyle().icon().isNull()) {
                GeoDataStyle::Ptr newStyle(new GeoDataStyle(*style));
                newStyle->setIconStyle(categoryStyle->iconStyle());
                style = newStyle;
            }
        }
    } else if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType) {
        GeoDataPolyStyle polyStyle = style->polyStyle();
        GeoDataLineStyle lineStyle = style->lineStyle();
        lineStyle.setCosmeticOutline(true);

        if(visualCategory == GeoDataFeature::AdminLevel2){
            if (osmData.containsTag(QStringLiteral("maritime"), QStringLiteral("yes"))) {
                lineStyle.setColor("#88b3bf");
                polyStyle.setColor("#88b3bf");
                if (osmData.containsTag(QStringLiteral("marble:disputed"), QStringLiteral("yes"))) {
                    lineStyle.setPenStyle( Qt::DashLine );
                }
            }
        }
        else if ((visualCategory >= GeoDataFeature::HighwayService &&
                visualCategory <= GeoDataFeature::HighwayMotorway) ||
                visualCategory == GeoDataFeature::TransportAirportRunway) {

            if (parameters.tileLevel <= 7) {
                /** @todo: Dummy implementation for dynamic style changes based on tile level, replace with sane values */
                lineStyle.setPhysicalWidth(0.0);
                lineStyle.setWidth(2.0);
            } else if (parameters.tileLevel <= 9) {
                /** @todo: Dummy implementation for dynamic style changes based on tile level, replace with sane values */
                lineStyle.setPhysicalWidth(0.0);
                lineStyle.setWidth(3.0);
            } else {
                auto tagIter = osmData.findTag(QStringLiteral("width"));
                if (tagIter != osmData.tagsEnd()) {
                    QString const widthValue = QString(tagIter.value()).remove(QStringLiteral(" meters")).remove(QStringLiteral(" m"));
                    bool ok;
                    float const width = widthValue.toFloat(&ok);
                    lineStyle.setPhysicalWidth(ok ? qBound(0.1f, width, 200.0f) : 0.0f);
                } else {
                    bool const isOneWay = osmData.containsTag(QStringLiteral("oneway"), QStringLiteral("yes")) ||
                                          osmData.containsTag(QStringLiteral("oneway"), QStringLiteral("-1"));
                    int const lanes = isOneWay ? 1 : 2; // also for motorway which implicitly is one way, but has two lanes and each direction has its own highway
                    double const laneWidth = 3.0;
                    double const margins = visualCategory == GeoDataFeature::HighwayMotorway ? 2.0 : (isOneWay ? 1.0 : 0.0);
                    double const physicalWidth = margins + lanes * laneWidth;
                    lineStyle.setPhysicalWidth(physicalWidth);
                }
            }

            QString const accessValue = osmData.tagValue(QStringLiteral("access"));
            if (accessValue == QLatin1String("private") ||
                accessValue == QLatin1String("no") ||
                accessValue == QLatin1String("agricultural") ||
                accessValue == QLatin1String("delivery") ||
                accessValue == QLatin1String("forestry")) {
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
            if (parameters.tileLevel <= 3) {
                lineStyle.setWidth(1);
                lineStyle.setPhysicalWidth(0.0);
            } else if (parameters.tileLevel <= 7) {
                lineStyle.setWidth(2);
                lineStyle.setPhysicalWidth(0.0);
            } else {
                QString const widthValue = osmData.tagValue(QStringLiteral("width")).remove(QStringLiteral(" meters")).remove(QStringLiteral(" m"));
                bool ok;
                float const width = widthValue.toFloat(&ok);
                lineStyle.setPhysicalWidth(ok ? qBound(0.1f, width, 200.0f) : 0.0f);
            }
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
    } else if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType) {
        GeoDataPolyStyle polyStyle = style->polyStyle();
        GeoDataLineStyle lineStyle = style->lineStyle();
        bool adjustStyle = false;
        if (visualCategory == GeoDataFeature::Bathymetry) {
            auto tagIter = osmData.findTag(QStringLiteral("ele"));
            if (tagIter != osmData.tagsEnd()) {
                const QString& elevation = tagIter.value();
                if (elevation == QLatin1String("4000")) {
                    polyStyle.setColor("#a5c9c9");
                    lineStyle.setColor("#a5c9c9");
                    adjustStyle = true;
                }
            }
        } else if (visualCategory == GeoDataFeature::HighwayPedestrian) {
            polyStyle.setOutline(false);
            adjustStyle = true;
        }

        if (adjustStyle) {
            GeoDataStyle::Ptr newStyle(new GeoDataStyle(*style));
            newStyle->setPolyStyle(polyStyle);
            newStyle->setLineStyle(lineStyle);
            style = newStyle;
        }

    }

    return style;
}

GeoDataStyle::ConstPtr StyleBuilder::presetStyle(GeoDataFeature::GeoDataVisualCategory visualCategory) const
{
    if (!d->m_defaultStyleInitialized) {
        d->initializeDefaultStyles();
    }

    if (visualCategory != GeoDataFeature::None && d->m_defaultStyle[visualCategory] ) {
        return d->m_defaultStyle[visualCategory];
    } else {
        return d->m_defaultStyle[GeoDataFeature::Default];
    }
}


QStringList StyleBuilder::renderOrder() const
{
    static QStringList paintLayerOrder;

    if (paintLayerOrder.isEmpty()) {
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::Landmass);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::UrbanArea);
        for ( int i = GeoDataFeature::LanduseAllotments; i <= GeoDataFeature::LanduseVineyard; i++ ) {
            if ((GeoDataFeature::GeoDataVisualCategory)i != GeoDataFeature::LanduseGrass) {
                paintLayerOrder << Private::createPaintLayerItem("Polygon", (GeoDataFeature::GeoDataVisualCategory)i);
            }
        }
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::Bathymetry);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::NaturalBeach);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::NaturalWetland);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::NaturalGlacier);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::NaturalIceShelf);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::NaturalCliff);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::NaturalPeak);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::MilitaryDangerArea);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::LeisurePark);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::LeisurePitch);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::LeisureSportsCentre);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::LeisureStadium);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::NaturalWood);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::LanduseGrass);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::HighwayPedestrian);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::LeisurePlayground);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::NaturalScrub);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::LeisureTrack);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::TransportParking);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::TransportParkingSpace);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::ManmadeBridge);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::BarrierCityWall);

        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::AmenityGraveyard);

        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::AmenityKindergarten);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::EducationCollege);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::EducationSchool);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::EducationUniversity);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::HealthHospital);

        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataFeature::Landmass);

        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::NaturalWater);
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataFeature::NaturalWater, "outline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataFeature::NaturalWater, "inline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataFeature::NaturalWater, "label");


        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataFeature::NaturalReef, "outline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataFeature::NaturalReef, "inline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataFeature::NaturalReef, "label");
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::LeisureMarina);

        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::TransportAirportApron);

        for ( int i = GeoDataFeature::HighwaySteps; i <= GeoDataFeature::HighwayMotorway; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "outline");
        }
        for ( int i = GeoDataFeature::HighwaySteps; i <= GeoDataFeature::HighwayMotorway; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "inline");
        }
        for ( int i = GeoDataFeature::HighwaySteps; i <= GeoDataFeature::HighwayMotorway; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "label");
        }
        for ( int i = GeoDataFeature::RailwayRail; i <= GeoDataFeature::RailwayFunicular; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "outline");
        }
        for ( int i = GeoDataFeature::RailwayRail; i <= GeoDataFeature::RailwayFunicular; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "inline");
        }
        for ( int i = GeoDataFeature::RailwayRail; i <= GeoDataFeature::RailwayFunicular; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "label");
        }

        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataFeature::TransportPlatform);

        for ( int i = GeoDataFeature::AdminLevel1; i <= GeoDataFeature::AdminLevel11; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "outline");
        }
        for ( int i = GeoDataFeature::AdminLevel1; i <= GeoDataFeature::AdminLevel11; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "inline");
        }
        for ( int i = GeoDataFeature::AdminLevel1; i <= GeoDataFeature::AdminLevel11; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataFeature::GeoDataVisualCategory)i, "label");
        }
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::Bathymetry);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::AmenityGraveyard);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::NaturalWood);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::NaturalBeach);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::NaturalWetland);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::NaturalGlacier);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::NaturalIceShelf);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::NaturalScrub);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::LeisureMarina);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::LeisurePark);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::LeisurePlayground);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::LeisurePitch);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::LeisureSportsCentre);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::LeisureStadium);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::LeisureTrack);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::TransportParking);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::ManmadeBridge);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::BarrierCityWall);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::NaturalWater);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::NaturalReef);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::Landmass);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::NaturalCliff);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::NaturalPeak);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::AmenityKindergarten);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::EducationCollege);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::EducationSchool);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::EducationUniversity);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::HealthHospital);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataFeature::MilitaryDangerArea);

        paintLayerOrder << QStringLiteral("Polygon/Building/frame");
        paintLayerOrder << QStringLiteral("Polygon/Building/roof");

        paintLayerOrder << QStringLiteral("Photo");

        Q_ASSERT(QSet<QString>::fromList(paintLayerOrder).size() == paintLayerOrder.size());
    }

    return paintLayerOrder;
}

void StyleBuilder::reset()
{
    d->m_defaultStyleInitialized = false;
}

int StyleBuilder::minimumZoomLevel(const GeoDataPlacemark &placemark) const
{
    return d->m_defaultMinZoomLevels[placemark.visualCategory()];
}

int StyleBuilder::maximumZoomLevel() const
{
    return d->m_maximumZoomLevel;
}

QString StyleBuilder::visualCategoryName(GeoDataFeature::GeoDataVisualCategory category)
{
    static QHash<GeoDataFeature::GeoDataVisualCategory, QString> visualCategoryNames;

    if (visualCategoryNames.isEmpty()) {
        visualCategoryNames[GeoDataFeature::GeoDataFeature::None] = "None";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::Default] = "Default";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::Unknown] = "Unknown";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::SmallCity] = "SmallCity";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::SmallCountyCapital] = "SmallCountyCapital";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::SmallStateCapital] = "SmallStateCapital";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::SmallNationCapital] = "SmallNationCapital";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::MediumCity] = "MediumCity";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::MediumCountyCapital] = "MediumCountyCapital";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::MediumStateCapital] = "MediumStateCapital";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::MediumNationCapital] = "MediumNationCapital";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::BigCity] = "BigCity";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::BigCountyCapital] = "BigCountyCapital";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::BigStateCapital] = "BigStateCapital";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::BigNationCapital] = "BigNationCapital";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::LargeCity] = "LargeCity";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::LargeCountyCapital] = "LargeCountyCapital";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::LargeStateCapital] = "LargeStateCapital";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::LargeNationCapital] = "LargeNationCapital";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::Nation] = "Nation";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::PlaceCity] = "PlaceCity";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::PlaceCityCapital] = "PlaceCityCapital";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::PlaceSuburb] = "PlaceSuburb";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::PlaceHamlet] = "PlaceHamlet";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::PlaceLocality] = "PlaceLocality";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::PlaceTown] = "PlaceTown";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::PlaceTownCapital] = "PlaceTownCapital";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::PlaceVillage] = "PlaceVillage";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::PlaceVillageCapital] = "PlaceVillageCapital";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::Mountain] = "Mountain";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::Volcano] = "Volcano";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::Mons] = "Mons";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::Valley] = "Valley";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::Continent] = "Continent";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::Ocean] = "Ocean";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::OtherTerrain] = "OtherTerrain";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::Crater] = "Crater";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::Mare] = "Mare";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::GeographicPole] = "GeographicPole";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::MagneticPole] = "MagneticPole";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::ShipWreck] = "ShipWreck";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::AirPort] = "AirPort";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::Observatory] = "Observatory";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::MilitaryDangerArea] = "MilitaryDangerArea";
        visualCategoryNames[GeoDataFeature::GeoDataFeature::OsmSite] = "OsmSite";
        visualCategoryNames[GeoDataFeature::Coordinate] = "Coordinate";
        visualCategoryNames[GeoDataFeature::MannedLandingSite] = "MannedLandingSite";
        visualCategoryNames[GeoDataFeature::RoboticRover] = "RoboticRover";
        visualCategoryNames[GeoDataFeature::UnmannedSoftLandingSite] = "UnmannedSoftLandingSite";
        visualCategoryNames[GeoDataFeature::UnmannedHardLandingSite] = "UnmannedHardLandingSite";
        visualCategoryNames[GeoDataFeature::Folder] = "Folder";
        visualCategoryNames[GeoDataFeature::Bookmark] = "Bookmark";
        visualCategoryNames[GeoDataFeature::NaturalWater] = "NaturalWater";
        visualCategoryNames[GeoDataFeature::NaturalReef] = "NaturalReef";
        visualCategoryNames[GeoDataFeature::NaturalWood] = "NaturalWood";
        visualCategoryNames[GeoDataFeature::NaturalBeach] = "NaturalBeach";
        visualCategoryNames[GeoDataFeature::NaturalWetland] = "NaturalWetland";
        visualCategoryNames[GeoDataFeature::NaturalGlacier] = "NaturalGlacier";
        visualCategoryNames[GeoDataFeature::NaturalIceShelf] = "NaturalIceShelf";
        visualCategoryNames[GeoDataFeature::NaturalScrub] = "NaturalScrub";
        visualCategoryNames[GeoDataFeature::NaturalCliff] = "NaturalCliff";
        visualCategoryNames[GeoDataFeature::NaturalHeath] = "NaturalHeath";
        visualCategoryNames[GeoDataFeature::HighwayTrafficSignals] = "HighwayTrafficSignals";
        visualCategoryNames[GeoDataFeature::HighwaySteps] = "HighwaySteps";
        visualCategoryNames[GeoDataFeature::HighwayUnknown] = "HighwayUnknown";
        visualCategoryNames[GeoDataFeature::HighwayPath] = "HighwayPath";
        visualCategoryNames[GeoDataFeature::HighwayFootway] = "HighwayFootway";
        visualCategoryNames[GeoDataFeature::HighwayTrack] = "HighwayTrack";
        visualCategoryNames[GeoDataFeature::HighwayPedestrian] = "HighwayPedestrian";
        visualCategoryNames[GeoDataFeature::HighwayCycleway] = "HighwayCycleway";
        visualCategoryNames[GeoDataFeature::HighwayService] = "HighwayService";
        visualCategoryNames[GeoDataFeature::HighwayRoad] = "HighwayRoad";
        visualCategoryNames[GeoDataFeature::HighwayResidential] = "HighwayResidential";
        visualCategoryNames[GeoDataFeature::HighwayLivingStreet] = "HighwayLivingStreet";
        visualCategoryNames[GeoDataFeature::HighwayUnclassified] = "HighwayUnclassified";
        visualCategoryNames[GeoDataFeature::HighwayTertiaryLink] = "HighwayTertiaryLink";
        visualCategoryNames[GeoDataFeature::HighwayTertiary] = "HighwayTertiary";
        visualCategoryNames[GeoDataFeature::HighwaySecondaryLink] = "HighwaySecondaryLink";
        visualCategoryNames[GeoDataFeature::HighwaySecondary] = "HighwaySecondary";
        visualCategoryNames[GeoDataFeature::HighwayPrimaryLink] = "HighwayPrimaryLink";
        visualCategoryNames[GeoDataFeature::HighwayPrimary] = "HighwayPrimary";
        visualCategoryNames[GeoDataFeature::HighwayTrunkLink] = "HighwayTrunkLink";
        visualCategoryNames[GeoDataFeature::HighwayTrunk] = "HighwayTrunk";
        visualCategoryNames[GeoDataFeature::HighwayMotorwayLink] = "HighwayMotorwayLink";
        visualCategoryNames[GeoDataFeature::HighwayMotorway] = "HighwayMotorway";
        visualCategoryNames[GeoDataFeature::Building] = "Building";
        visualCategoryNames[GeoDataFeature::AccomodationCamping] = "AccomodationCamping";
        visualCategoryNames[GeoDataFeature::AccomodationHostel] = "AccomodationHostel";
        visualCategoryNames[GeoDataFeature::AccomodationHotel] = "AccomodationHotel";
        visualCategoryNames[GeoDataFeature::AccomodationMotel] = "AccomodationMotel";
        visualCategoryNames[GeoDataFeature::AccomodationYouthHostel] = "AccomodationYouthHostel";
        visualCategoryNames[GeoDataFeature::AccomodationGuestHouse] = "AccomodationGuestHouse";
        visualCategoryNames[GeoDataFeature::AmenityLibrary] = "AmenityLibrary";
        visualCategoryNames[GeoDataFeature::AmenityKindergarten] = "AmenityKindergarten";
        visualCategoryNames[GeoDataFeature::EducationCollege] = "EducationCollege";
        visualCategoryNames[GeoDataFeature::EducationSchool] = "EducationSchool";
        visualCategoryNames[GeoDataFeature::EducationUniversity] = "EducationUniversity";
        visualCategoryNames[GeoDataFeature::FoodBar] = "FoodBar";
        visualCategoryNames[GeoDataFeature::FoodBiergarten] = "FoodBiergarten";
        visualCategoryNames[GeoDataFeature::FoodCafe] = "FoodCafe";
        visualCategoryNames[GeoDataFeature::FoodFastFood] = "FoodFastFood";
        visualCategoryNames[GeoDataFeature::FoodPub] = "FoodPub";
        visualCategoryNames[GeoDataFeature::FoodRestaurant] = "FoodRestaurant";
        visualCategoryNames[GeoDataFeature::HealthDentist] = "HealthDentist";
        visualCategoryNames[GeoDataFeature::HealthDoctors] = "HealthDoctors";
        visualCategoryNames[GeoDataFeature::HealthHospital] = "HealthHospital";
        visualCategoryNames[GeoDataFeature::HealthPharmacy] = "HealthPharmacy";
        visualCategoryNames[GeoDataFeature::HealthVeterinary] = "HealthVeterinary";
        visualCategoryNames[GeoDataFeature::MoneyAtm] = "MoneyAtm";
        visualCategoryNames[GeoDataFeature::MoneyBank] = "MoneyBank";
        visualCategoryNames[GeoDataFeature::AmenityArchaeologicalSite] = "AmenityArchaeologicalSite";
        visualCategoryNames[GeoDataFeature::AmenityEmbassy] = "AmenityEmbassy";
        visualCategoryNames[GeoDataFeature::AmenityEmergencyPhone] = "AmenityEmergencyPhone";
        visualCategoryNames[GeoDataFeature::AmenityWaterPark] = "AmenityWaterPark";
        visualCategoryNames[GeoDataFeature::AmenityCommunityCentre] = "AmenityCommunityCentre";
        visualCategoryNames[GeoDataFeature::AmenityFountain] = "AmenityFountain";
        visualCategoryNames[GeoDataFeature::AmenityNightClub] = "AmenityNightClub";
        visualCategoryNames[GeoDataFeature::AmenityBench] = "AmenityBench";
        visualCategoryNames[GeoDataFeature::AmenityCourtHouse] = "AmenityCourtHouse";
        visualCategoryNames[GeoDataFeature::AmenityFireStation] = "AmenityFireStation";
        visualCategoryNames[GeoDataFeature::AmenityHuntingStand] = "AmenityHuntingStand";
        visualCategoryNames[GeoDataFeature::AmenityPolice] = "AmenityPolice";
        visualCategoryNames[GeoDataFeature::AmenityPostBox] = "AmenityPostBox";
        visualCategoryNames[GeoDataFeature::AmenityPostOffice] = "AmenityPostOffice";
        visualCategoryNames[GeoDataFeature::AmenityPrison] = "AmenityPrison";
        visualCategoryNames[GeoDataFeature::AmenityRecycling] = "AmenityRecycling";
        visualCategoryNames[GeoDataFeature::AmenityShelter] = "AmenityShelter";
        visualCategoryNames[GeoDataFeature::AmenityTelephone] = "AmenityTelephone";
        visualCategoryNames[GeoDataFeature::AmenityToilets] = "AmenityToilets";
        visualCategoryNames[GeoDataFeature::AmenityTownHall] = "AmenityTownHall";
        visualCategoryNames[GeoDataFeature::AmenityWasteBasket] = "AmenityWasteBasket";
        visualCategoryNames[GeoDataFeature::AmenityDrinkingWater] = "AmenityDrinkingWater";
        visualCategoryNames[GeoDataFeature::AmenityGraveyard] = "AmenityGraveyard";
        visualCategoryNames[GeoDataFeature::BarrierCityWall] = "BarrierCityWall";
        visualCategoryNames[GeoDataFeature::BarrierGate] = "BarrierGate";
        visualCategoryNames[GeoDataFeature::BarrierLiftGate] = "BarrierLiftGate";
        visualCategoryNames[GeoDataFeature::BarrierWall] = "BarrierWall";
        visualCategoryNames[GeoDataFeature::NaturalPeak] = "NaturalPeak";
        visualCategoryNames[GeoDataFeature::NaturalTree] = "NaturalTree";
        visualCategoryNames[GeoDataFeature::ShopBeverages] = "ShopBeverages";
        visualCategoryNames[GeoDataFeature::ShopHifi] = "ShopHifi";
        visualCategoryNames[GeoDataFeature::ShopSupermarket] = "ShopSupermarket";
        visualCategoryNames[GeoDataFeature::ShopAlcohol] = "ShopAlcohol";
        visualCategoryNames[GeoDataFeature::ShopBakery] = "ShopBakery";
        visualCategoryNames[GeoDataFeature::ShopButcher] = "ShopButcher";
        visualCategoryNames[GeoDataFeature::ShopConfectionery] = "ShopConfectionery";
        visualCategoryNames[GeoDataFeature::ShopConvenience] = "ShopConvenience";
        visualCategoryNames[GeoDataFeature::ShopGreengrocer] = "ShopGreengrocer";
        visualCategoryNames[GeoDataFeature::ShopSeafood] = "ShopSeafood";
        visualCategoryNames[GeoDataFeature::ShopDepartmentStore] = "ShopDepartmentStore";
        visualCategoryNames[GeoDataFeature::ShopKiosk] = "ShopKiosk";
        visualCategoryNames[GeoDataFeature::ShopBag] = "ShopBag";
        visualCategoryNames[GeoDataFeature::ShopClothes] = "ShopClothes";
        visualCategoryNames[GeoDataFeature::ShopFashion] = "ShopFashion";
        visualCategoryNames[GeoDataFeature::ShopJewelry] = "ShopJewelry";
        visualCategoryNames[GeoDataFeature::ShopShoes] = "ShopShoes";
        visualCategoryNames[GeoDataFeature::ShopVarietyStore] = "ShopVarietyStore";
        visualCategoryNames[GeoDataFeature::ShopBeauty] = "ShopBeauty";
        visualCategoryNames[GeoDataFeature::ShopChemist] = "ShopChemist";
        visualCategoryNames[GeoDataFeature::ShopCosmetics] = "ShopCosmetics";
        visualCategoryNames[GeoDataFeature::ShopHairdresser] = "ShopHairdresser";
        visualCategoryNames[GeoDataFeature::ShopOptician] = "ShopOptician";
        visualCategoryNames[GeoDataFeature::ShopPerfumery] = "ShopPerfumery";
        visualCategoryNames[GeoDataFeature::ShopDoitYourself] = "ShopDoitYourself";
        visualCategoryNames[GeoDataFeature::ShopFlorist] = "ShopFlorist";
        visualCategoryNames[GeoDataFeature::ShopHardware] = "ShopHardware";
        visualCategoryNames[GeoDataFeature::ShopFurniture] = "ShopFurniture";
        visualCategoryNames[GeoDataFeature::ShopElectronics] = "ShopElectronics";
        visualCategoryNames[GeoDataFeature::ShopMobilePhone] = "ShopMobilePhone";
        visualCategoryNames[GeoDataFeature::ShopBicycle] = "ShopBicycle";
        visualCategoryNames[GeoDataFeature::ShopCar] = "ShopCar";
        visualCategoryNames[GeoDataFeature::ShopCarRepair] = "ShopCarRepair";
        visualCategoryNames[GeoDataFeature::ShopCarParts] = "ShopCarParts";
        visualCategoryNames[GeoDataFeature::ShopMotorcycle] = "ShopMotorcycle";
        visualCategoryNames[GeoDataFeature::ShopOutdoor] = "ShopOutdoor";
        visualCategoryNames[GeoDataFeature::ShopMusicalInstrument] = "ShopMusicalInstrument";
        visualCategoryNames[GeoDataFeature::ShopPhoto] = "ShopPhoto";
        visualCategoryNames[GeoDataFeature::ShopBook] = "ShopBook";
        visualCategoryNames[GeoDataFeature::ShopGift] = "ShopGift";
        visualCategoryNames[GeoDataFeature::ShopStationery] = "ShopStationery";
        visualCategoryNames[GeoDataFeature::ShopLaundry] = "ShopLaundry";
        visualCategoryNames[GeoDataFeature::ShopPet] = "ShopPet";
        visualCategoryNames[GeoDataFeature::ShopToys] = "ShopToys";
        visualCategoryNames[GeoDataFeature::ShopTravelAgency] = "ShopTravelAgency";
        visualCategoryNames[GeoDataFeature::Shop] = "Shop";
        visualCategoryNames[GeoDataFeature::ManmadeBridge] = "ManmadeBridge";
        visualCategoryNames[GeoDataFeature::ManmadeLighthouse] = "ManmadeLighthouse";
        visualCategoryNames[GeoDataFeature::ManmadePier] = "ManmadePier";
        visualCategoryNames[GeoDataFeature::ManmadeWaterTower] = "ManmadeWaterTower";
        visualCategoryNames[GeoDataFeature::ManmadeWindMill] = "ManmadeWindMill";
        visualCategoryNames[GeoDataFeature::TouristAttraction] = "TouristAttraction";
        visualCategoryNames[GeoDataFeature::TouristCastle] = "TouristCastle";
        visualCategoryNames[GeoDataFeature::TouristCinema] = "TouristCinema";
        visualCategoryNames[GeoDataFeature::TouristInformation] = "TouristInformation";
        visualCategoryNames[GeoDataFeature::TouristMonument] = "TouristMonument";
        visualCategoryNames[GeoDataFeature::TouristMuseum] = "TouristMuseum";
        visualCategoryNames[GeoDataFeature::TouristRuin] = "TouristRuin";
        visualCategoryNames[GeoDataFeature::TouristTheatre] = "TouristTheatre";
        visualCategoryNames[GeoDataFeature::TouristThemePark] = "TouristThemePark";
        visualCategoryNames[GeoDataFeature::TouristViewPoint] = "TouristViewPoint";
        visualCategoryNames[GeoDataFeature::TouristZoo] = "TouristZoo";
        visualCategoryNames[GeoDataFeature::TouristAlpineHut] = "TouristAlpineHut";
        visualCategoryNames[GeoDataFeature::TransportAerodrome] = "TransportAerodrome";
        visualCategoryNames[GeoDataFeature::TransportHelipad] = "TransportHelipad";
        visualCategoryNames[GeoDataFeature::TransportAirportTerminal] = "TransportAirportTerminal";
        visualCategoryNames[GeoDataFeature::TransportAirportGate] = "TransportAirportGate";
        visualCategoryNames[GeoDataFeature::TransportAirportRunway] = "TransportAirportRunway";
        visualCategoryNames[GeoDataFeature::TransportAirportTaxiway] = "TransportAirportTaxiway";
        visualCategoryNames[GeoDataFeature::TransportAirportApron] = "TransportAirportApron";
        visualCategoryNames[GeoDataFeature::TransportBusStation] = "TransportBusStation";
        visualCategoryNames[GeoDataFeature::TransportBusStop] = "TransportBusStop";
        visualCategoryNames[GeoDataFeature::TransportCarShare] = "TransportCarShare";
        visualCategoryNames[GeoDataFeature::TransportFuel] = "TransportFuel";
        visualCategoryNames[GeoDataFeature::TransportParking] = "TransportParking";
        visualCategoryNames[GeoDataFeature::TransportParkingSpace] = "TransportParkingSpace";
        visualCategoryNames[GeoDataFeature::TransportPlatform] = "TransportPlatform";
        visualCategoryNames[GeoDataFeature::TransportRentalBicycle] = "TransportRentalBicycle";
        visualCategoryNames[GeoDataFeature::TransportRentalCar] = "TransportRentalCar";
        visualCategoryNames[GeoDataFeature::TransportTaxiRank] = "TransportTaxiRank";
        visualCategoryNames[GeoDataFeature::TransportTrainStation] = "TransportTrainStation";
        visualCategoryNames[GeoDataFeature::TransportTramStop] = "TransportTramStop";
        visualCategoryNames[GeoDataFeature::TransportBicycleParking] = "TransportBicycleParking";
        visualCategoryNames[GeoDataFeature::TransportMotorcycleParking] = "TransportMotorcycleParking";
        visualCategoryNames[GeoDataFeature::TransportSubwayEntrance] = "TransportSubwayEntrance";
        visualCategoryNames[GeoDataFeature::ReligionPlaceOfWorship] = "ReligionPlaceOfWorship";
        visualCategoryNames[GeoDataFeature::ReligionBahai] = "ReligionBahai";
        visualCategoryNames[GeoDataFeature::ReligionBuddhist] = "ReligionBuddhist";
        visualCategoryNames[GeoDataFeature::ReligionChristian] = "ReligionChristian";
        visualCategoryNames[GeoDataFeature::ReligionMuslim] = "ReligionMuslim";
        visualCategoryNames[GeoDataFeature::ReligionHindu] = "ReligionHindu";
        visualCategoryNames[GeoDataFeature::ReligionJain] = "ReligionJain";
        visualCategoryNames[GeoDataFeature::ReligionJewish] = "ReligionJewish";
        visualCategoryNames[GeoDataFeature::ReligionShinto] = "ReligionShinto";
        visualCategoryNames[GeoDataFeature::ReligionSikh] = "ReligionSikh";
        visualCategoryNames[GeoDataFeature::LeisureGolfCourse] = "LeisureGolfCourse";
        visualCategoryNames[GeoDataFeature::LeisureMarina] = "LeisureMarina";
        visualCategoryNames[GeoDataFeature::LeisurePark] = "LeisurePark";
        visualCategoryNames[GeoDataFeature::LeisurePlayground] = "LeisurePlayground";
        visualCategoryNames[GeoDataFeature::LeisurePitch] = "LeisurePitch";
        visualCategoryNames[GeoDataFeature::LeisureSportsCentre] = "LeisureSportsCentre";
        visualCategoryNames[GeoDataFeature::LeisureStadium] = "LeisureStadium";
        visualCategoryNames[GeoDataFeature::LeisureTrack] = "LeisureTrack";
        visualCategoryNames[GeoDataFeature::LeisureSwimmingPool] = "LeisureSwimmingPool";
        visualCategoryNames[GeoDataFeature::LanduseAllotments] = "LanduseAllotments";
        visualCategoryNames[GeoDataFeature::LanduseBasin] = "LanduseBasin";
        visualCategoryNames[GeoDataFeature::LanduseCemetery] = "LanduseCemetery";
        visualCategoryNames[GeoDataFeature::LanduseCommercial] = "LanduseCommercial";
        visualCategoryNames[GeoDataFeature::LanduseConstruction] = "LanduseConstruction";
        visualCategoryNames[GeoDataFeature::LanduseFarmland] = "LanduseFarmland";
        visualCategoryNames[GeoDataFeature::LanduseFarmyard] = "LanduseFarmyard";
        visualCategoryNames[GeoDataFeature::LanduseGarages] = "LanduseGarages";
        visualCategoryNames[GeoDataFeature::LanduseGrass] = "LanduseGrass";
        visualCategoryNames[GeoDataFeature::LanduseIndustrial] = "LanduseIndustrial";
        visualCategoryNames[GeoDataFeature::LanduseLandfill] = "LanduseLandfill";
        visualCategoryNames[GeoDataFeature::LanduseMeadow] = "LanduseMeadow";
        visualCategoryNames[GeoDataFeature::LanduseMilitary] = "LanduseMilitary";
        visualCategoryNames[GeoDataFeature::LanduseQuarry] = "LanduseQuarry";
        visualCategoryNames[GeoDataFeature::LanduseRailway] = "LanduseRailway";
        visualCategoryNames[GeoDataFeature::LanduseReservoir] = "LanduseReservoir";
        visualCategoryNames[GeoDataFeature::LanduseResidential] = "LanduseResidential";
        visualCategoryNames[GeoDataFeature::LanduseRetail] = "LanduseRetail";
        visualCategoryNames[GeoDataFeature::LanduseOrchard] = "LanduseOrchard";
        visualCategoryNames[GeoDataFeature::LanduseVineyard] = "LanduseVineyard";
        visualCategoryNames[GeoDataFeature::RailwayRail] = "RailwayRail";
        visualCategoryNames[GeoDataFeature::RailwayNarrowGauge] = "RailwayNarrowGauge";
        visualCategoryNames[GeoDataFeature::RailwayTram] = "RailwayTram";
        visualCategoryNames[GeoDataFeature::RailwayLightRail] = "RailwayLightRail";
        visualCategoryNames[GeoDataFeature::RailwayAbandoned] = "RailwayAbandoned";
        visualCategoryNames[GeoDataFeature::RailwaySubway] = "RailwaySubway";
        visualCategoryNames[GeoDataFeature::RailwayPreserved] = "RailwayPreserved";
        visualCategoryNames[GeoDataFeature::RailwayMiniature] = "RailwayMiniature";
        visualCategoryNames[GeoDataFeature::RailwayConstruction] = "RailwayConstruction";
        visualCategoryNames[GeoDataFeature::RailwayMonorail] = "RailwayMonorail";
        visualCategoryNames[GeoDataFeature::RailwayFunicular] = "RailwayFunicular";
        visualCategoryNames[GeoDataFeature::PowerTower] = "PowerTower";
        visualCategoryNames[GeoDataFeature::Satellite] = "Satellite";
        visualCategoryNames[GeoDataFeature::Landmass] = "Landmass";
        visualCategoryNames[GeoDataFeature::UrbanArea] = "UrbanArea";
        visualCategoryNames[GeoDataFeature::InternationalDateLine] = "InternationalDateLine";
        visualCategoryNames[GeoDataFeature::Bathymetry] = "Bathymetry";
        visualCategoryNames[GeoDataFeature::AdminLevel1] = "AdminLevel1";
        visualCategoryNames[GeoDataFeature::AdminLevel2] = "AdminLevel2";
        visualCategoryNames[GeoDataFeature::AdminLevel3] = "AdminLevel3";
        visualCategoryNames[GeoDataFeature::AdminLevel4] = "AdminLevel4";
        visualCategoryNames[GeoDataFeature::AdminLevel5] = "AdminLevel5";
        visualCategoryNames[GeoDataFeature::AdminLevel6] = "AdminLevel6";
        visualCategoryNames[GeoDataFeature::AdminLevel7] = "AdminLevel7";
        visualCategoryNames[GeoDataFeature::AdminLevel8] = "AdminLevel8";
        visualCategoryNames[GeoDataFeature::AdminLevel9] = "AdminLevel9";
        visualCategoryNames[GeoDataFeature::AdminLevel10] = "AdminLevel10";
        visualCategoryNames[GeoDataFeature::AdminLevel11] = "AdminLevel11";
        visualCategoryNames[GeoDataFeature::BoundaryMaritime] = "BoundaryMaritime";
        visualCategoryNames[GeoDataFeature::LastIndex] = "LastIndex";
    }

    Q_ASSERT(visualCategoryNames.contains(category));
    return visualCategoryNames[category];
}

GeoDataFeature::GeoDataVisualCategory StyleBuilder::osmVisualCategory(const StyleBuilder::OsmTag &tag)
{
    Private::initializeOsmVisualCategories();

    return Private::s_visualCategories.value(tag, GeoDataFeature::None);
}

QHash<StyleBuilder::OsmTag, GeoDataFeature::GeoDataVisualCategory>::const_iterator StyleBuilder::begin()
{
    Private::initializeOsmVisualCategories();

    return Private::s_visualCategories.constBegin();
}

QHash<StyleBuilder::OsmTag, GeoDataFeature::GeoDataVisualCategory>::const_iterator StyleBuilder::end()
{
    Private::initializeOsmVisualCategories();

    return Private::s_visualCategories.constEnd();
}

QStringList StyleBuilder::shopValues()
{
    // from https://taginfo.openstreetmap.org/keys/building#values
    static const QStringList osmShopValues = QStringList()
        << "cheese" << "chocolate" << "coffee" << "deli" << "dairy" << "farm"
        << "pasta" << "pastry" << "tea" << "wine" << "general" << "mall"
        << "baby_goods" << "boutique" << "fabric" << "leather" << "tailor" << "watches"
        << "charity" << "second_hand" << "erotic" << "hearing_aids" << "herbalist" << "massage"
        << "medical_supply" << "tattoo" << "bathroom_furnishing" << "electrical" << "energy" << "furnace"
        << "garden_centre" << "garden_furniture" << "gas" << "glaziery" << "houseware" << "locksmith"
        << "paint" << "trade" << "antiques" << "bed" << "candles" << "carpet"
        << "curtain" << "interior_decoration" << "kitchen" << "lamps" << "window_blind" << "computer"
        << "radiotechnics" << "vacuum_cleaner" << "fishing" << "free_flying" << "hunting" << "outdoor"
        << "scuba_diving" << "sports" << "tyres" << "swimming_pool" << "art" << "craft"
        << "frame" << "games" << "model" << "music" << "trophy" << "video"
        << "video_games" << "anime" << "ticket" << "copyshop" << "dry_cleaning" << "e-cigarette"
        << "funeral_directors" << "money_lender" << "pawnbroker" << "pyrotechnics" << "religion" << "storage_rental"
        << "tobacco" << "weapons" << "user defined";

    return osmShopValues;
}

QSet<StyleBuilder::OsmTag> StyleBuilder::buildingTags()
{
    static const QString building = QStringLiteral("building");
    // from https://taginfo.openstreetmap.org/keys/building#values
    static const QSet<OsmTag> osmBuildingTags = QSet<OsmTag>()
        << OsmTag(building, "yes")
        << OsmTag(building, "house")
        << OsmTag(building, "residential")
        << OsmTag(building, "garage")
        << OsmTag(building, "apartments")
        << OsmTag(building, "hut")
        << OsmTag(building, "industrial")
        << OsmTag(building, "detached")
        << OsmTag(building, "roof")
        << OsmTag(building, "garages")
        << OsmTag(building, "commercial")
        << OsmTag(building, "terrace")
        << OsmTag(building, "shed")
        << OsmTag(building, "school")
        << OsmTag(building, "retail")
        << OsmTag(building, "farm_auxiliary")
        << OsmTag(building, "church")
        << OsmTag(building, "cathedral")
        << OsmTag(building, "greenhouse")
        << OsmTag(building, "barn")
        << OsmTag(building, "service")
        << OsmTag(building, "manufacture")
        << OsmTag(building, "construction")
        << OsmTag(building, "cabin")
        << OsmTag(building, "farm")
        << OsmTag(building, "warehouse")
        << OsmTag(building, "House")
        << OsmTag(building, "office")
        << OsmTag(building, "civic")
        << OsmTag(building, "Residential")
        << OsmTag(building, "hangar")
        << OsmTag(building, "public")
        << OsmTag(building, "university")
        << OsmTag(building, "hospital")
        << OsmTag(building, "chapel")
        << OsmTag(building, "hotel")
        << OsmTag(building, "train_station")
        << OsmTag(building, "dormitory")
        << OsmTag(building, "kindergarten")
        << OsmTag(building, "stable")
        << OsmTag(building, "storage_tank")
        << OsmTag(building, "shop")
        << OsmTag(building, "college")
        << OsmTag(building, "supermarket")
        << OsmTag(building, "factory")
        << OsmTag(building, "bungalow")
        << OsmTag(building, "tower")
        << OsmTag(building, "silo")
        << OsmTag(building, "storage")
        << OsmTag(building, "station")
        << OsmTag(building, "education")
        << OsmTag(building, "carport")
        << OsmTag(building, "houseboat")
        << OsmTag(building, "castle")
        << OsmTag(building, "social_facility")
        << OsmTag(building, "water_tower")
        << OsmTag(building, "container")
        << OsmTag(building, "exhibition_hall")
        << OsmTag(building, "monastery")
        << OsmTag(building, "bunker")
        << OsmTag(building, "shelter")
        << OsmTag("building:part", "yes")
        << OsmTag("building:part", "antenna");

    return osmBuildingTags;
}

GeoDataFeature::GeoDataVisualCategory StyleBuilder::determineVisualCategory(const OsmPlacemarkData &osmData)
{
    if (osmData.containsTagKey(QStringLiteral("area:highway")) ||              // Not supported yet
            osmData.containsTag("boundary", "protected_area") ||   // Not relevant for the default map
            osmData.containsTag("boundary", "postal_code") ||
            osmData.containsTag("boundary", "aerial_views") ||     // Created by OSM editor(s) application for digitalization
            osmData.containsTagKey(QStringLiteral("closed:highway")) ||
            osmData.containsTagKey(QStringLiteral("abandoned:highway")) ||
            osmData.containsTagKey(QStringLiteral("abandoned:natural")) ||
            osmData.containsTagKey(QStringLiteral("abandoned:building")) ||
            osmData.containsTagKey(QStringLiteral("abandoned:leisure")) ||
            osmData.containsTagKey(QStringLiteral("disused:highway")) ||
            osmData.containsTag("highway", "razed")) {
        return GeoDataFeature::None;
    }

    if (osmData.containsTag("historic", "castle") && osmData.containsTag("castle_type", "kremlin")) {
        return GeoDataFeature::None;
    }

    if( osmData.containsTag("natural", "glacier") && osmData.containsTag("glacier:type", "shelf") ){
        return GeoDataFeature::NaturalIceShelf;
    }

    for (auto iter = osmData.tagsBegin(), end=osmData.tagsEnd(); iter != end; ++iter) {
        const auto tag = OsmTag(iter.key(), iter.value());
        GeoDataFeature::GeoDataVisualCategory category = osmVisualCategory(tag);
        if (category != GeoDataFeature::None) {
            if (category == GeoDataFeature::PlaceCity && osmData.containsTag("capital", "yes")) {
                category = GeoDataFeature::PlaceCityCapital;
            } else if (category == GeoDataFeature::PlaceTown && osmData.containsTag("capital", "yes")) {
                category = GeoDataFeature::PlaceTownCapital;
            } else if (category == GeoDataFeature::PlaceVillage && osmData.containsTag("capital", "yes")) {
                category = GeoDataFeature::PlaceVillageCapital;
            }

            return category;
        }
    }

    return GeoDataFeature::None;
}

StyleParameters::StyleParameters(const GeoDataFeature *feature_, int tileLevel_) :
    feature(feature_),
    tileLevel(tileLevel_)
{
    // nothing to do
}

}
