//
// This file is part of the Marble Virtual Globe.
//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMWAYTAGWRITER_H
#define MARBLE_OSMWAYTAGWRITER_H

namespace Marble
{

class GeoDataLineString;
class GeoWriter;
class OsmPlacemarkData;

class OsmWayTagWriter
{

public:
    static void writeWay( const GeoDataLineString& lineString,
                          const OsmPlacemarkData& osmData, GeoWriter& writer );
};

}

#endif


