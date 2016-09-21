//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMTAGTAGWRITER_H
#define MARBLE_OSMTAGTAGWRITER_H

// Marble

namespace Marble
{

class OsmPlacemarkData;
class GeoWriter;

class OsmTagTagWriter
{

public:
    /**
     * @brief writeTags writes multiple OSM <tag k="key" v="value">
     */
    static void writeTags( const OsmPlacemarkData &osmData, GeoWriter& writer );
};

}

#endif


