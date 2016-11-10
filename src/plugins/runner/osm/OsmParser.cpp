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
#include "osm/OsmObjectManager.h"
#include "GeoDataDocument.h"
#include "GeoDataPoint.h"
#include "GeoDataTypes.h"
#include "GeoDataStyle.h"
#include "GeoDataPolyStyle.h"
#include <MarbleZipReader.h>
#include "o5mreader.h"

#include <QFile>
#include <QFileInfo>
#include <QBuffer>
#include <QSet>

namespace Marble {

GeoDataDocument *OsmParser::parse(const QString &filename, QString &error)
{
    QFileInfo const fileInfo(filename);
    if (!fileInfo.exists() || !fileInfo.isReadable()) {
        error = QString("Cannot read file %1").arg(filename);
        return 0;
    }

    if (fileInfo.completeSuffix() == QLatin1String("o5m")) {
        return parseO5m(filename, error);
    } else {
        return parseXml(filename, error);
    }
}

GeoDataDocument* OsmParser::parseO5m(const QString &filename, QString &error)
{
    O5mreader* reader;
    O5mreaderDataset data;
    O5mreaderIterateRet outerState, innerState;
    char *key, *value;
    // share string data on the heap at least for this file
    QSet<QString> stringPool;

    OsmNodes nodes;
    OsmWays ways;
    OsmRelations relations;
    QHash<uint8_t, QString> relationTypes;
    relationTypes[O5MREADER_DS_NODE] = QStringLiteral("node");
    relationTypes[O5MREADER_DS_WAY] = QStringLiteral("way");
    relationTypes[O5MREADER_DS_REL] = QStringLiteral("relation");

    auto file = fopen(filename.toStdString().c_str(), "rb");
    o5mreader_open(&reader, file);

    while( (outerState = o5mreader_iterateDataSet(reader, &data)) == O5MREADER_ITERATE_RET_NEXT) {
        switch (data.type) {
        case O5MREADER_DS_NODE:
        {
            OsmNode& node = nodes[data.id];
            node.osmData().setId(data.id);
            node.setCoordinates(GeoDataCoordinates(data.lon*1.0e-7, data.lat*1.0e-7,
                                                   0.0, GeoDataCoordinates::Degree));
            while ((innerState = o5mreader_iterateTags(reader, &key, &value)) == O5MREADER_ITERATE_RET_NEXT) {
                const QString keyString = *stringPool.insert(QString::fromUtf8(key));
                const QString valueString = *stringPool.insert(QString::fromUtf8(value));
                node.osmData().addTag(keyString, valueString);
            }
        }
            break;
        case O5MREADER_DS_WAY:
        {
            OsmWay &way = ways[data.id];
            way.osmData().setId(data.id);
            uint64_t nodeId;
            while ((innerState = o5mreader_iterateNds(reader, &nodeId)) == O5MREADER_ITERATE_RET_NEXT) {
                way.addReference(nodeId);
            }
            while ((innerState = o5mreader_iterateTags(reader, &key, &value)) == O5MREADER_ITERATE_RET_NEXT) {
                const QString keyString = *stringPool.insert(QString::fromUtf8(key));
                const QString valueString = *stringPool.insert(QString::fromUtf8(value));
                way.osmData().addTag(keyString, valueString);
            }
        }
            break;
        case O5MREADER_DS_REL:
        {
            OsmRelation &relation = relations[data.id];
            relation.osmData().setId(data.id);
            char *role;
            uint8_t type;
            uint64_t refId;
            while ((innerState = o5mreader_iterateRefs(reader, &refId, &type, &role)) == O5MREADER_ITERATE_RET_NEXT) {
                const QString roleString = *stringPool.insert(QString::fromUtf8(role));
                relation.addMember(refId, roleString, relationTypes[type]);
            }
            while ((innerState = o5mreader_iterateTags(reader, &key, &value)) == O5MREADER_ITERATE_RET_NEXT) {
                const QString keyString = *stringPool.insert(QString::fromUtf8(key));
                const QString valueString = *stringPool.insert(QString::fromUtf8(value));
                relation.osmData().addTag(keyString, valueString);
            }
        }
            break;
        }
    }

    fclose(file);
    error = reader->errMsg;
    o5mreader_close(reader);
    return createDocument(nodes, ways, relations);
}

GeoDataDocument* OsmParser::parseXml(const QString &filename, QString &error)
{
    QXmlStreamReader parser;
    QFile file;
    QBuffer buffer;
    QFileInfo fileInfo(filename);
    if (fileInfo.completeSuffix() == QLatin1String("osm.zip")) {
        MarbleZipReader zipReader(filename);
        if (zipReader.fileInfoList().size() != 1) {
            int const fileNumber = zipReader.fileInfoList().size();
            error = QStringLiteral("Unexpected number of files (%1) in %2").arg(fileNumber).arg(filename);
            return nullptr;
        }
        QByteArray const data = zipReader.fileData(zipReader.fileInfoList().first().filePath);
        buffer.setData(data);
        buffer.open(QBuffer::ReadOnly);
        parser.setDevice(&buffer);
    } else {
        file.setFileName(filename);
        if (!file.open(QFile::ReadOnly)) {
            error = QStringLiteral("Cannot open file %1").arg(filename);
            return nullptr;
        }
        parser.setDevice(&file);
    }

    OsmPlacemarkData* osmData(0);
    QString parentTag;
    qint64 parentId(0);
    // share string data on the heap at least for this file
    QSet<QString> stringPool;

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
            parentId = parser.attributes().value(QLatin1String("id")).toLongLong();

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
            const QXmlStreamAttributes &attributes = parser.attributes();
            const QString keyString = *stringPool.insert(attributes.value(QLatin1String("k")).toString());
            const QString valueString = *stringPool.insert(attributes.value(QLatin1String("v")).toString());
            osmData->addTag(keyString, valueString);
        } else if (tagName == osm::osmTag_nd && parentTag == osm::osmTag_way) {
            m_ways[parentId].addReference(parser.attributes().value(QLatin1String("ref")).toLongLong());
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
    backgroundPolyStyle.setColor(QStringLiteral("#f1eee8"));
    GeoDataStyle::Ptr backgroundStyle(new GeoDataStyle);
    backgroundStyle->setPolyStyle( backgroundPolyStyle );
    backgroundStyle->setId(QStringLiteral("background"));
    document->addStyle( backgroundStyle );

    QSet<qint64> usedNodes, usedWays;
    foreach(OsmRelation const &relation, relations) {
        relation.create(document, ways, nodes, usedNodes, usedWays);
    }
    foreach(qint64 id, usedWays) {
        ways.remove(id);
    }

    foreach(OsmWay const &way, ways) {
        way.create(document, nodes, usedNodes);
    }

    foreach(qint64 id, usedNodes) {
        if (nodes[id].osmData().isEmpty()) {
            nodes.remove(id);
        }
    }

    foreach(OsmNode const &node, nodes) {
        node.create(document);
    }

    return document;
}

}
