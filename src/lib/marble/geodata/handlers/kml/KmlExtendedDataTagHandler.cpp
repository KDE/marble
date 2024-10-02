// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlExtendedDataTagHandler.h"

#include "GeoDataExtendedData.h"
#include "GeoDataFeature.h"
#include "GeoDataTrack.h"
#include "KmlElementDictionary.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(ExtendedData)

GeoNode *KmlExtendedDataTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_ExtendedData)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataFeature>()) {
        GeoDataExtendedData extendedData;
        parentItem.nodeAs<GeoDataFeature>()->setExtendedData(extendedData);
        return static_cast<GeoDataExtendedData *>(&parentItem.nodeAs<GeoDataFeature>()->extendedData());
    } else if (parentItem.is<GeoDataTrack>()) {
        GeoDataExtendedData extendedData;
        parentItem.nodeAs<GeoDataTrack>()->setExtendedData(extendedData);
        return static_cast<GeoDataExtendedData *>(&parentItem.nodeAs<GeoDataTrack>()->extendedData());
    } else {
        return nullptr;
    }
}

}
}
