// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlBgColorTagHandler.h"

#include "GeoDataBalloonStyle.h"
#include "GeoDataListStyle.h"
#include "KmlColorTagHandler.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(bgColor)

GeoNode *KmlbgColorTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_bgColor)));

    GeoStackItem parentItem = parser.parentElement();

    QColor const color = KmlcolorTagHandler::parseColor(parser.readElementText().trimmed());

    if (parentItem.represents(kmlTag_BalloonStyle)) {
        parentItem.nodeAs<GeoDataBalloonStyle>()->setBackgroundColor(color);
    } else if (parentItem.represents(kmlTag_ListStyle)) {
        parentItem.nodeAs<GeoDataListStyle>()->setBackgroundColor(color);
    }

    return nullptr;
}

}
}
