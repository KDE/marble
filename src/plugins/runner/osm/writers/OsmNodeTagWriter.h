// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMNODETAGWRITER_H
#define MARBLE_OSMNODETAGWRITER_H

#include "translators/OsmConverter.h"

#include <QList>

namespace Marble
{
class GeoWriter;
class OsmWayData;

class OsmNodeTagWriter
{
public:
    static void writeAllNodes(const OsmConverter::Nodes &osmData, GeoWriter &writer);

private:
    static void writeNode(const OsmConverter::Node &node, GeoWriter &writer);
};

}

#endif
