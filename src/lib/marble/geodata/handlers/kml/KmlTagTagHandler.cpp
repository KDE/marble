// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2015 Marius-Valeriu Stanciu <stanciumarius94@gmail.com>
//

#include "KmlTagTagHandler.h"

#include "KmlElementDictionary.h"
#include "osm/OsmPlacemarkData.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER_MX(tag)

GeoNode *KmltagTagHandler::parse(GeoParser &parser) const
{
    if (parser.parentElement().is<OsmPlacemarkData>()) {
        QString key = parser.attribute("k");
        QString value = parser.attribute("v");
        auto osmData = parser.parentElement().nodeAs<OsmPlacemarkData>();
        osmData->addTag(key, value);
    }

    return nullptr;
}

}
}
