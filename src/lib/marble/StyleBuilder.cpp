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

    GeoDataStyle::ConstPtr presetStyle(GeoDataPlacemark::GeoDataVisualCategory visualCategory) const;

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

    // Having an outline with the same color as the fill results in degraded
    // performance and degraded display quality for no good reason
    // Q_ASSERT( !(outline && color == outlineColor && brushStyle == Qt::SolidPattern) );
    void initializeDefaultStyles();

    static QString createPaintLayerItem(const QString &itemType, GeoDataPlacemark::GeoDataVisualCategory visualCategory, const QString &subType = QString());

    static void initializeOsmVisualCategories();

    int m_defaultMinZoomLevels[GeoDataPlacemark::LastIndex];
    int m_maximumZoomLevel;
    QColor m_defaultLabelColor;
    QFont m_defaultFont;
    GeoDataStyle::Ptr m_defaultStyle[GeoDataPlacemark::LastIndex];
    GeoDataStyle::Ptr m_styleTreeAutumn;
    GeoDataStyle::Ptr m_styleTreeWinter;
    bool m_defaultStyleInitialized;

    /**
     * @brief s_visualCategories contains osm tag mappings to GeoDataVisualCategories
     */
    static QHash<OsmTag, GeoDataPlacemark::GeoDataVisualCategory> s_visualCategories;
};

QHash<StyleBuilder::OsmTag, GeoDataPlacemark::GeoDataVisualCategory> StyleBuilder::Private::s_visualCategories;

StyleBuilder::Private::Private() :
    m_maximumZoomLevel(15),
    m_defaultLabelColor(Qt::black),
    m_defaultFont(QStringLiteral("Sans Serif")),
    m_defaultStyle(),
    m_defaultStyleInitialized(false)
{
    for ( int i = 0; i < GeoDataPlacemark::LastIndex; i++ )
        m_defaultMinZoomLevels[i] = m_maximumZoomLevel;

    m_defaultMinZoomLevels[GeoDataPlacemark::Default]             = 1;
    m_defaultMinZoomLevels[GeoDataPlacemark::PlaceCity]           = 9;
    m_defaultMinZoomLevels[GeoDataPlacemark::PlaceCityCapital]    = 9;
    m_defaultMinZoomLevels[GeoDataPlacemark::PlaceSuburb]         = 13;
    m_defaultMinZoomLevels[GeoDataPlacemark::PlaceHamlet]         = 15;
    m_defaultMinZoomLevels[GeoDataPlacemark::PlaceLocality]       = 15;
    m_defaultMinZoomLevels[GeoDataPlacemark::PlaceTown]           = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::PlaceTownCapital]    = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::PlaceVillage]        = 13;
    m_defaultMinZoomLevels[GeoDataPlacemark::PlaceVillageCapital] = 13;
    m_defaultMinZoomLevels[GeoDataPlacemark::NaturalReef]         = 3;
    m_defaultMinZoomLevels[GeoDataPlacemark::NaturalWater]        = 3;
    m_defaultMinZoomLevels[GeoDataPlacemark::NaturalWood]         = 8;
    m_defaultMinZoomLevels[GeoDataPlacemark::NaturalBeach]        = 10;
    m_defaultMinZoomLevels[GeoDataPlacemark::NaturalWetland]      = 10;
    m_defaultMinZoomLevels[GeoDataPlacemark::NaturalGlacier]      = 3;
    m_defaultMinZoomLevels[GeoDataPlacemark::NaturalIceShelf]     = 3;
    m_defaultMinZoomLevels[GeoDataPlacemark::NaturalScrub]        = 10;
    m_defaultMinZoomLevels[GeoDataPlacemark::NaturalCliff]        = 15;
    m_defaultMinZoomLevels[GeoDataPlacemark::NaturalPeak]         = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::BarrierCityWall]     = 15;
    m_defaultMinZoomLevels[GeoDataPlacemark::Building]            = 15;

    m_defaultMinZoomLevels[GeoDataPlacemark::ManmadeBridge]       = 15;

        // OpenStreetMap highways
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwaySteps]        = 15;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwayUnknown]      = 13;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwayPath]         = 13;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwayTrack]        = 13;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwayPedestrian]   = 13;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwayFootway]      = 13;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwayCycleway]     = 13;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwayService]      = 13;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwayRoad]         = 13;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwayTertiaryLink] = 10;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwayTertiary]     = 10;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwaySecondaryLink]= 10;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwaySecondary]    = 9;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwayPrimaryLink]  = 10;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwayPrimary]      = 8;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwayTrunkLink]    = 10;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwayTrunk]        = 7;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwayMotorwayLink] = 10;
    m_defaultMinZoomLevels[GeoDataPlacemark::HighwayMotorway]     = 6;
    m_defaultMinZoomLevels[GeoDataPlacemark::TransportAirportRunway] = 15;
    m_defaultMinZoomLevels[GeoDataPlacemark::TransportAirportTaxiway] = 15;
    m_defaultMinZoomLevels[GeoDataPlacemark::TransportAirportApron] = 15;

#if 0 // not needed as long as default min zoom level is 15
    for(int i = GeoDataPlacemark::AccomodationCamping; i <= GeoDataPlacemark::ReligionSikh; i++)
        s_defaultMinZoomLevels[i] = 15;
