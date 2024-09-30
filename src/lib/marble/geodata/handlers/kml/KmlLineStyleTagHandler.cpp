/*
    SPDX-FileCopyrightText: 2008-2009 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlLineStyleTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataLineStyle.h"
#include "GeoDataStyle.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(LineStyle)

GeoNode *KmlLineStyleTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_LineStyle)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Style)) {
        GeoDataLineStyle style;
        KmlObjectTagHandler::parseIdentifiers(parser, &style);
        parentItem.nodeAs<GeoDataStyle>()->setLineStyle(style);
        return &parentItem.nodeAs<GeoDataStyle>()->lineStyle();
    }
    return nullptr;
}

}
}
