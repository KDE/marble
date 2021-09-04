// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Stanciu Marius-Valeriu <stanciumarius94@gmail.com>
//

#ifndef MARBLE_OSMTAGTAGWRITER_H
#define MARBLE_OSMTAGTAGWRITER_H

#include <QSet>

namespace Marble
{

class OsmPlacemarkData;
class GeoWriter;

class OsmTagTagWriter
{

public:
    /**
     * @brief writeTags writes multiple OSM \<tag k="key" v="value"\>
     */
    static void writeTags( const OsmPlacemarkData &osmData, GeoWriter& writer );

private:
    static QSet<QString> m_blacklist;
};

}

#endif


