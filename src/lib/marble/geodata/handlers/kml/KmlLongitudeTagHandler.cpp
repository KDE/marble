// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlLongitudeTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"

#include "GeoDataCamera.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLocation.h"
#include "GeoDataLookAt.h"
#include "GeoParser.h"
#include "MarbleGlobal.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(longitude)

GeoNode *KmllongitudeTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_longitude)));

    GeoStackItem parentItem = parser.parentElement();
    if (parentItem.is<GeoDataLookAt>()) {
        qreal longitude = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataLookAt>()->setLongitude(longitude, GeoDataCoordinates::Degree);
    } else if (parentItem.is<GeoDataCamera>()) {
        qreal longitude = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataCamera>()->setLongitude(longitude, GeoDataCoordinates::Degree);
    } else if (parentItem.is<GeoDataLocation>()) {
        qreal longitude = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataLocation>()->setLongitude(longitude, GeoDataCoordinates::Degree);
    }

    return nullptr;
}

}
}
