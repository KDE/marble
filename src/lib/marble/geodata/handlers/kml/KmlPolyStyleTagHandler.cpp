/*
    SPDX-FileCopyrightText: 2008-2009 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlPolyStyleTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataPolyStyle.h"
#include "GeoDataStyle.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(PolyStyle)

GeoNode *KmlPolyStyleTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_PolyStyle)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Style)) {
        GeoDataPolyStyle style;
        KmlObjectTagHandler::parseIdentifiers(parser, &style);
        parentItem.nodeAs<GeoDataStyle>()->setPolyStyle(style);
        return &parentItem.nodeAs<GeoDataStyle>()->polyStyle();
    }
    return nullptr;
}

}
}
