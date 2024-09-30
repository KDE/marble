// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlTimeStampTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataFeature.h"
#include "GeoDataTimeStamp.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(TimeStamp)

GeoNode *KmlTimeStampTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_TimeStamp)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.is<GeoDataFeature>()) {
        GeoDataTimeStamp timestamp;
        KmlObjectTagHandler::parseIdentifiers(parser, &timestamp);
        parentItem.nodeAs<GeoDataFeature>()->setTimeStamp(timestamp);
        return &parentItem.nodeAs<GeoDataFeature>()->timeStamp();
    }
    return nullptr;
}

}
}
