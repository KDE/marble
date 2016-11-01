//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2016      Dennis Nienhüser <nienhueser@kde.org>
//

#ifndef MARBLE_O5MWRITER_H
#define MARBLE_O5MWRITER_H

#include "OsmConverter.h"
#include "GeoWriterBackend.h"

namespace Marble
{

class GeoDataLineString;
class OsmPlacemarkData;

class O5mWriter: public GeoWriterBackend
{
public:
  bool write(QIODevice *device, const GeoDataDocument &document) override;

private:
  typedef QPair<QString, QString> StringPair;
  typedef QHash<StringPair, qint32> StringTable;

  void writeHeader(QDataStream& stream) const;
  void writeNodes(const OsmConverter::Nodes &nodes, QDataStream& stream) const;
  void writeWays(const OsmConverter::Ways &ways, QDataStream& stream) const;
  void writePolygons(const OsmConverter::Polygons &polygons, QDataStream& stream) const;
  void writeTrailer(QDataStream& stream) const;

  void writeRelationMembers(const GeoDataPolygon &polygon, qint64 &lastId, const OsmPlacemarkData &osmData, StringTable &stringTable, QDataStream &stream) const;
  void writeReferences(const GeoDataLineString &lineString, qint64 &lastId, const OsmPlacemarkData &osmData, QDataStream &stream) const;
  void writeVersion(const OsmPlacemarkData &osmData, QDataStream &stream) const;
  void writeTags(const OsmPlacemarkData &osmData, StringTable &stringTable, QDataStream &stream) const;
  void writeStringPair(const StringPair &pair, StringTable &stringTable, QDataStream &stream) const;
  void writeSigned(qint64 value, QDataStream &stream) const;
  void writeUnsigned(quint32 value, QDataStream &stream) const;
  qint32 deltaTo(double value, double previous) const;
};

}

#endif
