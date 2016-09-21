//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
//

#include "GeoPolygonGraphicsItem.h"

#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"
#include "GeoPainter.h"
#include "GeoDataTypes.h"
#include "GeoDataPlacemark.h"
#include "ViewportParams.h"
#include "GeoDataStyle.h"
#include "MarbleDirs.h"
#include "MarbleDebug.h"
#include "OsmPlacemarkData.h"
#include "StyleBuilder.h"

#include <qmath.h>

namespace Marble
{

GeoPolygonGraphicsItem::GeoPolygonGraphicsItem(const GeoDataFeature *feature, const GeoDataPolygon *polygon) :
    GeoGraphicsItem(feature),
    m_polygon(polygon),
    m_ring(0),
    m_buildingHeight(extractBuildingHeight(feature)),
    m_buildingLabel(extractBuildingLabel(feature)),
    m_entries(extractNamedEntries(feature))
{
    const GeoDataFeature::GeoDataVisualCategory visualCategory = feature->visualCategory();
    if (isBuilding(visualCategory))
    {
        setZValue(this->zValue() + m_buildingHeight);
        Q_ASSERT(m_buildingHeight > 0.0);

        QStringList paintLayers;
        paintLayers << QStringLiteral("Polygon/Building/frame")
                    << QStringLiteral("Polygon/Building/roof");
        setPaintLayers(paintLayers);
    }
    else {
        if (visualCategory == GeoDataFeature::Bathymetry) {
            const int elevation = extractBathymetryElevation(feature);
            setZValue(zValue() + elevation);
        }

        const QString paintLayer = QLatin1String("Polygon/") + StyleBuilder::visualCategoryName(visualCategory);
        setPaintLayers(QStringList() << paintLayer);
    }
}

GeoPolygonGraphicsItem::GeoPolygonGraphicsItem(const GeoDataFeature *feature, const GeoDataLinearRing *ring) :
    GeoGraphicsItem(feature),
    m_polygon(0),
    m_ring(ring),
    m_buildingHeight(extractBuildingHeight(feature)),
    m_buildingLabel(extractBuildingLabel(feature)),
    m_entries(extractNamedEntries(feature))
{
    const GeoDataFeature::GeoDataVisualCategory visualCategory = feature->visualCategory();
    if (isBuilding(visualCategory))
    {
        setZValue(this->zValue() + m_buildingHeight);
        Q_ASSERT(m_buildingHeight > 0.0);

        QStringList paintLayers;
        paintLayers << QStringLiteral("Polygon/Building/frame")
                    << QStringLiteral("Polygon/Building/roof");
        setPaintLayers(paintLayers);
    }
    else {
        if (visualCategory == GeoDataFeature::Bathymetry) {
            const int elevation = extractBathymetryElevation(feature);
            setZValue(zValue() + elevation);
        }

        const QString paintLayer = QLatin1String("Polygon/") + StyleBuilder::visualCategoryName(visualCategory);
        setPaintLayers(QStringList() << paintLayer);
    }
}

int GeoPolygonGraphicsItem::extractBathymetryElevation(const GeoDataFeature *feature)
{
    const GeoDataFeature::GeoDataVisualCategory visualCategory = feature->visualCategory();
    if (visualCategory != GeoDataFeature::Bathymetry) {
        return 0;
    }

    int elevation = 0;

    if (feature->nodeType() == GeoDataTypes::GeoDataPlacemarkType) {
        const GeoDataPlacemark *placemark = static_cast<const GeoDataPlacemark *>(feature);
        const OsmPlacemarkData &osmData = placemark->osmData();

        const auto tagIter = osmData.findTag(QStringLiteral("ele"));
        if (tagIter != osmData.tagsEnd()) {
            elevation = tagIter.value().toInt();
        }
    }

    return elevation;


}

bool GeoPolygonGraphicsItem::isBuilding(GeoDataFeature::GeoDataVisualCategory visualCategory)
{
    switch (visualCategory) {
    case GeoDataFeature::Building:
        //case GeoDataFeature::AccomodationCamping:
    case GeoDataFeature::AccomodationHostel:
    case GeoDataFeature::AccomodationHotel:
    case GeoDataFeature::AccomodationMotel:
    case GeoDataFeature::AccomodationYouthHostel:
    case GeoDataFeature::AmenityLibrary:
    case GeoDataFeature::EducationCollege:
    //case GeoDataFeature::EducationSchool:
    case GeoDataFeature::EducationUniversity:
    case GeoDataFeature::FoodBar:
    case GeoDataFeature::FoodBiergarten:
    case GeoDataFeature::FoodCafe:
    case GeoDataFeature::FoodFastFood:
    case GeoDataFeature::FoodPub:
    case GeoDataFeature::FoodRestaurant:
    case GeoDataFeature::HealthDoctors:
    case GeoDataFeature::HealthHospital:
    case GeoDataFeature::HealthPharmacy:
    case GeoDataFeature::MoneyAtm:
    case GeoDataFeature::MoneyBank:
    case GeoDataFeature::ShopBeverages:
    case GeoDataFeature::ShopHifi:
    case GeoDataFeature::ShopSupermarket:
        //case GeoDataFeature::TouristAttraction:
        //case GeoDataFeature::TouristCastle:
    case GeoDataFeature::TouristCinema:
        //case GeoDataFeature::TouristMonument:
    case GeoDataFeature::TouristMuseum:
        //case GeoDataFeature::TouristRuin:
    case GeoDataFeature::TouristTheatre:
        //case GeoDataFeature::TouristThemePark:
        //case GeoDataFeature::TouristViewPoint:
        //case GeoDataFeature::TouristZoo:
    case GeoDataFeature::ReligionPlaceOfWorship:
    case GeoDataFeature::ReligionBahai:
    case GeoDataFeature::ReligionBuddhist:
    case GeoDataFeature::ReligionChristian:
    case GeoDataFeature::ReligionHindu:
    case GeoDataFeature::ReligionJain:
    case GeoDataFeature::ReligionJewish:
    case GeoDataFeature::ReligionShinto:
    case GeoDataFeature::ReligionSikh:
        return true;

    default:
        break;
    }

    return false;
}

void GeoPolygonGraphicsItem::initializeBuildingPainting(const GeoPainter* painter, const ViewportParams *viewport,
                                                        bool &drawAccurate3D, bool &isCameraAboveBuilding, bool &hasInnerBoundaries,
                                                        QVector<QPolygonF*>& outlinePolygons,
                                                        QVector<QPolygonF*>& innerPolygons) const
{
    drawAccurate3D = false;
    isCameraAboveBuilding = false;

    QPointF offsetAtCorner = buildingOffset(QPointF(0, 0), viewport, &isCameraAboveBuilding);
    qreal maxOffset = qMax( qAbs( offsetAtCorner.x() ), qAbs( offsetAtCorner.y() ) );
    drawAccurate3D = painter->mapQuality() == HighQuality ? maxOffset > 5.0 : maxOffset > 8.0;

    // Since subtracting one fully contained polygon from another results in a single
    // polygon with a "connecting line" between the inner and outer part we need
    // to first paint the inner area with no pen and then the outlines with the correct pen.
    hasInnerBoundaries = m_polygon ? !m_polygon->innerBoundaries().isEmpty() : false;
    if (m_polygon) {
        if (hasInnerBoundaries) {
            screenPolygons(viewport, m_polygon, innerPolygons, outlinePolygons);
        }
        viewport->screenCoordinates(m_polygon->outerBoundary(), outlinePolygons);
    } else if (m_ring) {
        viewport->screenCoordinates(*m_ring, outlinePolygons);
    }
}

QPointF GeoPolygonGraphicsItem::centroid(const QPolygonF &polygon, double &area)
{
    auto centroid = QPointF(0.0, 0.0);
    area = 0.0;
    for (auto i=0, n=polygon.size(); i<n; ++i) {
        auto const x0 = polygon[i].x();
        auto const y0 = polygon[i].y();
        auto const j = i == n-1 ? 0 : i+1;
        auto const x1 = polygon[j].x();
        auto const y1 = polygon[j].y();
        auto const a = x0*y1 - x1*y0;
        area += a;
        centroid.rx() += (x0 + x1)*a;
        centroid.ry() += (y0 + y1)*a;
    }

    area *= 0.5;
    return centroid / (6.0*area);
}

QPointF GeoPolygonGraphicsItem::buildingOffset(const QPointF &point, const ViewportParams *viewport, bool* isCameraAboveBuilding) const
{
    qreal const cameraFactor = 0.5 * tan(0.5 * 110 * DEG2RAD);
    Q_ASSERT(m_buildingHeight > 0.0);
    qreal const buildingFactor = m_buildingHeight / EARTH_RADIUS;

    qreal const cameraHeightPixel = viewport->width() * cameraFactor;
    qreal buildingHeightPixel = viewport->radius() * buildingFactor;
    qreal const cameraDistance = cameraHeightPixel-buildingHeightPixel;

    if (isCameraAboveBuilding) {
        *isCameraAboveBuilding = cameraDistance > 0;
    }

    qreal const cc = cameraDistance * cameraHeightPixel;
    qreal const cb = cameraDistance * buildingHeightPixel;

    // The following lines calculate the same result, but are potentially slower due
    // to using more trigonometric method calls
    // qreal const alpha1 = atan2(offsetX, cameraHeightPixel);
    // qreal const alpha2 = atan2(offsetX, cameraHeightPixel-buildingHeightPixel);
    // qreal const shiftX = 2 * (cameraHeightPixel-buildingHeightPixel) * sin(0.5*(alpha2-alpha1));

    qreal const offsetX = point.x() - viewport->width() / 2.0;
    qreal const offsetY = point.y() - viewport->height() / 2.0;

    qreal const shiftX = offsetX * cb / (cc + offsetX);
    qreal const shiftY = offsetY * cb / (cc + offsetY);

    return QPointF(shiftX, shiftY);
}

double GeoPolygonGraphicsItem::extractBuildingHeight(const GeoDataFeature *feature)
{
    if (!isBuilding(feature->visualCategory())) {
        return 0;
    }

    double height = 8.0;

    if (feature->nodeType() == GeoDataTypes::GeoDataPlacemarkType) {
        const GeoDataPlacemark *placemark = static_cast<const GeoDataPlacemark *>(feature);
        const OsmPlacemarkData &osmData = placemark->osmData();
        QHash<QString, QString>::const_iterator tagIter;
        if ((tagIter = osmData.findTag(QStringLiteral("height"))) != osmData.tagsEnd()) {
            /** @todo Also parse non-SI units, see https://wiki.openstreetmap.org/wiki/Key:height#Height_of_buildings */
            QString const heightValue = QString(tagIter.value()).remove(QStringLiteral(" meters")).remove(QStringLiteral(" m"));
            bool extracted = false;
            double extractedHeight = heightValue.toDouble(&extracted);
            if (extracted) {
                height = extractedHeight;
            }
        } else if ((tagIter = osmData.findTag(QStringLiteral("building:levels"))) != osmData.tagsEnd()) {
            int const levels = tagIter.value().toInt();
            int const skipLevels = osmData.tagValue(QStringLiteral("building:min_level")).toInt();
            /** @todo Is 35 as an upper bound for the number of levels sane? */
            height = 3.0 * qBound(1, 1+levels-skipLevels, 35);
        }
    }

    return qBound(1.0, height, 1000.0);
}

QString GeoPolygonGraphicsItem::extractBuildingLabel(const GeoDataFeature *feature)
{
    if (isBuilding(feature->visualCategory()) && feature->nodeType() == GeoDataTypes::GeoDataPlacemarkType) {
        const GeoDataPlacemark *placemark = static_cast<const GeoDataPlacemark *>(feature);

        if (!placemark->name().isEmpty()) {
            return placemark->name();
        }
        const OsmPlacemarkData &osmData = placemark->osmData();
        auto tagIter = osmData.findTag(QStringLiteral("addr:housename"));
        if (tagIter != osmData.tagsEnd()) {
            return tagIter.value();
        }
        tagIter = osmData.findTag(QStringLiteral("addr:housenumber"));
        if (tagIter != osmData.tagsEnd()) {
            return tagIter.value();
        }
    }

    return QString();
}

QVector<GeoPolygonGraphicsItem::NamedEntry> GeoPolygonGraphicsItem::extractNamedEntries(const GeoDataFeature *feature)
{
    QVector<NamedEntry> entries;

    if (isBuilding(feature->visualCategory()) && feature->nodeType() == GeoDataTypes::GeoDataPlacemarkType) {
        const GeoDataPlacemark *placemark = static_cast<const GeoDataPlacemark *>(feature);

        const auto end = placemark->osmData().nodeReferencesEnd();
        for (auto iter = placemark->osmData().nodeReferencesBegin(); iter != end; ++iter) {
            const auto tagIter = iter.value().findTag(QStringLiteral("addr:housenumber"));
            if (tagIter != iter.value().tagsEnd()) {
                NamedEntry entry;
                entry.point = iter.key();
                entry.label = tagIter.value();
                entries.push_back(entry);
            }
        }
    }

    return entries;
}

const GeoDataLatLonAltBox& GeoPolygonGraphicsItem::latLonAltBox() const
{
    if( m_polygon ) {
        return m_polygon->latLonAltBox();
    }

    return m_ring->latLonAltBox();
}

void GeoPolygonGraphicsItem::paint( GeoPainter* painter, const ViewportParams* viewport, const QString &layer ) {
    if (layer.endsWith(QLatin1String("/frame"))) {
        paintFrame(painter, viewport);
    } else if (layer.endsWith(QLatin1String("/roof"))) {
        paintRoof(painter, viewport);
    } else if (m_buildingHeight == 0.0) {
        painter->save();
        configurePainter(painter, viewport, false);
        if ( m_polygon ) {
            painter->drawPolygon( *m_polygon );
        } else if ( m_ring ) {
            painter->drawPolygon( *m_ring );
        }

        const GeoDataIconStyle& iconStyle = style()->iconStyle();
        bool const hasIcon = !iconStyle.iconPath().isEmpty();
        if (hasIcon) {
            QImage const icon = iconStyle.scaledIcon();
            painter->drawImage(latLonAltBox().center(), icon);
        }
        painter->restore();
    } else {
        mDebug() << "Didn't expect to have to paint layer " << layer << ", ignoring it.";
    }
}

void GeoPolygonGraphicsItem::paintRoof( GeoPainter* painter, const ViewportParams* viewport )
{
    bool drawAccurate3D;
    bool isCameraAboveBuilding;
    bool hasInnerBoundaries;
    QVector<QPolygonF*> outlinePolygons;
    QVector<QPolygonF*> innerPolygons;
    initializeBuildingPainting(painter, viewport, drawAccurate3D, isCameraAboveBuilding, hasInnerBoundaries, outlinePolygons, innerPolygons);
    if (!isCameraAboveBuilding) {
        return; // do not render roof if we look inside the building
    }

    painter->save();
    QPen const currentPen = configurePainter(painter, viewport, false);

    bool const hasIcon = !style()->iconStyle().iconPath().isEmpty();
    qreal maxSize(0.0);
    QPointF roofCenter;

    if (hasInnerBoundaries) {
        painter->setPen(Qt::NoPen);
    }

    // first paint the area and icon (and the outline if there are no inner boundaries)

    double maxArea = 0.0;
    foreach(QPolygonF* outlinePolygon, outlinePolygons) {
        QRectF const boundingRect = outlinePolygon->boundingRect();
        QPolygonF buildingRoof;
        if (hasIcon || !m_buildingLabel.isEmpty() || !m_entries.isEmpty()) {
            QSizeF const polygonSize = boundingRect.size();
            qreal size = polygonSize.width() * polygonSize.height();
            if (size > maxSize) {
                maxSize = size;
                double area;
                roofCenter = centroid(*outlinePolygon, area);
                maxArea = qMax(area, maxArea);
                roofCenter += buildingOffset(roofCenter, viewport);
            }
        }
        if ( drawAccurate3D) {
            buildingRoof.reserve(outlinePolygon->size());
            foreach(const QPointF &point, *outlinePolygon) {
                buildingRoof << point + buildingOffset(point, viewport);
            }
            if (hasInnerBoundaries) {
                QRegion clip(buildingRoof.toPolygon());

                foreach(QPolygonF* innerPolygon, innerPolygons) {
                    QPolygonF buildingInner;
                    buildingInner.reserve(innerPolygon->size());
                    foreach(const QPointF &point, *innerPolygon) {
                        buildingInner << point + buildingOffset(point, viewport);
                    }
                    clip-=QRegion(buildingInner.toPolygon());
                }
                painter->setClipRegion(clip);
            }
            painter->drawPolygon(buildingRoof);
        } else {
            QPointF const offset = buildingOffset(boundingRect.center(), viewport);
            painter->translate(offset);
            if (hasInnerBoundaries) {
                QRegion clip(outlinePolygon->toPolygon());

                foreach(QPolygonF* clipPolygon, innerPolygons) {
                    clip-=QRegion(clipPolygon->toPolygon());
                }
                painter->setClipRegion(clip);
            }
            painter->drawPolygon(*outlinePolygon);
            painter->translate(-offset);
        }

        if (hasIcon && !roofCenter.isNull()) {
            QImage const icon = style()->iconStyle().scaledIcon();
            QPointF const iconCenter(icon.size().width()/2.0, icon.size().height()/2.0);
            painter->drawImage(roofCenter-iconCenter, icon);
        } else if (drawAccurate3D && !m_buildingLabel.isEmpty() && !roofCenter.isNull()) {
            double const w2 = 0.5 * painter->fontMetrics().width(m_buildingLabel);
            double const ascent = painter->fontMetrics().ascent();
            double const descent = painter->fontMetrics().descent();
            double const a2 = 0.5 * painter->fontMetrics().ascent();
            QPointF const textPosition = roofCenter - QPointF(w2, -a2);
            if (buildingRoof.containsPoint(textPosition + QPointF(-2, -ascent), Qt::OddEvenFill)
                    && buildingRoof.containsPoint(textPosition + QPointF(-2, descent), Qt::OddEvenFill)
                    && buildingRoof.containsPoint(textPosition + QPointF(2+2*w2, descent), Qt::OddEvenFill)
                    && buildingRoof.containsPoint(textPosition + QPointF(2+2*w2, -ascent), Qt::OddEvenFill)
                    ) {
                painter->drawText(textPosition, m_buildingLabel);
            }
        }
    }

    // Render additional housenumbers at building entries
    if (!m_entries.isEmpty() && maxArea > 1600 * m_entries.size()) {
        QBrush brush = painter->brush();
        QColor const brushColor = brush.color();
        QColor lighterColor = brushColor.lighter(110);
        lighterColor.setAlphaF(0.9);
        brush.setColor(lighterColor);
        painter->setBrush(brush);
        foreach(const auto &entry, m_entries) {
            qreal x, y;
            viewport->screenCoordinates(entry.point, x, y);
            QPointF point(x, y);
            point += buildingOffset(point, viewport);
            auto const width = painter->fontMetrics().width(entry.label);
            auto const height = painter->fontMetrics().height();
            QRectF rectangle(point, QSizeF(qMax(1.2*width, 1.1*height), 1.2*height));
            rectangle.moveCenter(point);
            painter->drawRoundedRect(rectangle, 3, 3);
            painter->drawText(rectangle, Qt::AlignCenter, entry.label);
        }
        brush.setColor(brushColor);
        painter->setBrush(brush);
    }

    // then paint the outlines if there are inner boundaries

    if (hasInnerBoundaries) {
        painter->setPen(currentPen);
        foreach(QPolygonF * polygon, outlinePolygons) {
            QRectF const boundingRect = polygon->boundingRect();
            if ( drawAccurate3D) {
                QPolygonF buildingRoof;
                buildingRoof.reserve(polygon->size());
                foreach(const QPointF &point, *polygon) {
                    buildingRoof << point + buildingOffset(point, viewport);
                }
                painter->drawPolyline(buildingRoof);
            } else {
                QPointF const offset = buildingOffset(boundingRect.center(), viewport);
                painter->translate(offset);
                painter->drawPolyline(*polygon);
                painter->translate(-offset);
            }
        }
    }

    qDeleteAll(outlinePolygons);
    painter->restore();
}


void GeoPolygonGraphicsItem::paintFrame(GeoPainter *painter, const ViewportParams *viewport)
{
    if (m_buildingHeight == 0.0) {
        return;
    }

    if ((m_polygon && !viewport->resolves(m_polygon->outerBoundary().latLonAltBox(), 4))
            || (m_ring && !viewport->resolves(m_ring->latLonAltBox(), 4))) {
        return;
    }

    painter->save();

    bool drawAccurate3D;
    bool isCameraAboveBuilding;
    bool hasInnerBoundaries;
    QVector<QPolygonF*> outlinePolygons;
    QVector<QPolygonF*> innerPolygons;
    initializeBuildingPainting(painter, viewport, drawAccurate3D, isCameraAboveBuilding, hasInnerBoundaries, outlinePolygons, innerPolygons);

    configurePainter(painter, viewport, true);
    foreach(QPolygonF* outlinePolygon, outlinePolygons) {
        if (outlinePolygon->isEmpty()) {
            continue;
        }
        if ( drawAccurate3D && isCameraAboveBuilding ) {
            // draw the building sides
            int const size = outlinePolygon->size();
            QPointF & a = (*outlinePolygon)[0];
            QPointF shiftA = a + buildingOffset(a, viewport);
            for (int i=1; i<size; ++i) {
                QPointF const & b = (*outlinePolygon)[i];
                QPointF const shiftB = b + buildingOffset(b, viewport);
                QPolygonF buildingSide = QPolygonF() << a << shiftA << shiftB << b;
                if (hasInnerBoundaries) {
                    //smoothen away our loss of antialiasing due to the QRegion Qt-bug workaround
                    painter->setPen(QPen(painter->brush().color(), 1.5));
                }
                painter->drawPolygon(buildingSide);
                a = b;
                shiftA = shiftB;
            }
        } else {
            // don't draw the building sides - just draw the base frame instead
            if (hasInnerBoundaries) {
                QRegion clip(outlinePolygon->toPolygon());

                foreach(QPolygonF* clipPolygon, innerPolygons) {
                    clip-=QRegion(clipPolygon->toPolygon());
                }
                painter->setClipRegion(clip);
            }
            painter->drawPolygon(*outlinePolygon);
        }
    }
    qDeleteAll(outlinePolygons);

    painter->restore();
}

void GeoPolygonGraphicsItem::screenPolygons(const ViewportParams *viewport, const GeoDataPolygon *polygon, QVector<QPolygonF *> &innerPolygons, QVector<QPolygonF *> &outlines)
{
    Q_ASSERT(polygon);

    QVector<QPolygonF*> outerPolygons;
    viewport->screenCoordinates( polygon->outerBoundary(), outerPolygons );

    outlines << outerPolygons;

    QVector<GeoDataLinearRing> innerBoundaries = polygon->innerBoundaries();
    foreach (const GeoDataLinearRing &innerBoundary, innerBoundaries) {
        QVector<QPolygonF*> innerPolygonsPerBoundary;
        viewport->screenCoordinates(innerBoundary, innerPolygonsPerBoundary);

        outlines << innerPolygonsPerBoundary;
        innerPolygons.reserve(innerPolygons.size() + innerPolygonsPerBoundary.size());
        foreach( QPolygonF* innerPolygonPerBoundary, innerPolygonsPerBoundary ) {
            innerPolygons << innerPolygonPerBoundary;
        }
    }
}

QPen GeoPolygonGraphicsItem::configurePainter(GeoPainter *painter, const ViewportParams *viewport, bool isBuildingFrame)
{
    QPen currentPen = painter->pen();

    GeoDataStyle::ConstPtr style = this->style();
    if (!style) {
        painter->setPen( QPen() );
    }
    else {
        const GeoDataPolyStyle& polyStyle = style->polyStyle();

        if (!polyStyle.outline() || isBuildingFrame) {
            currentPen.setColor( Qt::transparent );
        }
        else {
            const GeoDataLineStyle& lineStyle = style->lineStyle();

            currentPen.setColor(lineStyle.paintedColor());
            currentPen.setWidthF(lineStyle.width());
            currentPen.setCapStyle(lineStyle.capStyle());
            currentPen.setStyle(lineStyle.penStyle());
        }

        painter->setPen(currentPen);

        if (!polyStyle.fill()) {
            painter->setBrush(QColor(Qt::transparent));
        }
        else {
            const QColor paintedColor = polyStyle.paintedColor();
            if ( isBuildingFrame ) {
                painter->setBrush(paintedColor.darker(150));
            }
            else if (painter->brush().color() != paintedColor ||
                     painter->brush().style() != polyStyle.brushStyle()) {
                const QImage textureImage = polyStyle.textureImage();
                if (!textureImage.isNull()) {
                    GeoDataCoordinates coords = latLonAltBox().center();
                    qreal x, y;
                    viewport->screenCoordinates(coords, x, y);
                    const QString texturePath = polyStyle.texturePath();
                    if (m_cachedTexturePath != texturePath ||
                        m_cachedTextureColor != paintedColor) {
                        if (textureImage.hasAlphaChannel()) {
                            m_cachedTexture = QImage ( textureImage.size(), QImage::Format_ARGB32_Premultiplied );
                            m_cachedTexture.fill(paintedColor);
                            QPainter imagePainter(&m_cachedTexture );
                            imagePainter.drawImage(0, 0, textureImage);
                        }
                        else {
                            m_cachedTexture = textureImage;
                        }
                        m_cachedTexturePath = texturePath;
                        m_cachedTextureColor = paintedColor;
                    }
                    QBrush brush(m_cachedTexture);
                    painter->setBrush(brush);
                    painter->setBrushOrigin(QPoint(x,y));
                }
                else {
                    painter->setBrush(QBrush(paintedColor, polyStyle.brushStyle()));
                }
            }
        }
    }

    return currentPen;
}

}
