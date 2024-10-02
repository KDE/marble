/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlFillTagHandler.h"

#include "GeoDataPolyStyle.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(fill)

GeoNode *KmlfillTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_fill)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_PolyStyle)) {
        bool fill = static_cast<bool>(parser.readElementText().trimmed().toInt());

        parentItem.nodeAs<GeoDataPolyStyle>()->setFill(fill);
    }

    return nullptr;
}

}
}
