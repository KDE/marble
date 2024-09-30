/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlInnerBoundaryIsTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataPolygon.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(innerBoundaryIs)

GeoNode *KmlinnerBoundaryIsTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_innerBoundaryIs)));

    GeoStackItem parentItem = parser.parentElement();

    return parentItem.nodeAs<GeoDataPolygon>();
}

}
}
