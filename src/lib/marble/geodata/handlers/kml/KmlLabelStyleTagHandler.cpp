/*
    SPDX-FileCopyrightText: 2008-2009 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlLabelStyleTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataLabelStyle.h"
#include "GeoDataStyle.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(LabelStyle)

GeoNode *KmlLabelStyleTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_LabelStyle)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Style)) {
        GeoDataLabelStyle style;
        KmlObjectTagHandler::parseIdentifiers(parser, &style);

        parentItem.nodeAs<GeoDataStyle>()->setLabelStyle(style);
        return &parentItem.nodeAs<GeoDataStyle>()->labelStyle();
    }
    return nullptr;
}

}
}
