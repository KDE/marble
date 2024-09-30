// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlTextColorTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataBalloonStyle.h"
#include "GeoDataParser.h"
#include "KmlColorTagHandler.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(textColor)

GeoNode *KmltextColorTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_textColor)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_BalloonStyle)) {
        QColor const color = KmlcolorTagHandler::parseColor(parser.readElementText().trimmed());
        parentItem.nodeAs<GeoDataBalloonStyle>()->setTextColor(color);
    }
    return nullptr;
}

}
}
