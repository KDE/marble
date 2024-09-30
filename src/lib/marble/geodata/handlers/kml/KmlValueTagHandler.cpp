// SPDX-License-Identifier: LGPL-2.1-or-later
//
// SPDX-FileCopyrightText: 2010 Harshit Jain <hjain.itbhu@gmail.com>
//

#include "KmlValueTagHandler.h"

#include "MarbleDebug.h"

#include "GeoDataData.h"
#include "GeoDataSimpleArrayData.h"
#include "GeoParser.h"
#include "KmlElementDictionary.h"

#include <QVariant>

namespace Marble
{
namespace kml
{
KML_DEFINE_TAG_HANDLER(value)
KML_DEFINE_TAG_HANDLER_GX22(value)

GeoNode *KmlvalueTagHandler::parse(GeoParser &parser) const
{
    Q_ASSERT(parser.isStartElement() && parser.isValidElement(QLatin1StringView(kmlTag_value)));

    GeoStackItem parentItem = parser.parentElement();

    if (parentItem.represents(kmlTag_Data)) {
        QString value = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoDataData>()->setValue(QVariant(value));
    } else if (parentItem.represents(kmlTag_SimpleArrayData)) {
        QString value = parser.readElementText().trimmed();
        parentItem.nodeAs<GeoDataSimpleArrayData>()->append(QVariant(value));
    }
    return nullptr;
}

}
}
