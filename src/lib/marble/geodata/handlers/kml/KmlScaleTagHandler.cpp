// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2013 Mayank Madan <maddiemadan@gmail.com>
//

#include "KmlScaleTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataGeometry.h"
#include "GeoDataModel.h"
#include "GeoDataParser.h"
#include "GeoDataScale.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Scale)

GeoNode *KmlScaleTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Scale)));

    GeoDataScale scale;
    KmlObjectTagHandler::parseIdentifiers(parser, &scale);
    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Model)) {
        parentItem.nodeAs<GeoDataModel>()->setScale(scale);
        return &parentItem.nodeAs<GeoDataModel>()->scale();
    } else {
        return nullptr;
    }
}

}
}
