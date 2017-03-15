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
#include "GeoDataRelation.h"
#include "GeoDataLinearRing.h"
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
    m_relations.clear();

    // Writing all the component nodes ( points, nodes of polylines, nodes of polygons )
    for (auto feature: document->featureList()) {
        if (auto placemark = geodata_cast<GeoDataPlacemark>(feature)) {
            // If the placemark's osmData is not complete, it is initialized by the OsmObjectManager
            OsmObjectManager::initializeOsmData( placemark );
            const OsmPlacemarkData & osmData = placemark->osmData();

            if (geodata_cast<GeoDataPoint>(placemark->geometry())) {
                m_nodes << OsmConverter::Node(placemark->coordinate(), osmData);
            } else if (const auto lineString = geodata_cast<GeoDataLineString>(placemark->geometry())) {
                for (auto const &coordinates: *lineString) {
                    m_nodes << OsmConverter::Node(coordinates, osmData.nodeReference(coordinates));
                }
                m_ways << OsmConverter::Way(lineString, osmData);
            } else if (const auto linearRing = geodata_cast<GeoDataLinearRing>(placemark->geometry())) {
                for (auto const &coordinates: *linearRing) {
                    m_nodes << OsmConverter::Node(coordinates, osmData.nodeReference(coordinates));
                }
                m_ways << OsmConverter::Way(linearRing, osmData);
            } else if (const auto polygon = geodata_cast<GeoDataPolygon>(placemark->geometry())) {
                int index = -1;

                // Writing all the outerRing's nodes
                const GeoDataLinearRing &outerRing = polygon->outerBoundary();
                const OsmPlacemarkData outerRingOsmData = osmData.memberReference( index );
                for (auto const &coordinates: outerRing) {
                    m_nodes << OsmConverter::Node(coordinates, outerRingOsmData.nodeReference(coordinates));
                }
                m_ways << OsmConverter::Way(&outerRing, outerRingOsmData);

                // Writing all nodes for each innerRing
                for (auto const &innerRing: polygon->innerBoundaries() ) {
                    ++index;
                    const OsmPlacemarkData innerRingOsmData = osmData.memberReference( index );
                    for (auto const &coordinates: innerRing) {
                        m_nodes << OsmConverter::Node(coordinates, innerRingOsmData.nodeReference(coordinates));
                    }
                    m_ways << OsmConverter::Way(&innerRing, innerRingOsmData);
                }
                m_relations.append(OsmConverter::Relation(placemark, osmData));
            }
        } else if (const auto placemark = geodata_cast<GeoDataRelation>(feature)) {
            m_relations.append(OsmConverter::Relation(placemark, placemark->osmData()));
        }
    }

    // Sort by id ascending since some external tools rely on that
    std::sort(m_nodes.begin(), m_nodes.end(), [] (const Node &a, const Node &b) { return a.second.id() < b.second.id(); });
    std::sort(m_ways.begin(), m_ways.end(), [] (const Way &a, const Way &b) { return a.second.id() < b.second.id(); });
    std::sort(m_relations.begin(), m_relations.end(), [] (const Relation &a, const Relation &b) { return a.second.id() < b.second.id(); });
}

const OsmConverter::Nodes &OsmConverter::nodes() const
{
    return m_nodes;
}

const OsmConverter::Ways &OsmConverter::ways() const
{
    return m_ways;
}

const OsmConverter::Relations &OsmConverter::relations() const
{
    return m_relations;
}

}

