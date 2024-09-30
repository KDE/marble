// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#include "KmlMinFadeExtentTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataLod.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(minFadeExtent)

GeoNode *KmlminFadeExtentTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_minFadeExtent)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Lod)) {
        float minFadeExtent = parser.readElementText().trimmed().toFloat();

        parentItem.nodeAs<GeoDataLod>()->setMinFadeExtent(minFadeExtent);
    }

    return nullptr;
}

}
}
