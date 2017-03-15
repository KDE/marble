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
#include "OsmcSymbol.h"
#include "GeoDataGeometry.h"
#include "GeoDataLinearRing.h"
#include "GeoDataLineString.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
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
#include <QHash>
#include <QDebug>

namespace Marble
{

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
                                     const QColor &color = QColor(0xBE, 0xAD, 0xAD),
                                     const QColor &outline = QColor(0xBE, 0xAD, 0xAD).darker(),
                                     bool fill = true, bool renderOutline = true) const;
    GeoDataStyle::Ptr createOsmPOIStyle(const QFont &font, const QString &bitmap,
                                        const QColor &textColor = Qt::black,
                                        const QColor &color = QColor(0xBE, 0xAD, 0xAD),
                                        const QColor &outline = QColor(0xBE, 0xAD, 0xAD).darker()) const;
    GeoDataStyle::Ptr createOsmPOIRingStyle(const QFont &font, const QString &bitmap,
                                            const QColor &textColor = Qt::black,
                                            const QColor &color = QColor(0xBE, 0xAD, 0xAD),
                                            const QColor &outline = QColor(0xBE, 0xAD, 0xAD).darker()) const;
    GeoDataStyle::Ptr createOsmPOIAreaStyle(const QFont &font, const QString &bitmap,
                                            const QColor &textColor = Qt::black,
                                            const QColor &color = QColor(0xBE, 0xAD, 0xAD),
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
    GeoDataStyle::Ptr createIconWayStyle(const QColor& color, const QFont &font, const QColor &textColor, double lineWidth=1.0, const QString& iconPath = QString()) const;

    GeoDataStyle::ConstPtr createRelationStyle(const StyleParameters &parameters);
    GeoDataStyle::ConstPtr createPlacemarkStyle(const StyleParameters &parameters);
    GeoDataStyle::ConstPtr adjustPisteStyle(const StyleParameters &parameters, const GeoDataStyle::ConstPtr &style);
    void adjustWayWidth(const StyleParameters &parameters, GeoDataLineStyle &lineStyle) const;

    // Having an outline with the same color as the fill results in degraded
    // performance and degraded display quality for no good reason
    // Q_ASSERT( !(outline && color == outlineColor && brushStyle == Qt::SolidPattern) );
    void initializeDefaultStyles();

    static QString createPaintLayerItem(const QString &itemType, GeoDataPlacemark::GeoDataVisualCategory visualCategory, const QString &subType = QString());

    static void initializeOsmVisualCategories();
    static void initializeMinimumZoomLevels();

    int m_maximumZoomLevel;
    QColor m_defaultLabelColor;
    QFont m_defaultFont;
    GeoDataStyle::Ptr m_defaultStyle[GeoDataPlacemark::LastIndex];
    GeoDataStyle::Ptr m_styleTreeAutumn;
    GeoDataStyle::Ptr m_styleTreeWinter;
    bool m_defaultStyleInitialized;

    QHash<QString, GeoDataStyle::Ptr> m_styleCache;
    QHash<GeoDataPlacemark::GeoDataVisualCategory, GeoDataStyle::Ptr> m_buildingStyles;
    QSet<QLocale::Country> m_oceanianCountries;

    /**
     * @brief s_visualCategories contains osm tag mappings to GeoDataVisualCategories
     */
    static QHash<OsmTag, GeoDataPlacemark::GeoDataVisualCategory> s_visualCategories;
    static int s_defaultMinZoomLevels[GeoDataPlacemark::LastIndex];
    static bool s_defaultMinZoomLevelsInitialized;
    static QHash<GeoDataPlacemark::GeoDataVisualCategory, qint64> s_popularities;
};

QHash<StyleBuilder::OsmTag, GeoDataPlacemark::GeoDataVisualCategory> StyleBuilder::Private::s_visualCategories;
int StyleBuilder::Private::s_defaultMinZoomLevels[GeoDataPlacemark::LastIndex];
bool StyleBuilder::Private::s_defaultMinZoomLevelsInitialized = false;
QHash<GeoDataPlacemark::GeoDataVisualCategory, qint64> StyleBuilder::Private::s_popularities;

StyleBuilder::Private::Private() :
    m_maximumZoomLevel(15),
    m_defaultLabelColor(Qt::black),
    m_defaultFont(QStringLiteral("Sans Serif")),
    m_defaultStyle(),
    m_defaultStyleInitialized(false),
    m_oceanianCountries(
{
    QLocale::Australia, QLocale::NewZealand, QLocale::Fiji,
            QLocale::PapuaNewGuinea, QLocale::NewCaledonia, QLocale::SolomonIslands,
            QLocale::Samoa, QLocale::Vanuatu, QLocale::Guam,
            QLocale::FrenchPolynesia, QLocale::Tonga, QLocale::Palau,
            QLocale::Kiribati, QLocale::CookIslands, QLocale::Micronesia,
            QLocale::MarshallIslands, QLocale::NauruCountry,
            QLocale::AmericanSamoa, QLocale::Niue, QLocale::Pitcairn,
            QLocale::WallisAndFutunaIslands, QLocale::NorfolkIsland
})
{
#if QT_VERSION >= 0x050700
    m_oceanianCountries << QLocale::TuvaluCountry << QLocale::OutlyingOceania;
#else
    m_oceanianCountries << QLocale::Tuvalu;
#endif
    initializeMinimumZoomLevels();
    for (int i = 0; i < GeoDataPlacemark::LastIndex; ++i) {
        m_maximumZoomLevel = qMax(m_maximumZoomLevel, s_defaultMinZoomLevels[i]);
    }
}

GeoDataStyle::Ptr StyleBuilder::Private::createPOIStyle(const QFont &font, const QString &path,
        const QColor &textColor, const QColor &color, const QColor &outlineColor, bool fill, bool renderOutline) const
{
    GeoDataStyle::Ptr style =  createStyle(1, 0, color, outlineColor, fill, renderOutline, Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false, QVector<qreal>(), font);
    style->setIconStyle(GeoDataIconStyle(path));
    auto const screen = QApplication::screens().first();
    double const physicalSize = 6.0; // mm
    int const pixelSize = qRound(physicalSize * screen->physicalDotsPerInch() / (IN2M * M2MM));
    style->iconStyle().setSize(QSize(pixelSize, pixelSize));
    style->setLabelStyle(GeoDataLabelStyle(font, textColor));
    style->labelStyle().setAlignment(GeoDataLabelStyle::Center);
    return style;
}

GeoDataStyle::Ptr StyleBuilder::Private::createOsmPOIStyle(const QFont &font, const QString &imagePath,
        const QColor &textColor, const QColor &color, const QColor &outlineColor) const
{
    QString const path = MarbleDirs::path(QLatin1String("svg/osmcarto/svg/") + imagePath + QLatin1String(".svg"));
    return createPOIStyle(font, path, textColor, color, outlineColor, false, false);
}

GeoDataStyle::Ptr StyleBuilder::Private::createOsmPOIRingStyle(const QFont &font, const QString &imagePath,
        const QColor &textColor, const QColor &color, const QColor &outlineColor) const
{
    QString const path = MarbleDirs::path(QLatin1String("svg/osmcarto/svg/") + imagePath + QLatin1String(".svg"));
    return createPOIStyle(font, path, textColor, color, outlineColor, false, true);
}

GeoDataStyle::Ptr StyleBuilder::Private::createOsmPOIAreaStyle(const QFont &font, const QString &imagePath,
        const QColor &textColor, const QColor &color, const QColor &outlineColor) const
{
    QString const path = MarbleDirs::path(QLatin1String("svg/osmcarto/svg/") + imagePath + QLatin1String(".svg"));
    return createPOIStyle(font, path, textColor, color, outlineColor, true, false);
}


GeoDataStyle::Ptr StyleBuilder::Private::createHighwayStyle(const QColor& color, const QColor& outlineColor, const QFont& font,
        const QColor& fontColor, qreal width, qreal realWidth, Qt::PenStyle penStyle,
        Qt::PenCapStyle capStyle, bool lineBackground) const
{
    return createStyle(width, realWidth, color, outlineColor, true, true, Qt::SolidPattern, penStyle, capStyle, lineBackground, QVector< qreal >(), font, fontColor);
}

GeoDataStyle::Ptr StyleBuilder::Private::createWayStyle(const QColor& color, const QColor& outlineColor,
        bool fill, bool outline, Qt::BrushStyle brushStyle, const QString& texturePath) const
{
    return createStyle(1, 0, color, outlineColor, fill, outline, brushStyle, Qt::SolidLine, Qt::RoundCap, false, QVector<qreal>(), m_defaultFont, Qt::black, texturePath);
}

GeoDataStyle::Ptr StyleBuilder::Private::createIconWayStyle(const QColor &color, const QFont &font, const QColor &textColor, double lineWidth, const QString &iconPath) const
{
    auto const path = iconPath.isEmpty() ? iconPath : MarbleDirs::path(iconPath);
    auto style = createPOIStyle(font, path, textColor, color, color, true, true);
    style->lineStyle().setWidth(float(lineWidth));
    return style;
}

GeoDataStyle::ConstPtr StyleBuilder::Private::createRelationStyle(const StyleParameters &parameters)
{
    Q_ASSERT(parameters.relation);
    const GeoDataPlacemark *const placemark = parameters.placemark;
    auto const visualCategory = placemark->visualCategory();
    bool const isHighway = visualCategory >= GeoDataPlacemark::HighwaySteps && visualCategory <= GeoDataPlacemark::HighwayMotorway;
    bool const isRailway = visualCategory >= GeoDataPlacemark::RailwayRail && visualCategory <= GeoDataPlacemark::RailwayFunicular;
    if (isHighway || isRailway) {
        if (parameters.relation->relationType() == GeoDataRelation::RouteHiking &&
            parameters.relation->osmData().containsTagKey(QStringLiteral("osmc:symbol"))) {
            QString const osmcSymbolValue = parameters.relation->osmData().tagValue(QStringLiteral("osmc:symbol"));
            // Take cached Style instance if possible
            QString const cacheKey = QStringLiteral("/route/hiking/%1").arg(osmcSymbolValue);
            if (m_styleCache.contains(cacheKey)) {
                return m_styleCache[cacheKey];
            }

            auto style = presetStyle(visualCategory);
            auto lineStyle = style->lineStyle();
            if (isHighway) {
                adjustWayWidth(parameters, lineStyle);
            }
            auto iconStyle = style->iconStyle();
            GeoDataStyle::Ptr newStyle(new GeoDataStyle(*style));
            OsmcSymbol symbol = OsmcSymbol(osmcSymbolValue);
            lineStyle.setColor(symbol.wayColor());
            iconStyle.setIcon(symbol.icon());
            newStyle->setLineStyle(lineStyle);
            newStyle->setIconStyle(iconStyle);
            style = newStyle;
            m_styleCache.insert(cacheKey, newStyle);
            return style;
        }

        if (parameters.relation->relationType() >= GeoDataRelation::RouteRoad &&
            parameters.relation->relationType() <= GeoDataRelation::RouteInlineSkates) {
            auto const colorValue = parameters.relation->osmData().tagValue(QStringLiteral("colour"));
            QString color = colorValue;
            if (!QColor::isValidColor(colorValue)) {
                switch (parameters.relation->relationType()) {
                case GeoDataRelation::RouteTrain:
                    color = QStringLiteral("navy"); break;
                case GeoDataRelation::RouteSubway:
                    color = QStringLiteral("cornflowerblue"); break;
                case GeoDataRelation::RouteTram:
                    color = QStringLiteral("steelblue"); break;
                case GeoDataRelation::RouteBus:
                case GeoDataRelation::RouteTrolleyBus:
                    color = QStringLiteral("tomato"); break;
                case GeoDataRelation::RouteBicycle:
                case GeoDataRelation::RouteMountainbike:
                case GeoDataRelation::RouteFoot:
                case GeoDataRelation::RouteHiking:
                case GeoDataRelation::RouteHorse:
                case GeoDataRelation::RouteInlineSkates:
                    color = QStringLiteral("paleturquoise"); break;
                case GeoDataRelation::UnknownType:
                case GeoDataRelation:: RouteRoad:
                case GeoDataRelation::RouteDetour:
                case GeoDataRelation::RouteFerry:
                case GeoDataRelation::RouteSkiDownhill:
                case GeoDataRelation::RouteSkiNordic:
                case GeoDataRelation::RouteSkitour:
                case GeoDataRelation::RouteSled:
                    color = QString(); break;
                }
            }
            // Take cached Style instance if possible
            QString const cacheKey = QStringLiteral("/route/%1/%2").arg(parameters.relation->relationType()).arg(color);
            if (m_styleCache.contains(cacheKey)) {
                return m_styleCache[cacheKey];
            }

            auto style = presetStyle(visualCategory);
            auto lineStyle = style->lineStyle();
            if (isHighway) {
                adjustWayWidth(parameters, lineStyle);
            }
            GeoDataStyle::Ptr newStyle(new GeoDataStyle(*style));
            if (!color.isEmpty()) {
                lineStyle.setColor(QColor(color));
                auto labelStyle = style->labelStyle();
                labelStyle.setColor(GeoDataColorStyle::contrastColor(color));
                newStyle->setLabelStyle(labelStyle);
            }
            newStyle->setLineStyle(lineStyle);
            style = newStyle;
            m_styleCache.insert(cacheKey, newStyle);
            return style;
        }
    }
    return GeoDataStyle::ConstPtr();
}

