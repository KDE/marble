// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2014 Sanjiban Bairagya <sanjiban22393@gmail.com>
//

#include "KmlCreateTagHandler.h"

#include "GeoDataCreate.h"
#include "GeoDataUpdate.h"
#include "KmlElementDictionary.h"
#include "KmlObjectTagHandler.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Create)

GeoNode *KmlCreateTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Create)));

    auto create = new GeoDataCreate;
    KmlObjectTagHandler::parseIdentifiers(parser, create);
    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Update)) {
        parentItem.nodeAs<GeoDataUpdate>()->setCreate(create);
        return create;
    } else {
        delete create;
        return nullptr;
    }
}

}
}