#endif

    m_defaultMinZoomLevels[GeoDataPlacemark::AmenityGraveyard]    = 14;
    m_defaultMinZoomLevels[GeoDataPlacemark::AmenityFountain]     = 17;
    m_defaultMinZoomLevels[GeoDataPlacemark::AmenityBench]        = 19;
    m_defaultMinZoomLevels[GeoDataPlacemark::AmenityWasteBasket]  = 19;

    m_defaultMinZoomLevels[GeoDataPlacemark::MilitaryDangerArea]  = 11;

    m_defaultMinZoomLevels[GeoDataPlacemark::LeisureMarina]       = 13;
    m_defaultMinZoomLevels[GeoDataPlacemark::LeisurePark]         = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LeisurePlayground]   = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseAllotments]   = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseBasin]        = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseCemetery]     = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseCommercial]   = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseConstruction] = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseFarmland]     = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseFarmyard]     = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseGarages]      = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseGrass]        = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseIndustrial]   = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseLandfill]     = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseMeadow]       = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseMilitary]     = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseQuarry]       = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseRailway]      = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseReservoir]    = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseResidential]  = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseRetail]       = 11;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseOrchard]      = 14;
    m_defaultMinZoomLevels[GeoDataPlacemark::LanduseVineyard]     = 14;

    m_defaultMinZoomLevels[GeoDataPlacemark::RailwayRail]         = 6;
    m_defaultMinZoomLevels[GeoDataPlacemark::RailwayNarrowGauge]  = 6;
    m_defaultMinZoomLevels[GeoDataPlacemark::RailwayTram]         = 14;
    m_defaultMinZoomLevels[GeoDataPlacemark::RailwayLightRail]    = 12;
    m_defaultMinZoomLevels[GeoDataPlacemark::RailwayAbandoned]    = 10;
    m_defaultMinZoomLevels[GeoDataPlacemark::RailwaySubway]       = 13;
    m_defaultMinZoomLevels[GeoDataPlacemark::RailwayPreserved]    = 13;
    m_defaultMinZoomLevels[GeoDataPlacemark::RailwayMiniature]    = 13;
    m_defaultMinZoomLevels[GeoDataPlacemark::RailwayConstruction] = 10;
    m_defaultMinZoomLevels[GeoDataPlacemark::RailwayMonorail]     = 12;
    m_defaultMinZoomLevels[GeoDataPlacemark::RailwayFunicular]    = 13;
    m_defaultMinZoomLevels[GeoDataPlacemark::TransportPlatform]   = 16;

    m_defaultMinZoomLevels[GeoDataPlacemark::Satellite]           = 0;

    m_defaultMinZoomLevels[GeoDataPlacemark::Landmass]            = 0;
    m_defaultMinZoomLevels[GeoDataPlacemark::UrbanArea]           = 3;
    m_defaultMinZoomLevels[GeoDataPlacemark::InternationalDateLine]      = 1;
    m_defaultMinZoomLevels[GeoDataPlacemark::Bathymetry]          = 1;


    m_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel1]         = 0;
    m_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel2]         = 1;
    m_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel3]         = 1;
    m_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel4]         = 2;
    m_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel5]         = 4;
    m_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel6]         = 5;
    m_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel7]         = 5;
    m_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel8]         = 7;
    m_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel9]         = 7;
    m_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel10]        = 8;
    m_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel11]        = 8;

    m_defaultMinZoomLevels[GeoDataPlacemark::BoundaryMaritime]    = 1;

    for (int i = 0; i < GeoDataPlacemark::LastIndex; ++i) {
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

    m_defaultStyle[GeoDataPlacemark::None]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::Default]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/default_location.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::Unknown]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::SmallCity]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_4_white.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::SmallCountyCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_4_yellow.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::SmallStateCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_4_orange.png" ),
              QFont( defaultFamily, defaultSize, 50, true  ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::SmallNationCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_4_red.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::MediumCity]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_3_white.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::MediumCountyCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_3_yellow.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::MediumStateCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_3_orange.png" ),
              QFont( defaultFamily, defaultSize, 50, true  ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::MediumNationCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_3_red.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::BigCity]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_2_white.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::BigCountyCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_2_yellow.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::BigStateCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_2_orange.png" ),
              QFont( defaultFamily, defaultSize, 50, true  ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::BigNationCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_2_red.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::LargeCity]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_1_white.png" ),
              QFont( defaultFamily, defaultSize, 75, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::LargeCountyCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_1_yellow.png" ),
              QFont( defaultFamily, defaultSize, 75, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::LargeStateCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_1_orange.png" ),
              QFont( defaultFamily, defaultSize, 75, true  ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::LargeNationCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/city_1_red.png" ),
              QFont( defaultFamily, defaultSize, 75, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::Nation]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, (int)(defaultSize * 1.5 ), 75, false ), QColor( "#404040" ) ));
    // Align area labels centered
    m_defaultStyle[GeoDataPlacemark::Nation]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    QFont osmCityFont = QFont(defaultFamily, (int)(defaultSize * 1.5 ), 75, false);
    m_defaultStyle[GeoDataPlacemark::PlaceCity] = createOsmPOIStyle(osmCityFont, "place/place-6", QColor( "#202020" ));
    m_defaultStyle[GeoDataPlacemark::PlaceCityCapital] = createOsmPOIStyle(osmCityFont, "place/place-capital-6", QColor( "#202020" ));
    m_defaultStyle[GeoDataPlacemark::PlaceSuburb] = createOsmPOIStyle(osmCityFont, QString(), QColor( "#707070" ));
    m_defaultStyle[GeoDataPlacemark::PlaceHamlet] = createOsmPOIStyle(osmCityFont, QString(), QColor( "#707070" ));
    QFont localityFont = osmCityFont;
    localityFont.setPointSize(defaultSize);
    m_defaultStyle[GeoDataPlacemark::PlaceLocality] = createOsmPOIStyle(localityFont, QString(), QColor( "#707070" ));
    m_defaultStyle[GeoDataPlacemark::PlaceTown] = createOsmPOIStyle(osmCityFont, "place/place-6", QColor( "#404040" ));
    m_defaultStyle[GeoDataPlacemark::PlaceTownCapital] = createOsmPOIStyle(osmCityFont, "place/place-capital-6", QColor( "#404040" ));
    m_defaultStyle[GeoDataPlacemark::PlaceVillage] = createOsmPOIStyle(osmCityFont, "place/place-6", QColor( "#505050" ));
    m_defaultStyle[GeoDataPlacemark::PlaceVillageCapital] = createOsmPOIStyle(osmCityFont, "place/place-capital-6", QColor( "#505050" ));
    for (int i=GeoDataPlacemark::PlaceCity; i<=GeoDataPlacemark::PlaceVillageCapital; ++i) {
        m_defaultStyle[GeoDataPlacemark::GeoDataVisualCategory(i)]->polyStyle().setFill(false);
        m_defaultStyle[GeoDataPlacemark::GeoDataVisualCategory(i)]->polyStyle().setOutline(false);
        m_defaultStyle[GeoDataPlacemark::GeoDataVisualCategory(i)]->labelStyle().setAlignment( GeoDataLabelStyle::Center );
        m_defaultStyle[GeoDataPlacemark::GeoDataVisualCategory(i)]->iconStyle().setScale(0.25);
    }

    m_defaultStyle[GeoDataPlacemark::Mountain]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/mountain_1.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::Volcano]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/volcano_1.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::Mons]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/mountain_1.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::Valley]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/valley.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::Continent]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, (int)(defaultSize * 1.7 ), 50, false ), QColor( "#bf0303" ) ));
    // Align area labels centered
    m_defaultStyle[GeoDataPlacemark::Continent]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    m_defaultStyle[GeoDataPlacemark::Ocean]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, (int)(defaultSize * 1.7 ), 50, true ), QColor( "#2c72c7" ) ));
    // Align area labels centered
    m_defaultStyle[GeoDataPlacemark::Ocean]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    m_defaultStyle[GeoDataPlacemark::OtherTerrain]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/other.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::Crater]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/crater.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.9 ), 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::Mare]
        = GeoDataStyle::Ptr(new GeoDataStyle( QString(),
              QFont( defaultFamily, (int)(defaultSize * 1.7 ), 50, false ), QColor( "#bf0303" ) ));
    // Align area labels centered
    m_defaultStyle[GeoDataPlacemark::Mare]->labelStyle().setAlignment( GeoDataLabelStyle::Center );

    m_defaultStyle[GeoDataPlacemark::GeographicPole]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/pole_1.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::MagneticPole]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/pole_2.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::ShipWreck]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/shipwreck.png" ),
              QFont( defaultFamily, (int)(defaultSize * 0.8 ), 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::AirPort]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/airport.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::Observatory]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/observatory.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::OsmSite]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/osm.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::Coordinate]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/coordinate.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));


    m_defaultStyle[GeoDataPlacemark::MannedLandingSite]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/manned_landing.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::RoboticRover]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/robotic_rover.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::UnmannedSoftLandingSite]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/unmanned_soft_landing.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::UnmannedHardLandingSite]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/unmanned_hard_landing.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    m_defaultStyle[GeoDataPlacemark::Bookmark]
        = createPOIStyle(QFont( defaultFamily, defaultSize, 50, false ),
                                                MarbleDirs::path("svg/bookmark.svg"), defaultLabelColor );
    m_defaultStyle[GeoDataPlacemark::Bookmark]->iconStyle().setScale(0.75);

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
    m_defaultStyle[GeoDataPlacemark::AccomodationCamping]      = createOsmPOIRingStyle(osmFont, "transportation/camping.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::AccomodationHostel]       = createOsmPOIStyle(osmFont, "transportation/hostel.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::AccomodationHotel]        = createOsmPOIStyle(osmFont, "transportation/hotel.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::AccomodationMotel]        = createOsmPOIStyle(osmFont, "transportation/motel.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::AccomodationYouthHostel]  = createOsmPOIStyle(osmFont, "transportation/hostel.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::AccomodationGuestHouse]   = createOsmPOIStyle(osmFont, "transportation/guest_house.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::AmenityLibrary]           = createOsmPOIStyle(osmFont, "amenity/library.20", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityKindergarten]      = createOsmPOIAreaStyle(osmFont, QString(), amenityColor, educationalAreasAndHospital, amenityColor);
    m_defaultStyle[GeoDataPlacemark::EducationCollege]         = createOsmPOIAreaStyle(osmFont, QString(), amenityColor, educationalAreasAndHospital, amenityColor);
    m_defaultStyle[GeoDataPlacemark::EducationSchool]          = createOsmPOIAreaStyle(osmFont, QString(), amenityColor, educationalAreasAndHospital, amenityColor);
    m_defaultStyle[GeoDataPlacemark::EducationUniversity]      = createOsmPOIAreaStyle(osmFont, QString(), amenityColor, educationalAreasAndHospital, amenityColor);
    m_defaultStyle[GeoDataPlacemark::FoodBar]                  = createOsmPOIStyle(osmFont, "amenity/bar.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::FoodBiergarten]           = createOsmPOIStyle(osmFont, "amenity/biergarten.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::FoodCafe]                 = createOsmPOIStyle(osmFont, "amenity/cafe.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::FoodFastFood]             = createOsmPOIStyle(osmFont, "amenity/fast_food.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::FoodPub]                  = createOsmPOIStyle(osmFont, "amenity/pub.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::FoodRestaurant]           = createOsmPOIStyle(osmFont, "amenity/restaurant.16", amenityColor);

    m_defaultStyle[GeoDataPlacemark::HealthDentist]            = createOsmPOIStyle(osmFont, "health/dentist.16", healthColor);
    m_defaultStyle[GeoDataPlacemark::HealthDoctors]            = createOsmPOIStyle(osmFont, "health/doctors.16", healthColor);
    m_defaultStyle[GeoDataPlacemark::HealthHospital]           = createOsmPOIStyle(osmFont, "health/hospital.16", healthColor, educationalAreasAndHospital, amenityColor);
    m_defaultStyle[GeoDataPlacemark::HealthPharmacy]           = createOsmPOIStyle(osmFont, "health/pharmacy.16", healthColor);
    m_defaultStyle[GeoDataPlacemark::HealthVeterinary]         = createOsmPOIStyle(osmFont, "health/veterinary-14", healthColor);

    m_defaultStyle[GeoDataPlacemark::MoneyAtm]                 = createOsmPOIStyle(osmFont, "amenity/atm.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::MoneyBank]                = createOsmPOIStyle(osmFont, "amenity/bank.16", amenityColor);

    m_defaultStyle[GeoDataPlacemark::AmenityArchaeologicalSite] = createOsmPOIAreaStyle(osmFont, "amenity/archaeological_site.16", amenityColor, Qt::transparent);
    m_defaultStyle[GeoDataPlacemark::AmenityEmbassy]           = createOsmPOIStyle(osmFont, "transportation/embassy.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::AmenityEmergencyPhone]    = createOsmPOIStyle(osmFont, "amenity/emergency_phone.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityWaterPark]         = createOsmPOIStyle(osmFont, "amenity/water_park.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityCommunityCentre]   = createOsmPOIStyle(osmFont, "amenity/community_centre-14", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityFountain]          = createOsmPOIStyle(osmFont, "amenity/fountain-14", QColor("#7989de"), waterColor, waterColor.darker(150));
    m_defaultStyle[GeoDataPlacemark::AmenityNightClub]         = createOsmPOIStyle(osmFont, "amenity/nightclub.18", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityBench]             = createOsmPOIStyle(osmFont, "individual/bench.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityCourtHouse]        = createOsmPOIStyle(osmFont, "amenity/courthouse-16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityFireStation]       = createOsmPOIStyle(osmFont, "amenity/firestation.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityHuntingStand]      = createOsmPOIStyle(osmFont, "manmade/hunting-stand.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityPolice]            = createOsmPOIStyle(osmFont, "amenity/police.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityPostBox]           = createOsmPOIStyle(osmFont, "amenity/post_box-12", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityPostOffice]        = createOsmPOIStyle(osmFont, "amenity/post_office-14", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityPrison]            = createOsmPOIStyle(osmFont, "amenity/prison.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityRecycling]         = createOsmPOIStyle(osmFont, "amenity/recycling.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityShelter]           = createOsmPOIStyle(osmFont, "amenity/shelter-14", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityTelephone]         = createOsmPOIStyle(osmFont, "amenity/telephone.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityToilets]           = createOsmPOIStyle(osmFont, "amenity/toilets.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityTownHall]          = createOsmPOIStyle(osmFont, "amenity/town_hall.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityWasteBasket]       = createOsmPOIStyle(osmFont, "individual/waste_basket.10", amenityColor);

    m_defaultStyle[GeoDataPlacemark::AmenityDrinkingWater]     = createOsmPOIStyle(osmFont, "amenity/drinking_water.16", amenityColor);

    m_defaultStyle[GeoDataPlacemark::NaturalPeak]              = createOsmPOIStyle(osmFont, "individual/peak", amenityColor);
    m_defaultStyle[GeoDataPlacemark::NaturalPeak]->iconStyle().setScale(0.33);
    m_defaultStyle[GeoDataPlacemark::NaturalTree]              = createOsmPOIStyle(osmFont, "individual/tree-29", amenityColor); // tree-16 provides the official icon
    m_styleTreeAutumn                                          = createOsmPOIStyle(osmFont, "individual/tree-29-autumn", amenityColor);
    m_styleTreeWinter                                          = createOsmPOIStyle(osmFont, "individual/tree-29-winter", amenityColor);

    m_defaultStyle[GeoDataPlacemark::ShopBeverages]            = createOsmPOIStyle(osmFont, "shop/beverages-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopHifi]                 = createOsmPOIStyle(osmFont, "shop/hifi-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopSupermarket]          = createOsmPOIStyle(osmFont, "shop/shop_supermarket", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopAlcohol]              = createOsmPOIStyle(osmFont, "shop/shop_alcohol.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopBakery]               = createOsmPOIStyle(osmFont, "shop/shop_bakery.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopButcher]              = createOsmPOIStyle(osmFont, "shop/butcher-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopConfectionery]        = createOsmPOIStyle(osmFont, "shop/confectionery.14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopConvenience]          = createOsmPOIStyle(osmFont, "shop/shop_convenience", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopGreengrocer]          = createOsmPOIStyle(osmFont, "shop/greengrocer-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopSeafood]              = createOsmPOIStyle(osmFont, "shop/seafood-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopDepartmentStore]      = createOsmPOIStyle(osmFont, "shop/department_store-16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopKiosk]                = createOsmPOIStyle(osmFont, "shop/kiosk-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopBag]                  = createOsmPOIStyle(osmFont, "shop/bag-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopClothes]              = createOsmPOIStyle(osmFont, "shop/shop_clothes.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopFashion]              = createOsmPOIStyle(osmFont, "shop/shop_clothes.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopJewelry]              = createOsmPOIStyle(osmFont, "shop/shop_jewelry.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopShoes]                = createOsmPOIStyle(osmFont, "shop/shop_shoes.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopVarietyStore]         = createOsmPOIStyle(osmFont, "shop/variety_store-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopBeauty]               = createOsmPOIStyle(osmFont, "shop/beauty-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopChemist]              = createOsmPOIStyle(osmFont, "shop/chemist-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopCosmetics]            = createOsmPOIStyle(osmFont, "shop/perfumery-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopHairdresser]          = createOsmPOIStyle(osmFont, "shop/shop_hairdresser.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopOptician]             = createOsmPOIStyle(osmFont, "shop/shop_optician.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopPerfumery]            = createOsmPOIStyle(osmFont, "shop/perfumery-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopDoitYourself]         = createOsmPOIStyle(osmFont, "shop/shop_diy.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopFlorist]              = createOsmPOIStyle(osmFont, "shop/florist.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopHardware]             = createOsmPOIStyle(osmFont, "shop/shop_diy.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopFurniture]            = createOsmPOIStyle(osmFont, "shop/shop_furniture.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopElectronics]          = createOsmPOIStyle(osmFont, "shop/shop_electronics.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopMobilePhone]          = createOsmPOIStyle(osmFont, "shop/shop_mobile_phone.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopBicycle]              = createOsmPOIStyle(osmFont, "shop/shop_bicycle.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopCar]                  = createOsmPOIStyle(osmFont, "shop/shop_car", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopCarRepair]            = createOsmPOIStyle(osmFont, "shop/shopping_car_repair.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopCarParts]             = createOsmPOIStyle(osmFont, "shop/car_parts-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopMotorcycle]           = createOsmPOIStyle(osmFont, "shop/motorcycle-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopOutdoor]              = createOsmPOIStyle(osmFont, "shop/outdoor-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopMusicalInstrument]    = createOsmPOIStyle(osmFont, "shop/musical_instrument-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopPhoto]                = createOsmPOIStyle(osmFont, "shop/photo-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopBook]                 = createOsmPOIStyle(osmFont, "shop/shop_books.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopGift]                 = createOsmPOIStyle(osmFont, "shop/shop_gift.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopStationery]           = createOsmPOIStyle(osmFont, "shop/stationery-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopLaundry]              = createOsmPOIStyle(osmFont, "shop/laundry-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopPet]                  = createOsmPOIStyle(osmFont, "shop/shop_pet.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopToys]                 = createOsmPOIStyle(osmFont, "shop/toys-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopTravelAgency]         = createOsmPOIStyle(osmFont, "shop/travel_agency-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::Shop]                     = createOsmPOIStyle(osmFont, "shop/shop-14", shopColor);

    m_defaultStyle[GeoDataPlacemark::ManmadeBridge]            = createWayStyle(QColor("#b8b8b8"), Qt::transparent, true, true);
    m_defaultStyle[GeoDataPlacemark::ManmadeLighthouse]        = createOsmPOIStyle(osmFont, "transportation/lighthouse.16", transportationColor, "#f2efe9", QColor( "#f2efe9" ).darker());
    m_defaultStyle[GeoDataPlacemark::ManmadePier]              = createStyle(0.0, 3.0, "#f2efe9", "#f2efe9", true, false, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont);
    m_defaultStyle[GeoDataPlacemark::ManmadeWaterTower]        = createOsmPOIStyle(osmFont, "amenity/water_tower.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::ManmadeWindMill]          = createOsmPOIStyle(osmFont, "amenity/windmill.16", amenityColor);

    m_defaultStyle[GeoDataPlacemark::TouristAttraction]        = createOsmPOIStyle(osmFont, "amenity/tourist_memorial.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::TouristCastle]            = createOsmPOIRingStyle(osmFont, "amenity/cinema.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::TouristCinema]            = createOsmPOIStyle(osmFont, "amenity/cinema.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::TouristInformation]       = createOsmPOIStyle(osmFont, "amenity/information.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::TouristMonument]          = createOsmPOIStyle(osmFont, "amenity/monument.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::TouristMuseum]            = createOsmPOIStyle(osmFont, "amenity/museum.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::TouristRuin]              = createOsmPOIRingStyle(osmFont, QString(), amenityColor);
    m_defaultStyle[GeoDataPlacemark::TouristTheatre]           = createOsmPOIStyle(osmFont, "amenity/theatre.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::TouristThemePark]         = createOsmPOIStyle(osmFont, QString(), amenityColor);
    m_defaultStyle[GeoDataPlacemark::TouristViewPoint]         = createOsmPOIStyle(osmFont, "amenity/viewpoint.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::TouristZoo]               = createOsmPOIRingStyle(osmFont, QString(), amenityColor, Qt::transparent);
    m_defaultStyle[GeoDataPlacemark::TouristAlpineHut]         = createOsmPOIStyle(osmFont, "transportation/alpinehut.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportAerodrome]       = createOsmPOIStyle(osmFont, "airtransport/aerodrome", airTransportColor);
    m_defaultStyle[GeoDataPlacemark::TransportHelipad]         = createOsmPOIStyle(osmFont, "airtransport/helipad", airTransportColor);
    m_defaultStyle[GeoDataPlacemark::TransportAirportTerminal] = createOsmPOIAreaStyle(osmFont, QString(), airTransportColor);
    m_defaultStyle[GeoDataPlacemark::TransportAirportGate]     = createOsmPOIAreaStyle(osmFont, QString(), airTransportColor);
    m_defaultStyle[GeoDataPlacemark::TransportBusStation]      = createOsmPOIStyle(osmFont, "transportation/bus_station.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportBusStop]         = createOsmPOIStyle(osmFont, "transportation/bus_stop.12", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportCarShare]        = createOsmPOIStyle(osmFont, "transportation/car_share.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportFuel]            = createOsmPOIStyle(osmFont, "transportation/fuel.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportParking]         = createOsmPOIAreaStyle(osmFont, "transportation/parking", transportationColor, "#F6EEB6", QColor( "#F6EEB6" ).darker());
    m_defaultStyle[GeoDataPlacemark::TransportParkingSpace]    = createWayStyle("#F6EEB6", QColor( "#F6EEB6" ).darker(), true, true);
    m_defaultStyle[GeoDataPlacemark::TransportPlatform]        = createWayStyle("#bbbbbb", Qt::transparent, true, false);
    m_defaultStyle[GeoDataPlacemark::TransportTrainStation]    = createOsmPOIStyle(osmFont, "individual/railway_station", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportTrainStation]->iconStyle().setScale(0.5);
    m_defaultStyle[GeoDataPlacemark::TransportTramStop]        = createOsmPOIStyle(osmFont, QString(), transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportRentalBicycle]   = createOsmPOIStyle(osmFont, "transportation/rental_bicycle.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportRentalCar]       = createOsmPOIStyle(osmFont, "transportation/rental_car.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportTaxiRank]        = createOsmPOIStyle(osmFont, "transportation/taxi.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportBicycleParking]  = createOsmPOIAreaStyle(osmFont, "transportation/bicycle_parking.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportMotorcycleParking] = createOsmPOIAreaStyle(osmFont, "transportation/motorcycle_parking.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportSubwayEntrance]  = createOsmPOIStyle(osmFont, "transportation/subway_entrance", transportationColor);
    m_defaultStyle[GeoDataPlacemark::ReligionPlaceOfWorship]   = createOsmPOIStyle(osmFont, QString() /* "black/place_of_worship.16" */);
    m_defaultStyle[GeoDataPlacemark::ReligionBahai]            = createOsmPOIStyle(osmFont, QString());
    m_defaultStyle[GeoDataPlacemark::ReligionBuddhist]         = createOsmPOIStyle(osmFont, "black/buddhist.16");
    m_defaultStyle[GeoDataPlacemark::ReligionChristian]        = createOsmPOIStyle(osmFont, "black/christian.16");
    m_defaultStyle[GeoDataPlacemark::ReligionMuslim]           = createOsmPOIStyle(osmFont, "black/muslim.16");
    m_defaultStyle[GeoDataPlacemark::ReligionHindu]            = createOsmPOIStyle(osmFont, "black/hinduist.16");
    m_defaultStyle[GeoDataPlacemark::ReligionJain]             = createOsmPOIStyle(osmFont, QString());
    m_defaultStyle[GeoDataPlacemark::ReligionJewish]           = createOsmPOIStyle(osmFont, "black/jewish.16");
    m_defaultStyle[GeoDataPlacemark::ReligionShinto]           = createOsmPOIStyle(osmFont, "black/shintoist.16");
    m_defaultStyle[GeoDataPlacemark::ReligionSikh]             = createOsmPOIStyle(osmFont, "black/sikhist.16");

    m_defaultStyle[GeoDataPlacemark::HighwayTrafficSignals]    = createOsmPOIStyle(osmFont, "individual/traffic_light");
    m_defaultStyle[GeoDataPlacemark::HighwayTrafficSignals]->iconStyle().setScale(0.75);

    m_defaultStyle[GeoDataPlacemark::PowerTower]               = createOsmPOIStyle(osmFont, "individual/power_tower", QColor( "#888888" ));
    m_defaultStyle[GeoDataPlacemark::PowerTower]->iconStyle().setScale(0.6);

    m_defaultStyle[GeoDataPlacemark::BarrierCityWall]          = createStyle(6.0, 3.0, "#787878", Qt::transparent, true, false, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, Qt::transparent);
    m_defaultStyle[GeoDataPlacemark::BarrierGate]              = createOsmPOIStyle(osmFont, "individual/gate");
    m_defaultStyle[GeoDataPlacemark::BarrierLiftGate]          = createOsmPOIStyle(osmFont, "individual/liftgate");
    m_defaultStyle[GeoDataPlacemark::BarrierWall]              = createStyle(2.0, 0.0, "#787878", Qt::transparent, true, false, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, Qt::transparent);

    m_defaultStyle[GeoDataPlacemark::HighwaySteps]             = createStyle(0.0, 2.0, "#fa8072", QColor(Qt::white), true, true, Qt::SolidPattern, Qt::CustomDashLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataPlacemark::HighwayUnknown]           = createHighwayStyle("#dddddd", "#bbbbbb", osmFont, "000000", 0, 1);
    m_defaultStyle[GeoDataPlacemark::HighwayPath]              = createHighwayStyle("#dddde8", "#999999", osmFont, "000000", 0, 1, Qt::DotLine, Qt::SquareCap, true);
    m_defaultStyle[GeoDataPlacemark::HighwayTrack]             = createHighwayStyle("#996600", QColor(Qt::white), osmFont, QColor(Qt::transparent), 1, 1, Qt::DashLine, Qt::SquareCap, true);
    m_defaultStyle[GeoDataPlacemark::HighwayPedestrian]        = createHighwayStyle("#dddde8", "#999999", osmFont, "000000", 0, 2);
    m_defaultStyle[GeoDataPlacemark::HighwayPedestrian]->polyStyle().setOutline(false);
    m_defaultStyle[GeoDataPlacemark::HighwayFootway]           = createHighwayStyle("#fa8072", QColor(Qt::white), osmFont, "000000", 0, 2, Qt::DotLine, Qt::SquareCap, true);
    m_defaultStyle[GeoDataPlacemark::HighwayCycleway]          = createHighwayStyle(QColor(Qt::blue), QColor(Qt::white), osmFont, "000000", 0, 2, Qt::DotLine, Qt::SquareCap, true);
    m_defaultStyle[GeoDataPlacemark::HighwayService]           = createHighwayStyle("#ffffff", "#bbbbbb", osmFont, "000000", 1, 6);
    m_defaultStyle[GeoDataPlacemark::HighwayResidential]       = createHighwayStyle("#ffffff", "#bbbbbb", osmFont, "000000", 3, 6);
    m_defaultStyle[GeoDataPlacemark::HighwayLivingStreet]      = createHighwayStyle("#ffffff", "#bbbbbb", osmFont, "000000", 3, 6);
    m_defaultStyle[GeoDataPlacemark::HighwayRoad]              = createHighwayStyle("#dddddd", "#bbbbbb", osmFont, "000000", 3, 6);
    m_defaultStyle[GeoDataPlacemark::HighwayUnclassified]      = createHighwayStyle("#ffffff", "#bbbbbb", osmFont, "000000", 3, 6);
    m_defaultStyle[GeoDataPlacemark::HighwayTertiary]          = createHighwayStyle("#ffffff", "#8f8f8f", osmFont, "000000", 6, 6);
    m_defaultStyle[GeoDataPlacemark::HighwayTertiaryLink]      = createHighwayStyle("#ffffff", "#8f8f8f", osmFont, "000000", 6, 6);
    m_defaultStyle[GeoDataPlacemark::HighwaySecondary]         = createHighwayStyle("#f7fabf", "#707d05", osmFont, "000000", 6, 6);
    m_defaultStyle[GeoDataPlacemark::HighwaySecondaryLink]     = createHighwayStyle("#f7fabf", "#707d05", osmFont, "000000", 6, 6);
    m_defaultStyle[GeoDataPlacemark::HighwayPrimary]           = createHighwayStyle("#fcd6a4", "#a06b00", osmFont, "000000", 9, 6);
    m_defaultStyle[GeoDataPlacemark::HighwayPrimaryLink]       = createHighwayStyle("#fcd6a4", "#a06b00", osmFont, "000000", 6, 6);
    m_defaultStyle[GeoDataPlacemark::HighwayTrunk]             = createHighwayStyle("#f9b29c", "#c84e2f", osmFont, "000000", 9, 6);
    m_defaultStyle[GeoDataPlacemark::HighwayTrunkLink]         = createHighwayStyle("#f9b29c", "#c84e2f", osmFont, "000000", 9, 6);
    m_defaultStyle[GeoDataPlacemark::HighwayMotorway]          = createHighwayStyle("#e892a2", "#dc2a67", osmFont, "000000", 9, 10);
    m_defaultStyle[GeoDataPlacemark::HighwayMotorwayLink]      = createHighwayStyle("#e892a2", "#dc2a67", osmFont, "000000", 9, 10);
    m_defaultStyle[GeoDataPlacemark::TransportAirportRunway]   = createHighwayStyle("#bbbbcc", "#bbbbcc", osmFont, "000000", 0, 1, Qt::NoPen);
    m_defaultStyle[GeoDataPlacemark::TransportAirportTaxiway]  = createHighwayStyle("#bbbbcc", "#bbbbcc", osmFont, "000000", 0, 1, Qt::NoPen);
    m_defaultStyle[GeoDataPlacemark::TransportAirportApron]    = createWayStyle("#e9d1ff", Qt::transparent, true, false);

    m_defaultStyle[GeoDataPlacemark::NaturalWater]             = createStyle(4, 0, waterColor, waterColor, true, false,
                                                                                                   Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false, QVector< qreal >(),
                                                                                                   osmFont, waterColor.darker(150));

    m_defaultStyle[GeoDataPlacemark::NaturalReef]              = createStyle(5.5, 0, "#36677c", "#36677c", true, false,
                                                                                                   Qt::Dense7Pattern, Qt::DotLine, Qt::RoundCap, false, QVector< qreal >(),
                                                                                                   osmFont, waterColor.darker(150));

    m_defaultStyle[GeoDataPlacemark::AmenityGraveyard]         = createWayStyle("#AACBAF", "#AACBAF", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_generic.png"));

    m_defaultStyle[GeoDataPlacemark::NaturalWood]              = createWayStyle("#8DC46C", "#8DC46C", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/forest.png"));
    m_defaultStyle[GeoDataPlacemark::NaturalBeach]             = createWayStyle("#FFF1BA", "#FFF1BA", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/beach.png"));
    m_defaultStyle[GeoDataPlacemark::NaturalWetland]           = createWayStyle("#DDECEC", "#DDECEC", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/wetland.png"));
    m_defaultStyle[GeoDataPlacemark::NaturalGlacier]           = createWayStyle("#DDECEC", "#DDECEC", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/glacier.png"));
    m_defaultStyle[GeoDataPlacemark::NaturalIceShelf]          = createWayStyle("#8ebebe", "#8ebebe", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/glacier.png"));
    m_defaultStyle[GeoDataPlacemark::NaturalScrub]             = createWayStyle("#B5E3B5", "#B5E3B5", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/scrub.png"));
    m_defaultStyle[GeoDataPlacemark::NaturalCliff]             = createWayStyle(Qt::transparent, Qt::transparent, true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/cliff2.png"));
    m_defaultStyle[GeoDataPlacemark::NaturalHeath]             = createWayStyle("#d6d99f", QColor("#d6d99f").darker(150), true, false);

    m_defaultStyle[GeoDataPlacemark::LeisureGolfCourse]        = createWayStyle(QColor("#b5e3b5"), QColor("#b5e3b5").darker(150), true, true);
    m_defaultStyle[GeoDataPlacemark::LeisureMarina]            = createOsmPOIStyle(osmFont, QString(), QColor("#95abd5"), QColor("#aec8d1"), QColor("#95abd5").darker(150));
    m_defaultStyle[GeoDataPlacemark::LeisurePark]              = createWayStyle(QColor("#c8facc"), QColor("#c8facc").darker(150), true, true);
    m_defaultStyle[GeoDataPlacemark::LeisurePlayground]        = createOsmPOIAreaStyle(osmFont, "amenity/playground.16", amenityColor, "#CCFFF1", "#BDFFED");
    m_defaultStyle[GeoDataPlacemark::LeisurePitch]             = createWayStyle("#8ad3af", QColor("#8ad3af").darker(150), true, true);
    m_defaultStyle[GeoDataPlacemark::LeisureSportsCentre]      = createWayStyle("#33cc99", QColor("#33cc99").darker(150), true, true);
    m_defaultStyle[GeoDataPlacemark::LeisureStadium]           = createWayStyle("#33cc99", QColor("#33cc99").darker(150), true, true);
    m_defaultStyle[GeoDataPlacemark::LeisureTrack]             = createWayStyle("#74dcba", QColor("#74dcba").darker(150), true, true);
    m_defaultStyle[GeoDataPlacemark::LeisureSwimmingPool]      = createWayStyle(waterColor, waterColor.darker(150), true, true);

    m_defaultStyle[GeoDataPlacemark::LanduseAllotments]        = createWayStyle("#E4C6AA", "#E4C6AA", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/allotments.png"));
    m_defaultStyle[GeoDataPlacemark::LanduseBasin]             = createWayStyle(QColor(0xB5, 0xD0, 0xD0, 0x80 ), QColor( 0xB5, 0xD0, 0xD0 ));
    m_defaultStyle[GeoDataPlacemark::LanduseCemetery]          = createWayStyle("#AACBAF", "#AACBAF", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/grave_yard_generic.png"));
    m_defaultStyle[GeoDataPlacemark::LanduseCommercial]        = createWayStyle("#F2DAD9", "#D1B2B0", true, true);
    m_defaultStyle[GeoDataPlacemark::LanduseConstruction]      = createWayStyle("#b6b592", "#b6b592", true, false);
    m_defaultStyle[GeoDataPlacemark::LanduseFarmland]          = createWayStyle("#EDDDC9", "#C8B69E", true, true);
    m_defaultStyle[GeoDataPlacemark::LanduseFarmyard]          = createWayStyle("#EFD6B5", "#D1B48C", true, true);
    m_defaultStyle[GeoDataPlacemark::LanduseGarages]           = createWayStyle("#E0DDCD", "#E0DDCD", true, false);
    m_defaultStyle[GeoDataPlacemark::LanduseGrass]             = createWayStyle("#A8C8A5", "#A8C8A5", true, false);
    m_defaultStyle[GeoDataPlacemark::LanduseIndustrial]        = createWayStyle("#DED0D5", "#DED0D5", true, false);
    m_defaultStyle[GeoDataPlacemark::LanduseLandfill]          = createWayStyle("#b6b592", "#b6b592", true, false);
    m_defaultStyle[GeoDataPlacemark::LanduseMeadow]            = createWayStyle("#cdebb0", "#cdebb0", true, false);
    m_defaultStyle[GeoDataPlacemark::LanduseMilitary]          = createWayStyle("#F3D8D2", "#F3D8D2", true, true, Qt::BDiagPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/military_red_hatch.png"));
    m_defaultStyle[GeoDataPlacemark::LanduseQuarry]            = createWayStyle("#C4C2C2", "#C4C2C2", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/quarry.png"));
    m_defaultStyle[GeoDataPlacemark::LanduseRailway]           = createWayStyle("#DED0D5", "#DED0D5", true, false);
    m_defaultStyle[GeoDataPlacemark::LanduseReservoir]         = createWayStyle(waterColor, waterColor, true, false);
    m_defaultStyle[GeoDataPlacemark::LanduseResidential]       = createWayStyle("#DCDCDC", "#DCDCDC", true, false);
    m_defaultStyle[GeoDataPlacemark::LanduseRetail]            = createWayStyle("#FFD6D1", "#D99C95", true, true);
    m_defaultStyle[GeoDataPlacemark::LanduseOrchard]           = createWayStyle("#AEDFA3", "#AEDFA3", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/orchard.png"));
    m_defaultStyle[GeoDataPlacemark::LanduseVineyard]          = createWayStyle("#AEDFA3", "#AEDFA3", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/vineyard.png"));

    m_defaultStyle[GeoDataPlacemark::MilitaryDangerArea]       = createWayStyle("#FFC0CB", "#FFC0CB", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/danger.png"));

    m_defaultStyle[GeoDataPlacemark::RailwayRail]              = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", true, true, Qt::SolidPattern, Qt::CustomDashLine, Qt::FlatCap, true, QVector< qreal >() << 2 << 3, osmFont, QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::RailwayNarrowGauge]       = createStyle(2.0, 1.0, "#706E70", "#EEEEEE", true, true, Qt::SolidPattern, Qt::CustomDashLine, Qt::FlatCap, true, QVector< qreal >() << 2 << 3, osmFont, QColor(Qt::transparent));
    // FIXME: the tram is currently being rendered as a polygon.
    m_defaultStyle[GeoDataPlacemark::RailwayTram]              = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::RailwayLightRail]         = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::RailwayAbandoned]         = createStyle(2.0, 1.435, Qt::transparent, "#706E70", false, false, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::RailwaySubway]            = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::RailwayPreserved]         = createStyle(2.0, 1.435, "#EEEEEE", "#706E70", true, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, true, QVector<qreal>(), osmFont, QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::RailwayMiniature]         = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::RailwayConstruction]      = createStyle(2.0, 1.435, "#EEEEEE", "#706E70", true, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, true, QVector<qreal>(), osmFont, QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::RailwayMonorail]          = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::RailwayFunicular]         = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, QColor(Qt::transparent));

    m_defaultStyle[GeoDataPlacemark::Building]                 = createStyle(1, 0, buildingColor, buildingColor.darker(),
                                                                                   true, true, Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false, QVector<qreal>(), osmFont);

    m_defaultStyle[GeoDataPlacemark::Landmass]                 = createWayStyle("#F1EEE8", "#F1EEE8", true, false);
    m_defaultStyle[GeoDataPlacemark::UrbanArea]                = createWayStyle("#E6E3DD", "#E6E3DD", true, false);
    m_defaultStyle[GeoDataPlacemark::InternationalDateLine]    = createStyle(1.0, 0.0, "#000000", "#000000", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont);
    m_defaultStyle[GeoDataPlacemark::Bathymetry]               = createWayStyle("#a5c9c9", "#a5c9c9", true, false);

    m_defaultStyle[GeoDataPlacemark::AdminLevel1]              = createStyle(0.0, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::CustomDashLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataPlacemark::AdminLevel2]              = createStyle(2.0, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataPlacemark::AdminLevel3]              = createStyle(1.8, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DashLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataPlacemark::AdminLevel4]              = createStyle(1.5, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataPlacemark::AdminLevel5]              = createStyle(1.25, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DashDotDotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataPlacemark::AdminLevel6]              = createStyle(1, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DashDotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataPlacemark::AdminLevel7]              = createStyle(1, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DashLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataPlacemark::AdminLevel8]              = createStyle(1, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DashLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataPlacemark::AdminLevel9]              = createStyle(1.5, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataPlacemark::AdminLevel10]             = createStyle(1.5, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);
    m_defaultStyle[GeoDataPlacemark::AdminLevel11]             = createStyle(1.5, 0.0, "#DF9CCF", "#DF9CCF", false, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false, QVector< qreal >() << 0.3 << 0.3, osmFont);

    m_defaultStyle[GeoDataPlacemark::BoundaryMaritime]         = createStyle(2.0, 0.0, "#88b3bf", "#88b3bf", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont);

    m_defaultStyle[GeoDataPlacemark::Satellite]
        = GeoDataStyle::Ptr(new GeoDataStyle( MarbleDirs::path( "bitmaps/satellite.png" ),
              QFont( defaultFamily, defaultSize, 50, false ), defaultLabelColor ));

    QFont tmp;

    // Fonts for areas ...
    tmp = m_defaultStyle[GeoDataPlacemark::Continent]->labelStyle().font();
    tmp.setLetterSpacing( QFont::AbsoluteSpacing, 2 );
    tmp.setCapitalization( QFont::AllUppercase );
    tmp.setBold( true );
    m_defaultStyle[GeoDataPlacemark::Continent]->labelStyle().setFont( tmp );

    // Fonts for areas ...
    tmp = m_defaultStyle[GeoDataPlacemark::Mare]->labelStyle().font();
    tmp.setLetterSpacing( QFont::AbsoluteSpacing, 2 );
    tmp.setCapitalization( QFont::AllUppercase );
    tmp.setBold( true );
    m_defaultStyle[GeoDataPlacemark::Mare]->labelStyle().setFont( tmp );

    // Now we need to underline the capitals ...

    tmp = m_defaultStyle[GeoDataPlacemark::SmallNationCapital]->labelStyle().font();
    tmp.setUnderline( true );
    m_defaultStyle[GeoDataPlacemark::SmallNationCapital]->labelStyle().setFont( tmp );

    tmp = m_defaultStyle[GeoDataPlacemark::MediumNationCapital]->labelStyle().font();
    tmp.setUnderline( true );
    m_defaultStyle[GeoDataPlacemark::MediumNationCapital]->labelStyle().setFont( tmp );

    tmp = m_defaultStyle[GeoDataPlacemark::BigNationCapital]->labelStyle().font();
    tmp.setUnderline( true );
    m_defaultStyle[GeoDataPlacemark::BigNationCapital]->labelStyle().setFont( tmp );

    tmp = m_defaultStyle[GeoDataPlacemark::LargeNationCapital]->labelStyle().font();
    tmp.setUnderline( true );
    m_defaultStyle[GeoDataPlacemark::LargeNationCapital]->labelStyle().setFont( tmp );
}

QString StyleBuilder::Private::createPaintLayerItem(const QString &itemType, GeoDataPlacemark::GeoDataVisualCategory visualCategory, const QString &subType)
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

    s_visualCategories[OsmTag("admin_level", "1")]              = GeoDataPlacemark::AdminLevel1;
    s_visualCategories[OsmTag("admin_level", "2")]              = GeoDataPlacemark::AdminLevel2;
    s_visualCategories[OsmTag("admin_level", "3")]              = GeoDataPlacemark::AdminLevel3;
    s_visualCategories[OsmTag("admin_level", "4")]              = GeoDataPlacemark::AdminLevel4;
    s_visualCategories[OsmTag("admin_level", "5")]              = GeoDataPlacemark::AdminLevel5;
    s_visualCategories[OsmTag("admin_level", "6")]              = GeoDataPlacemark::AdminLevel6;
    s_visualCategories[OsmTag("admin_level", "7")]              = GeoDataPlacemark::AdminLevel7;
    s_visualCategories[OsmTag("admin_level", "8")]              = GeoDataPlacemark::AdminLevel8;
    s_visualCategories[OsmTag("admin_level", "9")]              = GeoDataPlacemark::AdminLevel9;
    s_visualCategories[OsmTag("admin_level", "10")]             = GeoDataPlacemark::AdminLevel10;
    s_visualCategories[OsmTag("admin_level", "11")]             = GeoDataPlacemark::AdminLevel11;

    s_visualCategories[OsmTag("boundary", "maritime")]          = GeoDataPlacemark::BoundaryMaritime;

    s_visualCategories[OsmTag("amenity", "restaurant")]         = GeoDataPlacemark::FoodRestaurant;
    s_visualCategories[OsmTag("amenity", "fast_food")]          = GeoDataPlacemark::FoodFastFood;
    s_visualCategories[OsmTag("amenity", "pub")]                = GeoDataPlacemark::FoodPub;
    s_visualCategories[OsmTag("amenity", "bar")]                = GeoDataPlacemark::FoodBar;
    s_visualCategories[OsmTag("amenity", "cafe")]               = GeoDataPlacemark::FoodCafe;
    s_visualCategories[OsmTag("amenity", "biergarten")]         = GeoDataPlacemark::FoodBiergarten;
    s_visualCategories[OsmTag("amenity", "school")]             = GeoDataPlacemark::EducationSchool;
    s_visualCategories[OsmTag("amenity", "college")]            = GeoDataPlacemark::EducationCollege;
    s_visualCategories[OsmTag("amenity", "library")]            = GeoDataPlacemark::AmenityLibrary;
    s_visualCategories[OsmTag("amenity", "kindergarten")]       = GeoDataPlacemark::AmenityKindergarten;
    s_visualCategories[OsmTag("amenity", "childcare")]          = GeoDataPlacemark::AmenityKindergarten;
    s_visualCategories[OsmTag("amenity", "university")]         = GeoDataPlacemark::EducationUniversity;
    s_visualCategories[OsmTag("amenity", "bus_station")]        = GeoDataPlacemark::TransportBusStation;
    s_visualCategories[OsmTag("amenity", "car_sharing")]        = GeoDataPlacemark::TransportCarShare;
    s_visualCategories[OsmTag("amenity", "fuel")]               = GeoDataPlacemark::TransportFuel;
    s_visualCategories[OsmTag("amenity", "parking")]            = GeoDataPlacemark::TransportParking;
    s_visualCategories[OsmTag("amenity", "parking_space")]      = GeoDataPlacemark::TransportParkingSpace;
    s_visualCategories[OsmTag("amenity", "atm")]                = GeoDataPlacemark::MoneyAtm;
    s_visualCategories[OsmTag("amenity", "bank")]               = GeoDataPlacemark::MoneyBank;

    s_visualCategories[OsmTag("historic", "archaeological_site")] = GeoDataPlacemark::AmenityArchaeologicalSite;
    s_visualCategories[OsmTag("amenity", "embassy")]            = GeoDataPlacemark::AmenityEmbassy;
    s_visualCategories[OsmTag("emergency", "phone")]            = GeoDataPlacemark::AmenityEmergencyPhone;
    s_visualCategories[OsmTag("leisure", "water_park")]         = GeoDataPlacemark::AmenityWaterPark;
    s_visualCategories[OsmTag("amenity", "community_centre")]   = GeoDataPlacemark::AmenityCommunityCentre;
    s_visualCategories[OsmTag("amenity", "fountain")]           = GeoDataPlacemark::AmenityFountain;
    s_visualCategories[OsmTag("amenity", "nightclub")]          = GeoDataPlacemark::AmenityNightClub;
    s_visualCategories[OsmTag("amenity", "bench")]              = GeoDataPlacemark::AmenityBench;
    s_visualCategories[OsmTag("amenity", "courthouse")]         = GeoDataPlacemark::AmenityCourtHouse;
    s_visualCategories[OsmTag("amenity", "fire_station")]       = GeoDataPlacemark::AmenityFireStation;
    s_visualCategories[OsmTag("amenity", "hunting_stand")]      = GeoDataPlacemark::AmenityHuntingStand;
    s_visualCategories[OsmTag("amenity", "police")]             = GeoDataPlacemark::AmenityPolice;
    s_visualCategories[OsmTag("amenity", "post_box")]           = GeoDataPlacemark::AmenityPostBox;
    s_visualCategories[OsmTag("amenity", "post_office")]        = GeoDataPlacemark::AmenityPostOffice;
    s_visualCategories[OsmTag("amenity", "prison")]             = GeoDataPlacemark::AmenityPrison;
    s_visualCategories[OsmTag("amenity", "recycling")]          = GeoDataPlacemark::AmenityRecycling;
    s_visualCategories[OsmTag("amenity", "shelter")]            = GeoDataPlacemark::AmenityShelter;
    s_visualCategories[OsmTag("amenity", "telephone")]          = GeoDataPlacemark::AmenityTelephone;
    s_visualCategories[OsmTag("amenity", "toilets")]            = GeoDataPlacemark::AmenityToilets;
    s_visualCategories[OsmTag("amenity", "townhall")]           = GeoDataPlacemark::AmenityTownHall;
    s_visualCategories[OsmTag("amenity", "waste_basket")]       = GeoDataPlacemark::AmenityWasteBasket;
    s_visualCategories[OsmTag("amenity", "drinking_water")]     = GeoDataPlacemark::AmenityDrinkingWater;
    s_visualCategories[OsmTag("amenity", "graveyard")]          = GeoDataPlacemark::AmenityGraveyard;

    s_visualCategories[OsmTag("amenity", "dentist")]            = GeoDataPlacemark::HealthDentist;
    s_visualCategories[OsmTag("amenity", "pharmacy")]           = GeoDataPlacemark::HealthPharmacy;
    s_visualCategories[OsmTag("amenity", "hospital")]           = GeoDataPlacemark::HealthHospital;
    s_visualCategories[OsmTag("amenity", "doctors")]            = GeoDataPlacemark::HealthDoctors;
    s_visualCategories[OsmTag("amenity", "veterinary")]         = GeoDataPlacemark::HealthVeterinary;

    s_visualCategories[OsmTag("amenity", "cinema")]             = GeoDataPlacemark::TouristCinema;
    s_visualCategories[OsmTag("tourism", "information")]        = GeoDataPlacemark::TouristInformation;
    s_visualCategories[OsmTag("amenity", "theatre")]            = GeoDataPlacemark::TouristTheatre;
    s_visualCategories[OsmTag("amenity", "place_of_worship")]   = GeoDataPlacemark::ReligionPlaceOfWorship;

    s_visualCategories[OsmTag("natural", "peak")]               = GeoDataPlacemark::NaturalPeak;
    s_visualCategories[OsmTag("natural", "tree")]               = GeoDataPlacemark::NaturalTree;

    s_visualCategories[OsmTag("shop", "beverages")]             = GeoDataPlacemark::ShopBeverages;
    s_visualCategories[OsmTag("shop", "hifi")]                  = GeoDataPlacemark::ShopHifi;
    s_visualCategories[OsmTag("shop", "supermarket")]           = GeoDataPlacemark::ShopSupermarket;
    s_visualCategories[OsmTag("shop", "alcohol")]               = GeoDataPlacemark::ShopAlcohol;
    s_visualCategories[OsmTag("shop", "bakery")]                = GeoDataPlacemark::ShopBakery;
    s_visualCategories[OsmTag("shop", "butcher")]               = GeoDataPlacemark::ShopButcher;
    s_visualCategories[OsmTag("shop", "confectionery")]         = GeoDataPlacemark::ShopConfectionery;
    s_visualCategories[OsmTag("shop", "convenience")]           = GeoDataPlacemark::ShopConvenience;
    s_visualCategories[OsmTag("shop", "greengrocer")]           = GeoDataPlacemark::ShopGreengrocer;
    s_visualCategories[OsmTag("shop", "seafood")]               = GeoDataPlacemark::ShopSeafood;
    s_visualCategories[OsmTag("shop", "department_store")]      = GeoDataPlacemark::ShopDepartmentStore;
    s_visualCategories[OsmTag("shop", "kiosk")]                 = GeoDataPlacemark::ShopKiosk;
    s_visualCategories[OsmTag("shop", "bag")]                   = GeoDataPlacemark::ShopBag;
    s_visualCategories[OsmTag("shop", "clothes")]               = GeoDataPlacemark::ShopClothes;
    s_visualCategories[OsmTag("shop", "fashion")]               = GeoDataPlacemark::ShopFashion;
    s_visualCategories[OsmTag("shop", "jewelry")]               = GeoDataPlacemark::ShopJewelry;
    s_visualCategories[OsmTag("shop", "shoes")]                 = GeoDataPlacemark::ShopShoes;
    s_visualCategories[OsmTag("shop", "variety_store")]         = GeoDataPlacemark::ShopVarietyStore;
    s_visualCategories[OsmTag("shop", "beauty")]                = GeoDataPlacemark::ShopBeauty;
    s_visualCategories[OsmTag("shop", "chemist")]               = GeoDataPlacemark::ShopChemist;
    s_visualCategories[OsmTag("shop", "cosmetics")]             = GeoDataPlacemark::ShopCosmetics;
    s_visualCategories[OsmTag("shop", "hairdresser")]           = GeoDataPlacemark::ShopHairdresser;
    s_visualCategories[OsmTag("shop", "optician")]              = GeoDataPlacemark::ShopOptician;
    s_visualCategories[OsmTag("shop", "perfumery")]             = GeoDataPlacemark::ShopPerfumery;
    s_visualCategories[OsmTag("shop", "doityourself")]          = GeoDataPlacemark::ShopDoitYourself;
    s_visualCategories[OsmTag("shop", "florist")]               = GeoDataPlacemark::ShopFlorist;
    s_visualCategories[OsmTag("shop", "hardware")]              = GeoDataPlacemark::ShopHardware;
    s_visualCategories[OsmTag("shop", "furniture")]             = GeoDataPlacemark::ShopFurniture;
    s_visualCategories[OsmTag("shop", "electronics")]           = GeoDataPlacemark::ShopElectronics;
    s_visualCategories[OsmTag("shop", "mobile_phone")]          = GeoDataPlacemark::ShopMobilePhone;
    s_visualCategories[OsmTag("shop", "bicycle")]               = GeoDataPlacemark::ShopBicycle;
    s_visualCategories[OsmTag("shop", "car")]                   = GeoDataPlacemark::ShopCar;
    s_visualCategories[OsmTag("shop", "car_repair")]            = GeoDataPlacemark::ShopCarRepair;
    s_visualCategories[OsmTag("shop", "car_parts")]             = GeoDataPlacemark::ShopCarParts;
    s_visualCategories[OsmTag("shop", "motorcycle")]            = GeoDataPlacemark::ShopMotorcycle;
    s_visualCategories[OsmTag("shop", "outdoor")]               = GeoDataPlacemark::ShopOutdoor;
    s_visualCategories[OsmTag("shop", "musical_instrument")]    = GeoDataPlacemark::ShopMusicalInstrument;
    s_visualCategories[OsmTag("shop", "photo")]                 = GeoDataPlacemark::ShopPhoto;
    s_visualCategories[OsmTag("shop", "books")]                 = GeoDataPlacemark::ShopBook;
    s_visualCategories[OsmTag("shop", "gift")]                  = GeoDataPlacemark::ShopGift;
    s_visualCategories[OsmTag("shop", "stationery")]            = GeoDataPlacemark::ShopStationery;
    s_visualCategories[OsmTag("shop", "laundry")]               = GeoDataPlacemark::ShopLaundry;
    s_visualCategories[OsmTag("shop", "pet")]                   = GeoDataPlacemark::ShopPet;
    s_visualCategories[OsmTag("shop", "toys")]                  = GeoDataPlacemark::ShopToys;
    s_visualCategories[OsmTag("shop", "travel_agency")]         = GeoDataPlacemark::ShopTravelAgency;

    // Default for all other shops
    foreach (const QString &value, shopValues()) {
        s_visualCategories[OsmTag("shop", value)]               = GeoDataPlacemark::Shop;
    }

    s_visualCategories[OsmTag("man_made", "bridge")]            = GeoDataPlacemark::ManmadeBridge;
    s_visualCategories[OsmTag("man_made", "lighthouse")]        = GeoDataPlacemark::ManmadeLighthouse;
    s_visualCategories[OsmTag("man_made", "pier")]              = GeoDataPlacemark::ManmadePier;
    s_visualCategories[OsmTag("man_made", "water_tower")]       = GeoDataPlacemark::ManmadeWaterTower;
    s_visualCategories[OsmTag("man_made", "windmill")]          = GeoDataPlacemark::ManmadeWindMill;

    s_visualCategories[OsmTag("religion", "")]                  = GeoDataPlacemark::ReligionPlaceOfWorship;
    s_visualCategories[OsmTag("religion", "bahai")]             = GeoDataPlacemark::ReligionBahai;
    s_visualCategories[OsmTag("religion", "buddhist")]          = GeoDataPlacemark::ReligionBuddhist;
    s_visualCategories[OsmTag("religion", "christian")]         = GeoDataPlacemark::ReligionChristian;
    s_visualCategories[OsmTag("religion", "muslim")]            = GeoDataPlacemark::ReligionMuslim;
    s_visualCategories[OsmTag("religion", "hindu")]             = GeoDataPlacemark::ReligionHindu;
    s_visualCategories[OsmTag("religion", "jain")]              = GeoDataPlacemark::ReligionJain;
    s_visualCategories[OsmTag("religion", "jewish")]            = GeoDataPlacemark::ReligionJewish;
    s_visualCategories[OsmTag("religion", "shinto")]            = GeoDataPlacemark::ReligionShinto;
    s_visualCategories[OsmTag("religion", "sikh")]              = GeoDataPlacemark::ReligionSikh;

    s_visualCategories[OsmTag("historic", "memorial")]          = GeoDataPlacemark::TouristAttraction;
    s_visualCategories[OsmTag("tourism", "attraction")]         = GeoDataPlacemark::TouristAttraction;
    s_visualCategories[OsmTag("tourism", "camp_site")]          = GeoDataPlacemark::AccomodationCamping;
    s_visualCategories[OsmTag("tourism", "hostel")]             = GeoDataPlacemark::AccomodationHostel;
    s_visualCategories[OsmTag("tourism", "hotel")]              = GeoDataPlacemark::AccomodationHotel;
    s_visualCategories[OsmTag("tourism", "motel")]              = GeoDataPlacemark::AccomodationMotel;
    s_visualCategories[OsmTag("tourism", "guest_house")]        = GeoDataPlacemark::AccomodationGuestHouse;
    s_visualCategories[OsmTag("tourism", "museum")]             = GeoDataPlacemark::TouristMuseum;
    s_visualCategories[OsmTag("tourism", "theme_park")]         = GeoDataPlacemark::TouristThemePark;
    s_visualCategories[OsmTag("tourism", "viewpoint")]          = GeoDataPlacemark::TouristViewPoint;
    s_visualCategories[OsmTag("tourism", "zoo")]                = GeoDataPlacemark::TouristZoo;
    s_visualCategories[OsmTag("tourism", "alpine_hut")]         = GeoDataPlacemark::TouristAlpineHut;

    s_visualCategories[OsmTag("barrier", "city_wall")]           = GeoDataPlacemark::BarrierCityWall;
    s_visualCategories[OsmTag("barrier", "gate")]               = GeoDataPlacemark::BarrierGate;
    s_visualCategories[OsmTag("barrier", "lift_gate")]           = GeoDataPlacemark::BarrierLiftGate;
    s_visualCategories[OsmTag("barrier", "wall")]               = GeoDataPlacemark::BarrierWall;

    s_visualCategories[OsmTag("historic", "castle")]            = GeoDataPlacemark::TouristCastle;
    s_visualCategories[OsmTag("historic", "fort")]              = GeoDataPlacemark::TouristCastle;
    s_visualCategories[OsmTag("historic", "monument")]          = GeoDataPlacemark::TouristMonument;
    s_visualCategories[OsmTag("historic", "ruins")]             = GeoDataPlacemark::TouristRuin;

    s_visualCategories[OsmTag("highway", "traffic_signals")]    = GeoDataPlacemark::HighwayTrafficSignals;

    s_visualCategories[OsmTag("highway", "unknown")]            = GeoDataPlacemark::HighwayUnknown;
    s_visualCategories[OsmTag("highway", "steps")]              = GeoDataPlacemark::HighwaySteps;
    s_visualCategories[OsmTag("highway", "footway")]            = GeoDataPlacemark::HighwayFootway;
    s_visualCategories[OsmTag("highway", "cycleway")]           = GeoDataPlacemark::HighwayCycleway;
    s_visualCategories[OsmTag("highway", "path")]               = GeoDataPlacemark::HighwayPath;
    s_visualCategories[OsmTag("highway", "track")]              = GeoDataPlacemark::HighwayTrack;
    s_visualCategories[OsmTag("highway", "pedestrian")]         = GeoDataPlacemark::HighwayPedestrian;
    s_visualCategories[OsmTag("highway", "service")]            = GeoDataPlacemark::HighwayService;
    s_visualCategories[OsmTag("highway", "living_street")]      = GeoDataPlacemark::HighwayLivingStreet;
    s_visualCategories[OsmTag("highway", "unclassified")]       = GeoDataPlacemark::HighwayUnclassified;
    s_visualCategories[OsmTag("highway", "residential")]        = GeoDataPlacemark::HighwayResidential;
    s_visualCategories[OsmTag("highway", "road")]               = GeoDataPlacemark::HighwayRoad;
    s_visualCategories[OsmTag("highway", "tertiary_link")]      = GeoDataPlacemark::HighwayTertiaryLink;
    s_visualCategories[OsmTag("highway", "tertiary")]           = GeoDataPlacemark::HighwayTertiary;
    s_visualCategories[OsmTag("highway", "secondary_link")]     = GeoDataPlacemark::HighwaySecondaryLink;
    s_visualCategories[OsmTag("highway", "secondary")]          = GeoDataPlacemark::HighwaySecondary;
    s_visualCategories[OsmTag("highway", "primary_link")]       = GeoDataPlacemark::HighwayPrimaryLink;
    s_visualCategories[OsmTag("highway", "primary")]            = GeoDataPlacemark::HighwayPrimary;
    s_visualCategories[OsmTag("highway", "trunk_link")]         = GeoDataPlacemark::HighwayTrunkLink;
    s_visualCategories[OsmTag("highway", "trunk")]              = GeoDataPlacemark::HighwayTrunk;
    s_visualCategories[OsmTag("highway", "motorway_link")]      = GeoDataPlacemark::HighwayMotorwayLink;
    s_visualCategories[OsmTag("highway", "motorway")]           = GeoDataPlacemark::HighwayMotorway;

    s_visualCategories[OsmTag("natural", "water")]              = GeoDataPlacemark::NaturalWater;
    s_visualCategories[OsmTag("natural", "reef")]               = GeoDataPlacemark::NaturalReef;
    s_visualCategories[OsmTag("natural", "bay")]                = GeoDataPlacemark::NaturalWater;
    s_visualCategories[OsmTag("natural", "coastline")]          = GeoDataPlacemark::NaturalWater;
    s_visualCategories[OsmTag("waterway", "stream")]            = GeoDataPlacemark::NaturalWater;
    s_visualCategories[OsmTag("waterway", "river")]             = GeoDataPlacemark::NaturalWater;
    s_visualCategories[OsmTag("waterway", "riverbank")]         = GeoDataPlacemark::NaturalWater;
    s_visualCategories[OsmTag("waterway", "canal")]             = GeoDataPlacemark::NaturalWater;

    s_visualCategories[OsmTag("natural", "wood")]               = GeoDataPlacemark::NaturalWood;
    s_visualCategories[OsmTag("natural", "beach")]              = GeoDataPlacemark::NaturalBeach;
    s_visualCategories[OsmTag("natural", "wetland")]            = GeoDataPlacemark::NaturalWetland;
    s_visualCategories[OsmTag("natural", "glacier")]            = GeoDataPlacemark::NaturalGlacier;
    s_visualCategories[OsmTag("glacier:type", "shelf")]         = GeoDataPlacemark::NaturalIceShelf;
    s_visualCategories[OsmTag("natural", "scrub")]              = GeoDataPlacemark::NaturalScrub;
    s_visualCategories[OsmTag("natural", "cliff")]              = GeoDataPlacemark::NaturalCliff;

    s_visualCategories[OsmTag("military", "danger_area")]       = GeoDataPlacemark::MilitaryDangerArea;

    s_visualCategories[OsmTag("landuse", "forest")]             = GeoDataPlacemark::NaturalWood;
    s_visualCategories[OsmTag("landuse", "allotments")]         = GeoDataPlacemark::LanduseAllotments;
    s_visualCategories[OsmTag("landuse", "basin")]              = GeoDataPlacemark::LanduseBasin;
    s_visualCategories[OsmTag("landuse", "brownfield")]         = GeoDataPlacemark::LanduseConstruction;
    s_visualCategories[OsmTag("landuse", "cemetery")]           = GeoDataPlacemark::LanduseCemetery;
    s_visualCategories[OsmTag("landuse", "commercial")]         = GeoDataPlacemark::LanduseCommercial;
    s_visualCategories[OsmTag("landuse", "construction")]       = GeoDataPlacemark::LanduseConstruction;
    s_visualCategories[OsmTag("landuse", "farm")]               = GeoDataPlacemark::LanduseFarmland;
    s_visualCategories[OsmTag("landuse", "farmland")]           = GeoDataPlacemark::LanduseFarmland;
    s_visualCategories[OsmTag("landuse", "greenhouse_horticulture")] = GeoDataPlacemark::LanduseFarmland;
    s_visualCategories[OsmTag("landuse", "farmyard")]           = GeoDataPlacemark::LanduseFarmland;
    s_visualCategories[OsmTag("landuse", "garages")]            = GeoDataPlacemark::LanduseGarages;
    s_visualCategories[OsmTag("landuse", "greenfield")]         = GeoDataPlacemark::LanduseConstruction;
    s_visualCategories[OsmTag("landuse", "industrial")]         = GeoDataPlacemark::LanduseIndustrial;
    s_visualCategories[OsmTag("landuse", "landfill")]           = GeoDataPlacemark::LanduseLandfill;
    s_visualCategories[OsmTag("landuse", "meadow")]             = GeoDataPlacemark::LanduseMeadow;
    s_visualCategories[OsmTag("landuse", "military")]           = GeoDataPlacemark::LanduseMilitary;
    s_visualCategories[OsmTag("landuse", "orchard")]            = GeoDataPlacemark::LanduseFarmland;
    s_visualCategories[OsmTag("landuse", "quarry")]             = GeoDataPlacemark::LanduseQuarry;
    s_visualCategories[OsmTag("landuse", "railway")]            = GeoDataPlacemark::LanduseRailway;
    s_visualCategories[OsmTag("landuse", "recreation_ground")]  = GeoDataPlacemark::LeisurePark;
    s_visualCategories[OsmTag("landuse", "reservoir")]          = GeoDataPlacemark::LanduseReservoir;
    s_visualCategories[OsmTag("landuse", "residential")]        = GeoDataPlacemark::LanduseResidential;
    s_visualCategories[OsmTag("landuse", "retail")]             = GeoDataPlacemark::LanduseRetail;
    s_visualCategories[OsmTag("landuse", "orchard")]            = GeoDataPlacemark::LanduseOrchard;
    s_visualCategories[OsmTag("landuse", "vineyard")]           = GeoDataPlacemark::LanduseVineyard;
    s_visualCategories[OsmTag("landuse", "village_green")]      = GeoDataPlacemark::LanduseGrass;
    s_visualCategories[OsmTag("landuse", "grass")]              = GeoDataPlacemark::LanduseGrass;

    s_visualCategories[OsmTag("leisure", "common")]             = GeoDataPlacemark::LanduseGrass;
    s_visualCategories[OsmTag("leisure", "garden")]             = GeoDataPlacemark::LanduseGrass;
    s_visualCategories[OsmTag("leisure", "golf_course")]        = GeoDataPlacemark::LeisureGolfCourse;
    s_visualCategories[OsmTag("leisure", "marina")]             = GeoDataPlacemark::LeisureMarina;
    s_visualCategories[OsmTag("leisure", "park")]               = GeoDataPlacemark::LeisurePark;
    s_visualCategories[OsmTag("leisure", "playground")]         = GeoDataPlacemark::LeisurePlayground;
    s_visualCategories[OsmTag("leisure", "pitch")]              = GeoDataPlacemark::LeisurePitch;
    s_visualCategories[OsmTag("leisure", "sports_centre")]      = GeoDataPlacemark::LeisureSportsCentre;
    s_visualCategories[OsmTag("leisure", "stadium")]            = GeoDataPlacemark::LeisureStadium;
    s_visualCategories[OsmTag("leisure", "track")]              = GeoDataPlacemark::LeisureTrack;
    s_visualCategories[OsmTag("leisure", "swimming_pool")]      = GeoDataPlacemark::LeisureSwimmingPool;

    s_visualCategories[OsmTag("railway", "rail")]               = GeoDataPlacemark::RailwayRail;
    s_visualCategories[OsmTag("railway", "narrow_gauge")]       = GeoDataPlacemark::RailwayNarrowGauge;
    s_visualCategories[OsmTag("railway", "tram")]               = GeoDataPlacemark::RailwayTram;
    s_visualCategories[OsmTag("railway", "light_rail")]         = GeoDataPlacemark::RailwayLightRail;
    s_visualCategories[OsmTag("railway", "preserved")]          = GeoDataPlacemark::RailwayPreserved;
    s_visualCategories[OsmTag("railway", "abandoned")]          = GeoDataPlacemark::RailwayAbandoned;
    s_visualCategories[OsmTag("railway", "disused")]            = GeoDataPlacemark::RailwayAbandoned;
    s_visualCategories[OsmTag("railway", "razed")]              = GeoDataPlacemark::RailwayAbandoned;
    s_visualCategories[OsmTag("railway", "subway")]             = GeoDataPlacemark::RailwaySubway;
    s_visualCategories[OsmTag("railway", "miniature")]          = GeoDataPlacemark::RailwayMiniature;
    s_visualCategories[OsmTag("railway", "construction")]       = GeoDataPlacemark::RailwayConstruction;
    s_visualCategories[OsmTag("railway", "monorail")]           = GeoDataPlacemark::RailwayMonorail;
    s_visualCategories[OsmTag("railway", "funicular")]          = GeoDataPlacemark::RailwayFunicular;
    s_visualCategories[OsmTag("railway", "platform")]           = GeoDataPlacemark::TransportPlatform;
    s_visualCategories[OsmTag("railway", "station")]            = GeoDataPlacemark::TransportTrainStation;
    s_visualCategories[OsmTag("railway", "halt")]               = GeoDataPlacemark::TransportTrainStation;

    s_visualCategories[OsmTag("power", "tower")]                = GeoDataPlacemark::PowerTower;

    s_visualCategories[OsmTag("aeroway", "aerodrome")]          = GeoDataPlacemark::TransportAerodrome;
    s_visualCategories[OsmTag("aeroway", "helipad")]            = GeoDataPlacemark::TransportHelipad;
    s_visualCategories[OsmTag("aeroway", "gate")]               = GeoDataPlacemark::TransportAirportGate;
    s_visualCategories[OsmTag("aeroway", "runway")]             = GeoDataPlacemark::TransportAirportRunway;
    s_visualCategories[OsmTag("aeroway", "apron")]              = GeoDataPlacemark::TransportAirportApron;
    s_visualCategories[OsmTag("aeroway", "taxiway")]            = GeoDataPlacemark::TransportAirportTaxiway;
    s_visualCategories[OsmTag("transport", "airpor_terminal")]  = GeoDataPlacemark::TransportAirportTerminal;
    s_visualCategories[OsmTag("transport", "bus_station")]      = GeoDataPlacemark::TransportBusStation;
    s_visualCategories[OsmTag("highway", "bus_stop")]           = GeoDataPlacemark::TransportBusStop;
    s_visualCategories[OsmTag("transport", "car_share")]        = GeoDataPlacemark::TransportCarShare;
    s_visualCategories[OsmTag("transport", "fuel")]             = GeoDataPlacemark::TransportFuel;
    s_visualCategories[OsmTag("transport", "parking")]          = GeoDataPlacemark::TransportParking;
    s_visualCategories[OsmTag("public_transport", "platform")]  = GeoDataPlacemark::TransportPlatform;
    s_visualCategories[OsmTag("amenity", "bicycle_rental")]     = GeoDataPlacemark::TransportRentalBicycle;
    s_visualCategories[OsmTag("amenity", "car_rental")]         = GeoDataPlacemark::TransportRentalCar;
    s_visualCategories[OsmTag("amenity", "taxi")]               = GeoDataPlacemark::TransportTaxiRank;
    s_visualCategories[OsmTag("transport", "train_station")]    = GeoDataPlacemark::TransportTrainStation;
    s_visualCategories[OsmTag("transport", "tram_stop")]        = GeoDataPlacemark::TransportTramStop;
    s_visualCategories[OsmTag("transport", "bus_stop")]         = GeoDataPlacemark::TransportBusStop;
    s_visualCategories[OsmTag("amenity", "bicycle_parking")]    = GeoDataPlacemark::TransportBicycleParking;
    s_visualCategories[OsmTag("amenity", "motorcycle_parking")] = GeoDataPlacemark::TransportMotorcycleParking;
    s_visualCategories[OsmTag("railway", "subway_entrance")]    = GeoDataPlacemark::TransportSubwayEntrance;

    s_visualCategories[OsmTag("place", "city")]                 = GeoDataPlacemark::PlaceCity;
    s_visualCategories[OsmTag("place", "suburb")]               = GeoDataPlacemark::PlaceSuburb;
    s_visualCategories[OsmTag("place", "hamlet")]               = GeoDataPlacemark::PlaceHamlet;
    s_visualCategories[OsmTag("place", "locality")]             = GeoDataPlacemark::PlaceLocality;
    s_visualCategories[OsmTag("place", "town")]                 = GeoDataPlacemark::PlaceTown;
    s_visualCategories[OsmTag("place", "village")]              = GeoDataPlacemark::PlaceVillage;

    //Custom Marble OSM Tags
    s_visualCategories[OsmTag("marble_land", "landmass")]       = GeoDataPlacemark::Landmass;
    s_visualCategories[OsmTag("settlement", "yes")]             = GeoDataPlacemark::UrbanArea;
    s_visualCategories[OsmTag("marble_line", "date")]           = GeoDataPlacemark::InternationalDateLine;
    s_visualCategories[OsmTag("marble:feature", "bathymetry")]  = GeoDataPlacemark::Bathymetry;

    // Default for buildings
    foreach (const auto &tag, buildingTags()) {
        s_visualCategories[tag]                                 = GeoDataPlacemark::Building;
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
    const GeoDataPlacemark *const placemark = parameters.placemark;

    if (!placemark) {
        Q_ASSERT(false && "Must not pass a null placemark to StyleBuilder::createStyle");
        return GeoDataStyle::Ptr();
    }

    if (placemark->customStyle()) {
        return placemark->customStyle();
    }

    auto const visualCategory = placemark->visualCategory();
    GeoDataStyle::ConstPtr style = d->presetStyle(visualCategory);

    OsmPlacemarkData const & osmData = placemark->osmData();
    if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPointType) {
        if (visualCategory == GeoDataPlacemark::NaturalTree) {
            GeoDataCoordinates const coordinates = placemark->coordinate();
            qreal const lat = coordinates.latitude(GeoDataCoordinates::Degree);
            if (qAbs(lat) > 15) {
                /** @todo Should maybe auto-adjust to MarbleClock at some point */
                int const month = QDate::currentDate().month();
                bool const southernHemisphere = lat < 0;
                if (southernHemisphere) {
                    if (month >= 3 && month <= 5) {
                        style = d->m_styleTreeAutumn;
                    } else if (month >= 6 && month <= 8) {
                        style = d->m_styleTreeWinter;
                    }
                } else {
                    if (month >= 9 && month <= 11) {
                        style = d->m_styleTreeAutumn;
                    } else if (month == 12 || month == 1 || month == 2) {
                        style = d->m_styleTreeWinter;
                    }
                }
            }
        }
    } else if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLinearRingType) {
        bool adjustStyle = false;

        GeoDataPolyStyle polyStyle = style->polyStyle();
        GeoDataLineStyle lineStyle = style->lineStyle();
        if (visualCategory == GeoDataPlacemark::NaturalWater) {
            if (osmData.containsTag(QStringLiteral("salt"), QStringLiteral("yes"))) {
                polyStyle.setColor("#ffff80");
                lineStyle.setPenStyle(Qt::DashLine);
                lineStyle.setWidth(2);
                adjustStyle = true;
            }
        }
        else if (visualCategory == GeoDataPlacemark::Bathymetry) {
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
        else if (visualCategory == GeoDataPlacemark::AmenityGraveyard || visualCategory == GeoDataPlacemark::LanduseCemetery) {
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
        }

        if (adjustStyle) {
            GeoDataStyle::Ptr newStyle(new GeoDataStyle(*style));
            newStyle->setPolyStyle(polyStyle);
            newStyle->setLineStyle(lineStyle);
            style = newStyle;
        }

        if (style->iconStyle().iconPath().isEmpty()) {
            const GeoDataPlacemark::GeoDataVisualCategory category = determineVisualCategory(osmData);
            const GeoDataStyle::ConstPtr categoryStyle = d->presetStyle(category);
            if (category != GeoDataPlacemark::None && !categoryStyle->iconStyle().icon().isNull()) {
                GeoDataStyle::Ptr newStyle(new GeoDataStyle(*style));
                newStyle->setIconStyle(categoryStyle->iconStyle());
                style = newStyle;
            }
        }
    } else if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType) {
        GeoDataPolyStyle polyStyle = style->polyStyle();
        GeoDataLineStyle lineStyle = style->lineStyle();
        GeoDataLabelStyle labelStyle = style->labelStyle();
        lineStyle.setCosmeticOutline(true);

        if(visualCategory == GeoDataPlacemark::AdminLevel2){
            if (osmData.containsTag(QStringLiteral("maritime"), QStringLiteral("yes"))) {
                lineStyle.setColor("#88b3bf");
                polyStyle.setColor("#88b3bf");
                if (osmData.containsTag(QStringLiteral("marble:disputed"), QStringLiteral("yes"))) {
                    lineStyle.setPenStyle( Qt::DashLine );
                }
            }
        }
        else if ((visualCategory >= GeoDataPlacemark::HighwayService &&
                visualCategory <= GeoDataPlacemark::HighwayMotorway) ||
                visualCategory == GeoDataPlacemark::TransportAirportRunway) {

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

            if (parameters.tileLevel <= 8) {
                /** @todo: Dummy implementation for dynamic style changes based on tile level, replace with sane values */
                lineStyle.setPhysicalWidth(0.0);
                lineStyle.setWidth(2.0);
            } else if (parameters.tileLevel <= 10) {
                /** @todo: Dummy implementation for dynamic style changes based on tile level, replace with sane values */
                lineStyle.setPhysicalWidth(0.0);
                lineStyle.setWidth(3.0);
            } else if (parameters.tileLevel <= 12) {
                /** @todo: Dummy implementation for dynamic style changes based on tile level, replace with sane values */
                lineStyle.setPhysicalWidth(0.0);
                lineStyle.setWidth(4.0);
            }else {
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
                    double const margins = visualCategory == GeoDataPlacemark::HighwayMotorway ? 2.0 : (isOneWay ? 1.0 : 0.0);
                    double const physicalWidth = margins + lanes * laneWidth;
                    lineStyle.setPhysicalWidth(physicalWidth);
                }
            }

        } else if (visualCategory == GeoDataPlacemark::NaturalWater) {
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
        newStyle->setLabelStyle(labelStyle);

        style = newStyle;
    } else if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType) {
        GeoDataPolyStyle polyStyle = style->polyStyle();
        GeoDataLineStyle lineStyle = style->lineStyle();
        bool adjustStyle = false;
        if (visualCategory == GeoDataPlacemark::Bathymetry) {
            auto tagIter = osmData.findTag(QStringLiteral("ele"));
            if (tagIter != osmData.tagsEnd()) {
                const QString& elevation = tagIter.value();
                if (elevation == QLatin1String("4000")) {
                    polyStyle.setColor("#a5c9c9");
                    lineStyle.setColor("#a5c9c9");
                    adjustStyle = true;
                }
            }
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

GeoDataStyle::ConstPtr StyleBuilder::Private::presetStyle(GeoDataPlacemark::GeoDataVisualCategory visualCategory) const
{
    if (!m_defaultStyleInitialized) {
        const_cast<StyleBuilder::Private *>(this)->initializeDefaultStyles(); // const cast due to lazy initialization
    }

    if (visualCategory != GeoDataPlacemark::None && m_defaultStyle[visualCategory] ) {
        return m_defaultStyle[visualCategory];
    } else {
        return m_defaultStyle[GeoDataPlacemark::Default];
    }
}


QStringList StyleBuilder::renderOrder() const
{
    static QStringList paintLayerOrder;

    if (paintLayerOrder.isEmpty()) {
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::Landmass);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::UrbanArea);
        for ( int i = GeoDataPlacemark::LanduseAllotments; i <= GeoDataPlacemark::LanduseVineyard; i++ ) {
            if ((GeoDataPlacemark::GeoDataVisualCategory)i != GeoDataPlacemark::LanduseGrass) {
                paintLayerOrder << Private::createPaintLayerItem("Polygon", (GeoDataPlacemark::GeoDataVisualCategory)i);
            }
        }
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::Bathymetry);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::NaturalBeach);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::NaturalWetland);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::NaturalGlacier);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::NaturalIceShelf);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::NaturalCliff);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::NaturalPeak);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::MilitaryDangerArea);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LeisurePark);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LeisurePitch);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LeisureSportsCentre);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LeisureStadium);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::NaturalWood);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseGrass);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::HighwayPedestrian);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LeisurePlayground);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::NaturalScrub);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LeisureTrack);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::TransportParking);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::TransportParkingSpace);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::ManmadeBridge);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::BarrierCityWall);

        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::AmenityGraveyard);

        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::AmenityKindergarten);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::EducationCollege);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::EducationSchool);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::EducationUniversity);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::HealthHospital);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LeisureSwimmingPool);

        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::Landmass);

        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::NaturalWater);
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::NaturalWater, "outline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::NaturalWater, "inline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::NaturalWater, "label");


        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::NaturalReef, "outline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::NaturalReef, "inline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::NaturalReef, "label");
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LeisureMarina);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::ManmadePier);
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::ManmadePier, "outline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::ManmadePier, "inline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::ManmadePier, "label");

        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::TransportAirportApron);

        for ( int i = GeoDataPlacemark::HighwaySteps; i <= GeoDataPlacemark::HighwayMotorway; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataPlacemark::GeoDataVisualCategory)i, "outline");
        }
        for ( int i = GeoDataPlacemark::HighwaySteps; i <= GeoDataPlacemark::HighwayMotorway; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataPlacemark::GeoDataVisualCategory)i, "inline");
        }
        for ( int i = GeoDataPlacemark::RailwayRail; i <= GeoDataPlacemark::RailwayFunicular; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataPlacemark::GeoDataVisualCategory)i, "outline");
        }
        for ( int i = GeoDataPlacemark::RailwayRail; i <= GeoDataPlacemark::RailwayFunicular; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataPlacemark::GeoDataVisualCategory)i, "inline");
        }
        for ( int i = GeoDataPlacemark::HighwaySteps; i <= GeoDataPlacemark::HighwayMotorway; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataPlacemark::GeoDataVisualCategory)i, "label");
        }
        for ( int i = GeoDataPlacemark::RailwayRail; i <= GeoDataPlacemark::RailwayFunicular; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataPlacemark::GeoDataVisualCategory)i, "label");
        }

        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::TransportPlatform);
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::TransportPlatform, "outline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::TransportPlatform, "inline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::TransportPlatform, "label");

        for ( int i = GeoDataPlacemark::AdminLevel1; i <= GeoDataPlacemark::AdminLevel11; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataPlacemark::GeoDataVisualCategory)i, "outline");
        }
        for ( int i = GeoDataPlacemark::AdminLevel1; i <= GeoDataPlacemark::AdminLevel11; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataPlacemark::GeoDataVisualCategory)i, "inline");
        }
        for ( int i = GeoDataPlacemark::AdminLevel1; i <= GeoDataPlacemark::AdminLevel11; i++ ) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", (GeoDataPlacemark::GeoDataVisualCategory)i, "label");
        }
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::Bathymetry);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::AmenityGraveyard);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::NaturalWood);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::NaturalBeach);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::NaturalWetland);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::NaturalGlacier);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::NaturalIceShelf);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::NaturalScrub);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::LeisureMarina);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::LeisurePark);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::LeisurePlayground);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::LeisurePitch);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::LeisureSportsCentre);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::LeisureStadium);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::LeisureTrack);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::TransportParking);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::ManmadeBridge);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::BarrierCityWall);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::NaturalWater);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::NaturalReef);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::Landmass);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::NaturalCliff);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::NaturalPeak);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::AmenityKindergarten);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::EducationCollege);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::EducationSchool);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::EducationUniversity);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::HealthHospital);
        paintLayerOrder << Private::createPaintLayerItem("Point", GeoDataPlacemark::MilitaryDangerArea);

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