GeoDataStyle::ConstPtr StyleBuilder::Private::createPlacemarkStyle(const StyleParameters &parameters)
{
    const GeoDataPlacemark *const placemark = parameters.placemark;
    QString styleCacheKey;

    OsmPlacemarkData const & osmData = placemark->osmData();
    auto const visualCategory = placemark->visualCategory();
    if (visualCategory == GeoDataPlacemark::Building) {
        auto const tagMap = osmTagMapping();
        auto const & osmData = placemark->osmData();
        auto const buildingTag = QStringLiteral("building");
        for (auto iter = osmData.tagsBegin(), end = osmData.tagsEnd(); iter != end; ++iter) {
            auto const osmTag = StyleBuilder::OsmTag(iter.key(), iter.value());
            if (iter.key() != buildingTag && tagMap.contains(osmTag)) {
                return m_buildingStyles.value(tagMap.value(osmTag), m_defaultStyle[visualCategory]);
            }
        }
    }

    GeoDataStyle::ConstPtr style = presetStyle(visualCategory);

    if (geodata_cast<GeoDataPoint>(placemark->geometry())) {
        if (visualCategory == GeoDataPlacemark::NaturalTree) {
            GeoDataCoordinates const coordinates = placemark->coordinate();
            qreal const lat = coordinates.latitude(GeoDataCoordinates::Degree);
            if (qAbs(lat) > 15) {
                /** @todo Should maybe auto-adjust to MarbleClock at some point */
                int const month = QDate::currentDate().month();
                bool const southernHemisphere = lat < 0;
                if (southernHemisphere) {
                    if (month >= 3 && month <= 5) {
                        style = m_styleTreeAutumn;
                    } else if (month >= 6 && month <= 8) {
                        style = m_styleTreeWinter;
                    }
                } else {
                    if (month >= 9 && month <= 11) {
                        style = m_styleTreeAutumn;
                    } else if (month == 12 || month == 1 || month == 2) {
                        style = m_styleTreeWinter;
                    }
                }
            }
        }
    } else if (geodata_cast<GeoDataLinearRing>(placemark->geometry())) {
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
        } else if (visualCategory == GeoDataPlacemark::Bathymetry) {
            auto tagIter = osmData.findTag(QStringLiteral("ele"));
            if (tagIter != osmData.tagsEnd()) {
                const QString& elevation = tagIter.value();
                if (elevation == QLatin1String("4000")) {
                    polyStyle.setColor("#94c2c2");
                    lineStyle.setColor("#94c2c2");
                    adjustStyle = true;
                }
            }
        } else if (visualCategory == GeoDataPlacemark::AmenityGraveyard || visualCategory == GeoDataPlacemark::LanduseCemetery) {
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
        } else if (visualCategory == GeoDataPlacemark::PisteDownhill) {
            return adjustPisteStyle(parameters, style);
        }

        if (adjustStyle) {
            GeoDataStyle::Ptr newStyle(new GeoDataStyle(*style));
            newStyle->setPolyStyle(polyStyle);
            newStyle->setLineStyle(lineStyle);
            style = newStyle;
        }

        if (style->iconStyle().iconPath().isEmpty()) {
            const GeoDataPlacemark::GeoDataVisualCategory category = determineVisualCategory(osmData);
            const GeoDataStyle::ConstPtr categoryStyle = presetStyle(category);
            if (category != GeoDataPlacemark::None && !categoryStyle->iconStyle().scaledIcon().isNull()) {
                GeoDataStyle::Ptr newStyle(new GeoDataStyle(*style));
                newStyle->setIconStyle(categoryStyle->iconStyle());
                style = newStyle;
            }
        }
    } else if (geodata_cast<GeoDataLineString>(placemark->geometry())) {
        GeoDataPolyStyle polyStyle = style->polyStyle();
        GeoDataLineStyle lineStyle = style->lineStyle();
        GeoDataLabelStyle labelStyle = style->labelStyle();
        GeoDataIconStyle iconStyle = style->iconStyle();
        lineStyle.setCosmeticOutline(true);

        bool adjustStyle = false;

        if (visualCategory == GeoDataPlacemark::AdminLevel2) {
            if (osmData.containsTag(QStringLiteral("maritime"), QStringLiteral("yes"))) {
                lineStyle.setColor("#88b3bf");
                polyStyle.setColor("#88b3bf");
                if (osmData.containsTag(QStringLiteral("marble:disputed"), QStringLiteral("yes"))) {
                    lineStyle.setPenStyle(Qt::DashLine);
                }
                adjustStyle = true;
            }
        } else if ((visualCategory >= GeoDataPlacemark::HighwayService &&
                    visualCategory <= GeoDataPlacemark::HighwayMotorway) ||
                   visualCategory == GeoDataPlacemark::TransportAirportRunway) {
            // Take cached Style instance if possible
            styleCacheKey = QStringLiteral("%1/%2").arg(parameters.tileLevel).arg(visualCategory);
            if (m_styleCache.contains(styleCacheKey)) {
                style = m_styleCache[styleCacheKey];
                return style;
            }

            adjustStyle = true;
            styleCacheKey = QStringLiteral("%1/%2").arg(parameters.tileLevel).arg(visualCategory);
            adjustWayWidth(parameters, lineStyle);

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

            if (osmData.containsTag("tunnel", "yes")) {
                QColor polyColor = polyStyle.color();
                qreal hue, sat, val;
                polyColor.getHsvF(&hue, &sat, &val);
                polyColor.setHsvF(hue, 0.25 * sat, 0.95 * val);
                polyStyle.setColor(polyColor);
                lineStyle.setColor(lineStyle.color().lighter(115));
            }

        } else if (visualCategory >= GeoDataPlacemark::WaterwayCanal && visualCategory <= GeoDataPlacemark::WaterwayStream) {

            adjustStyle = true;

            // Take cached Style instance if possible
            styleCacheKey = QStringLiteral("%1/%2").arg(parameters.tileLevel).arg(visualCategory);
            if (m_styleCache.contains(styleCacheKey)) {
                style = m_styleCache[styleCacheKey];
                return style;
            }


            if (parameters.tileLevel <= 3) {
                lineStyle.setWidth(1);
                lineStyle.setPhysicalWidth(0.0);
                styleCacheKey = QStringLiteral("%1/%2").arg(parameters.tileLevel).arg(visualCategory);
            } else if (parameters.tileLevel <= 7) {
                lineStyle.setWidth(2);
                lineStyle.setPhysicalWidth(0.0);
                styleCacheKey = QStringLiteral("%1/%2").arg(parameters.tileLevel).arg(visualCategory);
            } else {
                QString const widthValue = osmData.tagValue(QStringLiteral("width")).remove(QStringLiteral(" meters")).remove(QStringLiteral(" m"));
                bool ok;
                float const width = widthValue.toFloat(&ok);
                lineStyle.setPhysicalWidth(ok ? qBound(0.1f, width, 200.0f) : 0.0f);
            }
        } else if (visualCategory == GeoDataPlacemark::PisteDownhill) {
            return adjustPisteStyle(parameters, style);
        }

        if (adjustStyle) {
            GeoDataStyle::Ptr newStyle(new GeoDataStyle(*style));
            newStyle->setPolyStyle(polyStyle);
            newStyle->setLineStyle(lineStyle);
            newStyle->setLabelStyle(labelStyle);
            newStyle->setIconStyle(iconStyle);
            style = newStyle;
            if (!styleCacheKey.isEmpty()) {
                m_styleCache.insert(styleCacheKey, newStyle);
            }
        }


    } else if (geodata_cast<GeoDataPolygon>(placemark->geometry())) {
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
        } else if (visualCategory == GeoDataPlacemark::PisteDownhill) {
            return adjustPisteStyle(parameters, style);
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

GeoDataStyle::ConstPtr StyleBuilder::Private::adjustPisteStyle(const StyleParameters &parameters, const GeoDataStyle::ConstPtr &style)
{
    // Take cached Style instance if possible
    auto const & osmData = parameters.placemark->osmData();
    auto const visualCategory = parameters.placemark->visualCategory();
    auto const difficulty = osmData.tagValue("piste:difficulty");
    QString styleCacheKey = QStringLiteral("piste/%1/%2").arg(visualCategory).arg(difficulty);
    if (m_styleCache.contains(styleCacheKey)) {
        return m_styleCache[styleCacheKey];
    }

    GeoDataLineStyle lineStyle = style->lineStyle();

    auto green = QColor("#006600");;
    auto red = QColor("#cc0000");
    auto black = QColor("#151515");
    auto yellow = Qt::yellow;
    auto blue = QColor("#000099");
    auto orange = QColor(255, 165, 0);
    auto fallBack = Qt::lightGray;
    auto country = QLocale::system().country();
    if (country == QLocale::Japan) {
        if (difficulty == "easy") {
            lineStyle.setColor(green);
        } else if (difficulty == "intermediate") {
            lineStyle.setColor(red);
        } else if (difficulty == "advanced") {
            lineStyle.setColor(black);
        } else {
            lineStyle.setColor(fallBack);
        }
    } else if (country == QLocale::UnitedStates ||
               country == QLocale::UnitedStatesMinorOutlyingIslands ||
               country == QLocale::Canada ||
               m_oceanianCountries.contains(country)) {
        if (difficulty == "easy") {
            lineStyle.setColor(green);
        } else if (difficulty == "intermediate") {
            lineStyle.setColor(blue);
        } else if (difficulty == "advanced" || difficulty == "expert") {
            lineStyle.setColor(black);
        } else {
            lineStyle.setColor(fallBack);
        }
        // fallback on Europe
    } else {
        if (difficulty == "novice") {
            lineStyle.setColor(green);
        } else if (difficulty == "easy") {
            lineStyle.setColor(blue);
        } else if (difficulty == "intermediate") {
            lineStyle.setColor(red);
        } else if (difficulty == "advanced") {
            lineStyle.setColor(black);
        } else if (difficulty == "expert") {
            // scandinavian countries have different colors then the rest of Europe
            if (country == QLocale::Denmark ||
                country == QLocale::Norway ||
                country == QLocale::Sweden) {
                lineStyle.setColor(black);
            } else {
                lineStyle.setColor(orange);
            }
        } else if (difficulty == "freeride") {
            lineStyle.setColor(yellow);
        } else {
            lineStyle.setColor(fallBack);
        }
    }

    GeoDataPolyStyle polyStyle = style->polyStyle();
    polyStyle.setColor(lineStyle.color());
    GeoDataStyle::Ptr newStyle(new GeoDataStyle(*style));
    newStyle->setPolyStyle(polyStyle);
    newStyle->setLineStyle(lineStyle);
    m_styleCache.insert(styleCacheKey, newStyle);
    return newStyle;
}

void StyleBuilder::Private::adjustWayWidth(const StyleParameters &parameters, GeoDataLineStyle &lineStyle) const
{
    auto const & osmData = parameters.placemark->osmData();
    auto const visualCategory = parameters.placemark->visualCategory();
    if (parameters.tileLevel <= 8) {
        lineStyle.setPhysicalWidth(0.0);
        lineStyle.setWidth(2.0);
    } else if (parameters.tileLevel <= 10) {
        lineStyle.setPhysicalWidth(0.0);
        lineStyle.setWidth(3.0);
    } else if (parameters.tileLevel <= 12) {
        lineStyle.setPhysicalWidth(0.0);
        lineStyle.setWidth(4.0);
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
            double const margins = visualCategory == GeoDataPlacemark::HighwayMotorway ? 2.0 : (isOneWay ? 1.0 : 0.0);
            double const physicalWidth = margins + lanes * laneWidth;
            lineStyle.setPhysicalWidth(physicalWidth);
        }
    }
}

GeoDataStyle::Ptr StyleBuilder::Private::createStyle(qreal width, qreal realWidth, const QColor& color,
        const QColor& outlineColor, bool fill, bool outline, Qt::BrushStyle brushStyle, Qt::PenStyle penStyle,
        Qt::PenCapStyle capStyle, bool lineBackground, const QVector< qreal >& dashPattern,
        const QFont& font, const QColor& fontColor, const QString& texturePath) const
{
    GeoDataStyle *style = new GeoDataStyle;
    GeoDataLineStyle lineStyle(outlineColor);
    lineStyle.setCapStyle(capStyle);
    lineStyle.setPenStyle(penStyle);
    lineStyle.setWidth(width);
    lineStyle.setPhysicalWidth(realWidth);
    lineStyle.setBackground(lineBackground);
    lineStyle.setDashPattern(dashPattern);
    GeoDataPolyStyle polyStyle(color);
    polyStyle.setOutline(outline);
    polyStyle.setFill(fill);
    polyStyle.setBrushStyle(brushStyle);
    polyStyle.setTexturePath(texturePath);
    GeoDataLabelStyle labelStyle(font, fontColor);
    style->setLineStyle(lineStyle);
    style->setPolyStyle(polyStyle);
    style->setLabelStyle(labelStyle);
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
        = GeoDataStyle::Ptr(new GeoDataStyle(QString(),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::Default]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/default_location.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::Unknown]
        = GeoDataStyle::Ptr(new GeoDataStyle(QString(),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::SmallCity]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/city_4_white.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::SmallCountyCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/city_4_yellow.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::SmallStateCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/city_4_orange.png"),
                            QFont(defaultFamily, defaultSize, 50, true), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::SmallNationCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/city_4_red.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::MediumCity]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/city_3_white.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::MediumCountyCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/city_3_yellow.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::MediumStateCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/city_3_orange.png"),
                            QFont(defaultFamily, defaultSize, 50, true), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::MediumNationCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/city_3_red.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::BigCity]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/city_2_white.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::BigCountyCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/city_2_yellow.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::BigStateCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/city_2_orange.png"),
                            QFont(defaultFamily, defaultSize, 50, true), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::BigNationCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/city_2_red.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::LargeCity]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/city_1_white.png"),
                            QFont(defaultFamily, defaultSize, 75, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::LargeCountyCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/city_1_yellow.png"),
                            QFont(defaultFamily, defaultSize, 75, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::LargeStateCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/city_1_orange.png"),
                            QFont(defaultFamily, defaultSize, 75, true), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::LargeNationCapital]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/city_1_red.png"),
                            QFont(defaultFamily, defaultSize, 75, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::Nation]
        = GeoDataStyle::Ptr(new GeoDataStyle(QString(),
                            QFont(defaultFamily, int(defaultSize * 1.5), 75, false), QColor("#404040")));
    // Align area labels centered
    m_defaultStyle[GeoDataPlacemark::Nation]->labelStyle().setAlignment(GeoDataLabelStyle::Center);

    QFont osmCityFont = QFont(defaultFamily, int(defaultSize * 1.5), 75, false);
    m_defaultStyle[GeoDataPlacemark::PlaceCity] = createOsmPOIStyle(osmCityFont, "place/place-6", QColor("#202020"));
    m_defaultStyle[GeoDataPlacemark::PlaceCityCapital] = createOsmPOIStyle(osmCityFont, "place/place-capital-6", QColor("#202020"));
    m_defaultStyle[GeoDataPlacemark::PlaceCityNationalCapital] = createOsmPOIStyle(osmCityFont, "place/place-capital-adminlevel2", QColor("#202020"));
    m_defaultStyle[GeoDataPlacemark::PlaceSuburb] = createOsmPOIStyle(osmCityFont, QString(), QColor("#707070"));
    m_defaultStyle[GeoDataPlacemark::PlaceHamlet] = createOsmPOIStyle(osmCityFont, QString(), QColor("#707070"));
    QFont localityFont = osmCityFont;
    localityFont.setPointSize(defaultSize);
    m_defaultStyle[GeoDataPlacemark::PlaceLocality] = createOsmPOIStyle(localityFont, QString(), QColor("#707070"));
    m_defaultStyle[GeoDataPlacemark::PlaceTown] = createOsmPOIStyle(osmCityFont, "place/place-6", QColor("#404040"));
    m_defaultStyle[GeoDataPlacemark::PlaceTownCapital] = createOsmPOIStyle(osmCityFont, "place/place-capital-6", QColor("#404040"));
    m_defaultStyle[GeoDataPlacemark::PlaceTownNationalCapital] = createOsmPOIStyle(osmCityFont, "place/place-capital-adminlevel2", QColor("#404040"));
    m_defaultStyle[GeoDataPlacemark::PlaceVillage] = createOsmPOIStyle(osmCityFont, "place/place-6", QColor("#505050"));
    m_defaultStyle[GeoDataPlacemark::PlaceVillageCapital] = createOsmPOIStyle(osmCityFont, "place/place-capital-6", QColor("#505050"));
    m_defaultStyle[GeoDataPlacemark::PlaceVillageNationalCapital] = createOsmPOIStyle(osmCityFont, "place/place-capital-adminlevel2", QColor("#505050"));
    for (int i = GeoDataPlacemark::PlaceCity; i <= GeoDataPlacemark::PlaceVillageCapital; ++i) {
        m_defaultStyle[GeoDataPlacemark::GeoDataVisualCategory(i)]->polyStyle().setFill(false);
        m_defaultStyle[GeoDataPlacemark::GeoDataVisualCategory(i)]->polyStyle().setOutline(false);
        m_defaultStyle[GeoDataPlacemark::GeoDataVisualCategory(i)]->labelStyle().setAlignment(GeoDataLabelStyle::Center);

        if (i == GeoDataPlacemark::PlaceCityNationalCapital || i == GeoDataPlacemark::PlaceTownNationalCapital || i == GeoDataPlacemark::PlaceVillageNationalCapital) {
            m_defaultStyle[GeoDataPlacemark::GeoDataVisualCategory(i)]->iconStyle().setScale(0.55f);
        } else {
            m_defaultStyle[GeoDataPlacemark::GeoDataVisualCategory(i)]->iconStyle().setScale(0.25);
        }
    }

    m_defaultStyle[GeoDataPlacemark::Mountain]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/mountain_1.png"),
                            QFont(defaultFamily, int(defaultSize * 0.9), 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::Volcano]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/volcano_1.png"),
                            QFont(defaultFamily, int(defaultSize * 0.9), 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::Mons]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/mountain_1.png"),
                            QFont(defaultFamily, int(defaultSize * 0.9), 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::Valley]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/valley.png"),
                            QFont(defaultFamily, int(defaultSize * 0.9), 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::Continent]
        = GeoDataStyle::Ptr(new GeoDataStyle(QString(),
                            QFont(defaultFamily, int(defaultSize * 1.7), 50, false), QColor("#bf0303")));
    // Align area labels centered
    m_defaultStyle[GeoDataPlacemark::Continent]->labelStyle().setAlignment(GeoDataLabelStyle::Center);

    m_defaultStyle[GeoDataPlacemark::Ocean]
        = GeoDataStyle::Ptr(new GeoDataStyle(QString(),
                            QFont(defaultFamily, int(defaultSize * 1.7), 50, true), QColor("#2c72c7")));
    // Align area labels centered
    m_defaultStyle[GeoDataPlacemark::Ocean]->labelStyle().setAlignment(GeoDataLabelStyle::Center);

    m_defaultStyle[GeoDataPlacemark::OtherTerrain]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/other.png"),
                            QFont(defaultFamily, int(defaultSize * 0.9), 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::Crater]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/crater.png"),
                            QFont(defaultFamily, int(defaultSize * 0.9), 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::Mare]
        = GeoDataStyle::Ptr(new GeoDataStyle(QString(),
                            QFont(defaultFamily, int(defaultSize * 1.7), 50, false), QColor("#bf0303")));
    // Align area labels centered
    m_defaultStyle[GeoDataPlacemark::Mare]->labelStyle().setAlignment(GeoDataLabelStyle::Center);

    m_defaultStyle[GeoDataPlacemark::GeographicPole]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/pole_1.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::MagneticPole]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/pole_2.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::ShipWreck]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/shipwreck.png"),
                            QFont(defaultFamily, int(defaultSize * 0.8), 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::AirPort]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/airport.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::Observatory]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/observatory.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::OsmSite]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/osm.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::Coordinate]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/coordinate.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));


    m_defaultStyle[GeoDataPlacemark::MannedLandingSite]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/manned_landing.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::RoboticRover]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/robotic_rover.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::UnmannedSoftLandingSite]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/unmanned_soft_landing.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::UnmannedHardLandingSite]
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/unmanned_hard_landing.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    m_defaultStyle[GeoDataPlacemark::Bookmark]
        = createPOIStyle(QFont(defaultFamily, defaultSize, 50, false),
                         MarbleDirs::path("svg/bookmark.svg"), defaultLabelColor);
    m_defaultStyle[GeoDataPlacemark::Bookmark]->iconStyle().setScale(0.75);

    QColor const shopColor("#ac39ac");
    QColor const transportationColor("#0092da");
    QColor const amenityColor("#734a08");
    QColor const healthColor("#da0092");
    QColor const airTransportColor("#8461C4");
    QColor const educationalAreasAndHospital("#f0f0d8");
    QColor const buildingColor("#beadad");
    QColor const waterColor("#b5d0d0");
    // Allows to visualize multiple repaints of buildings
