// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2016 Akshat Tandon <akshat.tandon@research.iiit.ac.in>
//

#include "NodeReducer.h"
#include "GeoDataBuilding.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLineString.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPolygon.h"
#include "MarbleMath.h"

#include <QDebug>
#include <QVector>

namespace Marble
{

NodeReducer::NodeReducer(GeoDataDocument *document, const TileId &tileId)
    : m_removedNodes(0)
    , m_remainingNodes(0)
    , m_zoomLevel(tileId.zoomLevel())
{
    const GeoSceneMercatorTileProjection tileProjection;
    GeoDataLatLonBox tileBoundary = tileProjection.geoCoordinates(m_zoomLevel, tileId.x(), tileId.y());
    tileBoundary.scale(1.0 - 1e-4, 1.0 - 1e-4);
    tileBoundary.boundaries(m_tileBoundary[North], m_tileBoundary[South], m_tileBoundary[East], m_tileBoundary[West]);

    for (GeoDataPlacemark *placemark : document->placemarkList()) {
        GeoDataGeometry const *const geometry = placemark->geometry();
        auto const visualCategory = placemark->visualCategory();
        if (const auto prevLine = geodata_cast<GeoDataLineString>(geometry)) {
            GeoDataLineString *reducedLine = new GeoDataLineString;
            reduce(*prevLine, placemark->osmData(), visualCategory, reducedLine);
            placemark->setGeometry(reducedLine);
        } else if (const auto prevRing = geodata_cast<GeoDataLinearRing>(geometry)) {
            placemark->setGeometry(reducedRing(*prevRing, placemark, visualCategory));
        } else if (const auto prevPolygon = geodata_cast<GeoDataPolygon>(geometry)) {
            placemark->setGeometry(reducedPolygon(*prevPolygon, placemark, visualCategory));
        } else if (const auto building = geodata_cast<GeoDataBuilding>(geometry)) {
            if (const auto prevRing = geodata_cast<GeoDataLinearRing>(&building->multiGeometry()->at(0))) {
                GeoDataLinearRing *ring = reducedRing(*prevRing, placemark, visualCategory);
                GeoDataBuilding *newBuilding = new GeoDataBuilding(*building);
                newBuilding->multiGeometry()->clear();
                newBuilding->multiGeometry()->append(ring);
                placemark->setGeometry(newBuilding);
            } else if (const auto prevPolygon = geodata_cast<GeoDataPolygon>(&building->multiGeometry()->at(0))) {
                GeoDataPolygon *poly = reducedPolygon(*prevPolygon, placemark, visualCategory);
                GeoDataBuilding *newBuilding = new GeoDataBuilding(*building);
                newBuilding->multiGeometry()->clear();
                newBuilding->multiGeometry()->append(poly);
                placemark->setGeometry(newBuilding);
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
        int const factor = 1 << (qAbs(m_zoomLevel - 12));
        return multiplier / factor;
    } else {
        int const factor = 1 << (qAbs(m_zoomLevel - 10));
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
    if (t < 0.0) {
        ret = EARTH_RADIUS * a.sphericalDistanceTo(b);
    } else if (t > 1.0) {
        ret = EARTH_RADIUS * a.sphericalDistanceTo(c);
    } else {
        qreal const nom = qAbs(x21 * y10 - x10 * y21);
        qreal const den = sqrt(x21 * x21 + y21 * y21);
        ret = EARTH_RADIUS * nom / den;
    }

    return ret;
}

bool NodeReducer::touchesTileBorder(const GeoDataCoordinates &coordinates) const
{
    return coordinates.latitude() >= m_tileBoundary[North] || coordinates.latitude() <= m_tileBoundary[South] || coordinates.longitude() <= m_tileBoundary[West]
        || coordinates.longitude() >= m_tileBoundary[East];
}

qint64 NodeReducer::removedNodes() const
{
    return m_removedNodes;
}

GeoDataLinearRing *
NodeReducer::reducedRing(const GeoDataLinearRing &prevRing, GeoDataPlacemark *placemark, const GeoDataPlacemark::GeoDataVisualCategory &visualCategory)
{
    GeoDataLinearRing *reducedRing = new GeoDataLinearRing;
    reduce(prevRing, placemark->osmData(), visualCategory, reducedRing);
    return reducedRing;
}

GeoDataPolygon *
NodeReducer::reducedPolygon(const GeoDataPolygon &prevPolygon, GeoDataPlacemark *placemark, const GeoDataPlacemark::GeoDataVisualCategory &visualCategory)
{
    GeoDataPolygon *reducedPolygon = new GeoDataPolygon;
    GeoDataLinearRing const *prevRing = &(prevPolygon.outerBoundary());
    GeoDataLinearRing reducedRing;
    reduce(*prevRing, placemark->osmData().memberReference(-1), visualCategory, &reducedRing);
    reducedPolygon->setOuterBoundary(reducedRing);
    QVector<GeoDataLinearRing> const &innerBoundaries = prevPolygon.innerBoundaries();
    for (int i = 0; i < innerBoundaries.size(); i++) {
        prevRing = &innerBoundaries[i];
        GeoDataLinearRing reducedInnerRing;
        reduce(*prevRing, placemark->osmData().memberReference(i), visualCategory, &reducedInnerRing);
        reducedPolygon->appendInnerBoundary(reducedInnerRing);
    }
    return reducedPolygon;
}

}
