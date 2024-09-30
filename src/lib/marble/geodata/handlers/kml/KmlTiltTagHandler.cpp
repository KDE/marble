// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlTiltTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataCamera.h"
#include "GeoDataOrientation.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(tilt)

GeoNode *KmltiltTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_tilt)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.is<GeoDataCamera>()) {
        qreal tilt = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataCamera>()->setTilt(tilt);
    } else if (parentItem.is<GeoDataOrientation>()) {
        double tilt = parser.readElementText().trimmed().toDouble();
        parentItem.nodeAs<GeoDataOrientation>()->setTilt(tilt);
    }
    return nullptr;
}

}
}
