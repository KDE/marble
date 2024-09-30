/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlOuterBoundaryIsTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataPolygon.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(outerBoundaryIs)

GeoNode *KmlouterBoundaryIsTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_outerBoundaryIs)));

    GeoStackItem parentItem = parser.parentElement();

    return parentItem.nodeAs<GeoDataPolygon>();
}

}
}
