// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
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

