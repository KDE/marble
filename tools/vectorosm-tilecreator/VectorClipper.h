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

#include "clipper/clipper.hpp"

namespace Marble {

class GeoDataLinearRing;

class VectorClipper : public BaseFilter
{
public:
    VectorClipper(GeoDataDocument* document, int maxZoomLevel);

    GeoDataDocument* clipTo(const GeoDataLatLonBox &box, bool filterSmallAreas);
    GeoDataDocument* clipTo(unsigned int zoomLevel, unsigned int tileX, unsigned int tileY);

private:
    GeoDataDocument* clipToBaseClipper(const GeoDataLatLonBox &box);
    QVector<GeoDataPlacemark*> potentialIntersections(const GeoDataLatLonBox &box) const;
    ClipperLib::Path clipPath(const GeoDataLatLonBox &box) const;
    bool canBeArea(GeoDataPlacemark::GeoDataVisualCategory visualCategory) const;
    qreal area(const GeoDataLinearRing &ring);

    template<class T>
    void clipString(const GeoDataPlacemark *placemark, const ClipperLib::Path &tileBoundary, qreal minArea, GeoDataDocument* document)
    {
        const T* ring = static_cast<const T*>(placemark->geometry());
        bool const isClosed = ring->isClosed() && canBeArea(placemark->visualCategory());
        if (isClosed && minArea > 0.0 && area(*static_cast<const GeoDataLinearRing*>(ring)) < minArea) {
            return;
        }
        using namespace ClipperLib;
        Path path;
        foreach(auto const & node, *ring) {
            path << IntPoint(qRound64(node.longitude() * m_scale), qRound64(node.latitude() * m_scale));
        }

        Clipper clipper;
        clipper.PreserveCollinear(true);
        clipper.AddPath(tileBoundary, ptClip, true);
        clipper.AddPath(path, ptSubject, isClosed);
        PolyTree tree;
        clipper.Execute(ctIntersection, tree);
        Paths paths;
        if (isClosed) {
            ClosedPathsFromPolyTree(tree, paths);
        } else {
            OpenPathsFromPolyTree(tree, paths);
        }
        foreach(const auto &path, paths) {
            T* ring = new T;
            foreach(const auto &point, path) {
                *ring << GeoDataCoordinates(double(point.X) / m_scale, double(point.Y) / m_scale);
            }

            GeoDataPlacemark* newPlacemark = new GeoDataPlacemark();
            newPlacemark->setGeometry(ring);
            copyTags(*placemark, *newPlacemark);
            document->append(newPlacemark);
        }
    }

    void clipPolygon(const GeoDataPlacemark *placemark, const ClipperLib::Path &tileBoundary, qreal minArea, GeoDataDocument* document);

    void copyTags(const GeoDataPlacemark &source, GeoDataPlacemark &target) const;
    void copyTags(const OsmPlacemarkData &originalPlacemarkData, OsmPlacemarkData& targetOsmData) const;

    static qint64 const m_scale = 10000000000;

    QMap<TileId, QVector<GeoDataPlacemark*> > m_items;
    int m_maxZoomLevel;
    GeoSceneMercatorTileProjection m_tileProjection;
    QHash<const GeoDataLinearRing*, qreal> m_areas;
};

}

#endif // TINYPLANETPROCESSOR_H
