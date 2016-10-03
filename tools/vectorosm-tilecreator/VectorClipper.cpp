//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#include "VectorClipper.h"

#include "BaseClipper.h"
#include "TileId.h"

#include "GeoDataTypes.h"
#include "GeoDataLatLonAltBox.h"
#include "GeoDataPolygon.h"
#include "GeoDataPlacemark.h"
#include "OsmPlacemarkData.h"
#include "OsmObjectManager.h"
#include "TileCoordsPyramid.h"

#include "clipper/clipper.hpp"

#include <QDebug>
#include <QPolygonF>
#include <QPair>

namespace Marble {

VectorClipper::VectorClipper(GeoDataDocument* document, int maxZoomLevel) :
    BaseFilter(document),
    m_maxZoomLevel(maxZoomLevel)
{
    foreach(auto placemark, placemarks()) {
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
    }
}

GeoDataDocument *VectorClipper::clipTo(const GeoDataLatLonBox &tileBoundary)
{
    bool const useBaseClipper = false;
    if (useBaseClipper) {
        return clipToBaseClipper(tileBoundary);
    }

    GeoDataDocument* tile = new GeoDataDocument();
    auto const clip = clipPath(tileBoundary);
    foreach (GeoDataPlacemark const * placemark, potentialIntersections(tileBoundary)) {
        GeoDataGeometry const * const geometry = placemark ? placemark->geometry() : nullptr;
        if (geometry && tileBoundary.intersects(geometry->latLonAltBox())) {
            if(geometry->nodeType() == GeoDataTypes::GeoDataPolygonType) {
                clipPolygon(placemark, clip, tile);
            } else if (geometry->nodeType() == GeoDataTypes::GeoDataLineStringType) {
                clipString<GeoDataLineString>(placemark, clip, tile);
            } else if (geometry->nodeType() == GeoDataTypes::GeoDataLinearRingType) {
                clipString<GeoDataLinearRing>(placemark, clip, tile);
            } else {
                tile->append(new GeoDataPlacemark(*placemark));
            }
        }
    }

    return tile;
}

GeoDataDocument *VectorClipper::clipToBaseClipper(const GeoDataLatLonBox &tileBoundary)
{
    GeoDataDocument* tile = new GeoDataDocument();
    BaseClipper clipper;
    clipper.initClipRect(tileBoundary, 20);

    foreach (GeoDataPlacemark const * placemark, placemarks()) {

        if(placemark && placemark->geometry() && tileBoundary.intersects(placemark->geometry()->latLonAltBox())) {

            if( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType) {
                GeoDataPolygon const * marblePolygon = static_cast<GeoDataPolygon const *>(placemark->geometry());
                int index = -1;

                using PolygonPair = QPair<GeoDataPlacemark*, QPolygonF>;
                QVector<PolygonPair> newMarblePolygons;

                bool const isClockwise = marblePolygon->outerBoundary().isClockwise();
                QPolygonF outerBoundaryPolygon = BaseClipper::toQPolygon(marblePolygon->outerBoundary(), !isClockwise);

                QVector<QPolygonF> outerBoundaries;
                clipper.clipPolyObject(outerBoundaryPolygon, outerBoundaries, true);

                //                qDebug() << "Size(s) after:";
                foreach(const QPolygonF& polygon, outerBoundaries) {

                    //                    qDebug() << polygon.size();

                    PolygonPair newMarblePolygon = qMakePair(new GeoDataPlacemark(), polygon);
                    GeoDataPolygon* geometry = new GeoDataPolygon();
                    geometry->setOuterBoundary(BaseClipper::toLineString<GeoDataLinearRing>(polygon, !isClockwise));
                    newMarblePolygon.first->setGeometry(geometry);

                    copyTags(*placemark, *(newMarblePolygon.first));
                    OsmObjectManager::initializeOsmData(newMarblePolygon.first);

                    placemark->osmData().memberReference(index);
                    copyTags(placemark->osmData().memberReference(index),
                             newMarblePolygon.first->osmData().memberReference(index));

                    newMarblePolygons.push_back(newMarblePolygon);
                }

                foreach (const GeoDataLinearRing& innerBoundary, marblePolygon->innerBoundaries()) {
                    ++index;
                    bool const isClockwise = innerBoundary.isClockwise();
                    QPolygonF innerBoundaryPolygon = BaseClipper::toQPolygon(innerBoundary, !isClockwise);

                    QVector<QPolygonF> clippedPolygons;

                    clipper.clipPolyObject(innerBoundaryPolygon, clippedPolygons, true);

                    foreach (const QPolygonF& polygon, clippedPolygons) {
                        bool isAdded = false;
                        foreach (const PolygonPair& newMarblePolygon, newMarblePolygons) {
                            if(!polygon.intersected(newMarblePolygon.second).isEmpty()) {
                                GeoDataPolygon* geometry = static_cast<GeoDataPolygon*>(newMarblePolygon.first->geometry());
                                geometry->appendInnerBoundary(BaseClipper::toLineString<GeoDataLinearRing>(polygon, !isClockwise));

                                OsmObjectManager::initializeOsmData(newMarblePolygon.first);

                                OsmPlacemarkData& innerRingData = newMarblePolygon.first->osmData().memberReference(geometry->innerBoundaries().size()-1);
                                OsmPlacemarkData const & placemarkInnerRingData = placemark->osmData().memberReference(index);

                                copyTags(placemarkInnerRingData, innerRingData);

                                isAdded = true;
                            }
                        }

                        if(!isAdded) {
                            qDebug() << "Polygon not added. Why?";
                        }
                    }
                }

                foreach (const PolygonPair& newMarblePolygon, newMarblePolygons) {
                    tile->append(newMarblePolygon.first);
                }

            } else if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType) {
                GeoDataLineString const * marbleWay = static_cast<GeoDataLineString const *>(placemark->geometry());

                QVector<QPolygonF> clippedPolygons;

                QPolygonF way = BaseClipper::toQPolygon(*marbleWay);

                clipper.clipPolyObject(way, clippedPolygons, false);

                //                qDebug() << "Size  before:" << way.size();
                //                qDebug() << "Size(s) after:";
                foreach(const QPolygonF& polygon, clippedPolygons) {

                    //                    qDebug() << polygon.size();

                    GeoDataLineString* newMarbleWay = new GeoDataLineString(BaseClipper::toLineString<GeoDataLineString>(polygon));

                    GeoDataPlacemark* newPlacemark = new GeoDataPlacemark();
                    newPlacemark->setGeometry(newMarbleWay);
                    copyTags(*placemark, *newPlacemark);

                    tile->append(newPlacemark);
                }
            } else if (placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLinearRingType) {

                GeoDataLinearRing const * marbleClosedWay = static_cast<GeoDataLinearRing const *>(placemark->geometry());

                QVector<QPolygonF> clippedPolygons;

                QPolygonF closedWay = BaseClipper::toQPolygon(*marbleClosedWay);

                clipper.clipPolyObject(closedWay, clippedPolygons, true);

                //                    qDebug() << "Size(s) after:";
                foreach(const QPolygonF& polygon, clippedPolygons) {

                    //                        qDebug() << polygon.size();

                    GeoDataLinearRing* newMarbleWay = new GeoDataLinearRing(BaseClipper::toLineString<GeoDataLinearRing>(polygon));

                    GeoDataPlacemark* newPlacemark = new GeoDataPlacemark();
                    newPlacemark->setGeometry(newMarbleWay);
                    copyTags(*placemark, *newPlacemark);

                    tile->append(newPlacemark);
                }

            } else {
                tile->append(new GeoDataPlacemark(*placemark));
            }
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
    unsigned int N = pow(2, zoomLevel);
    GeoDataLatLonBox tileBoundary;
    qreal north = TileId::tileY2lat(tileY, N);
    qreal south = TileId::tileY2lat(tileY+1, N);
    qreal west = TileId::tileX2lon(tileX, N);
    qreal east = TileId::tileX2lon(tileX+1, N);
    tileBoundary.setBoundaries(north, south, east, west);

    GeoDataDocument *tile = clipTo(tileBoundary);
    QString tileName = QString("%1/%2/%3").arg(zoomLevel).arg(tileX).arg(tileY);
    tile->setName(tileName);

    return tile;
}

ClipperLib::Path VectorClipper::clipPath(const GeoDataLatLonBox &box) const
{
    using namespace ClipperLib;
    Path path;
    int const steps = 20;
    double x = box.west() * m_scale;
    double const horizontalStep = (box.east() * m_scale - x) / (steps-1);
    double y = box.north() * m_scale;
    double const verticalStep = (box.south() * m_scale - y) / (steps-1);
    for (int i=0; i<steps; ++i) {
        path << IntPoint(qRound64(x), qRound64(y));
        x += horizontalStep;
    }
    for (int i=0; i<steps; ++i) {
        path << IntPoint(qRound64(x), qRound64(y));
        y += verticalStep;
    }
    for (int i=0; i<steps; ++i) {
        path << IntPoint(qRound64(x), qRound64(y));
        x -= horizontalStep;
    }
    for (int i=0; i<steps; ++i) {
        path << IntPoint(qRound64(x), qRound64(y));
        y -= verticalStep;
    }
    return path;
}

void VectorClipper::clipPolygon(const GeoDataPlacemark *placemark, const ClipperLib::Path &tileBoundary, GeoDataDocument *document)
{
    const GeoDataPolygon* polygon = static_cast<const GeoDataPolygon*>(placemark->geometry());
    using namespace ClipperLib;
    Path path;
    foreach(auto const & node, polygon->outerBoundary()) {
        path << IntPoint(qRound64(node.longitude() * m_scale), qRound64(node.latitude() * m_scale));
    }

    Clipper clipper;
    clipper.PreserveCollinear(true);
    clipper.AddPath(tileBoundary, ptClip, true);
    clipper.AddPath(path, ptSubject, true);
    Paths paths;
    clipper.Execute(ctIntersection, paths);
    foreach(const auto &path, paths) {
        GeoDataLinearRing outerRing;
        foreach(const auto &point, path) {
            outerRing << GeoDataCoordinates(double(point.X) / m_scale, double(point.Y) / m_scale);
        }

        GeoDataPlacemark* newPlacemark = new GeoDataPlacemark;
        GeoDataPolygon* newPolygon = new GeoDataPolygon;
        newPolygon->setOuterBoundary(outerRing);
        newPlacemark->setGeometry(newPolygon);
        int index = -1;
        OsmObjectManager::initializeOsmData(newPlacemark);
        copyTags(*placemark, *newPlacemark);
        copyTags(placemark->osmData().memberReference(index), newPlacemark->osmData().memberReference(index));

        auto const & innerBoundaries = polygon->innerBoundaries();
        for (index = 0; index < innerBoundaries.size(); ++index) {
            clipper.Clear();
            clipper.AddPath(path, ptClip, true);
            Path innerPath;
            foreach(auto const & node, innerBoundaries.at(index)) {
                innerPath << IntPoint(qRound64(node.longitude() * m_scale), qRound64(node.latitude() * m_scale));
            }
            clipper.AddPath(innerPath, ptSubject, true);
            Paths innerPaths;
            clipper.Execute(ctIntersection, innerPaths);
            foreach(auto const &innerPath, innerPaths) {
                GeoDataLinearRing innerRing;
                foreach(const auto &point, innerPath) {
                    innerRing << GeoDataCoordinates(double(point.X) / m_scale, double(point.Y) / m_scale);
                }
                newPolygon->appendInnerBoundary(innerRing);
                OsmObjectManager::initializeOsmData(newPlacemark);
                copyTags(placemark->osmData().memberReference(index), newPlacemark->osmData().memberReference(newPolygon->innerBoundaries().size()-1));
            }
        }

        document->append(newPlacemark);
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
