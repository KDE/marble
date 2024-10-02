/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlLineStringTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataLineString.h"
#include "GeoDataMultiGeometry.h"
#include "GeoDataPlacemark.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(LineString)

GeoNode *KmlLineStringTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_LineString)));

    GeoStackItem parentItem = parser.parentElement();

    auto lineString = new GeoDataLineString;
    KmlObjectTagHandler::parseIdentifiers(parser, lineString);

    if (parentItem.represents(kmlTag_Placemark)) {
        parentItem.nodeAs<GeoDataPlacemark>()->setGeometry(lineString);
        return parentItem.nodeAs<GeoDataPlacemark>()->geometry();

    } else if (parentItem.represents(kmlTag_MultiGeometry)) {
        parentItem.nodeAs<GeoDataMultiGeometry>()->append(lineString);
        return lineString;

    } else {
        delete lineString;
        return nullptr;
    }
}

}
}
