// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

// Self
#include "OsmDocumentTagTranslator.h"

// Marble
#include "GeoDataBuilding.h"
#include "GeoDataDocument.h"
#include "GeoDataGeometry.h"
#include "GeoDataLinearRing.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "GeoDataTypes.h"
#include "GeoWriter.h"
#include "OsmConverter.h"
#include "OsmElementDictionary.h"
#include "OsmNodeTagWriter.h"
#include "OsmRelationTagWriter.h"
#include "OsmWayTagWriter.h"
#include "osm/OsmObjectManager.h"
#include "osm/OsmPlacemarkData.h"

#include <QDebug>

namespace Marble
{

static GeoTagWriterRegistrar s_writerDocument(GeoTagWriter::QualifiedName(GeoDataTypes::GeoDataDocumentType, osm::osmTag_version06),
                                              new OsmDocumentTagTranslator());

bool OsmDocumentTagTranslator::write(const GeoNode *node, GeoWriter &writer) const
{
    const GeoDataDocument *document = static_cast<const GeoDataDocument *>(node);

    OsmConverter converter;
    converter.read(document);
    OsmNodeTagWriter::writeAllNodes(converter.nodes(), writer);

    qint64 lastId = 0;
    for (auto const &way : converter.ways()) {
        if (way.second.id() != lastId) {
            OsmWayTagWriter::writeWay(*way.first, way.second, writer);
            lastId = way.second.id();
        }
    }

    for (auto const &relation : converter.relations()) {
        if (auto placemark = geodata_cast<GeoDataPlacemark>(relation.first)) {
            if (const auto building = geodata_cast<GeoDataBuilding>(placemark->geometry())) {
                auto polygon = geodata_cast<GeoDataPolygon>(&building->multiGeometry()->at(0));
                Q_ASSERT(polygon);
                OsmRelationTagWriter::writeMultipolygon(*polygon, relation.second, writer);
            } else {
                auto polygon = geodata_cast<GeoDataPolygon>(placemark->geometry());
                Q_ASSERT(polygon);
                OsmRelationTagWriter::writeMultipolygon(*polygon, relation.second, writer);
            }
        }
    }

    return true;
}

}
