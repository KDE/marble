//
// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlDisplayNameTagHandler.h"

#include "GeoDataData.h"
#include "GeoDataSimpleField.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(displayName)

GeoNode *KmldisplayNameTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_displayName)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Data)) {
        QString displayName = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoDataData>()->setDisplayName(displayName);
    } else if (parentItem.represents(kmlTag_SimpleField)) {
        QString displayName = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoDataSimpleField>()->setDisplayName(displayName);
    }
    return nullptr;
}

}
}
