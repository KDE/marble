//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      David Kolozsvari <freedawson@gmail.com>
//

#ifndef TINYPLANETPROCESSOR_H
#define TINYPLANETPROCESSOR_H

#include "BaseFilter.h"
#include "OsmPlacemarkData.h"

#include <GeoDataLatLonBox.h>
#include "GeoDataPlacemark.h"
#include "GeoDataLinearRing.h"
#include <TileId.h>
#include <GeoSceneMercatorTileProjection.h>
#include <OsmObjectManager.h>

#include "clipper/clipper.hpp"

namespace Marble {

class GeoDataLinearRing;

class VectorClipper : public BaseFilter
{
public:
    VectorClipper(GeoDataDocument* document, int maxZoomLevel);

    GeoDataDocument* clipTo(unsigned int zoomLevel, unsigned int tileX, unsigned int tileY);
    static bool canBeArea(GeoDataPlacemark::GeoDataVisualCategory visualCategory);

private:
    GeoDataDocument* clipTo(const GeoDataLatLonBox &box, int zoomLevel);
    GeoDataDocument* clipToBaseClipper(const GeoDataLatLonBox &box);
    QVector<GeoDataPlacemark*> potentialIntersections(const GeoDataLatLonBox &box) const;
    ClipperLib::Path clipPath(const GeoDataLatLonBox &box, int zoomLevel) const;
    qreal area(const GeoDataLinearRing &ring);
    void setBorderPoints(OsmPlacemarkData &osmData, const QVector<int> &borderPoints, int length) const;
    void getBounds(const ClipperLib::Path &path, ClipperLib::cInt &minX, ClipperLib::cInt &maxX, ClipperLib::cInt &minY, ClipperLib::cInt &maxY) const;

    template<class T>
    void clipString(const GeoDataPlacemark *placemark, const ClipperLib::Path &tileBoundary, qreal minArea, GeoDataDocument* document)
    {
        const T* ring = static_cast<const T*>(placemark->geometry());
        bool const isClosed = ring->isClosed() && canBeArea(placemark->visualCategory());
        if (isClosed && minArea > 0.0 && area(*static_cast<const GeoDataLinearRing*>(ring)) < minArea) {
            return;
        }
        auto const & osmData = placemark->osmData();
        using namespace ClipperLib;
        Path subject;
        foreach(auto const & node, *ring) {
            subject << IntPoint(&node);
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
        foreach(const auto &path, paths) {
            GeoDataPlacemark* newPlacemark = new GeoDataPlacemark;
            newPlacemark->setVisible(placemark->isVisible());
            newPlacemark->setVisualCategory(placemark->visualCategory());
            T* newRing = new T;
            QVector<int> borderPoints;
            int index = 0;
            foreach(const auto &point, path) {
                GeoDataCoordinates const coordinates = point.coordinates();
                *newRing << coordinates;
                auto const originalOsmData = osmData.nodeReference(coordinates);
                if (originalOsmData.id() > 0) {
                    newPlacemark->osmData().addNodeReference(coordinates, originalOsmData);
                }
                if (isClosed && !point.isInside(minX, maxX, minY, maxY)) {
                    borderPoints << index;
                }
                ++index;
            }

            newPlacemark->setGeometry(newRing);
            if (placemark->osmData().id() > 0) {
                newPlacemark->osmData().addTag(QStringLiteral("mx:oid"), QString::number(placemark->osmData().id()));
            }
            copyTags(*placemark, *newPlacemark);
            setBorderPoints(newPlacemark->osmData(), borderPoints, newRing->size());
            OsmObjectManager::initializeOsmData(newPlacemark);
            document->append(newPlacemark);
        }
    }

    void clipPolygon(const GeoDataPlacemark *placemark, const ClipperLib::Path &tileBoundary, qreal minArea, GeoDataDocument* document);

    void copyTags(const GeoDataPlacemark &source, GeoDataPlacemark &target) const;
    void copyTags(const OsmPlacemarkData &originalPlacemarkData, OsmPlacemarkData& targetOsmData) const;

    QMap<TileId, QVector<GeoDataPlacemark*> > m_items;
    int m_maxZoomLevel;
    GeoSceneMercatorTileProjection m_tileProjection;
    QHash<const GeoDataLinearRing*, qreal> m_areas;
};

}

#endif // TINYPLANETPROCESSOR_H
