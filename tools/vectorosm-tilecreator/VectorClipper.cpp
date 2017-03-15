//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#include "VectorClipper.h"

#include "TileId.h"

#include "GeoDataLatLonAltBox.h"
#include "GeoDataPolygon.h"
#include "GeoDataPlacemark.h"
#include "GeoDataRelation.h"
#include "OsmPlacemarkData.h"
#include "OsmObjectManager.h"
#include "TileCoordsPyramid.h"

#include "clipper/clipper.hpp"

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
    auto const clip = clipPath(tileBoundary, zoomLevel);
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

ClipperLib::Path VectorClipper::clipPath(const GeoDataLatLonBox &box, int zoomLevel) const
{
    using namespace ClipperLib;
    Path path;
    int const steps = qMax(1, 22 - 2 * zoomLevel);
    qreal const scale = IntPoint::scale;
    double x = box.west() * scale;
    double const horizontalStep = (box.east() * scale - x) / steps;
    double y = box.north() * scale;
    double const verticalStep = (box.south() * scale - y) / steps;
    for (int i=0; i<steps; ++i) {
        path << IntPoint(qRound64(x), qRound64(y));
        x += horizontalStep;
    }
    path << IntPoint(qRound64(box.east() * scale), qRound64(box.north() * scale));
    for (int i=0; i<steps; ++i) {
        path << IntPoint(qRound64(x), qRound64(y));
        y += verticalStep;
    }
    path << IntPoint(qRound64(box.east() * scale), qRound64(box.south() * scale));
    for (int i=0; i<steps; ++i) {
        path << IntPoint(qRound64(x), qRound64(y));
        x -= horizontalStep;
    }
    path << IntPoint(qRound64(box.west() * scale), qRound64(box.south() * scale));
    for (int i=0; i<steps; ++i) {
        path << IntPoint(qRound64(x), qRound64(y));
        y -= verticalStep;
    }
    path << IntPoint(qRound64(box.west() * scale), qRound64(box.north() * scale));
    return path;
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
    auto const iter = m_areas.find(&ring);
    if (iter != m_areas.end()) {
        return *iter;
    }
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
    m_areas.insert(&ring, result);
    return result;
}

void VectorClipper::getBounds(const ClipperLib::Path &path, ClipperLib::cInt &minX, ClipperLib::cInt &maxX, ClipperLib::cInt &minY, ClipperLib::cInt &maxY) const
{
    Q_ASSERT(!path.empty());
    minX = path[0].X;
    maxX = minX;
    minY = path[0].Y;
    maxY = minY;
    for (auto const & point: path) {
        if (point.X < minX) {
            minX = point.X;
        } else if (point.X > maxX) {
            maxX = point.X;
        }
        if (point.Y < minY) {
            minY = point.Y;
        } else if (point.Y > maxY) {
            maxY = point.Y;
        }
    }
}

void VectorClipper::clipPolygon(const GeoDataPlacemark *placemark, const ClipperLib::Path &tileBoundary, qreal minArea,
                                GeoDataDocument *document, QSet<qint64> &osmIds)
{
    const GeoDataPolygon* polygon = static_cast<const GeoDataPolygon*>(placemark->geometry());
    if (minArea > 0.0 && area(polygon->outerBoundary()) < minArea) {
        return;
    }
    using namespace ClipperLib;
    Path path;
    for(auto const & node: polygon->outerBoundary()) {
        path << IntPoint(&node);
    }

    cInt minX, maxX, minY, maxY;
    getBounds(tileBoundary, minX, maxX, minY, maxY);
    Clipper clipper;
    clipper.PreserveCollinear(true);
    clipper.AddPath(tileBoundary, ptClip, true);
    clipper.AddPath(path, ptSubject, true);
    Paths paths;
    clipper.Execute(ctIntersection, paths);
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
        int nodeIndex = 0;
        for(const auto &point: path) {
            GeoDataCoordinates const coordinates = point.coordinates();
            outerRing << coordinates;
            auto const originalOsmData = outerRingOsmData.nodeReference(coordinates);
            if (originalOsmData.id() > 0) {
                newOuterRingOsmData.addNodeReference(coordinates, originalOsmData);
            }
            ++nodeIndex;
        }

        GeoDataPolygon* newPolygon = new GeoDataPolygon;
        newPolygon->setOuterBoundary(outerRing);
        newPlacemark->setGeometry(newPolygon);
        if (placemarkOsmData.id() > 0) {
            newPlacemarkOsmData.addTag(QStringLiteral("mx:oid"), QString::number(placemarkOsmData.id()));
        }
        copyTags(placemarkOsmData, newPlacemarkOsmData);
        copyTags(outerRingOsmData, newOuterRingOsmData);
        if (outerRingOsmData.id() > 0) {
            newOuterRingOsmData.addTag(QStringLiteral("mx:oid"), QString::number(outerRingOsmData.id()));
        }

        auto const & innerBoundaries = polygon->innerBoundaries();
        for (index = 0; index < innerBoundaries.size(); ++index) {
            auto const & innerBoundary = innerBoundaries.at(index);
            if (minArea > 0.0 && area(innerBoundary) < minArea) {
                continue;
            }

            auto const & innerRingOsmData = placemarkOsmData.memberReference(index);
            clipper.Clear();
            clipper.AddPath(path, ptClip, true);
            Path innerPath;
            for(auto const & node: innerBoundary) {
                innerPath << IntPoint(&node);
            }
            clipper.AddPath(innerPath, ptSubject, true);
            Paths innerPaths;
            clipper.Execute(ctIntersection, innerPaths);
            for(auto const &innerPath: innerPaths) {
                int const newIndex = newPolygon->innerBoundaries().size();
                auto & newInnerRingOsmData = newPlacemarkOsmData.memberReference(newIndex);
                GeoDataLinearRing innerRing;
                nodeIndex = 0;
                for(const auto &point: innerPath) {
                    GeoDataCoordinates const coordinates = point.coordinates();
                    innerRing << coordinates;
                    auto const originalOsmData = innerRingOsmData.nodeReference(coordinates);
                    if (originalOsmData.id() > 0) {
                        newInnerRingOsmData.addNodeReference(coordinates, originalOsmData);
                    }
                    ++nodeIndex;
                }
                newPolygon->appendInnerBoundary(innerRing);
                if (innerRingOsmData.id() > 0) {
                    newInnerRingOsmData.addTag(QStringLiteral("mx:oid"), QString::number(innerRingOsmData.id()));
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
