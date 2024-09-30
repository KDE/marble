// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#include "KmlLatLonAltBoxTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataLatLonAltBox.h"
#include "GeoDataRegion.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(LatLonAltBox)

GeoNode *KmlLatLonAltBoxTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_LatLonAltBox)));

    GeoDataLatLonAltBox box;
    KmlObjectTagHandler::parseIdentifiers(parser, &box);

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Region)) {
        parentItem.nodeAs<GeoDataRegion>()->setLatLonAltBox(box);
        return const_cast<GeoDataLatLonAltBox *>(&parentItem.nodeAs<GeoDataRegion>()->latLonAltBox());
    } else {
        return nullptr;
    }
}

}
}
