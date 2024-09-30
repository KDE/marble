// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2012 Dennis Nienhüser <nienhueser@kde.org>
//

#include "KmlLatLonBoxTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataGroundOverlay.h"
#include "GeoDataLatLonBox.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(LatLonBox)

GeoNode *KmlLatLonBoxTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_LatLonBox)));

    GeoDataLatLonBox box;
    KmlObjectTagHandler::parseIdentifiers(parser, &box);

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_GroundOverlay)) {
        parentItem.nodeAs<GeoDataGroundOverlay>()->setLatLonBox(box);
        return &parentItem.nodeAs<GeoDataGroundOverlay>()->latLonBox();
    } else {
        return nullptr;
    }
}

}
}
