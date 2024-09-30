/*
    SPDX-FileCopyrightText: 2008 Patrick Spendrin <ps_ml@gmx.de>

    SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "KmlIconTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataGroundOverlay.h"
#include "GeoDataIconStyle.h"
#include "GeoDataPhotoOverlay.h"
#include "GeoDataScreenOverlay.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Icon)

GeoNode *KmlIconTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Icon)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_IconStyle)) {
        return parentItem.nodeAs<GeoDataIconStyle>();
    } else if (parentItem.represents(kmlTag_GroundOverlay)) {
        return parentItem.nodeAs<GeoDataGroundOverlay>();
    } else if (parentItem.represents(kmlTag_PhotoOverlay)) {
        return parentItem.nodeAs<GeoDataPhotoOverlay>();
    } else if (parentItem.represents(kmlTag_ScreenOverlay)) {
        return parentItem.nodeAs<GeoDataScreenOverlay>();
    }
    return nullptr;
}

}
}