//    QColor const buildingColor(0, 255, 0, 64);

    QFont const osmFont(defaultFamily, 10, 50, false);
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

    m_defaultStyle[GeoDataPlacemark::HistoricArchaeologicalSite] = createOsmPOIAreaStyle(osmFont, "amenity/archaeological_site.16", amenityColor, Qt::transparent);
    m_defaultStyle[GeoDataPlacemark::AmenityEmbassy]           = createOsmPOIStyle(osmFont, "transportation/embassy.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::AmenityEmergencyPhone]    = createOsmPOIStyle(osmFont, "amenity/emergency_phone.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityMountainRescue]    = createOsmPOIStyle(osmFont, "amenity/mountain_rescue.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::LeisureWaterPark]         = createOsmPOIStyle(osmFont, "amenity/water_park.16", amenityColor);
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
    m_defaultStyle[GeoDataPlacemark::AmenityShelter]           = createOsmPOIStyle(osmFont, "transportation/shelter", transportationColor);
    m_defaultStyle[GeoDataPlacemark::AmenityTelephone]         = createOsmPOIStyle(osmFont, "amenity/telephone.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityToilets]           = createOsmPOIStyle(osmFont, "amenity/toilets.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityTownHall]          = createOsmPOIStyle(osmFont, "amenity/town_hall.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityWasteBasket]       = createOsmPOIStyle(osmFont, "individual/waste_basket.10", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityWasteBasket]->iconStyle().setScale(0.75);
    m_defaultStyle[GeoDataPlacemark::AmenityChargingStation]   = createOsmPOIStyle(osmFont, "transportation/charging_station");
    m_defaultStyle[GeoDataPlacemark::AmenityCarWash]           = createOsmPOIStyle(osmFont, "amenity/car_wash", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenitySocialFacility]    = createOsmPOIStyle(osmFont, "amenity/social_facility", amenityColor);

    m_defaultStyle[GeoDataPlacemark::AmenityDrinkingWater]     = createOsmPOIStyle(osmFont, "amenity/drinking_water.16", amenityColor);

    m_defaultStyle[GeoDataPlacemark::NaturalPeak]              = createOsmPOIStyle(osmFont, "individual/peak", amenityColor);
    m_defaultStyle[GeoDataPlacemark::NaturalPeak]->iconStyle().setScale(0.33f);
    m_defaultStyle[GeoDataPlacemark::NaturalVolcano]           = createOsmPOIStyle(osmFont, "individual/volcano", amenityColor);
    m_defaultStyle[GeoDataPlacemark::NaturalVolcano]->iconStyle().setScale(0.33f);
    m_defaultStyle[GeoDataPlacemark::NaturalTree]              = createOsmPOIStyle(osmFont, "individual/tree-29", amenityColor); // tree-16 provides the official icon
    m_styleTreeAutumn                                          = createOsmPOIStyle(osmFont, "individual/tree-29-autumn", amenityColor);
    m_styleTreeWinter                                          = createOsmPOIStyle(osmFont, "individual/tree-29-winter", amenityColor);
    qreal const treeIconScale = 0.75;
    m_defaultStyle[GeoDataPlacemark::NaturalTree]->iconStyle().setScale(treeIconScale);
    m_styleTreeAutumn->iconStyle().setScale(treeIconScale);
    m_styleTreeWinter->iconStyle().setScale(treeIconScale);

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
    m_defaultStyle[GeoDataPlacemark::ShopSports]               = createOsmPOIStyle(osmFont, "shop/sports", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopCopy]                 = createOsmPOIStyle(osmFont, "shop/copyshop", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopArt]                  = createOsmPOIStyle(osmFont, "shop/art", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopMusicalInstrument]    = createOsmPOIStyle(osmFont, "shop/musical_instrument-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopPhoto]                = createOsmPOIStyle(osmFont, "shop/photo-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopBook]                 = createOsmPOIStyle(osmFont, "shop/shop_books.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopGift]                 = createOsmPOIStyle(osmFont, "shop/shop_gift.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopStationery]           = createOsmPOIStyle(osmFont, "shop/stationery-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopLaundry]              = createOsmPOIStyle(osmFont, "shop/laundry-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopPet]                  = createOsmPOIStyle(osmFont, "shop/shop_pet.16", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopToys]                 = createOsmPOIStyle(osmFont, "shop/toys-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopTravelAgency]         = createOsmPOIStyle(osmFont, "shop/travel_agency-14", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopDeli]                 = createOsmPOIStyle(osmFont, "shop/deli", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopTobacco]              = createOsmPOIStyle(osmFont, "shop/tobacco", shopColor);
    m_defaultStyle[GeoDataPlacemark::ShopTea]                  = createOsmPOIStyle(osmFont, "shop/tea", shopColor);
    m_defaultStyle[GeoDataPlacemark::Shop]                     = createOsmPOIStyle(osmFont, "shop/shop-14", shopColor);

    m_defaultStyle[GeoDataPlacemark::ManmadeBridge]            = createWayStyle(QColor("#b8b8b8"), Qt::transparent, true, true);
    m_defaultStyle[GeoDataPlacemark::ManmadeLighthouse]        = createOsmPOIStyle(osmFont, "transportation/lighthouse.16", transportationColor, "#f2efe9", QColor("#f2efe9").darker());
    m_defaultStyle[GeoDataPlacemark::ManmadePier]              = createStyle(0.0, 3.0, "#f2efe9", "#f2efe9", true, false, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont);
    m_defaultStyle[GeoDataPlacemark::ManmadeWaterTower]        = createOsmPOIStyle(osmFont, "amenity/water_tower.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::ManmadeWindMill]          = createOsmPOIStyle(osmFont, "amenity/windmill.16", amenityColor);

    m_defaultStyle[GeoDataPlacemark::HistoricCastle]           = createOsmPOIRingStyle(osmFont, "amenity/cinema.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::HistoricMemorial]         = createOsmPOIStyle(osmFont, "amenity/tourist_memorial.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::HistoricMonument]         = createOsmPOIStyle(osmFont, "amenity/monument.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::HistoricRuins]             = createOsmPOIRingStyle(osmFont, QString(), amenityColor);

    m_defaultStyle[GeoDataPlacemark::TourismAttraction]        = createOsmPOIStyle(osmFont, "amenity/tourist_memorial.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::TourismArtwork]           = createOsmPOIStyle(osmFont, "amenity/artwork", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityCinema]            = createOsmPOIStyle(osmFont, "amenity/cinema.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::TourismInformation]       = createOsmPOIStyle(osmFont, "amenity/information.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::TourismMuseum]            = createOsmPOIStyle(osmFont, "amenity/museum.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::AmenityTheatre]           = createOsmPOIStyle(osmFont, "amenity/theatre.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::TourismThemePark]         = createOsmPOIStyle(osmFont, QString(), amenityColor);
    m_defaultStyle[GeoDataPlacemark::TourismViewPoint]         = createOsmPOIStyle(osmFont, "amenity/viewpoint.16", amenityColor);
    m_defaultStyle[GeoDataPlacemark::TourismZoo]               = createOsmPOIRingStyle(osmFont, QString(), amenityColor, Qt::transparent);
    m_defaultStyle[GeoDataPlacemark::TourismAlpineHut]         = createOsmPOIStyle(osmFont, "transportation/alpinehut.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TourismWildernessHut]     = createOsmPOIStyle(osmFont, "transportation/wilderness_hut", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportAerodrome]       = createOsmPOIStyle(osmFont, "airtransport/aerodrome", airTransportColor);
    m_defaultStyle[GeoDataPlacemark::TransportHelipad]         = createOsmPOIStyle(osmFont, "airtransport/helipad", airTransportColor);
    m_defaultStyle[GeoDataPlacemark::TransportAirportTerminal] = createOsmPOIAreaStyle(osmFont, QString(), airTransportColor);
    m_defaultStyle[GeoDataPlacemark::TransportAirportGate]     = createOsmPOIAreaStyle(osmFont, QString(), airTransportColor);
    m_defaultStyle[GeoDataPlacemark::TransportBusStation]      = createOsmPOIStyle(osmFont, "transportation/bus_station.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportBusStop]         = createOsmPOIStyle(osmFont, "transportation/bus_stop.12", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportCarShare]        = createOsmPOIStyle(osmFont, "transportation/car_share.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportFuel]            = createOsmPOIStyle(osmFont, "transportation/fuel.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportParking]         = createOsmPOIAreaStyle(osmFont, "transportation/parking", transportationColor, "#F6EEB6", QColor("#F6EEB6").darker());
    m_defaultStyle[GeoDataPlacemark::TransportBicycleParking]  = createOsmPOIAreaStyle(osmFont, "transportation/bicycle_parking.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportMotorcycleParking] = createOsmPOIAreaStyle(osmFont, "transportation/motorcycle_parking.16", transportationColor);
    qreal const parkingIconScale = 0.75;
    m_defaultStyle[GeoDataPlacemark::TransportParking]->iconStyle().setScale(parkingIconScale);
    m_defaultStyle[GeoDataPlacemark::TransportBicycleParking]->iconStyle().setScale(parkingIconScale);
    m_defaultStyle[GeoDataPlacemark::TransportMotorcycleParking]->iconStyle().setScale(parkingIconScale);
    m_defaultStyle[GeoDataPlacemark::TransportParkingSpace]    = createWayStyle("#F6EEB6", QColor("#F6EEB6").darker(), true, true);
    m_defaultStyle[GeoDataPlacemark::TransportPlatform]        = createWayStyle("#bbbbbb", Qt::transparent, true, false);
    m_defaultStyle[GeoDataPlacemark::TransportTrainStation]    = createOsmPOIStyle(osmFont, "individual/railway_station", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportTrainStation]->iconStyle().setScale(0.5);
    m_defaultStyle[GeoDataPlacemark::TransportTramStop]        = createOsmPOIStyle(osmFont, "individual/railway_station", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportTramStop]->iconStyle().setScale(0.33f);
    m_defaultStyle[GeoDataPlacemark::TransportRentalBicycle]   = createOsmPOIStyle(osmFont, "transportation/rental_bicycle.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportRentalCar]       = createOsmPOIStyle(osmFont, "transportation/rental_car.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportRentalSki]       = createOsmPOIStyle(osmFont, "transportation/rental_ski.16", transportationColor);
    m_defaultStyle[GeoDataPlacemark::TransportTaxiRank]        = createOsmPOIStyle(osmFont, "transportation/taxi.16", transportationColor);
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

    m_defaultStyle[GeoDataPlacemark::PowerTower]               = createOsmPOIStyle(osmFont, "individual/power_tower", QColor("#888888"));
    m_defaultStyle[GeoDataPlacemark::PowerTower]->iconStyle().setScale(0.6f);

    m_defaultStyle[GeoDataPlacemark::BarrierCityWall]          = createStyle(6.0, 3.0, "#787878", Qt::transparent, true, false, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, Qt::transparent);
    m_defaultStyle[GeoDataPlacemark::BarrierGate]              = createOsmPOIStyle(osmFont, "individual/gate");
    m_defaultStyle[GeoDataPlacemark::BarrierGate]->iconStyle().setScale(0.75);
    m_defaultStyle[GeoDataPlacemark::BarrierLiftGate]          = createOsmPOIStyle(osmFont, "individual/liftgate");
    m_defaultStyle[GeoDataPlacemark::BarrierLiftGate]->iconStyle().setScale(0.75);
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
    m_defaultStyle[GeoDataPlacemark::HighwayRaceway]           = createHighwayStyle("#ffc0cb", "#ffc0cb", osmFont, "000000", 1, 5);
    m_defaultStyle[GeoDataPlacemark::HighwayTrunk]             = createHighwayStyle("#f9b29c", "#c84e2f", osmFont, "000000", 9, 6);
    m_defaultStyle[GeoDataPlacemark::HighwayTrunkLink]         = createHighwayStyle("#f9b29c", "#c84e2f", osmFont, "000000", 9, 6);
    m_defaultStyle[GeoDataPlacemark::HighwayMotorway]          = createHighwayStyle("#e892a2", "#dc2a67", osmFont, "000000", 9, 10);
    m_defaultStyle[GeoDataPlacemark::HighwayMotorwayLink]      = createHighwayStyle("#e892a2", "#dc2a67", osmFont, "000000", 9, 10);
    m_defaultStyle[GeoDataPlacemark::HighwayCorridor]          = createHighwayStyle("#ffffff", "#bbbbbb", osmFont, "000000", 1, 3);
    m_defaultStyle[GeoDataPlacemark::TransportAirportRunway]   = createHighwayStyle("#bbbbcc", "#bbbbcc", osmFont, "000000", 0, 1, Qt::NoPen);
    m_defaultStyle[GeoDataPlacemark::TransportAirportTaxiway]  = createHighwayStyle("#bbbbcc", "#bbbbcc", osmFont, "000000", 0, 1, Qt::NoPen);
    m_defaultStyle[GeoDataPlacemark::TransportAirportApron]    = createWayStyle("#e9d1ff", Qt::transparent, true, false);
    m_defaultStyle[GeoDataPlacemark::TransportSpeedCamera]     = createOsmPOIStyle(osmFont, "individual/speedcamera");

    m_defaultStyle[GeoDataPlacemark::NaturalWater]             = createStyle(4, 0, waterColor, waterColor, true, false,
            Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false, QVector< qreal >(),
            osmFont, waterColor.darker(150));
    m_defaultStyle[GeoDataPlacemark::WaterwayRiver]            = createStyle(4, 0, waterColor, waterColor, true, false,
            Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false, QVector< qreal >(),
            osmFont, waterColor.darker(150));
    m_defaultStyle[GeoDataPlacemark::WaterwayCanal]           = m_defaultStyle[GeoDataPlacemark::WaterwayRiver];
    m_defaultStyle[GeoDataPlacemark::WaterwayDitch]           = m_defaultStyle[GeoDataPlacemark::WaterwayRiver];
    m_defaultStyle[GeoDataPlacemark::WaterwayDrain]           = m_defaultStyle[GeoDataPlacemark::WaterwayRiver];
    m_defaultStyle[GeoDataPlacemark::WaterwayStream]          = m_defaultStyle[GeoDataPlacemark::WaterwayRiver];
    m_defaultStyle[GeoDataPlacemark::WaterwayWeir]            = createStyle(4, 0, "#ffffff", "#87939b", true, false,
            Qt::SolidPattern, Qt::DotLine, Qt::RoundCap, true, QVector< qreal >(),
            osmFont, waterColor.darker(150));

    m_defaultStyle[GeoDataPlacemark::CrossingIsland]          = createOsmPOIStyle(osmFont, "transportation/zebra_crossing", transportationColor);
    m_defaultStyle[GeoDataPlacemark::CrossingIsland]->iconStyle().setScale(0.75);
    m_defaultStyle[GeoDataPlacemark::CrossingRailway]         = createOsmPOIStyle(osmFont, "transportation/railway_crossing", transportationColor);
    m_defaultStyle[GeoDataPlacemark::CrossingRailway]->iconStyle().setScale(0.5);
    m_defaultStyle[GeoDataPlacemark::CrossingSignals]         = createOsmPOIStyle(osmFont, "transportation/traffic_light_crossing", transportationColor);
    m_defaultStyle[GeoDataPlacemark::CrossingSignals]->iconStyle().setScale(0.75);
    m_defaultStyle[GeoDataPlacemark::CrossingZebra]           = createOsmPOIStyle(osmFont, "transportation/zebra_crossing", transportationColor);
    m_defaultStyle[GeoDataPlacemark::CrossingZebra]->iconStyle().setScale(0.75);

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
    m_defaultStyle[GeoDataPlacemark::NaturalCave]              = createOsmPOIStyle(osmFont, "amenities/cave", amenityColor);
    m_defaultStyle[GeoDataPlacemark::NaturalHeath]             = createWayStyle("#d6d99f", QColor("#d6d99f").darker(150), true, false);

    m_defaultStyle[GeoDataPlacemark::LeisureGolfCourse]        = createOsmPOIAreaStyle(osmFont, "leisure/golf", "#39ac39", "#b5e3b5", QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::LeisureMinigolfCourse]    = createOsmPOIAreaStyle(osmFont, "leisure/miniature_golf", "#39ac39", "#b5e3b5", QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::LeisureMarina]            = createOsmPOIStyle(osmFont, QString(), QColor("#95abd5"), QColor("#aec8d1"), QColor("#95abd5").darker(150));
    m_defaultStyle[GeoDataPlacemark::LeisurePark]              = createWayStyle(QColor("#c8facc"), QColor("#c8facc").darker(150), true, true);
    m_defaultStyle[GeoDataPlacemark::LeisurePlayground]        = createOsmPOIAreaStyle(osmFont, "amenity/playground.16", amenityColor, "#CCFFF1", "#BDFFED");
    m_defaultStyle[GeoDataPlacemark::LeisurePitch]             = createWayStyle("#8ad3af", QColor("#8ad3af").darker(150), true, true);
    m_defaultStyle[GeoDataPlacemark::LeisureSportsCentre]      = createWayStyle("#33cc99", QColor("#33cc99").darker(150), true, true);
    m_defaultStyle[GeoDataPlacemark::LeisureStadium]           = createWayStyle("#33cc99", QColor("#33cc99").darker(150), true, true);
    m_defaultStyle[GeoDataPlacemark::LeisureTrack]             = createWayStyle("#74dcba", QColor("#74dcba").darker(150), true, true);
    m_defaultStyle[GeoDataPlacemark::LeisureSwimmingPool]      = createWayStyle(waterColor, waterColor.darker(150), true, true);

    m_defaultStyle[GeoDataPlacemark::LanduseAllotments]        = createWayStyle("#E4C6AA", "#E4C6AA", true, false, Qt::SolidPattern, MarbleDirs::path("bitmaps/osmcarto/patterns/allotments.png"));
    m_defaultStyle[GeoDataPlacemark::LanduseBasin]             = createWayStyle(QColor(0xB5, 0xD0, 0xD0, 0x80), QColor(0xB5, 0xD0, 0xD0));
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
    m_defaultStyle[GeoDataPlacemark::RailwayTram]              = createStyle(2.0, 1.435, "#706E70", "#B7B6B7", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::RailwayLightRail]         = createStyle(2.0, 1.435, "#706E70", "#706E70", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::RailwayAbandoned]         = createStyle(2.0, 1.435, Qt::transparent, "#706E70", false, false, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::RailwaySubway]            = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::RailwayPreserved]         = createStyle(2.0, 1.435, "#EEEEEE", "#706E70", true, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, true, QVector<qreal>(), osmFont, QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::RailwayMiniature]         = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::RailwayConstruction]      = createStyle(2.0, 1.435, "#EEEEEE", "#706E70", true, true, Qt::SolidPattern, Qt::DotLine, Qt::FlatCap, true, QVector<qreal>(), osmFont, QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::RailwayMonorail]          = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, QColor(Qt::transparent));
    m_defaultStyle[GeoDataPlacemark::RailwayFunicular]         = createStyle(2.0, 1.435, "#706E70", "#EEEEEE", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont, QColor(Qt::transparent));

    m_defaultStyle[GeoDataPlacemark::Landmass]                 = createWayStyle("#F1EEE8", "#F1EEE8", true, false);
    m_defaultStyle[GeoDataPlacemark::UrbanArea]                = createWayStyle("#E6E3DD", "#E6E3DD", true, false);
    m_defaultStyle[GeoDataPlacemark::InternationalDateLine]    = createStyle(1.0, 0.0, "#000000", "#000000", false, true, Qt::SolidPattern, Qt::SolidLine, Qt::FlatCap, false, QVector<qreal>(), osmFont);
    m_defaultStyle[GeoDataPlacemark::Bathymetry]               = createWayStyle("#a5c9c9", "#a5c9c9", true, false);

    m_defaultStyle[GeoDataPlacemark::AerialwayStation]         = createOsmPOIStyle(osmFont, "individual/railway_station", transportationColor);
    m_defaultStyle[GeoDataPlacemark::AerialwayStation]->iconStyle().setScale(0.33f);
    m_defaultStyle[GeoDataPlacemark::AerialwayPylon]           = createOsmPOIStyle(osmFont, "individual/pylon", QColor("#dddddd"));
    m_defaultStyle[GeoDataPlacemark::AerialwayPylon]->iconStyle().setScale(0.33f);
    m_defaultStyle[GeoDataPlacemark::AerialwayCableCar]        = createIconWayStyle("#bbbbbb", osmFont, transportationColor, 1.0, QStringLiteral("svg/thenounproject/transportation-583813-cable-car.svg"));
    m_defaultStyle[GeoDataPlacemark::AerialwayGondola]         = createIconWayStyle("#bbbbbb", osmFont, transportationColor, 1.0, QStringLiteral("svg/thenounproject/transportation-21636-gondola.svg"));
    m_defaultStyle[GeoDataPlacemark::AerialwayChairLift]       = createIconWayStyle("#bbbbbb", osmFont, transportationColor, 1.0, QStringLiteral("svg/thenounproject/transportation-231-chair-lift.svg"));
    m_defaultStyle[GeoDataPlacemark::AerialwayMixedLift]       = createIconWayStyle("#bbbbbb", osmFont, transportationColor);
    m_defaultStyle[GeoDataPlacemark::AerialwayDragLift]        = createIconWayStyle("#bbbbbb", osmFont, transportationColor, 1.0, QStringLiteral("svg/thenounproject/transportation-8803-platter-lift.svg"));
    m_defaultStyle[GeoDataPlacemark::AerialwayTBar]            = createIconWayStyle("#bbbbbb", osmFont, transportationColor, 1.0, QStringLiteral("svg/thenounproject/transportation-8803-platter-lift.svg"));
    m_defaultStyle[GeoDataPlacemark::AerialwayJBar]            = createIconWayStyle("#bbbbbb", osmFont, transportationColor, 1.0, QStringLiteral("svg/thenounproject/transportation-8803-platter-lift.svg"));
    m_defaultStyle[GeoDataPlacemark::AerialwayPlatter]         = createIconWayStyle("#bbbbbb", osmFont, transportationColor, 1.0, QStringLiteral("svg/thenounproject/transportation-8803-platter-lift.svg"));
    m_defaultStyle[GeoDataPlacemark::AerialwayRopeTow]         = createIconWayStyle("#bbbbbb", osmFont, transportationColor);
    m_defaultStyle[GeoDataPlacemark::AerialwayMagicCarpet]     = createIconWayStyle("#bbbbbb", osmFont, transportationColor);
    m_defaultStyle[GeoDataPlacemark::AerialwayZipLine]         = createIconWayStyle("#bbbbbb", osmFont, transportationColor);
    m_defaultStyle[GeoDataPlacemark::AerialwayGoods]           = createIconWayStyle("#bbbbbb", osmFont, transportationColor);

    m_defaultStyle[GeoDataPlacemark::PisteDownhill]            = createStyle(9, 0.0, "#dddddd", Qt::transparent, true, false, Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false, QVector< qreal >(), osmFont, Qt::transparent);
    m_defaultStyle[GeoDataPlacemark::PisteNordic]              = createStyle(3, 0.0, "#fffafa", "#006666", true, false, Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false, QVector< qreal >(), osmFont, Qt::transparent, MarbleDirs::path("bitmaps/osmcarto/patterns/ice.png"));
    m_defaultStyle[GeoDataPlacemark::PisteSkitour]             = m_defaultStyle[GeoDataPlacemark::PisteNordic];
    m_defaultStyle[GeoDataPlacemark::PisteSled]                = m_defaultStyle[GeoDataPlacemark::PisteNordic];
    m_defaultStyle[GeoDataPlacemark::PisteHike]                = m_defaultStyle[GeoDataPlacemark::PisteNordic];
    m_defaultStyle[GeoDataPlacemark::PisteSleigh]              = m_defaultStyle[GeoDataPlacemark::PisteNordic];
    m_defaultStyle[GeoDataPlacemark::PisteIceSkate]            = m_defaultStyle[GeoDataPlacemark::PisteNordic];
    m_defaultStyle[GeoDataPlacemark::PisteSnowPark]            = m_defaultStyle[GeoDataPlacemark::PisteNordic];
    m_defaultStyle[GeoDataPlacemark::PistePlayground]          = m_defaultStyle[GeoDataPlacemark::PisteNordic];
    m_defaultStyle[GeoDataPlacemark::PisteSkiJump]             = createIconWayStyle("#bbbbbb", osmFont, transportationColor, 1.0, QStringLiteral("svg/thenounproject/sports-245-ski-jump.svg"));

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
        = GeoDataStyle::Ptr(new GeoDataStyle(MarbleDirs::path("bitmaps/satellite.png"),
                            QFont(defaultFamily, defaultSize, 50, false), defaultLabelColor));

    QFont tmp;

    // Fonts for areas ...
    tmp = m_defaultStyle[GeoDataPlacemark::Continent]->labelStyle().font();
    tmp.setLetterSpacing(QFont::AbsoluteSpacing, 2);
    tmp.setCapitalization(QFont::AllUppercase);
    tmp.setBold(true);
    m_defaultStyle[GeoDataPlacemark::Continent]->labelStyle().setFont(tmp);

    // Fonts for areas ...
    tmp = m_defaultStyle[GeoDataPlacemark::Mare]->labelStyle().font();
    tmp.setLetterSpacing(QFont::AbsoluteSpacing, 2);
    tmp.setCapitalization(QFont::AllUppercase);
    tmp.setBold(true);
    m_defaultStyle[GeoDataPlacemark::Mare]->labelStyle().setFont(tmp);

    // Now we need to underline the capitals ...

    tmp = m_defaultStyle[GeoDataPlacemark::SmallNationCapital]->labelStyle().font();
    tmp.setUnderline(true);
    m_defaultStyle[GeoDataPlacemark::SmallNationCapital]->labelStyle().setFont(tmp);

    tmp = m_defaultStyle[GeoDataPlacemark::MediumNationCapital]->labelStyle().font();
    tmp.setUnderline(true);
    m_defaultStyle[GeoDataPlacemark::MediumNationCapital]->labelStyle().setFont(tmp);

    tmp = m_defaultStyle[GeoDataPlacemark::BigNationCapital]->labelStyle().font();
    tmp.setUnderline(true);
    m_defaultStyle[GeoDataPlacemark::BigNationCapital]->labelStyle().setFont(tmp);

    tmp = m_defaultStyle[GeoDataPlacemark::LargeNationCapital]->labelStyle().font();
    tmp.setUnderline(true);
    m_defaultStyle[GeoDataPlacemark::LargeNationCapital]->labelStyle().setFont(tmp);

    // Buildings
    m_defaultStyle[GeoDataPlacemark::Building] = createStyle(1, 0, buildingColor, buildingColor.darker(),
            true, true, Qt::SolidPattern, Qt::SolidLine, Qt::RoundCap, false, QVector<qreal>(), osmFont);
    for (int i = 0; i < GeoDataPlacemark::LastIndex; ++i) {
        if (m_defaultStyle[i] && !m_defaultStyle[i]->iconStyle().iconPath().isEmpty()) {
            auto const category = GeoDataPlacemark::GeoDataVisualCategory(i);
            m_buildingStyles[category] = GeoDataStyle::Ptr(new GeoDataStyle(*m_defaultStyle[GeoDataPlacemark::Building]));
            m_buildingStyles[category]->iconStyle() = m_defaultStyle[i]->iconStyle();
            m_buildingStyles[category]->labelStyle() = m_defaultStyle[i]->labelStyle();
        }
    }

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

    s_visualCategories[OsmTag("amenity", "college")]            = GeoDataPlacemark::EducationCollege;
    s_visualCategories[OsmTag("amenity", "school")]             = GeoDataPlacemark::EducationSchool;
    s_visualCategories[OsmTag("amenity", "university")]         = GeoDataPlacemark::EducationUniversity;

    s_visualCategories[OsmTag("amenity", "childcare")]          = GeoDataPlacemark::AmenityKindergarten;
    s_visualCategories[OsmTag("amenity", "kindergarten")]       = GeoDataPlacemark::AmenityKindergarten;
    s_visualCategories[OsmTag("amenity", "library")]            = GeoDataPlacemark::AmenityLibrary;

    s_visualCategories[OsmTag("amenity", "bus_station")]        = GeoDataPlacemark::TransportBusStation;
    s_visualCategories[OsmTag("amenity", "car_sharing")]        = GeoDataPlacemark::TransportCarShare;
    s_visualCategories[OsmTag("amenity", "fuel")]               = GeoDataPlacemark::TransportFuel;
    s_visualCategories[OsmTag("amenity", "parking")]            = GeoDataPlacemark::TransportParking;
    s_visualCategories[OsmTag("amenity", "parking_space")]      = GeoDataPlacemark::TransportParkingSpace;

    s_visualCategories[OsmTag("amenity", "atm")]                = GeoDataPlacemark::MoneyAtm;
    s_visualCategories[OsmTag("amenity", "bank")]               = GeoDataPlacemark::MoneyBank;

    s_visualCategories[OsmTag("historic", "archaeological_site")] = GeoDataPlacemark::HistoricArchaeologicalSite;
    s_visualCategories[OsmTag("historic", "castle")]            = GeoDataPlacemark::HistoricCastle;
    s_visualCategories[OsmTag("historic", "fort")]              = GeoDataPlacemark::HistoricCastle;
    s_visualCategories[OsmTag("historic", "memorial")]          = GeoDataPlacemark::HistoricMemorial;
    s_visualCategories[OsmTag("historic", "monument")]          = GeoDataPlacemark::HistoricMonument;
    s_visualCategories[OsmTag("historic", "ruins")]             = GeoDataPlacemark::HistoricRuins;

    s_visualCategories[OsmTag("amenity", "bench")]              = GeoDataPlacemark::AmenityBench;
    s_visualCategories[OsmTag("amenity", "car_wash")]           = GeoDataPlacemark::AmenityCarWash;
    s_visualCategories[OsmTag("amenity", "charging_station")]   = GeoDataPlacemark::AmenityChargingStation;
    s_visualCategories[OsmTag("amenity", "cinema")]             = GeoDataPlacemark::AmenityCinema;
    s_visualCategories[OsmTag("amenity", "community_centre")]   = GeoDataPlacemark::AmenityCommunityCentre;
    s_visualCategories[OsmTag("amenity", "courthouse")]         = GeoDataPlacemark::AmenityCourtHouse;
    s_visualCategories[OsmTag("amenity", "drinking_water")]     = GeoDataPlacemark::AmenityDrinkingWater;
    s_visualCategories[OsmTag("amenity", "embassy")]            = GeoDataPlacemark::AmenityEmbassy;
    s_visualCategories[OsmTag("amenity", "fire_station")]       = GeoDataPlacemark::AmenityFireStation;
    s_visualCategories[OsmTag("amenity", "fountain")]           = GeoDataPlacemark::AmenityFountain;
    s_visualCategories[OsmTag("amenity", "graveyard")]          = GeoDataPlacemark::AmenityGraveyard;
    s_visualCategories[OsmTag("amenity", "hunting_stand")]      = GeoDataPlacemark::AmenityHuntingStand;
    s_visualCategories[OsmTag("amenity", "nightclub")]          = GeoDataPlacemark::AmenityNightClub;
    s_visualCategories[OsmTag("amenity", "police")]             = GeoDataPlacemark::AmenityPolice;
    s_visualCategories[OsmTag("amenity", "post_box")]           = GeoDataPlacemark::AmenityPostBox;
    s_visualCategories[OsmTag("amenity", "post_office")]        = GeoDataPlacemark::AmenityPostOffice;
    s_visualCategories[OsmTag("amenity", "prison")]             = GeoDataPlacemark::AmenityPrison;
    s_visualCategories[OsmTag("amenity", "recycling")]          = GeoDataPlacemark::AmenityRecycling;
    s_visualCategories[OsmTag("amenity", "shelter")]            = GeoDataPlacemark::AmenityShelter;
    s_visualCategories[OsmTag("amenity", "social_facility")]    = GeoDataPlacemark::AmenitySocialFacility;
    s_visualCategories[OsmTag("amenity", "telephone")]          = GeoDataPlacemark::AmenityTelephone;
    s_visualCategories[OsmTag("amenity", "theatre")]            = GeoDataPlacemark::AmenityTheatre;
    s_visualCategories[OsmTag("amenity", "toilets")]            = GeoDataPlacemark::AmenityToilets;
    s_visualCategories[OsmTag("amenity", "townhall")]           = GeoDataPlacemark::AmenityTownHall;
    s_visualCategories[OsmTag("amenity", "waste_basket")]       = GeoDataPlacemark::AmenityWasteBasket;
    s_visualCategories[OsmTag("emergency", "phone")]            = GeoDataPlacemark::AmenityEmergencyPhone;
    s_visualCategories[OsmTag("amenity", "mountain_rescue")]    = GeoDataPlacemark::AmenityMountainRescue;
    s_visualCategories[OsmTag("amenity", "dentist")]            = GeoDataPlacemark::HealthDentist;
    s_visualCategories[OsmTag("amenity", "doctors")]            = GeoDataPlacemark::HealthDoctors;
    s_visualCategories[OsmTag("amenity", "hospital")]           = GeoDataPlacemark::HealthHospital;
    s_visualCategories[OsmTag("amenity", "pharmacy")]           = GeoDataPlacemark::HealthPharmacy;
    s_visualCategories[OsmTag("amenity", "veterinary")]         = GeoDataPlacemark::HealthVeterinary;

    s_visualCategories[OsmTag("amenity", "place_of_worship")]   = GeoDataPlacemark::ReligionPlaceOfWorship;

    s_visualCategories[OsmTag("tourism", "information")]        = GeoDataPlacemark::TourismInformation;

    s_visualCategories[OsmTag("natural", "cave_entrance")]      = GeoDataPlacemark::NaturalCave;
    s_visualCategories[OsmTag("natural", "peak")]               = GeoDataPlacemark::NaturalPeak;
    s_visualCategories[OsmTag("natural", "tree")]               = GeoDataPlacemark::NaturalTree;
    s_visualCategories[OsmTag("natural", "volcano")]            = GeoDataPlacemark::NaturalVolcano;

    s_visualCategories[OsmTag("shop", "alcohol")]               = GeoDataPlacemark::ShopAlcohol;
    s_visualCategories[OsmTag("shop", "art")]                   = GeoDataPlacemark::ShopArt;
    s_visualCategories[OsmTag("shop", "bag")]                   = GeoDataPlacemark::ShopBag;
    s_visualCategories[OsmTag("shop", "bakery")]                = GeoDataPlacemark::ShopBakery;
    s_visualCategories[OsmTag("shop", "beauty")]                = GeoDataPlacemark::ShopBeauty;
    s_visualCategories[OsmTag("shop", "beverages")]             = GeoDataPlacemark::ShopBeverages;
    s_visualCategories[OsmTag("shop", "bicycle")]               = GeoDataPlacemark::ShopBicycle;
    s_visualCategories[OsmTag("shop", "books")]                 = GeoDataPlacemark::ShopBook;
    s_visualCategories[OsmTag("shop", "butcher")]               = GeoDataPlacemark::ShopButcher;
    s_visualCategories[OsmTag("shop", "car")]                   = GeoDataPlacemark::ShopCar;
    s_visualCategories[OsmTag("shop", "car_parts")]             = GeoDataPlacemark::ShopCarParts;
    s_visualCategories[OsmTag("shop", "car_repair")]            = GeoDataPlacemark::ShopCarRepair;
    s_visualCategories[OsmTag("shop", "chemist")]               = GeoDataPlacemark::ShopChemist;
    s_visualCategories[OsmTag("shop", "clothes")]               = GeoDataPlacemark::ShopClothes;
    s_visualCategories[OsmTag("shop", "confectionery")]         = GeoDataPlacemark::ShopConfectionery;
    s_visualCategories[OsmTag("shop", "convenience")]           = GeoDataPlacemark::ShopConvenience;
    s_visualCategories[OsmTag("shop", "copy")]                  = GeoDataPlacemark::ShopCopy;
    s_visualCategories[OsmTag("shop", "cosmetics")]             = GeoDataPlacemark::ShopCosmetics;
    s_visualCategories[OsmTag("shop", "deli")]                  = GeoDataPlacemark::ShopDeli;
    s_visualCategories[OsmTag("shop", "department_store")]      = GeoDataPlacemark::ShopDepartmentStore;
    s_visualCategories[OsmTag("shop", "doityourself")]          = GeoDataPlacemark::ShopDoitYourself;
    s_visualCategories[OsmTag("shop", "electronics")]           = GeoDataPlacemark::ShopElectronics;
    s_visualCategories[OsmTag("shop", "fashion")]               = GeoDataPlacemark::ShopFashion;
    s_visualCategories[OsmTag("shop", "florist")]               = GeoDataPlacemark::ShopFlorist;
    s_visualCategories[OsmTag("shop", "furniture")]             = GeoDataPlacemark::ShopFurniture;
    s_visualCategories[OsmTag("shop", "gift")]                  = GeoDataPlacemark::ShopGift;
    s_visualCategories[OsmTag("shop", "greengrocer")]           = GeoDataPlacemark::ShopGreengrocer;
    s_visualCategories[OsmTag("shop", "hairdresser")]           = GeoDataPlacemark::ShopHairdresser;
    s_visualCategories[OsmTag("shop", "hardware")]              = GeoDataPlacemark::ShopHardware;
    s_visualCategories[OsmTag("shop", "hifi")]                  = GeoDataPlacemark::ShopHifi;
    s_visualCategories[OsmTag("shop", "jewelry")]               = GeoDataPlacemark::ShopJewelry;
    s_visualCategories[OsmTag("shop", "kiosk")]                 = GeoDataPlacemark::ShopKiosk;
    s_visualCategories[OsmTag("shop", "laundry")]               = GeoDataPlacemark::ShopLaundry;
    s_visualCategories[OsmTag("shop", "mobile_phone")]          = GeoDataPlacemark::ShopMobilePhone;
    s_visualCategories[OsmTag("shop", "motorcycle")]            = GeoDataPlacemark::ShopMotorcycle;
    s_visualCategories[OsmTag("shop", "musical_instrument")]    = GeoDataPlacemark::ShopMusicalInstrument;
    s_visualCategories[OsmTag("shop", "optician")]              = GeoDataPlacemark::ShopOptician;
    s_visualCategories[OsmTag("shop", "outdoor")]               = GeoDataPlacemark::ShopOutdoor;
    s_visualCategories[OsmTag("shop", "perfumery")]             = GeoDataPlacemark::ShopPerfumery;
    s_visualCategories[OsmTag("shop", "pet")]                   = GeoDataPlacemark::ShopPet;
    s_visualCategories[OsmTag("shop", "photo")]                 = GeoDataPlacemark::ShopPhoto;
    s_visualCategories[OsmTag("shop", "seafood")]               = GeoDataPlacemark::ShopSeafood;
    s_visualCategories[OsmTag("shop", "shoes")]                 = GeoDataPlacemark::ShopShoes;
    s_visualCategories[OsmTag("shop", "sports")]                = GeoDataPlacemark::ShopSports;
    s_visualCategories[OsmTag("shop", "stationery")]            = GeoDataPlacemark::ShopStationery;
    s_visualCategories[OsmTag("shop", "supermarket")]           = GeoDataPlacemark::ShopSupermarket;
    s_visualCategories[OsmTag("shop", "tea")]                   = GeoDataPlacemark::ShopTea;
    s_visualCategories[OsmTag("shop", "tobacco")]               = GeoDataPlacemark::ShopTobacco;
    s_visualCategories[OsmTag("shop", "toys")]                  = GeoDataPlacemark::ShopToys;
    s_visualCategories[OsmTag("shop", "travel_agency")]         = GeoDataPlacemark::ShopTravelAgency;
    s_visualCategories[OsmTag("shop", "variety_store")]         = GeoDataPlacemark::ShopVarietyStore;


    // Default for all other shops
    for (const QString &value: shopValues()) {
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
    s_visualCategories[OsmTag("religion", "hindu")]             = GeoDataPlacemark::ReligionHindu;
    s_visualCategories[OsmTag("religion", "jain")]              = GeoDataPlacemark::ReligionJain;
    s_visualCategories[OsmTag("religion", "jewish")]            = GeoDataPlacemark::ReligionJewish;
    s_visualCategories[OsmTag("religion", "muslim")]            = GeoDataPlacemark::ReligionMuslim;
    s_visualCategories[OsmTag("religion", "shinto")]            = GeoDataPlacemark::ReligionShinto;
    s_visualCategories[OsmTag("religion", "sikh")]              = GeoDataPlacemark::ReligionSikh;

    s_visualCategories[OsmTag("tourism", "camp_site")]          = GeoDataPlacemark::AccomodationCamping;
    s_visualCategories[OsmTag("tourism", "guest_house")]        = GeoDataPlacemark::AccomodationGuestHouse;
    s_visualCategories[OsmTag("tourism", "hostel")]             = GeoDataPlacemark::AccomodationHostel;
    s_visualCategories[OsmTag("tourism", "hotel")]              = GeoDataPlacemark::AccomodationHotel;
    s_visualCategories[OsmTag("tourism", "motel")]              = GeoDataPlacemark::AccomodationMotel;

    s_visualCategories[OsmTag("tourism", "alpine_hut")]         = GeoDataPlacemark::TourismAlpineHut;
    s_visualCategories[OsmTag("tourism", "artwork")]            = GeoDataPlacemark::TourismArtwork;
    s_visualCategories[OsmTag("tourism", "attraction")]         = GeoDataPlacemark::TourismAttraction;
    s_visualCategories[OsmTag("tourism", "museum")]             = GeoDataPlacemark::TourismMuseum;
    s_visualCategories[OsmTag("tourism", "theme_park")]         = GeoDataPlacemark::TourismThemePark;
    s_visualCategories[OsmTag("tourism", "viewpoint")]          = GeoDataPlacemark::TourismViewPoint;
    s_visualCategories[OsmTag("tourism", "wilderness_hut")]     = GeoDataPlacemark::TourismWildernessHut;
    s_visualCategories[OsmTag("tourism", "zoo")]                = GeoDataPlacemark::TourismZoo;

    s_visualCategories[OsmTag("barrier", "city_wall")]          = GeoDataPlacemark::BarrierCityWall;
    s_visualCategories[OsmTag("barrier", "gate")]               = GeoDataPlacemark::BarrierGate;
    s_visualCategories[OsmTag("barrier", "lift_gate")]          = GeoDataPlacemark::BarrierLiftGate;
    s_visualCategories[OsmTag("barrier", "wall")]               = GeoDataPlacemark::BarrierWall;

    s_visualCategories[OsmTag("highway", "traffic_signals")]    = GeoDataPlacemark::HighwayTrafficSignals;

    s_visualCategories[OsmTag("highway", "cycleway")]           = GeoDataPlacemark::HighwayCycleway;
    s_visualCategories[OsmTag("highway", "footway")]            = GeoDataPlacemark::HighwayFootway;
    s_visualCategories[OsmTag("highway", "living_street")]      = GeoDataPlacemark::HighwayLivingStreet;
    s_visualCategories[OsmTag("highway", "motorway")]           = GeoDataPlacemark::HighwayMotorway;
    s_visualCategories[OsmTag("highway", "motorway_link")]      = GeoDataPlacemark::HighwayMotorwayLink;
    s_visualCategories[OsmTag("highway", "path")]               = GeoDataPlacemark::HighwayPath;
    s_visualCategories[OsmTag("highway", "pedestrian")]         = GeoDataPlacemark::HighwayPedestrian;
    s_visualCategories[OsmTag("highway", "primary")]            = GeoDataPlacemark::HighwayPrimary;
    s_visualCategories[OsmTag("highway", "primary_link")]       = GeoDataPlacemark::HighwayPrimaryLink;
    s_visualCategories[OsmTag("highway", "raceway")]            = GeoDataPlacemark::HighwayRaceway;
    s_visualCategories[OsmTag("highway", "residential")]        = GeoDataPlacemark::HighwayResidential;
    s_visualCategories[OsmTag("highway", "road")]               = GeoDataPlacemark::HighwayRoad;
    s_visualCategories[OsmTag("highway", "secondary")]          = GeoDataPlacemark::HighwaySecondary;
    s_visualCategories[OsmTag("highway", "secondary_link")]     = GeoDataPlacemark::HighwaySecondaryLink;
    s_visualCategories[OsmTag("highway", "service")]            = GeoDataPlacemark::HighwayService;
    s_visualCategories[OsmTag("highway", "steps")]              = GeoDataPlacemark::HighwaySteps;
    s_visualCategories[OsmTag("highway", "tertiary")]           = GeoDataPlacemark::HighwayTertiary;
    s_visualCategories[OsmTag("highway", "tertiary_link")]      = GeoDataPlacemark::HighwayTertiaryLink;
    s_visualCategories[OsmTag("highway", "track")]              = GeoDataPlacemark::HighwayTrack;
    s_visualCategories[OsmTag("highway", "trunk")]              = GeoDataPlacemark::HighwayTrunk;
    s_visualCategories[OsmTag("highway", "trunk_link")]         = GeoDataPlacemark::HighwayTrunkLink;
    s_visualCategories[OsmTag("highway", "unclassified")]       = GeoDataPlacemark::HighwayUnclassified;
    s_visualCategories[OsmTag("highway", "unknown")]            = GeoDataPlacemark::HighwayUnknown;
    s_visualCategories[OsmTag("highway", "corridor")]           = GeoDataPlacemark::HighwayCorridor;

    s_visualCategories[OsmTag("natural", "bay")]                = GeoDataPlacemark::NaturalWater;
    s_visualCategories[OsmTag("natural", "coastline")]          = GeoDataPlacemark::NaturalWater;
    s_visualCategories[OsmTag("natural", "reef")]               = GeoDataPlacemark::NaturalReef;
    s_visualCategories[OsmTag("natural", "water")]              = GeoDataPlacemark::NaturalWater;

    s_visualCategories[OsmTag("waterway", "canal")]             = GeoDataPlacemark::WaterwayCanal;
    s_visualCategories[OsmTag("waterway", "ditch")]             = GeoDataPlacemark::WaterwayDitch;
    s_visualCategories[OsmTag("waterway", "drain")]             = GeoDataPlacemark::WaterwayDrain;
    s_visualCategories[OsmTag("waterway", "river")]             = GeoDataPlacemark::WaterwayRiver;
    s_visualCategories[OsmTag("waterway", "riverbank")]         = GeoDataPlacemark::NaturalWater;
    s_visualCategories[OsmTag("waterway", "weir")]              = GeoDataPlacemark::WaterwayWeir;
    s_visualCategories[OsmTag("waterway", "stream")]            = GeoDataPlacemark::WaterwayStream;

    s_visualCategories[OsmTag("natural", "beach")]              = GeoDataPlacemark::NaturalBeach;
    s_visualCategories[OsmTag("natural", "cliff")]              = GeoDataPlacemark::NaturalCliff;
    s_visualCategories[OsmTag("natural", "glacier")]            = GeoDataPlacemark::NaturalGlacier;
    s_visualCategories[OsmTag("glacier:type", "shelf")]         = GeoDataPlacemark::NaturalIceShelf;
    s_visualCategories[OsmTag("natural", "scrub")]              = GeoDataPlacemark::NaturalScrub;
    s_visualCategories[OsmTag("natural", "wetland")]            = GeoDataPlacemark::NaturalWetland;
    s_visualCategories[OsmTag("natural", "wood")]               = GeoDataPlacemark::NaturalWood;

    s_visualCategories[OsmTag("military", "danger_area")]       = GeoDataPlacemark::MilitaryDangerArea;

    s_visualCategories[OsmTag("landuse", "allotments")]         = GeoDataPlacemark::LanduseAllotments;
    s_visualCategories[OsmTag("landuse", "basin")]              = GeoDataPlacemark::LanduseBasin;
    s_visualCategories[OsmTag("landuse", "brownfield")]         = GeoDataPlacemark::LanduseConstruction;
    s_visualCategories[OsmTag("landuse", "cemetery")]           = GeoDataPlacemark::LanduseCemetery;
    s_visualCategories[OsmTag("landuse", "commercial")]         = GeoDataPlacemark::LanduseCommercial;
    s_visualCategories[OsmTag("landuse", "construction")]       = GeoDataPlacemark::LanduseConstruction;
    s_visualCategories[OsmTag("landuse", "farm")]               = GeoDataPlacemark::LanduseFarmland;
    s_visualCategories[OsmTag("landuse", "farmland")]           = GeoDataPlacemark::LanduseFarmland;
    s_visualCategories[OsmTag("landuse", "farmyard")]           = GeoDataPlacemark::LanduseFarmland;
    s_visualCategories[OsmTag("landuse", "forest")]             = GeoDataPlacemark::NaturalWood;
    s_visualCategories[OsmTag("landuse", "garages")]            = GeoDataPlacemark::LanduseGarages;
    s_visualCategories[OsmTag("landuse", "grass")]              = GeoDataPlacemark::LanduseGrass;
    s_visualCategories[OsmTag("landuse", "greenfield")]         = GeoDataPlacemark::LanduseConstruction;
    s_visualCategories[OsmTag("landuse", "greenhouse_horticulture")] = GeoDataPlacemark::LanduseFarmland;
    s_visualCategories[OsmTag("landuse", "industrial")]         = GeoDataPlacemark::LanduseIndustrial;
    s_visualCategories[OsmTag("landuse", "landfill")]           = GeoDataPlacemark::LanduseLandfill;
    s_visualCategories[OsmTag("landuse", "meadow")]             = GeoDataPlacemark::LanduseMeadow;
    s_visualCategories[OsmTag("landuse", "military")]           = GeoDataPlacemark::LanduseMilitary;
    s_visualCategories[OsmTag("landuse", "orchard")]            = GeoDataPlacemark::LanduseFarmland;
    s_visualCategories[OsmTag("landuse", "orchard")]            = GeoDataPlacemark::LanduseOrchard;
    s_visualCategories[OsmTag("landuse", "quarry")]             = GeoDataPlacemark::LanduseQuarry;
    s_visualCategories[OsmTag("landuse", "railway")]            = GeoDataPlacemark::LanduseRailway;
    s_visualCategories[OsmTag("landuse", "recreation_ground")]  = GeoDataPlacemark::LeisurePark;
    s_visualCategories[OsmTag("landuse", "reservoir")]          = GeoDataPlacemark::LanduseReservoir;
    s_visualCategories[OsmTag("landuse", "residential")]        = GeoDataPlacemark::LanduseResidential;
    s_visualCategories[OsmTag("landuse", "retail")]             = GeoDataPlacemark::LanduseRetail;
    s_visualCategories[OsmTag("landuse", "village_green")]      = GeoDataPlacemark::LanduseGrass;
    s_visualCategories[OsmTag("landuse", "vineyard")]           = GeoDataPlacemark::LanduseVineyard;

    s_visualCategories[OsmTag("leisure", "common")]             = GeoDataPlacemark::LanduseGrass;
    s_visualCategories[OsmTag("leisure", "garden")]             = GeoDataPlacemark::LanduseGrass;

    s_visualCategories[OsmTag("leisure", "golf_course")]        = GeoDataPlacemark::LeisureGolfCourse;
    s_visualCategories[OsmTag("leisure", "marina")]             = GeoDataPlacemark::LeisureMarina;
    s_visualCategories[OsmTag("leisure", "miniature_golf")]     = GeoDataPlacemark::LeisureMinigolfCourse;
    s_visualCategories[OsmTag("leisure", "park")]               = GeoDataPlacemark::LeisurePark;
    s_visualCategories[OsmTag("leisure", "pitch")]              = GeoDataPlacemark::LeisurePitch;
    s_visualCategories[OsmTag("leisure", "playground")]         = GeoDataPlacemark::LeisurePlayground;
    s_visualCategories[OsmTag("leisure", "sports_centre")]      = GeoDataPlacemark::LeisureSportsCentre;
    s_visualCategories[OsmTag("leisure", "stadium")]            = GeoDataPlacemark::LeisureStadium;
    s_visualCategories[OsmTag("leisure", "swimming_pool")]      = GeoDataPlacemark::LeisureSwimmingPool;
    s_visualCategories[OsmTag("leisure", "track")]              = GeoDataPlacemark::LeisureTrack;
    s_visualCategories[OsmTag("leisure", "water_park")]         = GeoDataPlacemark::LeisureWaterPark;

    s_visualCategories[OsmTag("railway", "abandoned")]          = GeoDataPlacemark::RailwayAbandoned;
    s_visualCategories[OsmTag("railway", "construction")]       = GeoDataPlacemark::RailwayConstruction;
    s_visualCategories[OsmTag("railway", "disused")]            = GeoDataPlacemark::RailwayAbandoned;
    s_visualCategories[OsmTag("railway", "funicular")]          = GeoDataPlacemark::RailwayFunicular;
    s_visualCategories[OsmTag("railway", "halt")]               = GeoDataPlacemark::TransportTrainStation;
    s_visualCategories[OsmTag("railway", "light_rail")]         = GeoDataPlacemark::RailwayLightRail;
    s_visualCategories[OsmTag("railway", "miniature")]          = GeoDataPlacemark::RailwayMiniature;
    s_visualCategories[OsmTag("railway", "monorail")]           = GeoDataPlacemark::RailwayMonorail;
    s_visualCategories[OsmTag("railway", "narrow_gauge")]       = GeoDataPlacemark::RailwayNarrowGauge;
    s_visualCategories[OsmTag("railway", "platform")]           = GeoDataPlacemark::TransportPlatform;
    s_visualCategories[OsmTag("railway", "preserved")]          = GeoDataPlacemark::RailwayPreserved;
    s_visualCategories[OsmTag("railway", "rail")]               = GeoDataPlacemark::RailwayRail;
    s_visualCategories[OsmTag("railway", "razed")]              = GeoDataPlacemark::RailwayAbandoned;
    s_visualCategories[OsmTag("railway", "station")]            = GeoDataPlacemark::TransportTrainStation;
    s_visualCategories[OsmTag("public_transport", "station")]   = GeoDataPlacemark::TransportTrainStation;
    s_visualCategories[OsmTag("railway", "subway")]             = GeoDataPlacemark::RailwaySubway;
    s_visualCategories[OsmTag("railway", "tram")]               = GeoDataPlacemark::RailwayTram;

    s_visualCategories[OsmTag("power", "tower")]                = GeoDataPlacemark::PowerTower;

    s_visualCategories[OsmTag("aeroway", "aerodrome")]          = GeoDataPlacemark::TransportAerodrome;
    s_visualCategories[OsmTag("aeroway", "apron")]              = GeoDataPlacemark::TransportAirportApron;
    s_visualCategories[OsmTag("aeroway", "gate")]               = GeoDataPlacemark::TransportAirportGate;
    s_visualCategories[OsmTag("aeroway", "helipad")]            = GeoDataPlacemark::TransportHelipad;
    s_visualCategories[OsmTag("aeroway", "runway")]             = GeoDataPlacemark::TransportAirportRunway;
    s_visualCategories[OsmTag("aeroway", "taxiway")]            = GeoDataPlacemark::TransportAirportTaxiway;
    s_visualCategories[OsmTag("aeroway", "terminal")]           = GeoDataPlacemark::TransportAirportTerminal;

    s_visualCategories[OsmTag("piste:type", "downhill")]        = GeoDataPlacemark::PisteDownhill;
    s_visualCategories[OsmTag("piste:type", "nordic")]          = GeoDataPlacemark::PisteNordic;
    s_visualCategories[OsmTag("piste:type", "skitour")]         = GeoDataPlacemark::PisteSkitour;
    s_visualCategories[OsmTag("piste:type", "sled")]            = GeoDataPlacemark::PisteSled;
    s_visualCategories[OsmTag("piste:type", "hike")]            = GeoDataPlacemark::PisteHike;
    s_visualCategories[OsmTag("piste:type", "sleigh")]          = GeoDataPlacemark::PisteSleigh;
    s_visualCategories[OsmTag("piste:type", "ice_skate")]       = GeoDataPlacemark::PisteIceSkate;
    s_visualCategories[OsmTag("piste:type", "snow_park")]       = GeoDataPlacemark::PisteSnowPark;
    s_visualCategories[OsmTag("piste:type", "playground")]      = GeoDataPlacemark::PistePlayground;
    s_visualCategories[OsmTag("piste:type", "ski_jump")]        = GeoDataPlacemark::PisteSkiJump;

    s_visualCategories[OsmTag("amenity", "bicycle_parking")]    = GeoDataPlacemark::TransportBicycleParking;
    s_visualCategories[OsmTag("amenity", "bicycle_rental")]     = GeoDataPlacemark::TransportRentalBicycle;
    s_visualCategories[OsmTag("rental", "bicycle")]             = GeoDataPlacemark::TransportRentalBicycle;
    s_visualCategories[OsmTag("amenity", "car_rental")]         = GeoDataPlacemark::TransportRentalCar;
    s_visualCategories[OsmTag("rental", "car")]                 = GeoDataPlacemark::TransportRentalCar;
    s_visualCategories[OsmTag("amenity", "ski_rental")]         = GeoDataPlacemark::TransportRentalSki;
    s_visualCategories[OsmTag("rental", "ski")]                 = GeoDataPlacemark::TransportRentalSki;
    s_visualCategories[OsmTag("amenity", "motorcycle_parking")] = GeoDataPlacemark::TransportMotorcycleParking;
    s_visualCategories[OsmTag("amenity", "taxi")]               = GeoDataPlacemark::TransportTaxiRank;
    s_visualCategories[OsmTag("highway", "bus_stop")]           = GeoDataPlacemark::TransportBusStop;
    s_visualCategories[OsmTag("highway", "speed_camera")]       = GeoDataPlacemark::TransportSpeedCamera;
    s_visualCategories[OsmTag("public_transport", "platform")]  = GeoDataPlacemark::TransportPlatform;
    s_visualCategories[OsmTag("railway", "subway_entrance")]    = GeoDataPlacemark::TransportSubwayEntrance;
    s_visualCategories[OsmTag("railway", "tram_stop")]          = GeoDataPlacemark::TransportTramStop;

    s_visualCategories[OsmTag("place", "city")]                 = GeoDataPlacemark::PlaceCity;
    s_visualCategories[OsmTag("place", "hamlet")]               = GeoDataPlacemark::PlaceHamlet;
    s_visualCategories[OsmTag("place", "locality")]             = GeoDataPlacemark::PlaceLocality;
    s_visualCategories[OsmTag("place", "suburb")]               = GeoDataPlacemark::PlaceSuburb;
    s_visualCategories[OsmTag("place", "town")]                 = GeoDataPlacemark::PlaceTown;
    s_visualCategories[OsmTag("place", "village")]              = GeoDataPlacemark::PlaceVillage;

    s_visualCategories[OsmTag("aerialway", "station")]          = GeoDataPlacemark::AerialwayStation;
    s_visualCategories[OsmTag("aerialway", "pylon")]            = GeoDataPlacemark::AerialwayPylon;
    s_visualCategories[OsmTag("aerialway", "cable_car")]        = GeoDataPlacemark::AerialwayCableCar;
    s_visualCategories[OsmTag("aerialway", "gondola")]          = GeoDataPlacemark::AerialwayGondola;
    s_visualCategories[OsmTag("aerialway", "chair_lift")]       = GeoDataPlacemark::AerialwayChairLift;
    s_visualCategories[OsmTag("aerialway", "mixed_lift")]       = GeoDataPlacemark::AerialwayMixedLift;
    s_visualCategories[OsmTag("aerialway", "drag_lift")]        = GeoDataPlacemark::AerialwayDragLift;
    s_visualCategories[OsmTag("aerialway", "t-bar")]            = GeoDataPlacemark::AerialwayTBar;
    s_visualCategories[OsmTag("aerialway", "j-bar")]            = GeoDataPlacemark::AerialwayJBar;
    s_visualCategories[OsmTag("aerialway", "platter")]          = GeoDataPlacemark::AerialwayPlatter;
    s_visualCategories[OsmTag("aerialway", "rope_tow")]         = GeoDataPlacemark::AerialwayRopeTow;
    s_visualCategories[OsmTag("aerialway", "magic_carpet")]     = GeoDataPlacemark::AerialwayMagicCarpet;
    s_visualCategories[OsmTag("aerialway", "zip_line")]         = GeoDataPlacemark::AerialwayZipLine;
    s_visualCategories[OsmTag("aerialway", "goods")]            = GeoDataPlacemark::AerialwayGoods;

    //Custom Marble OSM Tags
    s_visualCategories[OsmTag("marble_land", "landmass")]       = GeoDataPlacemark::Landmass;
    s_visualCategories[OsmTag("settlement", "yes")]             = GeoDataPlacemark::UrbanArea;
    s_visualCategories[OsmTag("marble_line", "date")]           = GeoDataPlacemark::InternationalDateLine;
    s_visualCategories[OsmTag("marble:feature", "bathymetry")]  = GeoDataPlacemark::Bathymetry;

    // Default for buildings
    for (const auto &tag: buildingTags()) {
        s_visualCategories[tag]                                 = GeoDataPlacemark::Building;
    }
}

void StyleBuilder::Private::initializeMinimumZoomLevels()
{
    if (s_defaultMinZoomLevelsInitialized) {
        return;
    }

    s_defaultMinZoomLevelsInitialized = true;
    for (int i = 0; i < GeoDataPlacemark::LastIndex; i++) {
        s_defaultMinZoomLevels[i] = -1;
    }

    s_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel10] = 8;
    s_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel11] = 8;
    s_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel1] = 0;
    s_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel2] = 1;
    s_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel3] = 1;
    s_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel4] = 2;
    s_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel5] = 4;
    s_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel6] = 5;
    s_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel7] = 5;
    s_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel8] = 7;
    s_defaultMinZoomLevels[GeoDataPlacemark::AdminLevel9] = 7;

    s_defaultMinZoomLevels[GeoDataPlacemark::HistoricArchaeologicalSite] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityBench] = 19;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityFountain]     = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityGraveyard]    = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityTelephone]  = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityKindergarten]  = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityLibrary]  = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityWasteBasket]  = 19;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityToilets] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityTownHall] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::LeisureWaterPark]  = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityDrinkingWater] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityEmbassy] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityEmergencyPhone] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityMountainRescue] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityCommunityCentre] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityFountain] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityNightClub] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityCourtHouse] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityFireStation] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityHuntingStand] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityPolice] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityPostBox] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityPostOffice] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityPrison] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityRecycling] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityShelter] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityChargingStation] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityCarWash] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenitySocialFacility] = 17;

    s_defaultMinZoomLevels[GeoDataPlacemark::BarrierCityWall] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::BarrierGate]     = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::BarrierLiftGate] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::BarrierWall]     = 17;

    s_defaultMinZoomLevels[GeoDataPlacemark::Bathymetry]  = 1;

    s_defaultMinZoomLevels[GeoDataPlacemark::BoundaryMaritime]    = 1;

    s_defaultMinZoomLevels[GeoDataPlacemark::Building]    = 17;

    s_defaultMinZoomLevels[GeoDataPlacemark::Default]     = 1;

    s_defaultMinZoomLevels[GeoDataPlacemark::EducationCollege]  = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::EducationSchool]  = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::EducationUniversity]  = 15;

    s_defaultMinZoomLevels[GeoDataPlacemark::FoodBar]  = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::FoodBiergarten]  = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::FoodCafe]  = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::FoodFastFood]  = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::FoodPub]  = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::FoodRestaurant]  = 16;

    s_defaultMinZoomLevels[GeoDataPlacemark::HealthHospital]  = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::HealthPharmacy]  = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::HealthDentist]  = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::HealthDoctors]  = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::HealthVeterinary]  = 17;

    s_defaultMinZoomLevels[GeoDataPlacemark::HistoricMemorial]    = 17;

    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayCycleway]     = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayFootway]      = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayLivingStreet] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayMotorwayLink] = 10;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayMotorway]     = 6;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayPath] = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayPedestrian]   = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayPrimaryLink]  = 10;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayPrimary]      = 8;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayRaceway]      = 12;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayResidential]  = 14;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayRoad] = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwaySecondaryLink] = 10;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwaySecondary]    = 9;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayService]      = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwaySteps] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayTertiaryLink] = 10;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayTertiary]     = 10;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayTrack] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayTrunkLink]    = 10;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayTrunk] = 7;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayUnknown]      = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayUnclassified] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayTrafficSignals]      = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::HighwayCorridor]     = 18;

    s_defaultMinZoomLevels[GeoDataPlacemark::AccomodationCamping] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::AccomodationHostel] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::AccomodationHotel] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::AccomodationMotel] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::AccomodationYouthHostel] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::AccomodationGuestHouse] = 16;

    s_defaultMinZoomLevels[GeoDataPlacemark::InternationalDateLine]      = 1;

    s_defaultMinZoomLevels[GeoDataPlacemark::Landmass]    = 0;

    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseAllotments]   = 11;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseBasin] = 11;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseCemetery]     = 11;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseCommercial]   = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseConstruction] = 11;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseFarmland]     = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseFarmyard]     = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseGarages]      = 11;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseGrass] = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseIndustrial]   = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseLandfill]     = 11;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseMeadow]       = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseMilitary]     = 11;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseOrchard]      = 14;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseQuarry]       = 11;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseRailway]      = 11;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseReservoir]    = 11;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseResidential]  = 11;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseRetail]       = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::LanduseVineyard]     = 14;

    s_defaultMinZoomLevels[GeoDataPlacemark::LeisureGolfCourse]   = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::LeisureMarina]       = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::LeisurePark] = 11;
    s_defaultMinZoomLevels[GeoDataPlacemark::LeisurePlayground]   = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::LeisurePitch]   = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::LeisureStadium]   = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::LeisureSwimmingPool]   = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::LeisureSportsCentre]   = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::LeisureTrack]   = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::LeisureMinigolfCourse] = 16;

    s_defaultMinZoomLevels[GeoDataPlacemark::ManmadeBridge]       = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::ManmadeLighthouse]       = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::ManmadePier]       = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::ManmadeWaterTower]       = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::ManmadeWindMill]       = 15;

    s_defaultMinZoomLevels[GeoDataPlacemark::MilitaryDangerArea]  = 11;

    s_defaultMinZoomLevels[GeoDataPlacemark::MoneyAtm]    = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::MoneyBank]    = 16;

    s_defaultMinZoomLevels[GeoDataPlacemark::NaturalBeach] = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::NaturalCliff] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::NaturalGlacier]      = 3;
    s_defaultMinZoomLevels[GeoDataPlacemark::NaturalHeath]      = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::NaturalIceShelf]     = 3;
    s_defaultMinZoomLevels[GeoDataPlacemark::NaturalVolcano]      = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::NaturalPeak] = 11;
    s_defaultMinZoomLevels[GeoDataPlacemark::NaturalReef] = 3;
    s_defaultMinZoomLevels[GeoDataPlacemark::NaturalScrub] = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::NaturalTree] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::NaturalCave] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::NaturalWater] = 3;
    s_defaultMinZoomLevels[GeoDataPlacemark::NaturalWetland]      = 10;
    s_defaultMinZoomLevels[GeoDataPlacemark::NaturalWood] = 8;

    s_defaultMinZoomLevels[GeoDataPlacemark::PlaceCityNationalCapital] = 9;
    s_defaultMinZoomLevels[GeoDataPlacemark::PlaceCityCapital]    = 9;
    s_defaultMinZoomLevels[GeoDataPlacemark::PlaceCity]   = 9;
    s_defaultMinZoomLevels[GeoDataPlacemark::PlaceHamlet] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::PlaceLocality]       = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::PlaceSuburb] = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::PlaceTownNationalCapital] = 11;
    s_defaultMinZoomLevels[GeoDataPlacemark::PlaceTownCapital]    = 11;
    s_defaultMinZoomLevels[GeoDataPlacemark::PlaceTown]   = 11;
    s_defaultMinZoomLevels[GeoDataPlacemark::PlaceVillageNationalCapital] = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::PlaceVillageCapital] = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::PlaceVillage] = 13;

    s_defaultMinZoomLevels[GeoDataPlacemark::PowerTower] = 18;

    s_defaultMinZoomLevels[GeoDataPlacemark::RailwayAbandoned]    = 10;
    s_defaultMinZoomLevels[GeoDataPlacemark::RailwayConstruction] = 10;
    s_defaultMinZoomLevels[GeoDataPlacemark::RailwayFunicular]    = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::RailwayLightRail]    = 12;
    s_defaultMinZoomLevels[GeoDataPlacemark::RailwayMiniature]    = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::RailwayMonorail]     = 12;
    s_defaultMinZoomLevels[GeoDataPlacemark::RailwayNarrowGauge]  = 6;
    s_defaultMinZoomLevels[GeoDataPlacemark::RailwayPreserved]    = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::RailwayRail] = 6;
    s_defaultMinZoomLevels[GeoDataPlacemark::RailwaySubway]       = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::RailwayTram] = 14;

    s_defaultMinZoomLevels[GeoDataPlacemark::Satellite]   = 0;

    for (int shop = GeoDataPlacemark::ShopBeverages; shop <= GeoDataPlacemark::Shop; ++shop) {
        s_defaultMinZoomLevels[shop] = 17;
    }
    s_defaultMinZoomLevels[GeoDataPlacemark::ShopSupermarket] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::ShopDepartmentStore] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::ShopDoitYourself] = 16;

    s_defaultMinZoomLevels[GeoDataPlacemark::TourismAlpineHut]  = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::TourismWildernessHut] = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::TourismAttraction]  = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::TourismArtwork] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::HistoricCastle]  = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityCinema]  = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::TourismMuseum]  = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::HistoricRuins]  = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::AmenityTheatre]  = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::TourismThemePark]  = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::TourismViewPoint]  = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::TourismZoo]  = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::HistoricMonument]  = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::TourismInformation]  = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportAerodrome] = 9;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportAirportApron] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportAirportRunway] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportAirportTaxiway] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportBusStation]  = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportCarShare]  = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportFuel]  = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportHelipad] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportAirportTerminal] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportAirportGate] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportPlatform]   = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportSpeedCamera] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportRentalCar] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportRentalBicycle] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportRentalSki] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportTaxiRank]  = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportParking]  = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportBusStop]  = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportTrainStation]  = 13;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportTramStop]  = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportParkingSpace]  = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportBicycleParking]  = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportMotorcycleParking]  = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::TransportSubwayEntrance]  = 17;

    for (int religion = GeoDataPlacemark::ReligionPlaceOfWorship; religion <= GeoDataPlacemark::ReligionSikh; ++religion) {
        s_defaultMinZoomLevels[religion] = 17;
    }

    s_defaultMinZoomLevels[GeoDataPlacemark::UrbanArea]   = 3;

    s_defaultMinZoomLevels[GeoDataPlacemark::WaterwayCanal] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::WaterwayDitch] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::WaterwayDrain] = 17;
    s_defaultMinZoomLevels[GeoDataPlacemark::WaterwayStream] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::WaterwayRiver] = 3;
    s_defaultMinZoomLevels[GeoDataPlacemark::WaterwayWeir] = 17;

    s_defaultMinZoomLevels[GeoDataPlacemark::CrossingIsland] = 18;
    s_defaultMinZoomLevels[GeoDataPlacemark::CrossingRailway] = 18;
    s_defaultMinZoomLevels[GeoDataPlacemark::CrossingSignals] = 18;
    s_defaultMinZoomLevels[GeoDataPlacemark::CrossingZebra] = 18;

    s_defaultMinZoomLevels[GeoDataPlacemark::PisteDownhill] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::PisteNordic] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::PisteSkitour] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::PisteSled] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::PisteHike] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::PisteSleigh] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::PisteIceSkate] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::PisteSnowPark] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::PistePlayground] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::PisteSkiJump] = 15;

    s_defaultMinZoomLevels[GeoDataPlacemark::AerialwayStation] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::AerialwayPylon] = 16;
    s_defaultMinZoomLevels[GeoDataPlacemark::AerialwayCableCar] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::AerialwayGondola] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::AerialwayChairLift] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::AerialwayMixedLift] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::AerialwayDragLift] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::AerialwayTBar] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::AerialwayJBar] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::AerialwayPlatter] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::AerialwayRopeTow] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::AerialwayMagicCarpet] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::AerialwayZipLine] = 15;
    s_defaultMinZoomLevels[GeoDataPlacemark::AerialwayGoods] = 15;

    for (int i = GeoDataPlacemark::PlaceCity; i < GeoDataPlacemark::LastIndex; i++) {
        if (s_defaultMinZoomLevels[i] < 0) {
            qDebug() << "Missing default min zoom level for GeoDataPlacemark::GeoDataVisualCategory " << i;
            Q_ASSERT(false && "StyleBuilder::Private::initializeMinimumZoomLevels is incomplete");
            s_defaultMinZoomLevels[i] = 15;
        }
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

void StyleBuilder::setDefaultFont(const QFont& font)
{
    d->m_defaultFont = font;
    reset();
}

QColor StyleBuilder::defaultLabelColor() const
{
    return d->m_defaultLabelColor;
}

void StyleBuilder::setDefaultLabelColor(const QColor& color)
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

    if (parameters.relation) {
        auto style = d->createRelationStyle(parameters);
        if (style) {
            return style;
        }
    }

    return d->createPlacemarkStyle(parameters);
}