QString StyleBuilder::visualCategoryName(GeoDataPlacemark::GeoDataVisualCategory category)
{
    static QHash<GeoDataPlacemark::GeoDataVisualCategory, QString> visualCategoryNames;

    if (visualCategoryNames.isEmpty()) {
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::None] = "None";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::Default] = "Default";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::Unknown] = "Unknown";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::SmallCity] = "SmallCity";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::SmallCountyCapital] = "SmallCountyCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::SmallStateCapital] = "SmallStateCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::SmallNationCapital] = "SmallNationCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::MediumCity] = "MediumCity";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::MediumCountyCapital] = "MediumCountyCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::MediumStateCapital] = "MediumStateCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::MediumNationCapital] = "MediumNationCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::BigCity] = "BigCity";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::BigCountyCapital] = "BigCountyCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::BigStateCapital] = "BigStateCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::BigNationCapital] = "BigNationCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::LargeCity] = "LargeCity";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::LargeCountyCapital] = "LargeCountyCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::LargeStateCapital] = "LargeStateCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::LargeNationCapital] = "LargeNationCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::Nation] = "Nation";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceCity] = "PlaceCity";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceCityCapital] = "PlaceCityCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceSuburb] = "PlaceSuburb";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceHamlet] = "PlaceHamlet";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceLocality] = "PlaceLocality";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceTown] = "PlaceTown";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceTownCapital] = "PlaceTownCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceVillage] = "PlaceVillage";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceVillageCapital] = "PlaceVillageCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::Mountain] = "Mountain";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::Volcano] = "Volcano";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::Mons] = "Mons";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::Valley] = "Valley";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::Continent] = "Continent";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::Ocean] = "Ocean";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::OtherTerrain] = "OtherTerrain";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::Crater] = "Crater";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::Mare] = "Mare";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::GeographicPole] = "GeographicPole";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::MagneticPole] = "MagneticPole";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::ShipWreck] = "ShipWreck";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::AirPort] = "AirPort";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::Observatory] = "Observatory";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::MilitaryDangerArea] = "MilitaryDangerArea";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::OsmSite] = "OsmSite";
        visualCategoryNames[GeoDataPlacemark::Coordinate] = "Coordinate";
        visualCategoryNames[GeoDataPlacemark::MannedLandingSite] = "MannedLandingSite";
        visualCategoryNames[GeoDataPlacemark::RoboticRover] = "RoboticRover";
        visualCategoryNames[GeoDataPlacemark::UnmannedSoftLandingSite] = "UnmannedSoftLandingSite";
        visualCategoryNames[GeoDataPlacemark::UnmannedHardLandingSite] = "UnmannedHardLandingSite";
        visualCategoryNames[GeoDataPlacemark::Bookmark] = "Bookmark";
        visualCategoryNames[GeoDataPlacemark::NaturalWater] = "NaturalWater";
        visualCategoryNames[GeoDataPlacemark::NaturalReef] = "NaturalReef";
        visualCategoryNames[GeoDataPlacemark::NaturalWood] = "NaturalWood";
        visualCategoryNames[GeoDataPlacemark::NaturalBeach] = "NaturalBeach";
        visualCategoryNames[GeoDataPlacemark::NaturalWetland] = "NaturalWetland";
        visualCategoryNames[GeoDataPlacemark::NaturalGlacier] = "NaturalGlacier";
        visualCategoryNames[GeoDataPlacemark::NaturalIceShelf] = "NaturalIceShelf";
        visualCategoryNames[GeoDataPlacemark::NaturalScrub] = "NaturalScrub";
        visualCategoryNames[GeoDataPlacemark::NaturalCliff] = "NaturalCliff";
        visualCategoryNames[GeoDataPlacemark::NaturalHeath] = "NaturalHeath";
        visualCategoryNames[GeoDataPlacemark::HighwayTrafficSignals] = "HighwayTrafficSignals";
        visualCategoryNames[GeoDataPlacemark::HighwaySteps] = "HighwaySteps";
        visualCategoryNames[GeoDataPlacemark::HighwayUnknown] = "HighwayUnknown";
        visualCategoryNames[GeoDataPlacemark::HighwayPath] = "HighwayPath";
        visualCategoryNames[GeoDataPlacemark::HighwayFootway] = "HighwayFootway";
        visualCategoryNames[GeoDataPlacemark::HighwayTrack] = "HighwayTrack";
        visualCategoryNames[GeoDataPlacemark::HighwayPedestrian] = "HighwayPedestrian";
        visualCategoryNames[GeoDataPlacemark::HighwayCycleway] = "HighwayCycleway";
        visualCategoryNames[GeoDataPlacemark::HighwayService] = "HighwayService";
        visualCategoryNames[GeoDataPlacemark::HighwayRoad] = "HighwayRoad";
        visualCategoryNames[GeoDataPlacemark::HighwayResidential] = "HighwayResidential";
        visualCategoryNames[GeoDataPlacemark::HighwayLivingStreet] = "HighwayLivingStreet";
        visualCategoryNames[GeoDataPlacemark::HighwayUnclassified] = "HighwayUnclassified";
        visualCategoryNames[GeoDataPlacemark::HighwayTertiaryLink] = "HighwayTertiaryLink";
        visualCategoryNames[GeoDataPlacemark::HighwayTertiary] = "HighwayTertiary";
        visualCategoryNames[GeoDataPlacemark::HighwaySecondaryLink] = "HighwaySecondaryLink";
        visualCategoryNames[GeoDataPlacemark::HighwaySecondary] = "HighwaySecondary";
        visualCategoryNames[GeoDataPlacemark::HighwayPrimaryLink] = "HighwayPrimaryLink";
        visualCategoryNames[GeoDataPlacemark::HighwayPrimary] = "HighwayPrimary";
        visualCategoryNames[GeoDataPlacemark::HighwayTrunkLink] = "HighwayTrunkLink";
        visualCategoryNames[GeoDataPlacemark::HighwayTrunk] = "HighwayTrunk";
        visualCategoryNames[GeoDataPlacemark::HighwayMotorwayLink] = "HighwayMotorwayLink";
        visualCategoryNames[GeoDataPlacemark::HighwayMotorway] = "HighwayMotorway";
        visualCategoryNames[GeoDataPlacemark::Building] = "Building";
        visualCategoryNames[GeoDataPlacemark::AccomodationCamping] = "AccomodationCamping";
        visualCategoryNames[GeoDataPlacemark::AccomodationHostel] = "AccomodationHostel";
        visualCategoryNames[GeoDataPlacemark::AccomodationHotel] = "AccomodationHotel";
        visualCategoryNames[GeoDataPlacemark::AccomodationMotel] = "AccomodationMotel";
        visualCategoryNames[GeoDataPlacemark::AccomodationYouthHostel] = "AccomodationYouthHostel";
        visualCategoryNames[GeoDataPlacemark::AccomodationGuestHouse] = "AccomodationGuestHouse";
        visualCategoryNames[GeoDataPlacemark::AmenityLibrary] = "AmenityLibrary";
        visualCategoryNames[GeoDataPlacemark::AmenityKindergarten] = "AmenityKindergarten";
        visualCategoryNames[GeoDataPlacemark::EducationCollege] = "EducationCollege";
        visualCategoryNames[GeoDataPlacemark::EducationSchool] = "EducationSchool";
        visualCategoryNames[GeoDataPlacemark::EducationUniversity] = "EducationUniversity";
        visualCategoryNames[GeoDataPlacemark::FoodBar] = "FoodBar";
        visualCategoryNames[GeoDataPlacemark::FoodBiergarten] = "FoodBiergarten";
        visualCategoryNames[GeoDataPlacemark::FoodCafe] = "FoodCafe";
        visualCategoryNames[GeoDataPlacemark::FoodFastFood] = "FoodFastFood";
        visualCategoryNames[GeoDataPlacemark::FoodPub] = "FoodPub";
        visualCategoryNames[GeoDataPlacemark::FoodRestaurant] = "FoodRestaurant";
        visualCategoryNames[GeoDataPlacemark::HealthDentist] = "HealthDentist";
        visualCategoryNames[GeoDataPlacemark::HealthDoctors] = "HealthDoctors";
        visualCategoryNames[GeoDataPlacemark::HealthHospital] = "HealthHospital";
        visualCategoryNames[GeoDataPlacemark::HealthPharmacy] = "HealthPharmacy";
        visualCategoryNames[GeoDataPlacemark::HealthVeterinary] = "HealthVeterinary";
        visualCategoryNames[GeoDataPlacemark::MoneyAtm] = "MoneyAtm";
        visualCategoryNames[GeoDataPlacemark::MoneyBank] = "MoneyBank";
        visualCategoryNames[GeoDataPlacemark::AmenityArchaeologicalSite] = "AmenityArchaeologicalSite";
        visualCategoryNames[GeoDataPlacemark::AmenityEmbassy] = "AmenityEmbassy";
        visualCategoryNames[GeoDataPlacemark::AmenityEmergencyPhone] = "AmenityEmergencyPhone";
        visualCategoryNames[GeoDataPlacemark::AmenityWaterPark] = "AmenityWaterPark";
        visualCategoryNames[GeoDataPlacemark::AmenityCommunityCentre] = "AmenityCommunityCentre";
        visualCategoryNames[GeoDataPlacemark::AmenityFountain] = "AmenityFountain";
        visualCategoryNames[GeoDataPlacemark::AmenityNightClub] = "AmenityNightClub";
        visualCategoryNames[GeoDataPlacemark::AmenityBench] = "AmenityBench";
        visualCategoryNames[GeoDataPlacemark::AmenityCourtHouse] = "AmenityCourtHouse";
        visualCategoryNames[GeoDataPlacemark::AmenityFireStation] = "AmenityFireStation";
        visualCategoryNames[GeoDataPlacemark::AmenityHuntingStand] = "AmenityHuntingStand";
        visualCategoryNames[GeoDataPlacemark::AmenityPolice] = "AmenityPolice";
        visualCategoryNames[GeoDataPlacemark::AmenityPostBox] = "AmenityPostBox";
        visualCategoryNames[GeoDataPlacemark::AmenityPostOffice] = "AmenityPostOffice";
        visualCategoryNames[GeoDataPlacemark::AmenityPrison] = "AmenityPrison";
        visualCategoryNames[GeoDataPlacemark::AmenityRecycling] = "AmenityRecycling";
        visualCategoryNames[GeoDataPlacemark::AmenityShelter] = "AmenityShelter";
        visualCategoryNames[GeoDataPlacemark::AmenityTelephone] = "AmenityTelephone";
        visualCategoryNames[GeoDataPlacemark::AmenityToilets] = "AmenityToilets";
        visualCategoryNames[GeoDataPlacemark::AmenityTownHall] = "AmenityTownHall";
        visualCategoryNames[GeoDataPlacemark::AmenityWasteBasket] = "AmenityWasteBasket";
        visualCategoryNames[GeoDataPlacemark::AmenityDrinkingWater] = "AmenityDrinkingWater";
        visualCategoryNames[GeoDataPlacemark::AmenityGraveyard] = "AmenityGraveyard";
        visualCategoryNames[GeoDataPlacemark::BarrierCityWall] = "BarrierCityWall";
        visualCategoryNames[GeoDataPlacemark::BarrierGate] = "BarrierGate";
        visualCategoryNames[GeoDataPlacemark::BarrierLiftGate] = "BarrierLiftGate";
        visualCategoryNames[GeoDataPlacemark::BarrierWall] = "BarrierWall";
        visualCategoryNames[GeoDataPlacemark::NaturalPeak] = "NaturalPeak";
        visualCategoryNames[GeoDataPlacemark::NaturalTree] = "NaturalTree";
        visualCategoryNames[GeoDataPlacemark::ShopBeverages] = "ShopBeverages";
        visualCategoryNames[GeoDataPlacemark::ShopHifi] = "ShopHifi";
        visualCategoryNames[GeoDataPlacemark::ShopSupermarket] = "ShopSupermarket";
        visualCategoryNames[GeoDataPlacemark::ShopAlcohol] = "ShopAlcohol";
        visualCategoryNames[GeoDataPlacemark::ShopBakery] = "ShopBakery";
        visualCategoryNames[GeoDataPlacemark::ShopButcher] = "ShopButcher";
        visualCategoryNames[GeoDataPlacemark::ShopConfectionery] = "ShopConfectionery";
        visualCategoryNames[GeoDataPlacemark::ShopConvenience] = "ShopConvenience";
        visualCategoryNames[GeoDataPlacemark::ShopGreengrocer] = "ShopGreengrocer";
        visualCategoryNames[GeoDataPlacemark::ShopSeafood] = "ShopSeafood";
        visualCategoryNames[GeoDataPlacemark::ShopDepartmentStore] = "ShopDepartmentStore";
        visualCategoryNames[GeoDataPlacemark::ShopKiosk] = "ShopKiosk";
        visualCategoryNames[GeoDataPlacemark::ShopBag] = "ShopBag";
        visualCategoryNames[GeoDataPlacemark::ShopClothes] = "ShopClothes";
        visualCategoryNames[GeoDataPlacemark::ShopFashion] = "ShopFashion";
        visualCategoryNames[GeoDataPlacemark::ShopJewelry] = "ShopJewelry";
        visualCategoryNames[GeoDataPlacemark::ShopShoes] = "ShopShoes";
        visualCategoryNames[GeoDataPlacemark::ShopVarietyStore] = "ShopVarietyStore";
        visualCategoryNames[GeoDataPlacemark::ShopBeauty] = "ShopBeauty";
        visualCategoryNames[GeoDataPlacemark::ShopChemist] = "ShopChemist";
        visualCategoryNames[GeoDataPlacemark::ShopCosmetics] = "ShopCosmetics";
        visualCategoryNames[GeoDataPlacemark::ShopHairdresser] = "ShopHairdresser";
        visualCategoryNames[GeoDataPlacemark::ShopOptician] = "ShopOptician";
        visualCategoryNames[GeoDataPlacemark::ShopPerfumery] = "ShopPerfumery";
        visualCategoryNames[GeoDataPlacemark::ShopDoitYourself] = "ShopDoitYourself";
        visualCategoryNames[GeoDataPlacemark::ShopFlorist] = "ShopFlorist";
        visualCategoryNames[GeoDataPlacemark::ShopHardware] = "ShopHardware";
        visualCategoryNames[GeoDataPlacemark::ShopFurniture] = "ShopFurniture";
        visualCategoryNames[GeoDataPlacemark::ShopElectronics] = "ShopElectronics";
        visualCategoryNames[GeoDataPlacemark::ShopMobilePhone] = "ShopMobilePhone";
        visualCategoryNames[GeoDataPlacemark::ShopBicycle] = "ShopBicycle";
        visualCategoryNames[GeoDataPlacemark::ShopCar] = "ShopCar";
        visualCategoryNames[GeoDataPlacemark::ShopCarRepair] = "ShopCarRepair";
        visualCategoryNames[GeoDataPlacemark::ShopCarParts] = "ShopCarParts";
        visualCategoryNames[GeoDataPlacemark::ShopMotorcycle] = "ShopMotorcycle";
        visualCategoryNames[GeoDataPlacemark::ShopOutdoor] = "ShopOutdoor";
        visualCategoryNames[GeoDataPlacemark::ShopMusicalInstrument] = "ShopMusicalInstrument";
        visualCategoryNames[GeoDataPlacemark::ShopPhoto] = "ShopPhoto";
        visualCategoryNames[GeoDataPlacemark::ShopBook] = "ShopBook";
        visualCategoryNames[GeoDataPlacemark::ShopGift] = "ShopGift";
        visualCategoryNames[GeoDataPlacemark::ShopStationery] = "ShopStationery";
        visualCategoryNames[GeoDataPlacemark::ShopLaundry] = "ShopLaundry";
        visualCategoryNames[GeoDataPlacemark::ShopPet] = "ShopPet";
        visualCategoryNames[GeoDataPlacemark::ShopToys] = "ShopToys";
        visualCategoryNames[GeoDataPlacemark::ShopTravelAgency] = "ShopTravelAgency";
        visualCategoryNames[GeoDataPlacemark::Shop] = "Shop";
        visualCategoryNames[GeoDataPlacemark::ManmadeBridge] = "ManmadeBridge";
        visualCategoryNames[GeoDataPlacemark::ManmadeLighthouse] = "ManmadeLighthouse";
        visualCategoryNames[GeoDataPlacemark::ManmadePier] = "ManmadePier";
        visualCategoryNames[GeoDataPlacemark::ManmadeWaterTower] = "ManmadeWaterTower";
        visualCategoryNames[GeoDataPlacemark::ManmadeWindMill] = "ManmadeWindMill";
        visualCategoryNames[GeoDataPlacemark::TouristAttraction] = "TouristAttraction";
        visualCategoryNames[GeoDataPlacemark::TouristCastle] = "TouristCastle";
        visualCategoryNames[GeoDataPlacemark::TouristCinema] = "TouristCinema";
        visualCategoryNames[GeoDataPlacemark::TouristInformation] = "TouristInformation";
        visualCategoryNames[GeoDataPlacemark::TouristMonument] = "TouristMonument";
        visualCategoryNames[GeoDataPlacemark::TouristMuseum] = "TouristMuseum";
        visualCategoryNames[GeoDataPlacemark::TouristRuin] = "TouristRuin";
        visualCategoryNames[GeoDataPlacemark::TouristTheatre] = "TouristTheatre";
        visualCategoryNames[GeoDataPlacemark::TouristThemePark] = "TouristThemePark";
        visualCategoryNames[GeoDataPlacemark::TouristViewPoint] = "TouristViewPoint";
        visualCategoryNames[GeoDataPlacemark::TouristZoo] = "TouristZoo";
        visualCategoryNames[GeoDataPlacemark::TouristAlpineHut] = "TouristAlpineHut";
        visualCategoryNames[GeoDataPlacemark::TransportAerodrome] = "TransportAerodrome";
        visualCategoryNames[GeoDataPlacemark::TransportHelipad] = "TransportHelipad";
        visualCategoryNames[GeoDataPlacemark::TransportAirportTerminal] = "TransportAirportTerminal";
        visualCategoryNames[GeoDataPlacemark::TransportAirportGate] = "TransportAirportGate";
        visualCategoryNames[GeoDataPlacemark::TransportAirportRunway] = "TransportAirportRunway";
        visualCategoryNames[GeoDataPlacemark::TransportAirportTaxiway] = "TransportAirportTaxiway";
        visualCategoryNames[GeoDataPlacemark::TransportAirportApron] = "TransportAirportApron";
        visualCategoryNames[GeoDataPlacemark::TransportBusStation] = "TransportBusStation";
        visualCategoryNames[GeoDataPlacemark::TransportBusStop] = "TransportBusStop";
        visualCategoryNames[GeoDataPlacemark::TransportCarShare] = "TransportCarShare";
        visualCategoryNames[GeoDataPlacemark::TransportFuel] = "TransportFuel";
        visualCategoryNames[GeoDataPlacemark::TransportParking] = "TransportParking";
        visualCategoryNames[GeoDataPlacemark::TransportParkingSpace] = "TransportParkingSpace";
        visualCategoryNames[GeoDataPlacemark::TransportPlatform] = "TransportPlatform";
        visualCategoryNames[GeoDataPlacemark::TransportRentalBicycle] = "TransportRentalBicycle";
        visualCategoryNames[GeoDataPlacemark::TransportRentalCar] = "TransportRentalCar";
        visualCategoryNames[GeoDataPlacemark::TransportTaxiRank] = "TransportTaxiRank";
        visualCategoryNames[GeoDataPlacemark::TransportTrainStation] = "TransportTrainStation";
        visualCategoryNames[GeoDataPlacemark::TransportTramStop] = "TransportTramStop";
        visualCategoryNames[GeoDataPlacemark::TransportBicycleParking] = "TransportBicycleParking";
        visualCategoryNames[GeoDataPlacemark::TransportMotorcycleParking] = "TransportMotorcycleParking";
        visualCategoryNames[GeoDataPlacemark::TransportSubwayEntrance] = "TransportSubwayEntrance";
        visualCategoryNames[GeoDataPlacemark::ReligionPlaceOfWorship] = "ReligionPlaceOfWorship";
        visualCategoryNames[GeoDataPlacemark::ReligionBahai] = "ReligionBahai";
        visualCategoryNames[GeoDataPlacemark::ReligionBuddhist] = "ReligionBuddhist";
        visualCategoryNames[GeoDataPlacemark::ReligionChristian] = "ReligionChristian";
        visualCategoryNames[GeoDataPlacemark::ReligionMuslim] = "ReligionMuslim";
        visualCategoryNames[GeoDataPlacemark::ReligionHindu] = "ReligionHindu";
        visualCategoryNames[GeoDataPlacemark::ReligionJain] = "ReligionJain";
        visualCategoryNames[GeoDataPlacemark::ReligionJewish] = "ReligionJewish";
        visualCategoryNames[GeoDataPlacemark::ReligionShinto] = "ReligionShinto";
        visualCategoryNames[GeoDataPlacemark::ReligionSikh] = "ReligionSikh";
        visualCategoryNames[GeoDataPlacemark::LeisureGolfCourse] = "LeisureGolfCourse";
        visualCategoryNames[GeoDataPlacemark::LeisureMarina] = "LeisureMarina";
        visualCategoryNames[GeoDataPlacemark::LeisurePark] = "LeisurePark";
        visualCategoryNames[GeoDataPlacemark::LeisurePlayground] = "LeisurePlayground";
        visualCategoryNames[GeoDataPlacemark::LeisurePitch] = "LeisurePitch";
        visualCategoryNames[GeoDataPlacemark::LeisureSportsCentre] = "LeisureSportsCentre";
        visualCategoryNames[GeoDataPlacemark::LeisureStadium] = "LeisureStadium";
        visualCategoryNames[GeoDataPlacemark::LeisureTrack] = "LeisureTrack";
        visualCategoryNames[GeoDataPlacemark::LeisureSwimmingPool] = "LeisureSwimmingPool";
        visualCategoryNames[GeoDataPlacemark::LanduseAllotments] = "LanduseAllotments";
        visualCategoryNames[GeoDataPlacemark::LanduseBasin] = "LanduseBasin";
        visualCategoryNames[GeoDataPlacemark::LanduseCemetery] = "LanduseCemetery";
        visualCategoryNames[GeoDataPlacemark::LanduseCommercial] = "LanduseCommercial";
        visualCategoryNames[GeoDataPlacemark::LanduseConstruction] = "LanduseConstruction";
        visualCategoryNames[GeoDataPlacemark::LanduseFarmland] = "LanduseFarmland";
        visualCategoryNames[GeoDataPlacemark::LanduseFarmyard] = "LanduseFarmyard";
        visualCategoryNames[GeoDataPlacemark::LanduseGarages] = "LanduseGarages";
        visualCategoryNames[GeoDataPlacemark::LanduseGrass] = "LanduseGrass";
        visualCategoryNames[GeoDataPlacemark::LanduseIndustrial] = "LanduseIndustrial";
        visualCategoryNames[GeoDataPlacemark::LanduseLandfill] = "LanduseLandfill";
        visualCategoryNames[GeoDataPlacemark::LanduseMeadow] = "LanduseMeadow";
        visualCategoryNames[GeoDataPlacemark::LanduseMilitary] = "LanduseMilitary";
        visualCategoryNames[GeoDataPlacemark::LanduseQuarry] = "LanduseQuarry";
        visualCategoryNames[GeoDataPlacemark::LanduseRailway] = "LanduseRailway";
        visualCategoryNames[GeoDataPlacemark::LanduseReservoir] = "LanduseReservoir";
        visualCategoryNames[GeoDataPlacemark::LanduseResidential] = "LanduseResidential";
        visualCategoryNames[GeoDataPlacemark::LanduseRetail] = "LanduseRetail";
        visualCategoryNames[GeoDataPlacemark::LanduseOrchard] = "LanduseOrchard";
        visualCategoryNames[GeoDataPlacemark::LanduseVineyard] = "LanduseVineyard";
        visualCategoryNames[GeoDataPlacemark::RailwayRail] = "RailwayRail";
        visualCategoryNames[GeoDataPlacemark::RailwayNarrowGauge] = "RailwayNarrowGauge";
        visualCategoryNames[GeoDataPlacemark::RailwayTram] = "RailwayTram";
        visualCategoryNames[GeoDataPlacemark::RailwayLightRail] = "RailwayLightRail";
        visualCategoryNames[GeoDataPlacemark::RailwayAbandoned] = "RailwayAbandoned";
        visualCategoryNames[GeoDataPlacemark::RailwaySubway] = "RailwaySubway";
        visualCategoryNames[GeoDataPlacemark::RailwayPreserved] = "RailwayPreserved";
        visualCategoryNames[GeoDataPlacemark::RailwayMiniature] = "RailwayMiniature";
        visualCategoryNames[GeoDataPlacemark::RailwayConstruction] = "RailwayConstruction";
        visualCategoryNames[GeoDataPlacemark::RailwayMonorail] = "RailwayMonorail";
        visualCategoryNames[GeoDataPlacemark::RailwayFunicular] = "RailwayFunicular";
        visualCategoryNames[GeoDataPlacemark::PowerTower] = "PowerTower";
        visualCategoryNames[GeoDataPlacemark::Satellite] = "Satellite";
        visualCategoryNames[GeoDataPlacemark::Landmass] = "Landmass";
        visualCategoryNames[GeoDataPlacemark::UrbanArea] = "UrbanArea";
        visualCategoryNames[GeoDataPlacemark::InternationalDateLine] = "InternationalDateLine";
        visualCategoryNames[GeoDataPlacemark::Bathymetry] = "Bathymetry";
        visualCategoryNames[GeoDataPlacemark::AdminLevel1] = "AdminLevel1";
        visualCategoryNames[GeoDataPlacemark::AdminLevel2] = "AdminLevel2";
        visualCategoryNames[GeoDataPlacemark::AdminLevel3] = "AdminLevel3";
        visualCategoryNames[GeoDataPlacemark::AdminLevel4] = "AdminLevel4";
        visualCategoryNames[GeoDataPlacemark::AdminLevel5] = "AdminLevel5";
        visualCategoryNames[GeoDataPlacemark::AdminLevel6] = "AdminLevel6";
        visualCategoryNames[GeoDataPlacemark::AdminLevel7] = "AdminLevel7";
        visualCategoryNames[GeoDataPlacemark::AdminLevel8] = "AdminLevel8";
        visualCategoryNames[GeoDataPlacemark::AdminLevel9] = "AdminLevel9";
        visualCategoryNames[GeoDataPlacemark::AdminLevel10] = "AdminLevel10";
        visualCategoryNames[GeoDataPlacemark::AdminLevel11] = "AdminLevel11";
        visualCategoryNames[GeoDataPlacemark::BoundaryMaritime] = "BoundaryMaritime";
        visualCategoryNames[GeoDataPlacemark::LastIndex] = "LastIndex";
    }

    Q_ASSERT(visualCategoryNames.contains(category));
    return visualCategoryNames[category];
}

