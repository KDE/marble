// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlDataTagHandler.h"

#include "KmlElementDictionary.h"

#include "GeoDataData.h"
#include "GeoDataExtendedData.h"
#include "GeoParser.h"

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(Data)

GeoNode *KmlDataTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_Data)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_ExtendedData)) {
        GeoDataData data;

        QString name = parser.attribute("name").trimmed();
        data.setName(name);
        parentItem.nodeAs<GeoDataExtendedData>()->addValue(data);
        return static_cast<GeoDataData *>(&parentItem.nodeAs<GeoDataExtendedData>()->valueRef(name));
    } else {
        return nullptr;
    }
}

}
}
