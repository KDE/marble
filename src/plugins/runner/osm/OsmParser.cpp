//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2011      Thibaut Gridel <tgridel@free.fr>
// Copyright 2011      Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
// Copyright 2014      Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// Copyright 2015      Dennis Nienhüser <nienhueser@kde.org>
//

#include "OsmParser.h"
#include "OsmElementDictionary.h"
#include "osm/OsmPresetLibrary.h"
#include "osm/OsmObjectManager.h"
#include "GeoDataDocument.h"
#include "GeoDataPlacemark.h"
#include "GeoDataPoint.h"
#include "GeoDataTypes.h"
#include "GeoDataStyle.h"

#include <QFile>

namespace Marble {

GeoDataDocument *OsmParser::parse(const QString &filename, QString &error)
{
    QFile file(filename);
    if (!file.open(QFile::ReadOnly)) {
        error = QString("Cannot open file %1").arg(filename);
        return nullptr;
    }

    QXmlStreamReader parser;
    parser.setDevice(&file);
    OsmPlacemarkData* osmData(0);
    QString parentTag;
    qint64 parentId(0);

    OsmNodes m_nodes;
    OsmWays m_ways;
    OsmRelations m_relations;

    while (!parser.atEnd()) {
        parser.readNext();
        if (!parser.isStartElement()) {
            continue;
        }

        QStringRef const tagName = parser.name();
        if (tagName == osm::osmTag_node || tagName == osm::osmTag_way || tagName == osm::osmTag_relation) {
            parentTag = parser.name().toString();
            parentId = parser.attributes().value("id").toLongLong();

            if (tagName == osm::osmTag_node) {
                m_nodes[parentId].osmData() = OsmPlacemarkData::fromParserAttributes(parser.attributes());
                m_nodes[parentId].parseCoordinates(parser.attributes());
                osmData = &m_nodes[parentId].osmData();
            } else if (tagName == osm::osmTag_way) {
                m_ways[parentId].osmData() = OsmPlacemarkData::fromParserAttributes(parser.attributes());
                osmData = &m_ways[parentId].osmData();
            } else {
                Q_ASSERT(tagName == osm::osmTag_relation);
                m_relations[parentId].osmData() = OsmPlacemarkData::fromParserAttributes(parser.attributes());
                osmData = &m_relations[parentId].osmData();
            }
        } else if (tagName == osm::osmTag_tag) {
            osmData->addTag(parser.attributes().value("k").toString(), parser.attributes().value("v").toString());
        } else if (tagName == osm::osmTag_nd && parentTag == osm::osmTag_way) {
            m_ways[parentId].addReference(parser.attributes().value("ref").toLongLong());
        } else if (tagName == osm::osmTag_member && parentTag == osm::osmTag_relation) {
            m_relations[parentId].parseMember(parser.attributes());
        } // other tags like osm, bounds ignored
    }

    if (parser.hasError()) {
        error = parser.errorString();
        return nullptr;
    }

    return createDocument(m_nodes, m_ways, m_relations);
}

GeoDataDocument *OsmParser::createDocument(OsmNodes &nodes, OsmWays &ways, OsmRelations &relations)
{
    GeoDataDocument* document = new GeoDataDocument;
    GeoDataPolyStyle backgroundPolyStyle;
    backgroundPolyStyle.setFill( true );
    backgroundPolyStyle.setOutline( false );
    backgroundPolyStyle.setColor("#f1eee8");
    GeoDataStyle backgroundStyle;
    backgroundStyle.setPolyStyle( backgroundPolyStyle );
    backgroundStyle.setId( "background" );
    document->addStyle( backgroundStyle );

    foreach(OsmRelation const &relation, relations) {
        relation.create(document, ways, nodes);
    }

    foreach(OsmWay const &way, ways) {
        way.create(document, nodes);
    }

    foreach(OsmNode const &node, nodes) {
        node.create(document);
    }
    return document;
}

}