QHash<StyleBuilder::OsmTag, GeoDataPlacemark::GeoDataVisualCategory>::const_iterator StyleBuilder::begin()
{
    Private::initializeOsmVisualCategories();

    return Private::s_visualCategories.constBegin();
}

QHash<StyleBuilder::OsmTag, GeoDataPlacemark::GeoDataVisualCategory>::const_iterator StyleBuilder::end()
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

GeoDataPlacemark::GeoDataVisualCategory StyleBuilder::determineVisualCategory(const OsmPlacemarkData &osmData)
{
    if (osmData.containsTagKey(QStringLiteral("area:highway")) ||              // Not supported yet
            osmData.containsTag(QStringLiteral("boundary"), QStringLiteral("protected_area")) ||   // Not relevant for the default map
            osmData.containsTag(QStringLiteral("boundary"), QStringLiteral("postal_code")) ||
            osmData.containsTag(QStringLiteral("boundary"), QStringLiteral("aerial_views")) ||     // Created by OSM editor(s) application for digitalization
            osmData.containsTagKey(QStringLiteral("closed:highway")) ||
            osmData.containsTagKey(QStringLiteral("abandoned:highway")) ||
            osmData.containsTagKey(QStringLiteral("abandoned:natural")) ||
            osmData.containsTagKey(QStringLiteral("abandoned:building")) ||
            osmData.containsTagKey(QStringLiteral("abandoned:leisure")) ||
            osmData.containsTagKey(QStringLiteral("disused:highway")) ||
            osmData.containsTag(QStringLiteral("highway"), QStringLiteral("razed"))) {
        return GeoDataPlacemark::None;
    }

    QString const yes(QStringLiteral("yes"));
    if (osmData.containsTag(QStringLiteral("building"), yes)) {
        return GeoDataPlacemark::Building;
    }

    if (osmData.containsTag(QStringLiteral("historic"), QStringLiteral("castle")) && osmData.containsTag(QStringLiteral("castle_type"), QStringLiteral("kremlin"))) {
        return GeoDataPlacemark::None;
    }

    if( osmData.containsTag(QStringLiteral("natural"), QStringLiteral("glacier")) && osmData.containsTag(QStringLiteral("glacier:type"), QStringLiteral("shelf")) ){
        return GeoDataPlacemark::NaturalIceShelf;
    }

    Private::initializeOsmVisualCategories();

    QString const capital(QStringLiteral("capital"));
    for (auto iter = osmData.tagsBegin(), end=osmData.tagsEnd(); iter != end; ++iter) {
        const auto tag = OsmTag(iter.key(), iter.value());
        GeoDataPlacemark::GeoDataVisualCategory category = Private::s_visualCategories.value(tag, GeoDataPlacemark::None);
        if (category != GeoDataPlacemark::None) {
            if (category == GeoDataPlacemark::PlaceCity && osmData.containsTag(capital, yes)) {
                category = GeoDataPlacemark::PlaceCityCapital;
            } else if (category == GeoDataPlacemark::PlaceTown && osmData.containsTag(capital, yes)) {
                category = GeoDataPlacemark::PlaceTownCapital;
            } else if (category == GeoDataPlacemark::PlaceVillage && osmData.containsTag(capital, yes)) {
                category = GeoDataPlacemark::PlaceVillageCapital;
            }
        }

        if (category != GeoDataPlacemark::None) {
            return category;
        }
    }

    return GeoDataPlacemark::None;
}

StyleParameters::StyleParameters(const GeoDataPlacemark *placemark_, int tileLevel_) :
    placemark(placemark_),
    tileLevel(tileLevel_)
{
    // nothing to do
}

}
