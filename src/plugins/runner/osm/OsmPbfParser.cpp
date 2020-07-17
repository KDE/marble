/*
    SPDX-FileCopyrightText: 2020 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "OsmPbfParser.h"

#ifdef HAVE_PROTOBUF
#include "fileformat.pb.h"
#include "osmformat.pb.h"
#endif

#include <QDebug>
#include <QtEndian>

#include <zlib.h>

#include <cstdint>
#include <cstring>

using namespace Marble;

void OsmPbfParser::parse(const uint8_t *data, std::size_t len)
{
#ifdef HAVE_PROTOBUF
    const uint8_t *it = data;
    const uint8_t *end = data + len;
    while (parseBlob(it, end));
#endif
}

#ifdef HAVE_PROTOBUF
bool OsmPbfParser::parseBlob(const uint8_t *&it, const uint8_t *end)
{
    if (std::distance(it, end) < (int)sizeof(int32_t)) {
        return false;
    }
    int32_t blobHeaderSize = 0;
    std::memcpy(&blobHeaderSize, it, sizeof(int32_t));
    blobHeaderSize = qFromBigEndian(blobHeaderSize);
    it += sizeof(int32_t);

    if (blobHeaderSize < 0 || std::distance(it, end) < blobHeaderSize) {
        return false;
    }

    OSMPBF::BlobHeader blobHeader;
    if (!blobHeader.ParseFromArray(it, blobHeaderSize)) {
        return false;
    }
    it += blobHeaderSize;

    OSMPBF::Blob blob;
    if (std::distance(it, end) < blobHeader.datasize() || !blob.ParseFromArray(it, blobHeader.datasize())) {
        return false;
    }

    const uint8_t *dataBegin = nullptr;
    if (blob.has_raw()) {
        dataBegin = reinterpret_cast<const uint8_t*>(blob.raw().data());
    } else if (blob.has_zlib_data()) {
        m_zlibBuffer.resize(blob.raw_size());
        z_stream zStream;
        zStream.next_in = (uint8_t*)blob.zlib_data().data();
        zStream.avail_in = blob.zlib_data().size();
        zStream.next_out = (uint8_t*)m_zlibBuffer.data();
        zStream.avail_out = blob.raw_size();
        zStream.zalloc = nullptr;
        zStream.zfree = nullptr;
        zStream.opaque = nullptr;
        auto result = inflateInit(&zStream);
        if (result != Z_OK) {
            return false;
        }
        result = inflate(&zStream, Z_FINISH);
        if (result != Z_STREAM_END) {
            return false;
        }
        result = inflateEnd( &zStream );
        dataBegin = reinterpret_cast<const uint8_t*>(m_zlibBuffer.constData());
    } else {
        return false;
    }
    if (std::strcmp(blobHeader.type().c_str(), "OSMData") == 0) {
        parsePrimitiveBlock(dataBegin, blob.raw_size());
    }

    m_zlibBuffer.clear();
    it += blobHeader.datasize();
    return true;
}

void OsmPbfParser::parsePrimitiveBlock(const uint8_t *data, std::size_t len)
{
    OSMPBF::PrimitiveBlock block;
    if (!block.ParseFromArray(data, len)) {
        return;
    }

    for (int i = 0; i < block.primitivegroup_size(); ++i) {
        const auto &group = block.primitivegroup(i);

        if (group.nodes_size()) {
            qWarning() << "non-dense nodes - not implemented yet!";
        } else if (group.has_dense()) {
            parseDenseNodes(block, group);
        } else if (group.ways_size()) {
            parseWays(block, group);
        } else if (group.relations_size()) {
            parseRelations(block, group);
        }
    }
}

void OsmPbfParser::parseDenseNodes(const OSMPBF::PrimitiveBlock &block, const OSMPBF::PrimitiveGroup &group)
{
    int64_t idDelta = 0;
    int64_t latDelta = 0;
    int64_t lonDelta = 0;
    int tagIdx = 0;

    const auto dense = group.dense();
    for (int i = 0; i < dense.id_size(); ++i) {
        idDelta += dense.id(i);
        latDelta += dense.lat(i);
        lonDelta += dense.lon(i);

        auto &node = m_nodes[idDelta];
        node.osmData().setId(idDelta);
        node.setCoordinates(GeoDataCoordinates(lonDelta * 1.0e-7, latDelta * 1.0e-7, 0.0, GeoDataCoordinates::Degree));

        while (tagIdx < dense.keys_vals_size()) {
            const auto keyIdx = dense.keys_vals(tagIdx++);
            if (keyIdx == 0) {
                break;
            }
            const auto valIdx = dense.keys_vals(tagIdx++);
            const QString keyString = *m_stringPool.insert(QString::fromUtf8(block.stringtable().s(keyIdx).data()));
            const QString valueString = *m_stringPool.insert(QString::fromUtf8(block.stringtable().s(valIdx).data()));
            node.osmData().addTag(keyString, valueString);
        }
    }
}

void OsmPbfParser::parseWays(const OSMPBF::PrimitiveBlock &block, const OSMPBF::PrimitiveGroup &group)
{
    for (int i = 0; i < group.ways_size(); ++i) {
        const auto &w = group.ways(i);
        auto &way = m_ways[w.id()];
        way.osmData().setId(w.id());

        int64_t idDelta = 0;
        for (int j = 0; j < w.refs_size(); ++j) {
            idDelta += w.refs(j);
            way.addReference(idDelta);
        }

        for (int j = 0; j < w.keys_size(); ++j) {
            const QString keyString = *m_stringPool.insert(QString::fromUtf8(block.stringtable().s(w.keys(j)).data()));
            const QString valueString = *m_stringPool.insert(QString::fromUtf8(block.stringtable().s(w.vals(j)).data()));
            way.osmData().addTag(keyString, valueString);
        }
    }
}

void OsmPbfParser::parseRelations(const OSMPBF::PrimitiveBlock &block, const OSMPBF::PrimitiveGroup &group)
{
    for (int i = 0; i < group.relations_size(); ++i) {
        const auto &r = group.relations(i);

        auto &rel = m_relations[r.id()];
        rel.osmData().setId(r.id());

        int64_t idDelta = 0;
        for (int j = 0; j < r.memids_size(); ++j) {
            idDelta += r.memids(j);
            const QString role = *m_stringPool.insert(QString::fromUtf8(block.stringtable().s(r.roles_sid(j)).data()));
            QString typeName;
            const auto type = r.types(j);
            switch (type) {
                case OSMPBF::Relation_MemberType_NODE: typeName = QStringLiteral("node"); break;
                case OSMPBF::Relation_MemberType_WAY:  typeName = QStringLiteral("way"); break;
                case OSMPBF::Relation_MemberType_RELATION: typeName = QStringLiteral("relation"); break;
            }
            rel.addMember(idDelta, role, typeName);
        }

        for (int j = 0; j < r.keys_size(); ++j) {
            const QString keyString = *m_stringPool.insert(QString::fromUtf8(block.stringtable().s(r.keys(j)).data()));
            const QString valueString = *m_stringPool.insert(QString::fromUtf8(block.stringtable().s(r.vals(j)).data()));
            rel.osmData().addTag(keyString, valueString);
        }
    }
}
#endif
