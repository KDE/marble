// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlBalloonStyleTagHandler.h"

#include "GeoDataBalloonStyle.h"
#include "GeoDataStyle.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(BalloonStyle)

GeoNode *KmlBalloonStyleTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_BalloonStyle)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Style)) {
        GeoDataBalloonStyle style;
        KmlObjectTagHandler::parseIdentifiers(parser, &style);

        parentItem.nodeAs<GeoDataStyle>()->setBalloonStyle(style);
        return &parentItem.nodeAs<GeoDataStyle>()->balloonStyle();
    }
    return nullptr;
}

}
}