GeoDataStyle::ConstPtr StyleBuilder::Private::presetStyle(GeoDataPlacemark::GeoDataVisualCategory visualCategory) const
{
    if (!m_defaultStyleInitialized) {
        const_cast<StyleBuilder::Private *>(this)->initializeDefaultStyles(); // const cast due to lazy initialization
    }

    if (visualCategory != GeoDataPlacemark::None && m_defaultStyle[visualCategory]) {
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
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseResidential);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseAllotments);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseBasin);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseCemetery);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseCommercial);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseConstruction);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseFarmland);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseFarmyard);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseGarages);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseIndustrial);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseLandfill);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseMeadow);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseMilitary);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseQuarry);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseRailway);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseReservoir);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseRetail);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseOrchard);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LanduseVineyard);

        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::Bathymetry);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LeisureGolfCourse);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LeisureMinigolfCourse);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::NaturalBeach);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::NaturalWetland);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::NaturalGlacier);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::NaturalIceShelf);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::NaturalVolcano);
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
        for (int i = GeoDataPlacemark::WaterwayCanal; i <= GeoDataPlacemark::WaterwayStream; ++i) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "outline");
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "inline");
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "label");
        }

        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::NaturalReef, "outline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::NaturalReef, "inline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::NaturalReef, "label");
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::LeisureMarina);
        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::ManmadePier);
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::ManmadePier, "outline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::ManmadePier, "inline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::ManmadePier, "label");

        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::TransportAirportApron);

        for (int i = GeoDataPlacemark::HighwaySteps; i <= GeoDataPlacemark::HighwayMotorway; i++) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "outline");
        }
        for (int i = GeoDataPlacemark::HighwaySteps; i <= GeoDataPlacemark::HighwayMotorway; i++) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "inline");
        }
        for (int i = GeoDataPlacemark::RailwayRail; i <= GeoDataPlacemark::RailwayFunicular; i++) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "outline");
        }
        for (int i = GeoDataPlacemark::RailwayRail; i <= GeoDataPlacemark::RailwayFunicular; i++) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "inline");
        }
        // Highway labels shall appear on top of railways, hence here and not already above
        for (int i = GeoDataPlacemark::HighwaySteps; i <= GeoDataPlacemark::HighwayMotorway; i++) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "label");
        }
        for (int i = GeoDataPlacemark::RailwayRail; i <= GeoDataPlacemark::RailwayFunicular; i++) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "label");
        }

        paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::TransportPlatform);
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::TransportPlatform, "outline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::TransportPlatform, "inline");
        paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::TransportPlatform, "label");

        for (int i = GeoDataPlacemark::PisteDownhill; i <= GeoDataPlacemark::PisteSkiJump; ++i) {
            paintLayerOrder << Private::createPaintLayerItem("Polygon", GeoDataPlacemark::GeoDataVisualCategory(i));
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "outline");
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "inline");
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "label");
        }
        for (int i = GeoDataPlacemark::AerialwayCableCar; i <= GeoDataPlacemark::AerialwayGoods; ++i) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "outline");
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "inline");
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "label");
        }

        for (int i = GeoDataPlacemark::AdminLevel1; i <= GeoDataPlacemark::AdminLevel11; i++) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "outline");
        }
        for (int i = GeoDataPlacemark::AdminLevel1; i <= GeoDataPlacemark::AdminLevel11; i++) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "inline");
        }
        for (int i = GeoDataPlacemark::AdminLevel1; i <= GeoDataPlacemark::AdminLevel11; i++) {
            paintLayerOrder << Private::createPaintLayerItem("LineString", GeoDataPlacemark::GeoDataVisualCategory(i), "label");
        }

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
    Q_ASSERT(Private::s_defaultMinZoomLevelsInitialized);
    return Private::s_defaultMinZoomLevels[placemark.visualCategory()];
}

