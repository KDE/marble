// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2011 Thibaut Gridel <tgridel@free.fr>
// SPDX-FileCopyrightText: 2011 Konstantin Oblaukhov <oblaukhov.konstantin@gmail.com>
// SPDX-FileCopyrightText: 2014 Bernhard Beschow <bbeschow@cs.tu-berlin.de>
// SPDX-FileCopyrightText: 2015 Dennis Nienhüser <nienhueser@kde.org>
//

#include "OsmParser.h"
#include "GeoDataDocument.h"
#include "GeoDataPolyStyle.h"
#include "GeoDataStyle.h"
#include "OsmElementDictionary.h"
#include "OsmPbfParser.h"
#include "o5mreader.h"
#include "osm/OsmObjectManager.h"
#include <MarbleZipReader.h>

#include <QBuffer>
#include <QColor>
#include <QFile>
#include <QFileInfo>
#include <QSet>

#include <MarbleDebug.h>

namespace Marble
{

GeoDataDocument *OsmParser::parse(const QString &filename, QString &error)
{
    QFileInfo const fileInfo(filename);
    if (!fileInfo.exists() || !fileInfo.isReadable()) {
        error = QStringLiteral("Cannot read file %1").arg(filename);
        return nullptr;
    }

    if (fileInfo.suffix() == QLatin1StringView("o5m")) {
        return parseO5m(filename, error);
    } else if (filename.endsWith(QLatin1StringView(".osm.pbf"))) {
        return parseOsmPbf(filename, error);
    } else {
        return parseXml(filename, error);
    }
}

GeoDataDocument *OsmParser::parseO5m(const QString &filename, QString &error)
{
    O5mreader *reader;
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
    auto success = o5mreader_open(&reader, file);

    // Only executed if the opened file is not truncated, starts correctly, and
    // if memory for the reader and its O5M StringPairTable was fully allocated.
    if (success != O5MREADER_ERR_CODE_OK) {
        while ((outerState = o5mreader_iterateDataSet(reader, &data)) == O5MREADER_ITERATE_RET_NEXT) {
            switch (data.type) {
            case O5MREADER_DS_NODE: {
                OsmNode &node = nodes[data.id];
                node.osmData().setId(data.id);
                node.setCoordinates(GeoDataCoordinates(data.lon * 1.0e-7, data.lat * 1.0e-7, 0.0, GeoDataCoordinates::Degree));
                while ((innerState = o5mreader_iterateTags(reader, &key, &value)) == O5MREADER_ITERATE_RET_NEXT) {
                    const QString keyString = *stringPool.insert(QString::fromUtf8(key));
                    const QString valueString = *stringPool.insert(QString::fromUtf8(value));
                    node.osmData().addTag(keyString, valueString);
                }
            } break;
            case O5MREADER_DS_WAY: {
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
            } break;
            case O5MREADER_DS_REL: {
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
            } break;
            }
        }
    }

    fclose(file);
    error = reader->errMsg;
    if (!error.isEmpty())
        mDebug() << error;
    o5mreader_close(reader);
    return createDocument(nodes, ways, relations);
}

GeoDataDocument *OsmParser::parseXml(const QString &filename, QString &error)
{
    QXmlStreamReader parser;
    QFile file;
    QBuffer buffer;
    QFileInfo fileInfo(filename);
    if (fileInfo.completeSuffix() == QLatin1StringView("osm.zip")) {
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

    OsmPlacemarkData *osmData(nullptr);
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

        QStringView const tagName = parser.name();
        if (tagName == QStringView(QString::fromUtf8(osm::osmTag_node)) || tagName == QStringView(QString::fromUtf8(osm::osmTag_way))
            || tagName == QStringView(QString::fromUtf8(osm::osmTag_relation))) {
            parentTag = parser.name().toString();
            parentId = parser.attributes().value(QLatin1StringView("id")).toLongLong();

            if (tagName == QStringView(QString::fromUtf8(osm::osmTag_node))) {
                m_nodes[parentId].osmData() = OsmPlacemarkData::fromParserAttributes(parser.attributes());
                m_nodes[parentId].parseCoordinates(parser.attributes());
                osmData = &m_nodes[parentId].osmData();
            } else if (tagName == QStringView(QString::fromUtf8(osm::osmTag_way))) {
                m_ways[parentId].osmData() = OsmPlacemarkData::fromParserAttributes(parser.attributes());
                osmData = &m_ways[parentId].osmData();
            } else {
                Q_ASSERT(tagName == QStringView(QString::fromUtf8(osm::osmTag_relation)));
                m_relations[parentId].osmData() = OsmPlacemarkData::fromParserAttributes(parser.attributes());
                osmData = &m_relations[parentId].osmData();
            }
        } else if (osmData && tagName == QStringView(QString::fromUtf8(osm::osmTag_tag))) {
            const QXmlStreamAttributes &attributes = parser.attributes();
            const QString keyString = *stringPool.insert(attributes.value(QLatin1StringView("k")).toString());
            const QString valueString = *stringPool.insert(attributes.value(QLatin1StringView("v")).toString());
            osmData->addTag(keyString, valueString);
        } else if (tagName == QStringView(QString::fromUtf8(osm::osmTag_nd)) && parentTag == QStringView(QString::fromUtf8(osm::osmTag_way))) {
            m_ways[parentId].addReference(parser.attributes().value(QLatin1StringView("ref")).toLongLong());
        } else if (tagName == QStringView(QString::fromUtf8(osm::osmTag_member)) && parentTag == QStringView(QString::fromUtf8(osm::osmTag_relation))) {
            m_relations[parentId].parseMember(parser.attributes());
        } // other tags like osm, bounds ignored
    }

    if (parser.hasError()) {
        error = parser.errorString();
        return nullptr;
    }

    return createDocument(m_nodes, m_ways, m_relations);
}

GeoDataDocument *OsmParser::parseOsmPbf(const QString &filename, QString &error)
{
    QFile f(filename);
    if (!f.open(QFile::ReadOnly)) {
        error = f.errorString();
        return nullptr;
    }

    const auto data = f.map(0, f.size());
    OsmPbfParser p;
    p.parse(data, f.size());
    return createDocument(p.m_nodes, p.m_ways, p.m_relations);
}

GeoDataDocument *OsmParser::createDocument(OsmNodes &nodes, OsmWays &ways, OsmRelations &relations)
{
    auto document = new GeoDataDocument;
    GeoDataPolyStyle backgroundPolyStyle;
    backgroundPolyStyle.setFill(true);
    backgroundPolyStyle.setOutline(false);
    backgroundPolyStyle.setColor(QStringLiteral("#f1eee8"));
    GeoDataStyle::Ptr backgroundStyle(new GeoDataStyle);
    backgroundStyle->setPolyStyle(backgroundPolyStyle);
    backgroundStyle->setId(QStringLiteral("background"));
    document->addStyle(backgroundStyle);

    QSet<qint64> usedNodes, usedWays;
    for (auto const &relation : relations) {
        relation.createMultipolygon(document, ways, nodes, usedNodes, usedWays);
    }
    for (auto id : std::as_const(usedWays)) {
        ways.remove(id);
    }

    QHash<qint64, GeoDataPlacemark *> placemarks;
    for (auto iter = ways.constBegin(), end = ways.constEnd(); iter != end; ++iter) {
        auto placemark = iter.value().create(nodes, usedNodes);
        if (placemark) {
            document->append(placemark);
            placemarks[placemark->osmData().oid()] = placemark;
        }
    }

    for (auto id : std::as_const(usedNodes)) {
        if (nodes[id].osmData().isEmpty()) {
            nodes.remove(id);
        }
    }

    for (auto const &node : std::as_const(nodes)) {
        auto placemark = node.create();
        if (placemark) {
            document->append(placemark);
            placemarks[placemark->osmData().oid()] = placemark;
        }
    }

    for (auto const &relation : relations) {
        relation.createRelation(document, placemarks);
    }

    return document;
}

}
