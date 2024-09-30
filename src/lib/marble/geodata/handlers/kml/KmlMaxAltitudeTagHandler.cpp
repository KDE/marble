// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#include "KmlMaxAltitudeTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataLatLonAltBox.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(maxAltitude)

GeoNode *KmlmaxAltitudeTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_maxAltitude)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_LatLonAltBox)) {
        float maxAltitude = parser.readElementText().trimmed().toFloat();

        parentItem.nodeAs<GeoDataLatLonAltBox>()->setMaxAltitude(maxAltitude);
    }

    return nullptr;
}

}
}