int StyleBuilder::minimumZoomLevel(const GeoDataPlacemark::GeoDataVisualCategory &visualCategory)
{
    Private::initializeMinimumZoomLevels();
    return Private::s_defaultMinZoomLevels[visualCategory];
}

qint64 StyleBuilder::popularity(const GeoDataPlacemark *placemark)
{
    qint64 const defaultValue = 100;
    int const offset = 10;
    if (StyleBuilder::Private::s_popularities.isEmpty()) {
        QVector<GeoDataPlacemark::GeoDataVisualCategory> popularities;
        popularities << GeoDataPlacemark::PlaceCityNationalCapital;
        popularities << GeoDataPlacemark::PlaceTownNationalCapital;
        popularities << GeoDataPlacemark::PlaceCityCapital;
        popularities << GeoDataPlacemark::PlaceTownCapital;
        popularities << GeoDataPlacemark::PlaceCity;
        popularities << GeoDataPlacemark::PlaceTown;
        popularities << GeoDataPlacemark::PlaceSuburb;
        popularities << GeoDataPlacemark::PlaceVillageNationalCapital;
        popularities << GeoDataPlacemark::PlaceVillageCapital;
        popularities << GeoDataPlacemark::PlaceVillage;
        popularities << GeoDataPlacemark::PlaceHamlet;
        popularities << GeoDataPlacemark::PlaceLocality;

        popularities << GeoDataPlacemark::AmenityEmergencyPhone;
        popularities << GeoDataPlacemark::AmenityMountainRescue;
        popularities << GeoDataPlacemark::HealthHospital;
        popularities << GeoDataPlacemark::AmenityToilets;
        popularities << GeoDataPlacemark::MoneyAtm;
        popularities << GeoDataPlacemark::TransportSpeedCamera;

        popularities << GeoDataPlacemark::NaturalPeak;
        popularities << GeoDataPlacemark::NaturalVolcano;

        popularities << GeoDataPlacemark::AccomodationHotel;
        popularities << GeoDataPlacemark::AccomodationMotel;
        popularities << GeoDataPlacemark::AccomodationGuestHouse;
        popularities << GeoDataPlacemark::AccomodationYouthHostel;
        popularities << GeoDataPlacemark::AccomodationHostel;
        popularities << GeoDataPlacemark::AccomodationCamping;

        popularities << GeoDataPlacemark::HealthDentist;
        popularities << GeoDataPlacemark::HealthDoctors;
        popularities << GeoDataPlacemark::HealthPharmacy;
        popularities << GeoDataPlacemark::HealthVeterinary;

        popularities << GeoDataPlacemark::AmenityLibrary;
        popularities << GeoDataPlacemark::EducationCollege;
        popularities << GeoDataPlacemark::EducationSchool;
        popularities << GeoDataPlacemark::EducationUniversity;

        popularities << GeoDataPlacemark::FoodBar;
        popularities << GeoDataPlacemark::FoodBiergarten;
        popularities << GeoDataPlacemark::FoodCafe;
        popularities << GeoDataPlacemark::FoodFastFood;
        popularities << GeoDataPlacemark::FoodPub;
        popularities << GeoDataPlacemark::FoodRestaurant;

        popularities << GeoDataPlacemark::MoneyBank;

        popularities << GeoDataPlacemark::HistoricArchaeologicalSite;
        popularities << GeoDataPlacemark::AmenityCarWash;
        popularities << GeoDataPlacemark::AmenityEmbassy;
        popularities << GeoDataPlacemark::LeisureWaterPark;
        popularities << GeoDataPlacemark::AmenityCommunityCentre;
        popularities << GeoDataPlacemark::AmenityFountain;
        popularities << GeoDataPlacemark::AmenityNightClub;
        popularities << GeoDataPlacemark::AmenityCourtHouse;
        popularities << GeoDataPlacemark::AmenityFireStation;
        popularities << GeoDataPlacemark::AmenityShelter;
        popularities << GeoDataPlacemark::AmenityHuntingStand;
        popularities << GeoDataPlacemark::AmenityPolice;
        popularities << GeoDataPlacemark::AmenityPostBox;
        popularities << GeoDataPlacemark::AmenityPostOffice;
        popularities << GeoDataPlacemark::AmenityPrison;
        popularities << GeoDataPlacemark::AmenityRecycling;
        popularities << GeoDataPlacemark::AmenitySocialFacility;
        popularities << GeoDataPlacemark::AmenityTelephone;
        popularities << GeoDataPlacemark::AmenityTownHall;
        popularities << GeoDataPlacemark::AmenityDrinkingWater;
        popularities << GeoDataPlacemark::AmenityGraveyard;

        popularities << GeoDataPlacemark::ManmadeBridge;
        popularities << GeoDataPlacemark::ManmadeLighthouse;
        popularities << GeoDataPlacemark::ManmadePier;
        popularities << GeoDataPlacemark::ManmadeWaterTower;
        popularities << GeoDataPlacemark::ManmadeWindMill;

        popularities << GeoDataPlacemark::TourismAttraction;
        popularities << GeoDataPlacemark::TourismArtwork;
        popularities << GeoDataPlacemark::HistoricCastle;
        popularities << GeoDataPlacemark::AmenityCinema;
        popularities << GeoDataPlacemark::TourismInformation;
        popularities << GeoDataPlacemark::HistoricMonument;
        popularities << GeoDataPlacemark::TourismMuseum;
        popularities << GeoDataPlacemark::HistoricRuins;
        popularities << GeoDataPlacemark::AmenityTheatre;
        popularities << GeoDataPlacemark::TourismThemePark;
        popularities << GeoDataPlacemark::TourismViewPoint;
        popularities << GeoDataPlacemark::TourismZoo;
        popularities << GeoDataPlacemark::TourismAlpineHut;
        popularities << GeoDataPlacemark::TourismWildernessHut;

        popularities << GeoDataPlacemark::HistoricMemorial;

        popularities << GeoDataPlacemark::TransportAerodrome;
        popularities << GeoDataPlacemark::TransportHelipad;
        popularities << GeoDataPlacemark::TransportAirportTerminal;
        popularities << GeoDataPlacemark::TransportBusStation;
        popularities << GeoDataPlacemark::TransportBusStop;
        popularities << GeoDataPlacemark::TransportCarShare;
        popularities << GeoDataPlacemark::TransportFuel;
        popularities << GeoDataPlacemark::TransportParking;
        popularities << GeoDataPlacemark::TransportParkingSpace;
        popularities << GeoDataPlacemark::TransportPlatform;
        popularities << GeoDataPlacemark::TransportRentalBicycle;
        popularities << GeoDataPlacemark::TransportRentalCar;
        popularities << GeoDataPlacemark::TransportRentalSki;
        popularities << GeoDataPlacemark::TransportTaxiRank;
        popularities << GeoDataPlacemark::TransportTrainStation;
        popularities << GeoDataPlacemark::TransportTramStop;
        popularities << GeoDataPlacemark::TransportBicycleParking;
        popularities << GeoDataPlacemark::TransportMotorcycleParking;
        popularities << GeoDataPlacemark::TransportSubwayEntrance;
        popularities << GeoDataPlacemark::AerialwayStation;

        popularities << GeoDataPlacemark::ShopBeverages;
        popularities << GeoDataPlacemark::ShopHifi;
        popularities << GeoDataPlacemark::ShopSupermarket;
        popularities << GeoDataPlacemark::ShopAlcohol;
        popularities << GeoDataPlacemark::ShopBakery;
        popularities << GeoDataPlacemark::ShopButcher;
        popularities << GeoDataPlacemark::ShopConfectionery;
        popularities << GeoDataPlacemark::ShopConvenience;
        popularities << GeoDataPlacemark::ShopGreengrocer;
        popularities << GeoDataPlacemark::ShopSeafood;
        popularities << GeoDataPlacemark::ShopDepartmentStore;
        popularities << GeoDataPlacemark::ShopKiosk;
        popularities << GeoDataPlacemark::ShopBag;
        popularities << GeoDataPlacemark::ShopClothes;
        popularities << GeoDataPlacemark::ShopFashion;
        popularities << GeoDataPlacemark::ShopJewelry;
        popularities << GeoDataPlacemark::ShopShoes;
        popularities << GeoDataPlacemark::ShopVarietyStore;
        popularities << GeoDataPlacemark::ShopBeauty;
        popularities << GeoDataPlacemark::ShopChemist;
        popularities << GeoDataPlacemark::ShopCosmetics;
        popularities << GeoDataPlacemark::ShopHairdresser;
        popularities << GeoDataPlacemark::ShopOptician;
        popularities << GeoDataPlacemark::ShopPerfumery;
        popularities << GeoDataPlacemark::ShopDoitYourself;
        popularities << GeoDataPlacemark::ShopFlorist;
        popularities << GeoDataPlacemark::ShopHardware;
        popularities << GeoDataPlacemark::ShopFurniture;
        popularities << GeoDataPlacemark::ShopElectronics;
        popularities << GeoDataPlacemark::ShopMobilePhone;
        popularities << GeoDataPlacemark::ShopBicycle;
        popularities << GeoDataPlacemark::ShopCar;
        popularities << GeoDataPlacemark::ShopCarRepair;
        popularities << GeoDataPlacemark::ShopCarParts;
        popularities << GeoDataPlacemark::ShopMotorcycle;
        popularities << GeoDataPlacemark::ShopOutdoor;
        popularities << GeoDataPlacemark::ShopSports;
        popularities << GeoDataPlacemark::ShopCopy;
        popularities << GeoDataPlacemark::ShopArt;
        popularities << GeoDataPlacemark::ShopMusicalInstrument;
        popularities << GeoDataPlacemark::ShopPhoto;
        popularities << GeoDataPlacemark::ShopBook;
        popularities << GeoDataPlacemark::ShopGift;
        popularities << GeoDataPlacemark::ShopStationery;
        popularities << GeoDataPlacemark::ShopLaundry;
        popularities << GeoDataPlacemark::ShopPet;
        popularities << GeoDataPlacemark::ShopToys;
        popularities << GeoDataPlacemark::ShopTravelAgency;
        popularities << GeoDataPlacemark::ShopDeli;
        popularities << GeoDataPlacemark::ShopTobacco;
        popularities << GeoDataPlacemark::ShopTea;
        popularities << GeoDataPlacemark::Shop;

        popularities << GeoDataPlacemark::LeisureGolfCourse;
        popularities << GeoDataPlacemark::LeisureMinigolfCourse;
        popularities << GeoDataPlacemark::LeisurePark;
        popularities << GeoDataPlacemark::LeisurePlayground;
        popularities << GeoDataPlacemark::LeisurePitch;
        popularities << GeoDataPlacemark::LeisureSportsCentre;
        popularities << GeoDataPlacemark::LeisureStadium;
        popularities << GeoDataPlacemark::LeisureTrack;
        popularities << GeoDataPlacemark::LeisureSwimmingPool;

        popularities << GeoDataPlacemark::CrossingIsland;
        popularities << GeoDataPlacemark::CrossingRailway;
        popularities << GeoDataPlacemark::CrossingSignals;
        popularities << GeoDataPlacemark::CrossingZebra;
        popularities << GeoDataPlacemark::HighwayTrafficSignals;

        popularities << GeoDataPlacemark::BarrierGate;
        popularities << GeoDataPlacemark::BarrierLiftGate;
        popularities << GeoDataPlacemark::AmenityBench;
        popularities << GeoDataPlacemark::NaturalTree;
        popularities << GeoDataPlacemark::NaturalCave;
        popularities << GeoDataPlacemark::AmenityWasteBasket;
        popularities << GeoDataPlacemark::AerialwayPylon;
        popularities << GeoDataPlacemark::PowerTower;

        int value = defaultValue + offset * popularities.size();
        for (auto popularity : popularities) {
            StyleBuilder::Private::s_popularities[popularity] = value;
            value -= offset;
        }
    }

    bool const isPrivate = placemark->osmData().containsTag(QStringLiteral("access"), QStringLiteral("private"));
    int const base = defaultValue + (isPrivate ? 0 : offset * StyleBuilder::Private::s_popularities.size());
    return base + StyleBuilder::Private::s_popularities.value(placemark->visualCategory(), defaultValue);
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
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceCityNationalCapital] = "PlaceCityNationalCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceSuburb] = "PlaceSuburb";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceHamlet] = "PlaceHamlet";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceLocality] = "PlaceLocality";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceTown] = "PlaceTown";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceTownCapital] = "PlaceTownCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceTownNationalCapital] = "PlaceTownNationalCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceVillage] = "PlaceVillage";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceVillageCapital] = "PlaceVillageCapital";
        visualCategoryNames[GeoDataPlacemark::GeoDataPlacemark::PlaceVillageNationalCapital] = "PlaceVillageNationalCapital";
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
        visualCategoryNames[GeoDataPlacemark::HighwayRaceway] = "HighwayRaceway";
        visualCategoryNames[GeoDataPlacemark::HighwayTrunkLink] = "HighwayTrunkLink";
        visualCategoryNames[GeoDataPlacemark::HighwayTrunk] = "HighwayTrunk";
        visualCategoryNames[GeoDataPlacemark::HighwayMotorwayLink] = "HighwayMotorwayLink";
        visualCategoryNames[GeoDataPlacemark::HighwayMotorway] = "HighwayMotorway";
        visualCategoryNames[GeoDataPlacemark::HighwayCorridor] = "HighwayCorridor";
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
        visualCategoryNames[GeoDataPlacemark::AmenityEmbassy] = "AmenityEmbassy";
        visualCategoryNames[GeoDataPlacemark::AmenityEmergencyPhone] = "AmenityEmergencyPhone";
        visualCategoryNames[GeoDataPlacemark::AmenityMountainRescue] = "AmenityMountainRescue";
        visualCategoryNames[GeoDataPlacemark::LeisureWaterPark] = "LeisureWaterPark";
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
        visualCategoryNames[GeoDataPlacemark::AmenityChargingStation] = "ChargingStation";
        visualCategoryNames[GeoDataPlacemark::AmenityCarWash] = "CarWash";
        visualCategoryNames[GeoDataPlacemark::AmenitySocialFacility] = "SocialFacility";
        visualCategoryNames[GeoDataPlacemark::BarrierCityWall] = "BarrierCityWall";
        visualCategoryNames[GeoDataPlacemark::BarrierGate] = "BarrierGate";
        visualCategoryNames[GeoDataPlacemark::BarrierLiftGate] = "BarrierLiftGate";
        visualCategoryNames[GeoDataPlacemark::BarrierWall] = "BarrierWall";
        visualCategoryNames[GeoDataPlacemark::NaturalVolcano] = "NaturalVolcano";
        visualCategoryNames[GeoDataPlacemark::NaturalPeak] = "NaturalPeak";
        visualCategoryNames[GeoDataPlacemark::NaturalTree] = "NaturalTree";
        visualCategoryNames[GeoDataPlacemark::NaturalCave] = "NaturalCave";
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
        visualCategoryNames[GeoDataPlacemark::ShopSports] = "ShopSports";
        visualCategoryNames[GeoDataPlacemark::ShopCopy] = "ShopCopy";
        visualCategoryNames[GeoDataPlacemark::ShopArt] = "ShopArt";
        visualCategoryNames[GeoDataPlacemark::ShopMusicalInstrument] = "ShopMusicalInstrument";
        visualCategoryNames[GeoDataPlacemark::ShopPhoto] = "ShopPhoto";
        visualCategoryNames[GeoDataPlacemark::ShopBook] = "ShopBook";
        visualCategoryNames[GeoDataPlacemark::ShopGift] = "ShopGift";
        visualCategoryNames[GeoDataPlacemark::ShopStationery] = "ShopStationery";
        visualCategoryNames[GeoDataPlacemark::ShopLaundry] = "ShopLaundry";
        visualCategoryNames[GeoDataPlacemark::ShopPet] = "ShopPet";
        visualCategoryNames[GeoDataPlacemark::ShopToys] = "ShopToys";
        visualCategoryNames[GeoDataPlacemark::ShopTravelAgency] = "ShopTravelAgency";
        visualCategoryNames[GeoDataPlacemark::ShopDeli] = "ShopDeli";
        visualCategoryNames[GeoDataPlacemark::ShopTobacco] = "ShopTobacco";
        visualCategoryNames[GeoDataPlacemark::ShopTea] = "ShopTea";
        visualCategoryNames[GeoDataPlacemark::Shop] = "Shop";
        visualCategoryNames[GeoDataPlacemark::ManmadeBridge] = "ManmadeBridge";
        visualCategoryNames[GeoDataPlacemark::ManmadeLighthouse] = "ManmadeLighthouse";
        visualCategoryNames[GeoDataPlacemark::ManmadePier] = "ManmadePier";
        visualCategoryNames[GeoDataPlacemark::ManmadeWaterTower] = "ManmadeWaterTower";
        visualCategoryNames[GeoDataPlacemark::ManmadeWindMill] = "ManmadeWindMill";
        visualCategoryNames[GeoDataPlacemark::TourismAttraction] = "TouristAttraction";
        visualCategoryNames[GeoDataPlacemark::TourismArtwork] = "TouristArtwork";
        visualCategoryNames[GeoDataPlacemark::HistoricArchaeologicalSite] = "HistoricArchaeologicalSite";
        visualCategoryNames[GeoDataPlacemark::HistoricCastle] = "HistoricCastle";
        visualCategoryNames[GeoDataPlacemark::HistoricMemorial] = "HistoricMemorial";
        visualCategoryNames[GeoDataPlacemark::HistoricMonument] = "HistoricMonument";
        visualCategoryNames[GeoDataPlacemark::AmenityCinema] = "TouristCinema";
        visualCategoryNames[GeoDataPlacemark::TourismInformation] = "TouristInformation";
        visualCategoryNames[GeoDataPlacemark::TourismMuseum] = "TouristMuseum";
        visualCategoryNames[GeoDataPlacemark::HistoricRuins] = "TouristRuin";
        visualCategoryNames[GeoDataPlacemark::AmenityTheatre] = "TouristTheatre";
        visualCategoryNames[GeoDataPlacemark::TourismThemePark] = "TouristThemePark";
        visualCategoryNames[GeoDataPlacemark::TourismViewPoint] = "TouristViewPoint";
        visualCategoryNames[GeoDataPlacemark::TourismZoo] = "TouristZoo";
        visualCategoryNames[GeoDataPlacemark::TourismAlpineHut] = "TouristAlpineHut";
        visualCategoryNames[GeoDataPlacemark::TourismWildernessHut] = "TouristWildernessHut";
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
        visualCategoryNames[GeoDataPlacemark::TransportRentalSki] = "TransportRentalSki";
        visualCategoryNames[GeoDataPlacemark::TransportTaxiRank] = "TransportTaxiRank";
        visualCategoryNames[GeoDataPlacemark::TransportTrainStation] = "TransportTrainStation";
        visualCategoryNames[GeoDataPlacemark::TransportTramStop] = "TransportTramStop";
        visualCategoryNames[GeoDataPlacemark::TransportSpeedCamera] = "TransportSpeedCamera";
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
        visualCategoryNames[GeoDataPlacemark::LeisureMinigolfCourse] = "LeisureMinigolfCourse";
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
        visualCategoryNames[GeoDataPlacemark::AerialwayStation] = "AerialwayStation";
        visualCategoryNames[GeoDataPlacemark::AerialwayPylon] = "AerialwayPylon";
        visualCategoryNames[GeoDataPlacemark::AerialwayCableCar] = "AerialwayCableCar";
        visualCategoryNames[GeoDataPlacemark::AerialwayGondola] = "AerialwayGondola";
        visualCategoryNames[GeoDataPlacemark::AerialwayChairLift] = "AerialwayChairLift";
        visualCategoryNames[GeoDataPlacemark::AerialwayMixedLift] = "AerialwayMixedLift";
        visualCategoryNames[GeoDataPlacemark::AerialwayDragLift] = "AerialwayDragLift";
        visualCategoryNames[GeoDataPlacemark::AerialwayTBar] = "AerialwayTBar";
        visualCategoryNames[GeoDataPlacemark::AerialwayJBar] = "AerialwayJBar";
        visualCategoryNames[GeoDataPlacemark::AerialwayPlatter] = "AerialwayPlatter";
        visualCategoryNames[GeoDataPlacemark::AerialwayRopeTow] = "AerialwayRopeTow";
        visualCategoryNames[GeoDataPlacemark::AerialwayMagicCarpet] = "AerialwayMagicCarpet";
        visualCategoryNames[GeoDataPlacemark::AerialwayZipLine] = "AerialwayZipLine";
        visualCategoryNames[GeoDataPlacemark::AerialwayGoods] = "AerialwayGoods";
        visualCategoryNames[GeoDataPlacemark::PisteDownhill] = "PisteDownhill";
        visualCategoryNames[GeoDataPlacemark::PisteNordic] = "PisteNordic";
        visualCategoryNames[GeoDataPlacemark::PisteSkitour] = "PisteSkitour";
        visualCategoryNames[GeoDataPlacemark::PisteSled] = "PisteSled";
        visualCategoryNames[GeoDataPlacemark::PisteHike] = "PisteHike";
        visualCategoryNames[GeoDataPlacemark::PisteSleigh] = "PisteSleigh";
        visualCategoryNames[GeoDataPlacemark::PisteIceSkate] = "PisteIceSkate";
        visualCategoryNames[GeoDataPlacemark::PisteSnowPark] = "PisteSnowPark";
        visualCategoryNames[GeoDataPlacemark::PistePlayground] = "PistePlayground";
        visualCategoryNames[GeoDataPlacemark::PisteSkiJump] = "PisteSkiJump";
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
        visualCategoryNames[GeoDataPlacemark::WaterwayCanal] = "WaterwayCanal";
        visualCategoryNames[GeoDataPlacemark::WaterwayDitch] = "WaterwayDitch";
        visualCategoryNames[GeoDataPlacemark::WaterwayDrain] = "WaterwayDrain";
        visualCategoryNames[GeoDataPlacemark::WaterwayStream] = "WaterwayStream";
        visualCategoryNames[GeoDataPlacemark::WaterwayRiver] = "WaterwayRiver";
        visualCategoryNames[GeoDataPlacemark::WaterwayWeir] = "WaterwayWeir";
        visualCategoryNames[GeoDataPlacemark::CrossingIsland] = "CrossingIsland";
        visualCategoryNames[GeoDataPlacemark::CrossingRailway] = "CrossingRailway";
        visualCategoryNames[GeoDataPlacemark::CrossingSignals] = "CrossingSignals";
        visualCategoryNames[GeoDataPlacemark::CrossingZebra] = "CrossingZebra";
        visualCategoryNames[GeoDataPlacemark::LastIndex] = "LastIndex";
    }

    Q_ASSERT(visualCategoryNames.contains(category));
    return visualCategoryNames[category];
}

