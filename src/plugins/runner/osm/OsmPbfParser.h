/*
    SPDX-FileCopyrightText: 2020 Volker Krause <vkrause@kde.org>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#ifndef MARBLE_OSMPBFPARSER_H
#define MARBLE_OSMPBFPARSER_H

#include "OsmNode.h"
#include "OsmRelation.h"
#include "OsmWay.h"

namespace OSMPBF
{
class PrimitiveBlock;
class PrimitiveGroup;
}

namespace Marble
{

class OsmPbfParser
{
public:
    void parse(const uint8_t *data, std::size_t len);

    OsmNodes m_nodes;
    OsmWays m_ways;
    OsmRelations m_relations;

private:
    bool parseBlob(const uint8_t *&it, const uint8_t *end);
    void parsePrimitiveBlock(const uint8_t *data, std::size_t len);
    void parseDenseNodes(const OSMPBF::PrimitiveBlock &block, const OSMPBF::PrimitiveGroup &group);
    void parseWays(const OSMPBF::PrimitiveBlock &block, const OSMPBF::PrimitiveGroup &group);
    void parseRelations(const OSMPBF::PrimitiveBlock &block, const OSMPBF::PrimitiveGroup &group);

    QByteArray m_zlibBuffer;
    QSet<QString> m_stringPool;
};

}

#endif // MARBLE_OSMPBFPARSER_H
