//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#include "O5mWriter.h"

#include "GeoDataTypes.h"
#include "GeoDataDocument.h"
#include "GeoDataLineString.h"
#include "GeoDataLinearRing.h"
#include "GeoDataPolygon.h"
#include "GeoWriter.h"
#include "osm/OsmPlacemarkData.h"

#include <QDataStream>
#include <QBuffer>

namespace Marble
{

bool O5mWriter::write(QIODevice *device, const GeoDataDocument &document)
{
    if (!device || !device->isWritable()) {
        return false;
    }

    OsmConverter converter;
    converter.read(&document);

    QDataStream stream(device);
    writeHeader(stream);
    writeNodes(converter.nodes(), stream);
    writeWays(converter.ways(), stream);
    writePolygons(converter.polygons(), stream);
    writeTrailer(stream);

    return true;
}

void O5mWriter::writeHeader(QDataStream &stream) const
{
    stream << qint8(0xff); // o5m file start indicator
    stream << qint8(0xe0); // o5m header block indicator
    stream << qint8(0x04) << qint8(0x6f) << qint8(0x35) << qint8(0x6d) << qint8(0x32); // o5m header
}

void O5mWriter::writeNodes(const OsmConverter::Nodes &nodes, QDataStream &stream) const
{
    if (nodes.empty()) {
        return;
    }

    stream << qint8(0xff); // reset delta encoding counters
    StringTable stringTable;
    qint64 lastId = 0;
    double lastLon = 0.0;
    double lastLat = 0.0;

    foreach(const auto & node, nodes) {
        if (node.second.id() == lastId) {
            continue;
        }

        stream << qint8(0x10); // node section start indicator

        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        QDataStream bufferStream(&buffer);

        OsmPlacemarkData const & osmData = node.second;
        qint64 idDiff = osmData.id() - lastId;
        writeSigned(idDiff, bufferStream);
        writeVersion(osmData, bufferStream);
        GeoDataCoordinates const & coordinates = node.first;
        double const lon = coordinates.longitude(GeoDataCoordinates::Degree);
        double const lat = coordinates.latitude(GeoDataCoordinates::Degree);
        writeSigned(deltaTo(lon, lastLon), bufferStream);
        writeSigned(deltaTo(lat, lastLat), bufferStream);
        writeTags(osmData, stringTable, bufferStream);

        writeUnsigned(buffer.size(), stream);
        stream.writeRawData(buffer.data().constData(), buffer.size());

        lastId = osmData.id();
        lastLon = lon;
        lastLat = lat;
    }
}

void O5mWriter::writeWays(const OsmConverter::Ways &ways, QDataStream &stream) const
{
    if (ways.empty()) {
        return;
    }

    stream << qint8(0xff); // reset delta encoding counters
    StringTable stringTable;
    qint64 lastId = 0;
    qint64 lastReferenceId = 0;

    foreach(const auto & way, ways) {
        Q_ASSERT(way.first);
        if (way.second.id() == lastId) {
            continue;
        }

        stream << qint8(0x11); // way start indicator
        OsmPlacemarkData const & osmData = way.second;

        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        QDataStream bufferStream(&buffer);

        qint64 idDiff = osmData.id() - lastId;
        writeSigned(idDiff, bufferStream);
        lastId = osmData.id();
        writeVersion(osmData, bufferStream);

        QBuffer referencesBuffer;
        referencesBuffer.open(QIODevice::WriteOnly);
        QDataStream referencesStream(&referencesBuffer);
        writeReferences(*way.first, lastReferenceId, osmData, referencesStream);
        writeUnsigned(referencesBuffer.size(), bufferStream);
        bufferStream.writeRawData(referencesBuffer.data().constData(), referencesBuffer.size());

        writeTags(osmData, stringTable, bufferStream);

        writeUnsigned(buffer.size(), stream);
        stream.writeRawData(buffer.data().constData(), buffer.size());
    }
}

void O5mWriter::writePolygons(const OsmConverter::Polygons &polygons, QDataStream &stream) const
{
    if (polygons.empty()) {
        return;
    }

    stream << qint8(0xff); // reset delta encoding counters
    StringTable stringTable;
    qint64 lastId = 0;
    qint64 lastReferenceId = 0;

    foreach(const auto & polygon, polygons) {
        if (polygon.second.id() == lastId) {
            continue;
        }

        stream << qint8(0x12); // relation start indicator
        OsmPlacemarkData const & osmData = polygon.second;

        QBuffer buffer;
        buffer.open(QIODevice::WriteOnly);
        QDataStream bufferStream(&buffer);

        qint64 idDiff = osmData.id() - lastId;
        writeSigned(idDiff, bufferStream);
        lastId = osmData.id();
        writeVersion(osmData, bufferStream);

        QBuffer referencesBuffer;
        referencesBuffer.open(QIODevice::WriteOnly);
        QDataStream referencesStream(&referencesBuffer);
        writeRelationMembers(*polygon.first, lastReferenceId, osmData, stringTable, referencesStream);
        writeUnsigned(referencesBuffer.size(), bufferStream);
        bufferStream.writeRawData(referencesBuffer.data().constData(), referencesBuffer.size());

        writeTags(osmData, stringTable, bufferStream);

        writeUnsigned(buffer.size(), stream);
        stream.writeRawData(buffer.data().constData(), buffer.size());
    }
}

void O5mWriter::writeTrailer(QDataStream &stream) const
{
    stream << qint8(0xfe); // o5m file end indicator
}

void O5mWriter::writeRelationMembers(const GeoDataPolygon &polygon, qint64 &lastId, const OsmPlacemarkData &osmData, StringTable &stringTable, QDataStream &stream) const
{
    qint64 id = osmData.memberReference(-1).id();
    qint64 idDiff = id - lastId;
    writeSigned(idDiff, stream);
    lastId = id;
    writeStringPair(StringPair("1outer", QString()), stringTable, stream); // type=way, role=outer
    for (int index = 0; index < polygon.innerBoundaries().size(); ++index) {
        id = osmData.memberReference( index ).id();
        qint64 idDiff = id - lastId;
        writeSigned(idDiff, stream);
        writeStringPair(StringPair("1inner", QString()), stringTable, stream); // type=way, role=inner
        lastId = id;
    }
}

void O5mWriter::writeReferences(const GeoDataLineString &lineString, qint64 &lastId, const OsmPlacemarkData &osmData, QDataStream &stream) const
{
    QVector<GeoDataCoordinates>::const_iterator it = lineString.constBegin();
    QVector<GeoDataCoordinates>::ConstIterator const end = lineString.constEnd();

    for ( ; it != end; ++it ) {
        qint64 id = osmData.nodeReference( *it ).id();
        qint64 idDiff = id - lastId;
        writeSigned(idDiff, stream);
        lastId = id;
    }

    if (!lineString.isEmpty() && lineString.isClosed()) {
        auto const startId = osmData.nodeReference(lineString.first()).id();
        auto const endId = osmData.nodeReference(lineString.last()).id();
        if (startId != endId) {
            qint64 idDiff = startId - lastId;
            writeSigned(idDiff, stream);
            lastId = startId;
        }
    }
}

void O5mWriter::writeVersion(const OsmPlacemarkData &, QDataStream &stream) const
{
    stream << qint8(0x00); // no version information
    /** @todo implement */
}

void O5mWriter::writeTags(const OsmPlacemarkData &osmData, StringTable &stringTable, QDataStream &stream) const
{
    for (auto iter=osmData.tagsBegin(), end = osmData.tagsEnd(); iter != end; ++iter) {
        writeStringPair(StringPair(iter.key(), iter.value()), stringTable, stream);
    }
}

void O5mWriter::writeStringPair(const StringPair &pair, StringTable &stringTable, QDataStream &stream) const
{
    auto const iter = stringTable.constFind(pair);
    if (iter == stringTable.cend()) {
        QByteArray data;
        data.push_back(char(0x00));
        data.push_back(pair.first.toUtf8());
        if (!pair.second.isEmpty()) {
            data.push_back(char(0x00));
            data.push_back(pair.second.toUtf8());
        }
        data.push_back(char(0x00));
        stream.writeRawData(data.constData(), data.size());
        stringTable.insert(pair, stringTable.size());
    } else {
        auto const reference = stringTable.size() - iter.value();
        writeUnsigned(reference, stream);
    }
}

void O5mWriter::writeSigned(qint64 value, QDataStream &stream) const
{
    bool const negative = value < 0;
    if (negative) {
        value = -value - 1;
    }
    quint8 word = (value >> 6) > 0 ? (1<<7) : 0;
    word |= ( (value << 1) & 0x7e);
    if (negative) {
        word |= 0x01;
    }
    value >>= 6;
    stream << word;

    while (value > 0) {
        word = ((value >> 7) > 0 ? 0x80 : 0x00) | (value & 0x7f);
        stream << word;
        value >>= 7;
    }
}

void O5mWriter::writeUnsigned(quint32 value, QDataStream &stream) const
{
    do {
        quint8 word = (value >> 7 & 0x7f) != 0x00 ? (1<<7) : 0;
        word |= value & 0x7f;
        value >>= 7;
        stream << word;
    } while ((value & 0x7f) != 0x0);
}

qint32 O5mWriter::deltaTo(double value, double previous) const
{
    double const diff = value - previous;
    return qRound(diff * 1e7);
}

MARBLE_ADD_WRITER(O5mWriter, "o5m")

}
