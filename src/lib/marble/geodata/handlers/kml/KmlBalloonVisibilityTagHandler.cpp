// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>

#include "KmlBalloonVisibilityTagHandler.h"
#include "GeoDataPlacemark.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

#include <QDebug>

namespace Marble
{
namespace kml
{
namespace gx
{
KML_DEFINE_TAG_HANDLER_GX22(balloonVisibility)

GeoNode *KmlballoonVisibilityTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_balloonVisibility)));

    QString content = parser.readElementText().trimmed();

    const bool visible = (content == QLatin1StringView("1"));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataPlacemark>()) {
        parentItem.nodeAs<GeoDataPlacemark>()->setBalloonVisible(visible);
    }

    return nullptr;
}

}
}
}
