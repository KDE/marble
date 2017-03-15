//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Akshat Tandon <akshat.tandon@research.iiit.ac.in>
//

#include "GeoDataPlacemark.h"
#include "GeoDataLineString.h"
#include "GeoDataPolygon.h"
#include "GeoDataCoordinates.h"
#include "MarbleMath.h"
#include "NodeReducer.h"
#include "OsmPlacemarkData.h"

#include <QDebug>
#include <QVector>

namespace Marble {

NodeReducer::NodeReducer(GeoDataDocument* document, const TileId &tileId) :
    m_removedNodes(0),
    m_remainingNodes(0),
    m_zoomLevel(tileId.zoomLevel())
{
    const GeoSceneMercatorTileProjection tileProjection;
    GeoDataLatLonBox tileBoundary = tileProjection.geoCoordinates(m_zoomLevel, tileId.x(), tileId.y());
    tileBoundary.scale(1.0-1e-4, 1.0-1e-4);
    tileBoundary.boundaries(m_tileBoundary[North], m_tileBoundary[South], m_tileBoundary[East], m_tileBoundary[West]);

    for (GeoDataPlacemark* placemark: document->placemarkList()) {
        GeoDataGeometry const * const geometry = placemark->geometry();
        auto const visualCategory = placemark->visualCategory();
        if (const auto prevLine = geodata_cast<GeoDataLineString>(geometry)) {
            GeoDataLineString* reducedLine = new GeoDataLineString;
            reduce(*prevLine, placemark->osmData(), visualCategory, reducedLine);
            placemark->setGeometry(reducedLine);
        } else if (m_zoomLevel < 17) {
            if (const auto prevRing = geodata_cast<GeoDataLinearRing>(geometry)) {
                GeoDataLinearRing* reducedRing = new GeoDataLinearRing;
                reduce(*prevRing, placemark->osmData(), visualCategory, reducedRing);
                placemark->setGeometry(reducedRing);
            } else if (const auto prevPolygon = geodata_cast<GeoDataPolygon>(geometry)) {
                GeoDataPolygon* reducedPolygon = new GeoDataPolygon;
                GeoDataLinearRing const * prevRing = &(prevPolygon->outerBoundary());
                GeoDataLinearRing reducedRing;
                reduce(*prevRing, placemark->osmData().memberReference(-1), visualCategory, &reducedRing);
                reducedPolygon->setOuterBoundary(reducedRing);
                QVector<GeoDataLinearRing> const & innerBoundaries = prevPolygon->innerBoundaries();
                for(int i = 0; i < innerBoundaries.size(); i++) {
                    prevRing = &innerBoundaries[i];
                    GeoDataLinearRing reducedInnerRing;
                    reduce(*prevRing, placemark->osmData().memberReference(i), visualCategory, &reducedInnerRing);
                    reducedPolygon->appendInnerBoundary(reducedInnerRing);
                }
                placemark->setGeometry(reducedPolygon);
            }
        }
    }
}

qint64 NodeReducer::remainingNodes() const
{
    return m_remainingNodes;
}

qreal NodeReducer::epsilonFor(qreal multiplier) const
{
    if (m_zoomLevel >= 17) {
        return 0.25;
    } else if (m_zoomLevel >= 10) {
        int const factor = 1 << (qAbs(m_zoomLevel-12));
        return multiplier / factor;
    } else {
        int const factor = 1 << (qAbs(m_zoomLevel-10));
        return multiplier * factor;
    }
}

qreal NodeReducer::perpendicularDistance(const GeoDataCoordinates &a, const GeoDataCoordinates &b, const GeoDataCoordinates &c) const
{
    qreal ret;
    qreal const y0 = a.latitude();
    qreal const x0 = a.longitude();
    qreal const y1 = b.latitude();
    qreal const x1 = b.longitude();
    qreal const y2 = c.latitude();
    qreal const x2 = c.longitude();
    qreal const y01 = x0 - x1;
    qreal const x01 = y0 - y1;
    qreal const y10 = x1 - x0;
    qreal const x10 = y1 - y0;
    qreal const y21 = x2 - x1;
    qreal const x21 = y2 - y1;
    qreal const len = (x1 - x2) * (x1 - x2) + (y1 - y2) * (y1 - y2);
    qreal const t = len == 0.0 ? -1.0 : (x01 * x21 + y01 * y21) / len;
    if ( t < 0.0 ) {
        ret = EARTH_RADIUS * distanceSphere(a, b);
    } else if ( t > 1.0 ) {
        ret = EARTH_RADIUS * distanceSphere(a, c);
    } else {
        qreal const nom = qAbs( x21 * y10 - x10 * y21 );
        qreal const den = sqrt( x21 * x21 + y21 * y21 );
        ret = EARTH_RADIUS * nom / den;
    }

    return ret;
}

bool NodeReducer::touchesTileBorder(const GeoDataCoordinates &coordinates) const
{
    return  coordinates.latitude() >= m_tileBoundary[North] ||
            coordinates.latitude() <= m_tileBoundary[South] ||
            coordinates.longitude() <= m_tileBoundary[West] ||
            coordinates.longitude() >= m_tileBoundary[East];
}

qint64 NodeReducer::removedNodes() const
{
    return m_removedNodes;
}

void NodeReducer::setBorderPoints(OsmPlacemarkData &osmData, const QVector<int> &borderPoints, int length) const
{
    int const n = borderPoints.size();
    if (n == 0) {
        return;
    }

    if (n > length) {
        qDebug() << "Invalid border points for length" << length << ":" << borderPoints;
        return;
    }

    typedef QPair<int, int> Segment;
    typedef QVector<Segment> Segments;
    Segments segments;
    Segment currentSegment;
    currentSegment.first = borderPoints.first();
    currentSegment.second = currentSegment.first;
    for (int i=1; i<n; ++i) {
        if (currentSegment.second+1 == borderPoints[i]) {
            // compress and continue segment
            ++currentSegment.second;
        } else {
            segments << currentSegment;
            currentSegment.first = borderPoints[i];
            currentSegment.second = currentSegment.first;
        }
    }

    if (segments.isEmpty() || currentSegment != segments.last()) {
        segments << currentSegment;
    }

    if (segments.size() > 1 && segments.last().second+1 == length && segments.first().first == 0) {
        segments.last().second = segments.first().second;
        segments.pop_front();
    }

    int wraps = 0;
    for (auto const &segment: segments) {
        if (segment.first >= segment.second) {
            ++wraps;
        }
        if (segment.first < 0 || segment.second < 0 || segment.first+1 > length || segment.second+1 > length) {
            qDebug() << "Wrong border points sequence for length " << length << ":" <<  borderPoints << ", intermediate " << segments;
            return;
        }
    }

    if (wraps > 1) {
        //qDebug() << "Wrong border points sequence:" <<  borderPoints;
        return;
    }

    QString value;
    value.reserve(segments.size() * (2 + QString::number(length).size()));
    for (auto const &segment: segments) {
        int diff = segment.second - segment.first;
        diff = diff > 0 ? diff : length + diff;
        value = value % QStringLiteral(";") % QString::number(segment.first) % QStringLiteral("+") % QString::number(diff);
    }
    osmData.addTag(QStringLiteral("mx:bp"), value.mid(1));
}

}
