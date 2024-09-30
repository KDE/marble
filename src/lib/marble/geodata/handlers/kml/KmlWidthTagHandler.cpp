/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlWidthTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataLineStyle.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(width)

GeoNode *KmlwidthTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_width)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_LineStyle)) {
        float width = parser.readElementText().trimmed().toFloat();

        parentItem.nodeAs<GeoDataLineStyle>()->setWidth(width);
    }

    return nullptr;
}

}
}
