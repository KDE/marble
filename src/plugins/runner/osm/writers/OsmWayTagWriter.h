//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
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


