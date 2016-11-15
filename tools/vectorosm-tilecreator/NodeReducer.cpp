//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Akshat Tandon <akshat.tandon@research.iiit.ac.in>
//

#include "BaseClipper.h"
#include "GeoDataPlacemark.h"
#include "GeoDataTypes.h"
#include "GeoDataLineString.h"
#include "GeoDataPolygon.h"
#include "GeoDataCoordinates.h"
#include "MarbleMath.h"
#include "NodeReducer.h"
#include "OsmPlacemarkData.h"

#include <QDebug>
#include <QVector>

namespace Marble {

NodeReducer::NodeReducer(GeoDataDocument* document, int zoomLevel) :
    BaseFilter(document),
    m_removedNodes(0),
    m_remainingNodes(0)
{
    foreach (GeoDataPlacemark* placemark, placemarks()) {
        GeoDataGeometry const * const geometry = placemark->geometry();
        if(geometry->nodeType() == GeoDataTypes::GeoDataLineStringType) {
            GeoDataLineString const * prevLine = static_cast<GeoDataLineString const *>(geometry);
            GeoDataLineString* reducedLine = new GeoDataLineString;
            reduce(*prevLine, placemark, reducedLine, zoomLevel);
            placemark->setGeometry(reducedLine);
        } else if (zoomLevel < 17) {
            if(geometry->nodeType() == GeoDataTypes::GeoDataLinearRingType) {
                GeoDataLinearRing const * prevRing = static_cast<GeoDataLinearRing const *>(geometry);
                GeoDataLinearRing* reducedRing = new GeoDataLinearRing;
                reduce(*prevRing, placemark, reducedRing, zoomLevel);
                placemark->setGeometry(reducedRing);
            } else if(geometry->nodeType() == GeoDataTypes::GeoDataPolygonType) {
                GeoDataPolygon* reducedPolygon = new GeoDataPolygon;
                GeoDataPolygon const * prevPolygon = static_cast<GeoDataPolygon const *>(geometry);
                GeoDataLinearRing const * prevRing = &(prevPolygon->outerBoundary());
                GeoDataLinearRing reducedRing;
                reduce(*prevRing, placemark, &reducedRing, zoomLevel);
                reducedPolygon->setOuterBoundary(reducedRing);
                QVector<GeoDataLinearRing> const & innerBoundaries = prevPolygon->innerBoundaries();
                for(int i = 0; i < innerBoundaries.size(); i++) {
                    prevRing = &innerBoundaries[i];
                    GeoDataLinearRing reducedInnerRing;
                    reduce(*prevRing, placemark, &reducedInnerRing, zoomLevel);
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

qreal NodeReducer::epsilonFor(int detailLevel, qreal multiplier) const
{
    if (detailLevel >= 17) {
        return 0.25;
    } else if (detailLevel >= 10) {
        int const factor = 1 << (qAbs(detailLevel-12));
        return multiplier / factor;
    } else {
        int const factor = 1 << (qAbs(detailLevel-10));
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
    qreal const t = (x01 * x21 + y01 * y21) / len;
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

qint64 NodeReducer::removedNodes() const
{
    return m_removedNodes;
}

}
