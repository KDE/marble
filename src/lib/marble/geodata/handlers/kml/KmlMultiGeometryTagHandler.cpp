/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlMultiGeometryTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataMultiGeometry.h"
#include "GeoDataPlacemark.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(MultiGeometry)

GeoNode *KmlMultiGeometryTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_MultiGeometry)));

    GeoStackItem parentItem = parser.parentElement();

    auto geom = new GeoDataMultiGeometry;
    KmlObjectTagHandler::parseIdentifiers(parser, geom);
    if (parentItem.represents(kmlTag_Placemark)) {
        parentItem.nodeAs<GeoDataPlacemark>()->setGeometry(geom);
        return parentItem.nodeAs<GeoDataPlacemark>()->geometry();

    } else if (parentItem.represents(kmlTag_MultiGeometry)) {
        parentItem.nodeAs<GeoDataMultiGeometry>()->append(geom);
        return geom;
    } else {
        delete geom;
        return nullptr;
    }
}

}
}
