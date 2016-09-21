//
// This file is part of the Marble Virtual Globe.
//
// This program is free software licensed under the GNU LGPL. You can
// find a copy of this license in LICENSE.txt in the top directory of
// the source code.
//
// Copyright 2015      Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMOBJECTATTRIBUTEWRITER_H
#define MARBLE_OSMOBJECTATTRIBUTEWRITER_H

namespace Marble
{
class OsmPlacemarkData;
class GeoWriter;

class OsmObjectAttributeWriter
{
public:

    /**
     * Writes the standard osm attributes: id, action, changeset, visible, timestamp, uid, user, version
     */
    static void writeAttributes( const OsmPlacemarkData &osmData, GeoWriter &writer );
};

}

#endif

