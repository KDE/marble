// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Dennis Nienhüser <nienhueser@kde.org>
// SPDX-FileCopyrightText: 2016 David Kolozsvari <freedawson@gmail.com>
//

#ifndef VECTORCLIPPER_H
#define VECTORCLIPPER_H

#include "OsmPlacemarkData.h"

#include <GeoDataLatLonBox.h>
#include "GeoDataPlacemark.h"
#include "GeoDataLinearRing.h"
#include "GeoDataBuilding.h"
#include "GeoDataMultiGeometry.h"
#include <TileId.h>
#include <GeoSceneMercatorTileProjection.h>
#include <OsmObjectManager.h>
#include "GeoDataDocument.h"

#include "clipper/clipper.hpp"
#include <QMap>
#include <QSet>

#include <memory>

namespace Marble {

class GeoDataLinearRing;
class GeoDataRelation;

class VectorClipper
{
public:
    VectorClipper(GeoDataDocument* document, int maxZoomLevel);

    GeoDataDocument* clipTo(unsigned int zoomLevel, unsigned int tileX, unsigned int tileY);
    static bool canBeArea(GeoDataPlacemark::GeoDataVisualCategory visualCategory);

private:
    GeoDataDocument* clipTo(const GeoDataLatLonBox &box, int zoomLevel);
    QVector<GeoDataPlacemark*> potentialIntersections(const GeoDataLatLonBox &box) const;
    ClipperLib::Path clipPath(const GeoDataLatLonBox &box, int zoomLevel) const;
    qreal area(const GeoDataLinearRing &ring);
    void getBounds(const ClipperLib::Path &path, ClipperLib::cInt &minX, ClipperLib::cInt &maxX, ClipperLib::cInt &minY, ClipperLib::cInt &maxY) const;

    // convert radian-based coordinates to 10^-7 degree (100 nanodegree) integer coordinates used by the clipper library
    constexpr static qint64 const s_pointScale = 10000000 / M_PI * 180;
    static inline ClipperLib::IntPoint coordinateToPoint(const GeoDataCoordinates &c)
    {
        return ClipperLib::IntPoint(qRound64(c.longitude() * s_pointScale), qRound64(c.latitude() * s_pointScale));
    }
    static inline GeoDataCoordinates pointToCoordinate(ClipperLib::IntPoint p)
    {
        return GeoDataCoordinates((double)p.X / s_pointScale, (double)p.Y / s_pointScale);
    }

    template<class T>
    static void pathToRing(const ClipperLib::Path &path, T *ring, const OsmPlacemarkData &originalOsmData, OsmPlacemarkData &newOsmData, const QHash<std::pair<ClipperLib::cInt, ClipperLib::cInt>, const GeoDataCoordinates*> &coordMap)
    {
        int index = 0;
        for(const auto &point: path) {
            const auto it = coordMap.find(std::make_pair(point.X, point.Y));
            if (it != coordMap.end()) {
                *ring << *it.value();
                auto const data = originalOsmData.nodeReference(*it.value());
                if (data.id() > 0) {
                    newOsmData.addNodeReference(*it.value(), data);
                }
            } else {
                *ring << pointToCoordinate(point);
            }
            ++index;
        }
    }

    template<class T>
    void clipString(const GeoDataPlacemark *placemark, const ClipperLib::Path &tileBoundary, qreal minArea,
                    GeoDataDocument* document, QSet<qint64> &osmIds)
    {
        if (osmIds.contains(placemark->osmData().id())) {
            return;
        }
        bool isBuilding = false;
        const T* ring;
        std::unique_ptr<GeoDataPlacemark> copyPlacemark;
        if (const auto building = geodata_cast<GeoDataBuilding>(placemark->geometry())) {
            ring = geodata_cast<T>(&static_cast<const GeoDataMultiGeometry*>(building->multiGeometry())->at(0));
            isBuilding = true;
        } else {
            copyPlacemark.reset(new GeoDataPlacemark(*placemark));
            ring = geodata_cast<T>(copyPlacemark->geometry());
        }
        auto const & osmData = placemark->osmData();
        bool const isClosed = ring->isClosed() && (canBeArea(placemark->visualCategory()) || osmData.tagValue(QStringLiteral("area")) == QLatin1String("yes"));
        if (isClosed && minArea > 0.0 && area(*static_cast<const GeoDataLinearRing*>(ring)) < minArea) {
            return;
        }
        using namespace ClipperLib;
        Path subject;
        QHash<std::pair<cInt, cInt>, const GeoDataCoordinates*> coordMap;
        for(auto const & node: *ring) {
            auto p = coordinateToPoint(node);
            coordMap.insert(std::make_pair(p.X, p.Y), &node);
            subject.push_back(std::move(p));
        }
        cInt minX, maxX, minY, maxY;
        getBounds(tileBoundary, minX, maxX, minY, maxY);

        Clipper clipper;
        clipper.PreserveCollinear(true);
        clipper.AddPath(tileBoundary, ptClip, true);
        clipper.AddPath(subject, ptSubject, isClosed);
        PolyTree tree;
        clipper.Execute(ctIntersection, tree);
        Paths paths;
        if (isClosed) {
            ClosedPathsFromPolyTree(tree, paths);
        } else {
            OpenPathsFromPolyTree(tree, paths);
        }
        for(const auto &path: paths) {
            GeoDataPlacemark* newPlacemark = new GeoDataPlacemark;
            newPlacemark->setVisible(placemark->isVisible());
            newPlacemark->setVisualCategory(placemark->visualCategory());
            T* newRing = new T;
            pathToRing(path, newRing, osmData, newPlacemark->osmData(), coordMap);

            if (isBuilding) {
                const auto building = geodata_cast<GeoDataBuilding>(placemark->geometry());
                GeoDataBuilding* newBuilding = new GeoDataBuilding(*building);
                newBuilding->multiGeometry()->clear();
                newBuilding->multiGeometry()->append(newRing);
                newPlacemark->setGeometry(newBuilding);
            } else {
                newPlacemark->setGeometry(newRing);
            }
            if (placemark->osmData().id() > 0) {
                newPlacemark->osmData().addTag(QStringLiteral("mx:oid"), QString::number(placemark->osmData().id()));
            }
            copyTags(*placemark, *newPlacemark);
            OsmObjectManager::initializeOsmData(newPlacemark);
            document->append(newPlacemark);
            osmIds << placemark->osmData().id();
        }
    }

    void clipPolygon(const GeoDataPlacemark *placemark, const ClipperLib::Path &tileBoundary, qreal minArea,
                     GeoDataDocument* document, QSet<qint64> &osmIds);

    void copyTags(const GeoDataPlacemark &source, GeoDataPlacemark &target) const;
    void copyTags(const OsmPlacemarkData &originalPlacemarkData, OsmPlacemarkData& targetOsmData) const;

    QMap<TileId, QVector<GeoDataPlacemark*> > m_items;
    int m_maxZoomLevel;
    GeoSceneMercatorTileProjection m_tileProjection;
    QSet<GeoDataRelation*> m_relations;
};

}

#endif // VECTORCLIPPER_H