QHash<StyleBuilder::OsmTag, GeoDataPlacemark::GeoDataVisualCategory> StyleBuilder::osmTagMapping()
{
    Private::initializeOsmVisualCategories();
    return Private::s_visualCategories;
}

QStringList StyleBuilder::shopValues()
{
    // from https://taginfo.openstreetmap.org/keys/building#values
    static const QStringList osmShopValues = QStringList()
            << "cheese" << "chocolate" << "coffee" << "dairy" << "farm"
            << "pasta" << "pastry" << "wine" << "general" << "mall"
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
            << "weapons" << "user defined";

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
    QString const yes(QStringLiteral("yes"));
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
        osmData.containsTag(QStringLiteral("highway"), QStringLiteral("razed")) ||
        osmData.containsTag(QStringLiteral("piste:abandoned"), yes)) {
        return GeoDataPlacemark::None;
    }

    if (osmData.containsTag(QStringLiteral("building"), yes)) {
        return GeoDataPlacemark::Building;
    }

    if (osmData.containsTag(QStringLiteral("historic"), QStringLiteral("castle")) && osmData.containsTag(QStringLiteral("castle_type"), QStringLiteral("kremlin"))) {
        return GeoDataPlacemark::None;
    }

    if (osmData.containsTag(QStringLiteral("natural"), QStringLiteral("glacier")) && osmData.containsTag(QStringLiteral("glacier:type"), QStringLiteral("shelf"))) {
        return GeoDataPlacemark::NaturalIceShelf;
    }

    if (osmData.containsTag(QStringLiteral("highway"), QStringLiteral("crossing"))) {
        QStringList const crossings = osmData.tagValue(QStringLiteral("crossing")).split(';');
        QString const crossingRef = osmData.tagValue(QStringLiteral("crossing_ref"));
        if (crossingRef == QStringLiteral("zebra") ||
            crossingRef == QStringLiteral("tiger") ||
            crossings.contains(QStringLiteral("zebra")) ||
            crossings.contains(QStringLiteral("tiger"))) {
            return GeoDataPlacemark::CrossingZebra;
        } else if (crossingRef == QStringLiteral("toucan") ||
                   crossingRef == QStringLiteral("pelican") ||
                   crossings.contains(QStringLiteral("traffic_signals")) ||
                   crossings.contains(QStringLiteral("toucan")) ||
                   crossings.contains(QStringLiteral("pelican"))) {
            return GeoDataPlacemark::CrossingSignals;
        } else if (crossings.contains(QStringLiteral("island"))) {
            return GeoDataPlacemark::CrossingIsland;
        }
    }
    if (osmData.containsTag(QStringLiteral("railway"), QStringLiteral("crossing")) ||
        osmData.containsTag(QStringLiteral("railway"), QStringLiteral("level_crossing"))) {
        return GeoDataPlacemark::CrossingRailway;
    }

    Private::initializeOsmVisualCategories();

    auto const pisteType = osmData.tagValue(QStringLiteral("piste:type"));
    if (!pisteType.isEmpty()) {
        auto const tag = OsmTag(QStringLiteral("piste:type"), pisteType);
        auto category = Private::s_visualCategories.value(tag, GeoDataPlacemark::None);
        if (category != GeoDataPlacemark::None) {
            return category;
        }
    }

    QString const capital(QStringLiteral("capital"));
    QString const admin_level(QStringLiteral("admin_level"));
    // National capitals have admin_level=2
    // More at http://wiki.openstreetmap.org/wiki/Key:capital#Using_relations_for_capitals
    QString const national_level(QStringLiteral("2"));

    for (auto iter = osmData.tagsBegin(), end = osmData.tagsEnd(); iter != end; ++iter) {
        const auto tag = OsmTag(iter.key(), iter.value());
        GeoDataPlacemark::GeoDataVisualCategory category = Private::s_visualCategories.value(tag, GeoDataPlacemark::None);
        if (category != GeoDataPlacemark::None) {
            if (category == GeoDataPlacemark::PlaceCity && osmData.containsTag(admin_level, national_level)) {
                category = GeoDataPlacemark::PlaceCityNationalCapital;
            } else if (category == GeoDataPlacemark::PlaceCity && osmData.containsTag(capital, yes)) {
                category = GeoDataPlacemark::PlaceCityCapital;
            } else if (category == GeoDataPlacemark::PlaceTown && osmData.containsTag(admin_level, national_level)) {
                category = GeoDataPlacemark::PlaceTownNationalCapital;
            } else if (category == GeoDataPlacemark::PlaceTown && osmData.containsTag(capital, yes)) {
                category = GeoDataPlacemark::PlaceTownCapital;
            } else if (category == GeoDataPlacemark::PlaceVillage && osmData.containsTag(admin_level, national_level)) {
                category = GeoDataPlacemark::PlaceVillageNationalCapital;
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
    tileLevel(tileLevel_),
    relation(nullptr)
{
    // nothing to do
}

}
