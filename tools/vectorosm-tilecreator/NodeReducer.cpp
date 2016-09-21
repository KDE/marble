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

#include <QDebug>
#include <QVector>

namespace Marble {

NodeReducer::NodeReducer(GeoDataDocument* document, int zoomLevel) :
    BaseFilter(document),
    m_resolution(resolutionForLevel(zoomLevel)),
    m_removedNodes(0),
    m_remainingNodes(0)
{
    foreach (GeoDataPlacemark* placemark, placemarks()) {

        if(placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType) {
            GeoDataLineString* prevLine = static_cast<GeoDataLineString*>(placemark->geometry());
            GeoDataLineString* reducedLine = reduce(prevLine);
            placemark->setGeometry(reducedLine);
        }

        else if(placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLinearRingType) {
            GeoDataLinearRing* prevRing = static_cast<GeoDataLinearRing*>(placemark->geometry());
            GeoDataLinearRing* reducedRing = reduce(prevRing);
            placemark->setGeometry(reducedRing);
        }

        else if(placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType) {
            GeoDataPolygon* reducedPolygon = new GeoDataPolygon;
            GeoDataPolygon* prevPolygon = static_cast<GeoDataPolygon*>(placemark->geometry());
            GeoDataLinearRing* prevRing = &(prevPolygon->outerBoundary());
            GeoDataLinearRing* reducedRing = reduce(prevRing);
            reducedPolygon->setOuterBoundary(*reducedRing);
            delete reducedRing;
            QVector<GeoDataLinearRing>& innerBoundaries = prevPolygon->innerBoundaries();
            for(int i = 0; i < innerBoundaries.size(); i++) {
                prevRing = &innerBoundaries[i];
                GeoDataLinearRing* reducedInnerRing = reduce(prevRing);
                reducedPolygon->appendInnerBoundary(*reducedInnerRing);
                delete reducedInnerRing;
            }
            placemark->setGeometry(reducedPolygon);
        }
    }
}

qint64 NodeReducer::remainingNodes() const
{
    return m_remainingNodes;
}

qint64 NodeReducer::removedNodes() const
{
    return m_removedNodes;
}

qreal NodeReducer::resolutionForLevel(int level) {
    switch (level) {
    case 0:
        return 0.0655360;
        break;
    case 1:
        return 0.0327680;
        break;
    case 2:
        return 0.0163840;
        break;
    case 3:
        return 0.0081920;
        break;
    case 4:
        return 0.0040960;
        break;
    case 5:
        return 0.0020480;
        break;
    case 6:
        return 0.0010240;
        break;
    case 7:
        return 0.0005120;
        break;
    case 8:
        return 0.0002560;
        break;
    case 9:
        return 0.0001280;
        break;
    case 10:
        return 0.0000640;
        break;
    case 11:
        return 0.0000320;
        break;
    case 12:
        return 0.0000160;
        break;
    case 13:
        return 0.0000080;
        break;
    case 14:
        return 0.0000040;
        break;
    case 15:
        return 0.0000020;
        break;
    case 16:
        return 0.0000010;
        break;
    default:
    case 17:
        return 0.0000005;
        break;
    }
}

}
