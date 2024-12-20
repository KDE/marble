// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Gaurav Gupta <1989.gaurav@googlemail.com>
// SPDX-FileCopyrightText: 2013 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlLatitudeTagHandler.h"

#include "MarbleDebug.h"

#include "KmlElementDictionary.h"
#include "MarbleGlobal.h"

#include "GeoDataCamera.h"
#include "GeoDataCoordinates.h"
#include "GeoDataLocation.h"
#include "GeoDataLookAt.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(latitude)
GeoNode *KmllatitudeTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_latitude)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataLookAt>()) {
        qreal latitude = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataLookAt>()->setLatitude(latitude, GeoDataCoordinates::Degree);
    } else if (parentItem.is<GeoDataCamera>()) {
        qreal latitude = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataCamera>()->setLatitude(latitude, GeoDataCoordinates::Degree);
    } else if (parentItem.is<GeoDataLocation>()) {
        qreal latitude = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataLocation>()->setLatitude(latitude, GeoDataCoordinates::Degree);
    }
    return nullptr;
}
}
}
