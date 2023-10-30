// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2016 David Kolozsvari <freedawson@gmail.com>
//

#include "VectorClipper.h"

#include "TileId.h"

#include "GeoDataLatLonAltBox.h"
#include "GeoDataPolygon.h"
#include "GeoDataRelation.h"
#include "OsmObjectManager.h"
#include "TileCoordsPyramid.h"


#include <QDebug>
#include <QPolygonF>
#include <QPair>
#include <QStringBuilder>

namespace Marble {

VectorClipper::VectorClipper(GeoDataDocument* document, int maxZoomLevel) :
    m_maxZoomLevel(maxZoomLevel)
{
    for (auto feature: document->featureList()) {
        if (const auto placemark = geodata_cast<GeoDataPlacemark>(feature)) {
            // Select zoom level such that the placemark fits in a single tile
            int zoomLevel;
            qreal north, south, east, west;
            placemark->geometry()->latLonAltBox().boundaries(north, south, east, west);
            for (zoomLevel = maxZoomLevel; zoomLevel >= 0; --zoomLevel) {
                if (TileId::fromCoordinates(GeoDataCoordinates(west, north), zoomLevel) ==
                        TileId::fromCoordinates(GeoDataCoordinates(east, south), zoomLevel)) {
                    break;
                }
            }
            TileId const key = TileId::fromCoordinates(GeoDataCoordinates(west, north), zoomLevel);
            m_items[key] << placemark;
        } else if (GeoDataRelation *relation = geodata_cast<GeoDataRelation>(feature)) {
            m_relations << relation;
        } else {
            Q_ASSERT(false && "only placemark variants are supported so far");
        }
    }
}

GeoDataDocument *VectorClipper::clipTo(const GeoDataLatLonBox &tileBoundary, int zoomLevel)
{
    bool const filterSmallAreas = zoomLevel > 10 && zoomLevel < 17;
    GeoDataDocument* tile = new GeoDataDocument();
    auto const clip = clipRect(tileBoundary);
    GeoDataLinearRing ring;
    ring << GeoDataCoordinates(tileBoundary.west(), tileBoundary.north());
    ring << GeoDataCoordinates(tileBoundary.east(), tileBoundary.north());
    ring << GeoDataCoordinates(tileBoundary.east(), tileBoundary.south());
    ring << GeoDataCoordinates(tileBoundary.west(), tileBoundary.south());
    qreal const minArea = filterSmallAreas ? 0.01 * area(ring) : 0.0;
    QSet<qint64> osmIds;
    for (GeoDataPlacemark const * placemark: potentialIntersections(tileBoundary)) {
        GeoDataGeometry const * const geometry = placemark ? placemark->geometry() : nullptr;
        if (geometry && tileBoundary.intersects(geometry->latLonAltBox())) {
            if (geodata_cast<GeoDataPolygon>(geometry)) {
                clipPolygon(placemark, clip, minArea, tile, osmIds);
            } else if (geodata_cast<GeoDataLineString>(geometry)) {
                clipString<GeoDataLineString>(placemark, clip, minArea, tile, osmIds);
            } else if (geodata_cast<GeoDataLinearRing>(geometry)) {
                clipString<GeoDataLinearRing>(placemark, clip, minArea, tile, osmIds);
            } else if (const auto building = geodata_cast<GeoDataBuilding>(geometry)) {
                if (geodata_cast<GeoDataPolygon>(&static_cast<const GeoDataMultiGeometry*>(building->multiGeometry())->at(0))) {
                    clipPolygon(placemark, clip, minArea, tile, osmIds);
                } else if (geodata_cast<GeoDataLinearRing>(&static_cast<const GeoDataMultiGeometry*>(building->multiGeometry())->at(0))) {
                    clipString<GeoDataLinearRing>(placemark, clip, minArea, tile, osmIds);
                }
            } else {
                tile->append(placemark->clone());
                osmIds << placemark->osmData().id();
            }
        }
    }

    for (auto relation: m_relations) {
        if (relation->containsAnyOf(osmIds)) {
            GeoDataRelation* multi = new GeoDataRelation;
            multi->osmData() = relation->osmData();
            tile->append(multi);
        }
    }
    return tile;
}

QVector<GeoDataPlacemark *> VectorClipper::potentialIntersections(const GeoDataLatLonBox &box) const
{
    qreal north, south, east, west;
    box.boundaries(north, south, east, west);
    TileId const topLeft = TileId::fromCoordinates(GeoDataCoordinates(west, north), m_maxZoomLevel);
    TileId const bottomRight = TileId::fromCoordinates(GeoDataCoordinates(east, south), m_maxZoomLevel);
    QRect rect;
    rect.setCoords(topLeft.x(), topLeft.y(), bottomRight.x(), bottomRight.y());

    TileCoordsPyramid pyramid(0, m_maxZoomLevel);
    pyramid.setBottomLevelCoords(rect);
    QVector<GeoDataPlacemark *> result;
    for (int level = pyramid.topLevel(), maxLevel = pyramid.bottomLevel(); level <= maxLevel; ++level) {
        int x1, y1, x2, y2;
        pyramid.coords(level).getCoords(&x1, &y1, &x2, &y2);
        for (int x = x1; x <= x2; ++x) {
            for (int y = y1; y <= y2; ++y) {
                result << m_items.value(TileId(0, level, x, y));
            }
        }
    }
    return result;
}

GeoDataDocument *VectorClipper::clipTo(unsigned int zoomLevel, unsigned int tileX, unsigned int tileY)
{
    const GeoDataLatLonBox tileBoundary = m_tileProjection.geoCoordinates(zoomLevel, tileX, tileY);

    GeoDataDocument *tile = clipTo(tileBoundary, zoomLevel);
    QString tileName = QString("%1/%2/%3").arg(zoomLevel).arg(tileX).arg(tileY);
    tile->setName(tileName);

    return tile;
}

Clipper2Lib::Rect64 VectorClipper::clipRect(const GeoDataLatLonBox &box)
{
    return { qRound64(box.west() * s_pointScale), qRound64(box.south() * s_pointScale),
             qRound64(box.east() * s_pointScale), qRound64(box.north() * s_pointScale) };
}

bool VectorClipper::canBeArea(GeoDataPlacemark::GeoDataVisualCategory visualCategory)
{
    if (visualCategory >= GeoDataPlacemark::HighwaySteps && visualCategory <= GeoDataPlacemark::HighwayMotorway) {
        return false;
    }
    if (visualCategory >= GeoDataPlacemark::RailwayRail && visualCategory <= GeoDataPlacemark::RailwayFunicular) {
        return false;
    }
    if (visualCategory >= GeoDataPlacemark::AdminLevel1 && visualCategory <= GeoDataPlacemark::AdminLevel11) {
        return false;
    }

    if (visualCategory == GeoDataPlacemark::BoundaryMaritime || visualCategory == GeoDataPlacemark::InternationalDateLine) {
        return false;
    }

    return true;
}

qreal VectorClipper::area(const GeoDataLinearRing &ring)
{
    int const n = ring.size();
    qreal area = 0;
    if (n<3) {
        return area;
    }
    for (int i = 1; i < n; ++i ){
        area += (ring[i].longitude() - ring[i-1].longitude() ) * ( ring[i].latitude() + ring[i-1].latitude());
    }
    area += (ring[0].longitude() - ring[n-1].longitude() ) * (ring[0].latitude() + ring[n-1].latitude());
    qreal const result = EARTH_RADIUS * EARTH_RADIUS * qAbs(area * 0.5);
    return result;
}

void VectorClipper::clipPolygon(const GeoDataPlacemark *placemark, const Clipper2Lib::Rect64 &tileBoundary, qreal minArea,
                                GeoDataDocument *document, QSet<qint64> &osmIds)
{
    bool isBuilding = false;
    GeoDataPolygon* polygon;
    std::unique_ptr<GeoDataPlacemark> copyPlacemark;
    if (const auto building = geodata_cast<GeoDataBuilding>(placemark->geometry())) {
        polygon = geodata_cast<GeoDataPolygon>(&static_cast<GeoDataMultiGeometry*>(building->multiGeometry())->at(0));
        isBuilding = true;
    } else {
        copyPlacemark.reset(new GeoDataPlacemark(*placemark));
        polygon = geodata_cast<GeoDataPolygon>(copyPlacemark->geometry());
    }

    if (minArea > 0.0 && area(polygon->outerBoundary()) < minArea) {
        return;
    }
    using namespace Clipper2Lib;
    Path64 path;
    path.reserve(qAsConst(polygon)->outerBoundary().size());
    for(auto const & node: qAsConst(polygon)->outerBoundary()) {
        path.push_back(coordinateToPoint(node));
    }

    Paths64 paths = Clipper2Lib::RectClip(tileBoundary, path);
    for(const auto &path: paths) {
        GeoDataPlacemark* newPlacemark = new GeoDataPlacemark;
        newPlacemark->setVisible(placemark->isVisible());
        newPlacemark->setVisualCategory(placemark->visualCategory());
        GeoDataLinearRing outerRing;
        OsmPlacemarkData const & placemarkOsmData = placemark->osmData();
        OsmPlacemarkData & newPlacemarkOsmData = newPlacemark->osmData();
        int index = -1;
        OsmPlacemarkData const & outerRingOsmData = placemarkOsmData.memberReference(index);
        OsmPlacemarkData & newOuterRingOsmData = newPlacemarkOsmData.memberReference(index);
        pathToRing(path, &outerRing, outerRingOsmData, newOuterRingOsmData);

        GeoDataPolygon* newPolygon = new GeoDataPolygon;
        newPolygon->setOuterBoundary(outerRing);
        if (isBuilding) {
            const auto building = geodata_cast<GeoDataBuilding>(placemark->geometry());
            GeoDataBuilding* newBuilding = new GeoDataBuilding(*building);
            newBuilding->multiGeometry()->clear();
            newBuilding->multiGeometry()->append(newPolygon);
            newPlacemark->setGeometry(newBuilding);
        } else {
            newPlacemark->setGeometry(newPolygon);
        }
        if (placemarkOsmData.id() > 0) {
            newPlacemarkOsmData.addTag(QStringLiteral("mx:oid"), QString::number(placemarkOsmData.id()));
        }
        copyTags(placemarkOsmData, newPlacemarkOsmData);
        copyTags(outerRingOsmData, newOuterRingOsmData);
        if (outerRingOsmData.id() > 0) {
            newOuterRingOsmData.addTag(QStringLiteral("mx:oid"), QString::number(outerRingOsmData.id()));
            osmIds.insert(outerRingOsmData.id());
        }

        auto const & innerBoundaries = qAsConst(polygon)->innerBoundaries();
        for (index = 0; index < innerBoundaries.size(); ++index) {
            auto const & innerBoundary = innerBoundaries.at(index);
            if (minArea > 0.0 && area(innerBoundary) < minArea) {
                continue;
            }

            auto const & innerRingOsmData = placemarkOsmData.memberReference(index);
            Path64 innerPath;
            innerPath.reserve(innerBoundary.size());
            for(auto const & node: innerBoundary) {
                innerPath.push_back(coordinateToPoint(node));
            }
            Paths64 innerPaths = Clipper2Lib::RectClip(tileBoundary, innerPath);
            for(auto const &innerPath: innerPaths) {
                int const newIndex = newPolygon->innerBoundaries().size();
                auto & newInnerRingOsmData = newPlacemarkOsmData.memberReference(newIndex);
                GeoDataLinearRing innerRing;
                pathToRing(innerPath, &innerRing, innerRingOsmData, newInnerRingOsmData);
                newPolygon->appendInnerBoundary(innerRing);
                if (innerRingOsmData.id() > 0) {
                    newInnerRingOsmData.addTag(QStringLiteral("mx:oid"), QString::number(innerRingOsmData.id()));
                    osmIds.insert(innerRingOsmData.id());
                }
                copyTags(innerRingOsmData, newInnerRingOsmData);
            }
        }

        OsmObjectManager::initializeOsmData(newPlacemark);
        document->append(newPlacemark);
        osmIds << placemark->osmData().id();
    }
}

void VectorClipper::copyTags(const GeoDataPlacemark &source, GeoDataPlacemark &target) const
{
    copyTags(source.osmData(), target.osmData());
}

void VectorClipper::copyTags(const OsmPlacemarkData &originalPlacemarkData, OsmPlacemarkData &targetOsmData) const
{
    for (auto iter=originalPlacemarkData.tagsBegin(), end=originalPlacemarkData.tagsEnd(); iter != end; ++iter) {
        targetOsmData.addTag(iter.key(), iter.value());
    }
}

}
