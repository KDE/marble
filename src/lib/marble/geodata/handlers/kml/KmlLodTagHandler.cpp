// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2009 Bastian Holst <bastianholst@gmx.de>
//

#include "KmlLodTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataLod.h"
#include "GeoDataRegion.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Lod)

GeoNode *KmlLodTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Lod)));

    GeoDataLod lod;
    KmlObjectTagHandler::parseIdentifiers(parser, &lod);
    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Region)) {
        parentItem.nodeAs<GeoDataRegion>()->setLod(lod);
        return &parentItem.nodeAs<GeoDataRegion>()->lod();
    } else {
        return nullptr;
    }
}

}
}
