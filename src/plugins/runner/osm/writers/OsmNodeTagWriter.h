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

namespace Marble
{
class GeoDataLineString;
class GeoDataCoordinates;
class GeoWriter;
class OsmPlacemarkData;
class OsmWayData;

class OsmNodeTagWriter
{

public:
    static void writeAllNodes( const OsmPlacemarkData& osmData, GeoWriter& writer );
    static void writeNode( const GeoDataCoordinates& coordinates,
                           const OsmPlacemarkData& osmData, GeoWriter& writer );
};

}

#endif

