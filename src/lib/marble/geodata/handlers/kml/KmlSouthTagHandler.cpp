// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#include "KmlSouthTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataLatLonAltBox.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(south)

GeoNode *KmlsouthTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_south)));

    GeoStackItem parentItem = parser.parentElement();

    qreal const south = parser.readElementText().trimmed().toDouble();
    if (parentItem.represents(kmlTag_LatLonAltBox)) {
        parentItem.nodeAs<GeoDataLatLonAltBox>()->setSouth(south, GeoDataCoordinates::Degree);
    } else if (parentItem.represents(kmlTag_LatLonBox)) {
        parentItem.nodeAs<GeoDataLatLonBox>()->setSouth(south, GeoDataCoordinates::Degree);
    }

    return nullptr;
}

}
}
