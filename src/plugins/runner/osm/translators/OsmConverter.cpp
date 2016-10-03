//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

//Self
#include "OsmDocumentTagTranslator.h"

//Marble
#include "OsmNodeTagWriter.h"
#include "OsmWayTagWriter.h"
#include "OsmElementDictionary.h"
#include "GeoDataDocument.h"
#include "GeoWriter.h"
#include "GeoDataPlacemark.h"
#include "GeoDataGeometry.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "GeoDataLinearRing.h"
#include "GeoDataTypes.h"
#include "osm/OsmPlacemarkData.h"
#include "osm/OsmObjectManager.h"
#include "OsmRelationTagWriter.h"

#include <QDebug>

namespace Marble
{

void OsmConverter::read(const GeoDataDocument *document)
{
    m_nodes.clear();
    m_ways.clear();
    m_polygons.clear();

    // Writing all the component nodes ( points, nodes of polylines, nodes of polygons )
    foreach (GeoDataPlacemark* placemark, document->placemarkList()) {
        // If the placemark's osmData is not complete, it is initialized by the OsmObjectManager
        OsmObjectManager::initializeOsmData( placemark );
        const OsmPlacemarkData & osmData = placemark->osmData();

        if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPointType ) {
            m_nodes << OsmConverter::Node(placemark->coordinate(), osmData);
        } else if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLineStringType ) {
            const GeoDataLineString* lineString = static_cast<const GeoDataLineString*>( placemark->geometry() );
            foreach(const GeoDataCoordinates &coordinates, *lineString) {
                m_nodes << OsmConverter::Node(coordinates, osmData.nodeReference(coordinates));
            }
            m_ways << OsmConverter::Way(lineString, osmData);
        } else if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataLinearRingType ) {
            const GeoDataLinearRing* linearRing = static_cast<const GeoDataLinearRing*>( placemark->geometry() );
            foreach(const GeoDataCoordinates &coordinates, *linearRing) {
                m_nodes << OsmConverter::Node(coordinates, osmData.nodeReference(coordinates));
            }
            m_ways << OsmConverter::Way(linearRing, osmData);
        } else if ( placemark->geometry()->nodeType() == GeoDataTypes::GeoDataPolygonType ) {
            const GeoDataPolygon *polygon = static_cast<const GeoDataPolygon*>( placemark->geometry() );
            int index = -1;

            // Writing all the outerRing's nodes
            const GeoDataLinearRing &outerRing = polygon->outerBoundary();
            const OsmPlacemarkData outerRingOsmData = osmData.memberReference( index );
            foreach(const GeoDataCoordinates &coordinates, outerRing) {
                m_nodes << OsmConverter::Node(coordinates, outerRingOsmData.nodeReference(coordinates));
            }
            m_ways << OsmConverter::Way(&outerRing, outerRingOsmData);

            // Writing all nodes for each innerRing
            foreach ( const GeoDataLinearRing &innerRing, polygon->innerBoundaries() ) {
                ++index;
                const OsmPlacemarkData innerRingOsmData = osmData.memberReference( index );
                foreach(const GeoDataCoordinates &coordinates, innerRing) {
                    m_nodes << OsmConverter::Node(coordinates, innerRingOsmData.nodeReference(coordinates));
                }
                m_ways << OsmConverter::Way(&innerRing, innerRingOsmData);
            }
            m_polygons.append(OsmConverter::Polygon(polygon, osmData));
        }
    }

    // Sort by id ascending since some external tools rely on that
    qSort(m_nodes.begin(), m_nodes.end(), [] (const Node &a, const Node &b) { return a.second.id() < b.second.id(); });
    qSort(m_ways.begin(), m_ways.end(), [] (const Way &a, const Way &b) { return a.second.id() < b.second.id(); });
    qSort(m_polygons.begin(), m_polygons.end(), [] (const Polygon &a, const Polygon &b) { return a.second.id() < b.second.id(); });
}

const OsmConverter::Nodes &OsmConverter::nodes() const
{
    return m_nodes;
}

const OsmConverter::Ways &OsmConverter::ways() const
{
    return m_ways;
}

const OsmConverter::Polygons &OsmConverter::polygons() const
{
    return m_polygons;
}

}

