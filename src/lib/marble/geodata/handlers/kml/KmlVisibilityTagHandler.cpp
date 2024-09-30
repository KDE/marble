/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlVisibilityTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataFeature.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(visibility)

GeoNode *KmlvisibilityTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_visibility)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataFeature>()) {
        QString visibility = parser.readElementText().trimmed();
        const bool visible = (visibility == QLatin1StringView("1"));
        parentItem.nodeAs<GeoDataFeature>()->setVisible(visible);
    }

    return nullptr;
}

}
}
