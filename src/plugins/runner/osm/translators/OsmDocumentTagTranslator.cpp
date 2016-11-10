//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

//Self
#include "OsmDocumentTagTranslator.h"

//Marble
#include "OsmNodeTagWriter.h"
#include "OsmWayTagWriter.h"
#include "OsmElementDictionary.h"
#include "GeoDataDocument.h"
#include "GeoWriter.h"
#include "GeoDataGeometry.h"
#include "GeoDataPoint.h"
#include "GeoDataPolygon.h"
#include "GeoDataLinearRing.h"
#include "GeoDataTypes.h"
#include "osm/OsmPlacemarkData.h"
#include "osm/OsmObjectManager.h"
#include "OsmRelationTagWriter.h"
#include "OsmConverter.h"

#include <QDebug>

namespace Marble
{

static GeoTagWriterRegistrar s_writerDocument( GeoTagWriter::QualifiedName( GeoDataTypes::GeoDataDocumentType,
                                                                            osm::osmTag_version06 ),
                                               new OsmDocumentTagTranslator() );


bool OsmDocumentTagTranslator::write( const GeoNode *node, GeoWriter& writer ) const
{
    const GeoDataDocument *document = static_cast<const GeoDataDocument*>(node);

    OsmConverter converter;
    converter.read(document);
    OsmNodeTagWriter::writeAllNodes(converter.nodes(), writer);

    qint64 lastId = 0;
    foreach(const auto &way, converter.ways()) {
        if (way.second.id() != lastId) {
            OsmWayTagWriter::writeWay(*way.first, way.second, writer);
            lastId = way.second.id();
        }
    }

    // Writing polygons
    foreach (const auto& polygon, converter.polygons()) {
        OsmRelationTagWriter::writeMultipolygon(*polygon.first, polygon.second, writer );
    }

    return true;
}

}

