// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Akshat Tandon <akshat.tandon@research.iiit.ac.in>
//

#ifndef MARBLE_NODEREDUCER_H
#define MARBLE_NODEREDUCER_H

#include "OsmPlacemarkData.h"
#include "VectorClipper.h"

namespace Marble {

class NodeReducer {
public:
    NodeReducer(GeoDataDocument* document, const TileId &tileId);

    qint64 removedNodes() const;
    qint64 remainingNodes() const;

private:
    qreal epsilonFor(qreal multiplier) const;
    qreal perpendicularDistance(const GeoDataCoordinates &a, const GeoDataCoordinates &b, const GeoDataCoordinates &c) const;
    bool touchesTileBorder(const GeoDataCoordinates &coordinates) const;

    GeoDataLinearRing* reducedRing(const GeoDataLinearRing& prevRing,
                                   GeoDataPlacemark* placemark,
                                   const GeoDataPlacemark::GeoDataVisualCategory& visualCategory);
    GeoDataPolygon* reducedPolygon(const GeoDataPolygon& prevPolygon,
                                   GeoDataPlacemark* placemark,
                                   const GeoDataPlacemark::GeoDataVisualCategory& visualCategory);

    template<class T>
    void reduce(T const & lineString, OsmPlacemarkData& osmData, GeoDataPlacemark::GeoDataVisualCategory visualCategory, T* reducedLine)
    {
        bool const isArea = lineString.isClosed() && VectorClipper::canBeArea(visualCategory);
        qreal const epsilon = epsilonFor(isArea ? 45.0 : 30.0);
        *reducedLine = douglasPeucker(lineString, osmData, epsilon);

        qint64 prevSize = lineString.size();
        qint64 reducedSize = reducedLine->size();
        m_removedNodes += (prevSize - reducedSize);
        m_remainingNodes += reducedSize;
    }

    template<class T>
    T extract(T const & lineString, int start, int end) const
    {
        T result;
        for (int i=start; i<=end; ++i) {
            result << lineString[i];
        }
        return result;
    }

    template<class T>
    T merge(T const & a, T const &b) const
    {
        T result = a;
        int const index = a.size();
        result << b;
        result.remove(index);
        return result;
    }

    template<class T>
    T douglasPeucker(T const & lineString, const OsmPlacemarkData &osmData, qreal epsilon) const
    {
        if (lineString.size() < 3) {
            return lineString;
        }

        double maxDistance = 0.0;
        int index = 1;
        int const end = lineString.size()-1;
        for (int i = 1; i<end; ++i) {
            double const distance = perpendicularDistance(lineString[i], lineString[0], lineString[end]);
            if (distance > maxDistance) {
                index = i;
                maxDistance = distance;
            }
        }

        if (maxDistance >= epsilon) {
            T const left = douglasPeucker(extract(lineString, 0, index), osmData, epsilon);
            T const right = douglasPeucker(extract(lineString, index, end), osmData, epsilon);
            return merge(left, right);
        }

        T result;
        result << lineString[0];
        for (int i=1; i<end; ++i) {
            // even if under the Douglas Peucker threshold, we keep the following nodes:
            // - nodes at a tile border
            // - nodes containing any OSM tag themselves
            // - nodes adjacent to synthetic nodes introduced by clipping. This is to avoid
            //   synthetic only line segments that geometry reassembly on the client might
            //   remove entirely and thus distort the original geometry.
            bool const keepNode = touchesTileBorder(lineString[i]) || !osmData.nodeReference(lineString[i]).isEmpty()
                || osmData.nodeReference(lineString[i-1]).id() < 0 || ((i + 1) < end && osmData.nodeReference(lineString[i+1]).id() < 0);
            if (keepNode) {
                result << lineString[i];
            }
        }
        result << lineString[end];
        return result;
    }

    qint64 m_removedNodes;
    qint64 m_remainingNodes;

    int m_zoomLevel;
    double m_tileBoundary[4];
    enum Boundary {
        West = 0,
        North = 1,
        East = 2,
        South = 3
    };
};

}

#endif
