// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlTimeSpanTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataFeature.h"
#include "GeoDataTimeSpan.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(TimeSpan)

GeoNode *KmlTimeSpanTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_TimeSpan)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.is<GeoDataFeature>()) {
        GeoDataTimeSpan timeSpan;
        KmlObjectTagHandler::parseIdentifiers(parser, &timeSpan);
        parentItem.nodeAs<GeoDataFeature>()->setTimeSpan(timeSpan);
        return &parentItem.nodeAs<GeoDataFeature>()->timeSpan();
    }

    return nullptr;
}

}
}
