// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#include "KmlNorthTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataLatLonAltBox.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(north)

GeoNode *KmlnorthTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_north)));

    GeoStackItem parentItem = parser.parentElement();

    qreal const north = parser.readElementText().trimmed().toDouble();
    if (parentItem.represents(kmlTag_LatLonAltBox)) {
        parentItem.nodeAs<GeoDataLatLonAltBox>()->setNorth(north, GeoDataCoordinates::Degree);
    } else if (parentItem.represents(kmlTag_LatLonBox)) {
        parentItem.nodeAs<GeoDataLatLonBox>()->setNorth(north, GeoDataCoordinates::Degree);
    }

    return nullptr;
}

}
}
