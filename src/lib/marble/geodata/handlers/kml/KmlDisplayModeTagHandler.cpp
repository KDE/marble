// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Mohammed Nafees <nafees.technocool@gmail.com>
//

#include "KmlDisplayModeTagHandler.h"

#include "GeoDataBalloonStyle.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(displayMode)

GeoNode *KmldisplayModeTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_displayMode)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_BalloonStyle)) {
        QString mode = parser.readElementText().trimmed();
        GeoDataBalloonStyle::DisplayMode displayMode = GeoDataBalloonStyle::Default;
        if (mode == QLatin1StringView("default")) {
            displayMode = GeoDataBalloonStyle::Default;
        } else if (mode == QLatin1StringView("hide")) {
            displayMode = GeoDataBalloonStyle::Hide;
        }

        parentItem.nodeAs<GeoDataBalloonStyle>()->setDisplayMode(displayMode);
    }
    return nullptr;
}

}
}
