//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
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
    static void writeAllNodes(const OsmConverter::Nodes &osmData, GeoWriter& writer );

private:
    static void writeNode(const OsmConverter::Node &node, GeoWriter& writer );
};

}

#endif

